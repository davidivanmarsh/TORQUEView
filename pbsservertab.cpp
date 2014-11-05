/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: pbsservertab.cpp.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
*/

#include "pbsservertab.h"
#include "ui_pbsservertab.h"
#include "logviewerdlg.h"
#include "editnodesfiledlg.h"
#include "mainwindow.h"
#include "errormsgdlg.h"
#include <QDateTime>
#include <QStatusBar>
#include <QMessageBox>
#include <QFile>



/*******************************************************************************
 *
*******************************************************************************/
PbsServerTab::PbsServerTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PbsServerTab)
{
    ui->setupUi(this);

	m_mainWindow = dynamic_cast<MainWindow*>(parent);
	if (m_mainWindow == NULL)
	{
		return;
	}

	ui->treeWidget_PbsServer->header()->resizeSection(0, 205);	// name
	ui->treeWidget_PbsServer->header()->resizeSection(1, 125);	// value
}

/*******************************************************************************
 *
*******************************************************************************/
PbsServerTab::~PbsServerTab()
{
    delete ui;
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsServerTab::clearLists()
{
	ui->treeWidget_PbsServer->clear();
	ui->plainTextEdit_NodesFile->clear();
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsServerTab::enableControls()
{
    // data is "live" (i.e, from either a local or remote host instead of a snapshot file), so enable functionality
    ui->btnStartServer->setEnabled(true);
    ui->btnStopServer->setEnabled(true);
    ui->btnRefresh->setEnabled(true);
    ui->btnViewServerLog->setEnabled(true);
	if (m_mainWindow->m_runningState == runningAs_AdminUser)  // if running With admin privileges
	{
		ui->btnEditNodesFile->setEnabled(true);
	}
	else
	{
		ui->btnEditNodesFile->setEnabled(false);
	}
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsServerTab::disableControls()
{
    // data is NOT "live" (i.e, it's from a snapshot file rather than a local or remote host), so disable some functionality
    ui->btnStartServer->setEnabled(false);
    ui->btnStopServer->setEnabled(false);
    ui->btnRefresh->setEnabled(false);
    ui->btnViewServerLog->setEnabled(false);
    ui->btnEditNodesFile->setEnabled(false);
}


/*******************************************************************************
 *
*******************************************************************************/
void PbsServerTab::showNoAdminRightsText()
{
	QString line = QString("(Not running with admin rights -- \"nodes\" file contents unavailable)");
	ui->plainTextEdit_NodesFile->clear();
	ui->plainTextEdit_NodesFile->appendPlainText( line );
}

/*******************************************************************************
 *
*******************************************************************************/
bool PbsServerTab::issueCmd_PbsServer()  // execute a pbs_server status command -- parse output data
{
	ui->treeWidget_PbsServer->clear();

	QString pbsServerStatusCommand;
	if (m_mainWindow->m_Config_DataSource == 0) // if is "Local host"
	{
		int PID = issueCmd_getLocalPbsServerPID();

		pbsServerStatusCommand = QString("%1 \"/proc/%2/status\"")
			.arg(m_mainWindow->m_Config_Cmd_Cat)
			.arg(PID);
	}
	else
	{
		pbsServerStatusCommand = QString("%1 \"%2\"")
			.arg(m_mainWindow->m_Config_Cmd_Cat)
			.arg("/proc/`pgrep pbs_server`/status");
	}


	// show "Wait cursor"
	QApplication::setOverrideCursor(Qt::WaitCursor);

	m_pbsServerStatus_Stdout.clear();
	m_pbsServerStatus_Stderr.clear();

	m_pbsServerStatusProcess = new QProcess(this);
	connect (m_pbsServerStatusProcess, SIGNAL(readyReadStandardOutput()),
			 this, SLOT(pbsServerStatus_getStdout())); // connect process signals with your code
	connect (m_pbsServerStatusProcess, SIGNAL(readyReadStandardError()),
			 this, SLOT(pbsServerStatus_getStderr())); // same here
	m_pbsServerStatusProcess->start(pbsServerStatusCommand);

	if (!m_pbsServerStatusProcess->waitForStarted())
	{
		QApplication::restoreOverrideCursor();
//		QMessageBox::critical(0, "Error trying to start qmgr process", "Process's waitForStarted() method returned error.\n");
		QMessageBox::critical(0, "Couldn't start 'pbsServerStatus' process", "'pbsServerStatus' process unable to start.  Torque is probably not running.\n");
		return false;
	}

	if (!m_pbsServerStatusProcess->waitForFinished(5000)) //If msecs is -1, this function will not time out
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Error trying to complete pbsServerStatus process", "Process's waitForFinished() method returned error.\n");
		return false;
	}

	delete m_pbsServerStatusProcess;
	m_pbsServerStatusProcess = NULL;


	pbsServerStatus_processStdout();

	// restore the original cursor
	QApplication::restoreOverrideCursor();


//	if (m_mainWindow->m_Config_ShowSTDERROutput)
//	{
//		// see if there are any errors to display
//		if (!m_pbsServerStatus_Stderr.isEmpty())
//		{
//			QMessageBox::critical(0, "Error issuing pbsServerStatus command",
//								  QString("Error issuing pbsServerStatus command.  Error message was: %1").arg(m_pbsServerStatus_Stderr));
//		}
//	}

//	ui->plainTextEdit_PbsServer->blockSignals(false);



//	QString nodesFilename = QString("%1/server_priv/nodes")
//			.arg(m_mainWindow->m_Config_PbsServerHomeDir);
//	m_nodesFilename = nodesFilename;

//	loadNodesFile( m_nodesFilename );    // load the "nodes" file from the "server_priv" directory

    return true;
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_PbsServer() - this gets called whenever the pbsServerStatus process has something to say...
void PbsServerTab::pbsServerStatus_getStdout()
{
	if (m_pbsServerStatusProcess != NULL)
	{
		QByteArray s = m_pbsServerStatusProcess->readAllStandardOutput(); // read normal output
		m_pbsServerStatus_Stdout.append( s );  // if there's any stdout
	}
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_PbsServer() - this gets called whenever the pbsServerStatus process has something to say...
void PbsServerTab::pbsServerStatus_getStderr()
{
	if (m_pbsServerStatusProcess != NULL)
	{
		QByteArray s = m_pbsServerStatusProcess->readAllStandardError(); // read error channel
		m_pbsServerStatus_Stderr.append( s );  // if there's any stderr
	}
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsServerTab::pbsServerStatus_processStdout() // parse the stdout data collected (above)
{
//	QStringList slQueues;
//	QString sCreateAndDefine = "# Create and define queue ";

//	ui->plainTextEdit_PbsServer->insertPlainText(QString("\n"));
	QStringList includeList;
	includeList << "Name:" << "State:" << "Tgid:" << "Pid:" << "PPid:" <<
				   "VmPeak:" << "VmSize:" << "VmLck:" <<  "VmHWM:" <<  "VmRSS:" <<  "VmData:" <<  "VmStk:" <<
				   "VmExe:" <<  "VmLib:" <<  "VmPTE:" <<  "VmSwap:" <<  "Threads:" <<  "Cpus_allowed_list:" <<
				   "Mems_allowed_list:" << "voluntary_ctxt_switches:" << "nonvoluntary_ctxt_switches:";

	QStringList lines = m_pbsServerStatus_Stdout.split("\n");
	foreach (QString line, lines)
	{
		QStringList sl = line.split("\t", QString::SkipEmptyParts );
		if (sl.count() == 2)
		{
			QString sName = sl[0];
			QString sValue = sl[1];
			if (includeList.contains( sName ) )
			{
				QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget_PbsServer);
				item->setText(0, sName);
				item->setText(1, sValue);
				item->setTextAlignment(1, Qt::AlignRight);
			}
		}


//		ui->plainTextEdit_PbsServer->appendPlainText(QString("%1").arg(line));
	}

//	// add all queues we found
//	ui->comboBox_Queues->clear();
//	ui->comboBox_Queues->addItems( slQueues );

//  ui->plainTextEdit_PbsServer->setFocus();

}


/*******************************************************************************
 *
*******************************************************************************/
int PbsServerTab::issueCmd_getLocalPbsServerPID()  // execute a command to get the local pbs_server PID
    // (for some reason, using `pgrep pbs_server' works on remote servers but not on local ones, so we have to
    // use this technique for getting the local PID value)
{
	QString getLocalPbsServerPIDCommand = QString("%1")
			.arg("pgrep pbs_server");

	// show "Wait cursor"
	QApplication::setOverrideCursor(Qt::WaitCursor);

	m_getLocalPbsServerPID_Stdout.clear();
	m_getLocalPbsServerPID_Stderr.clear();

	m_getLocalPbsServerPIDProcess = new QProcess(this);
	connect (m_getLocalPbsServerPIDProcess, SIGNAL(readyReadStandardOutput()),
			 this, SLOT(getLocalPbsServerPID_getStdout())); // connect process signals with your code
	connect (m_getLocalPbsServerPIDProcess, SIGNAL(readyReadStandardError()),
			 this, SLOT(getLocalPbsServerPID_getStderr())); // same here
	m_getLocalPbsServerPIDProcess->start(getLocalPbsServerPIDCommand);

	if (!m_getLocalPbsServerPIDProcess->waitForStarted())
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Couldn't start 'pgrep pbs_server' process", "'pgrep pbs_server' process unable to start.  Torque is probably not running.\n");
		return false;
	}

	if (!m_getLocalPbsServerPIDProcess->waitForFinished(5000)) //If msecs is -1, this function will not time out
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Error trying to complete 'pgrep pbs_server' process", "Process's waitForFinished() method returned error.\n");
		return false;
	}

	delete m_getLocalPbsServerPIDProcess;
	m_getLocalPbsServerPIDProcess = NULL;


	int PID = getLocalPbsServerPID_processStdout();

	// restore the original cursor
	QApplication::restoreOverrideCursor();


//	if (m_mainWindow->m_Config_ShowSTDERROutput)
//	{
//		// see if there are any errors to display
//		if (!m_pbsServerStatus_Stderr.isEmpty())
//		{
//			QMessageBox::critical(0, "Error issuing pbsServerStatus command",
//								  QString("Error issuing pbsServerStatus command.  Error message was: %1").arg(m_pbsServerStatus_Stderr));
//		}
//	}

    return PID;
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_getLocalPbsServerPID() - this gets called whenever the getLocalPbsServerPID process has something to say...
void PbsServerTab::getLocalPbsServerPID_getStdout()
{
	if (m_getLocalPbsServerPIDProcess != NULL)
	{
		QByteArray s = m_getLocalPbsServerPIDProcess->readAllStandardOutput(); // read normal output
		m_getLocalPbsServerPID_Stdout.append( s );  // if there's any stdout
	}
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_getLocalPbsServerPID() - this gets called whenever the getLocalPbsServerPID process has something to say...
void PbsServerTab::getLocalPbsServerPID_getStderr()
{
	if (m_getLocalPbsServerPIDProcess != NULL)
	{
		QByteArray s = m_getLocalPbsServerPIDProcess->readAllStandardError(); // read error channel
		m_getLocalPbsServerPID_Stderr.append( s );  // if there's any stderr
	}
}

/*******************************************************************************
 *
*******************************************************************************/
int PbsServerTab::getLocalPbsServerPID_processStdout() // parse the stdout data collected (above)
{
    QStringList lines = m_getLocalPbsServerPID_Stdout.split("\n");
    if (lines.count() > 0)
    {
        return lines[0].toInt();    // return the PID of the pbs_server process
    }
    return 0;  // else return 0
}



/*******************************************************************************
 *
*******************************************************************************/
void PbsServerTab::on_btnRefresh_clicked()  // refresh pbsServerStatus
{
    issueCmd_PbsServer();
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsServerTab::on_btnViewServerLog_clicked()
{
	// bring up the pbs_server's log viewer
	QString cmd = m_mainWindow->m_Config_Cmd_Tail;

//	QString logfilename = "/var/spool/torque//server_logs/";
//	QString logfilename = "/usr/test/torque/server_logs/";
	QString logfilename = QString("%1/server_logs/")
			.arg(m_mainWindow->m_Config_PbsServerHomeDir);

	QDateTime now = QDateTime::currentDateTime();
	QString sDate = now.toString("yyyyMMdd");
//	QString sDate = now.toString("yyyy-MM-dd hh:mm:ss a");
//	QString sTime = now.toString("hh:mm:ss a");
	logfilename.append(sDate);

	LogViewerDlg* logViewerDlg = new LogViewerDlg(m_mainWindow, logfilename, cmd, this);
	logViewerDlg->show();
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsServerTab::on_btnStartServer_clicked()
{
    QMessageBox::critical(0, "Start Server not implemented", "The Start Server functionality has not been implemented.");
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsServerTab::on_btnStopServer_clicked()
{
    QMessageBox::critical(0, "Stop Server not implemented", "The Stop Server functionality has not been implemented.");
}


/*******************************************************************************
 *
*******************************************************************************/
void PbsServerTab::writeDataToFile(QTextStream& out)
{
	// WRITE OUT PBS_SERVER DATA TO FILE:

	// if list is empty, bail out
	if (ui->treeWidget_PbsServer->topLevelItemCount() == 0)
		return;

	out << "\n";
	out << "\n";
	out << "\n";
	out << "#---------------------------------------------------------------" << "\n";
	out << "TORQUEView -- Pbs_server Output" << "\n";
	out << "#---------------------------------------------------------------" << "\n";
	out << "\n";
	out << "\n";

	// iterate through the tree and write out each item
	// (there are only top-level items so far, no child items, so this iterator should work OK)
	QTreeWidgetItemIterator it( ui->treeWidget_PbsServer );
	while (*it)
	{
		int columnCount = ui->treeWidget_PbsServer->columnCount();
		QStringList sl;
		for (int i=0; i < columnCount; i++)
		{
			sl.append( (*it)->text(i) );
		}
		QString line = sl.join("   ");
		out << line << "\n";

		++it;
	}
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsServerTab::writeNodesContentsToFile(QTextStream& out)
{
	// WRITE OUT "nodes" FILE CONTENTS TO A FILE:

	// get all the text
	QString s = ui->plainTextEdit_NodesFile->document()->toPlainText();
	if (s.isEmpty())
		return;  // bail out

	out << "\n";
	out << "\n";
	out << "\n";
	out << "#---------------------------------------------------------------" << "\n";
	out << "TORQUEView -- Nodes File Contents" << "\n";
	out << "#---------------------------------------------------------------" << "\n";
	out << "\n";
	out << "\n";

	out << s << endl;

}


/*******************************************************************************
 *
*******************************************************************************/
void PbsServerTab::initPbsServerFromFile( QTextStream &in )  // init the pbs_server tab from a file
{
	ui->treeWidget_PbsServer->clear();

	while (!in.atEnd())
	{
		QString line = in.readLine();
		if (line.isEmpty())
			continue;

		// have we gotten to the next section after the pbs_server section?
		if (line.startsWith( "#--------" )) // each section will be terminated by a "#-------" string to indicate the start of the next section
		{
			break;
		}

		QStringList sl = line.split(" ", QString::SkipEmptyParts );
		if (sl.count() > 1)
		{
			QString sName = sl[0];
\
			// combine all remaining parts of string into single value
			sl.removeAt(0);
			QString sValue = sl.join(" ");// sValue now has remainder of string

			QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget_PbsServer);
			item->setText(0, sName);
			item->setText(1, sValue);
			item->setTextAlignment(1, Qt::AlignRight);
		}

	} // while not end of file
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsServerTab::initNodesContentsFromFile( QTextStream &in )  // init the "nodes" file contents from a file
{
	ui->plainTextEdit_NodesFile->clear();

	while (!in.atEnd())
	{
		QString line = in.readLine();
		if (line.isEmpty())
			continue;

		// have we gotten to the next section after the pbs_server section?
		if (line.startsWith( "#--------" )) // each section will be terminated by a "#-------" string to indicate the start of the next section
		{
			break;
		}
		ui->plainTextEdit_NodesFile->appendPlainText( line );

	} // while not end of file
}


/*******************************************************************************
 *
*******************************************************************************/
void PbsServerTab::on_btnEditNodesFile_clicked()
{
	// get all the text
	QString sContent = ui->plainTextEdit_NodesFile->document()->toPlainText();

    EditNodesFileDlg dlg(sContent, this);
    if (!dlg.exec())
        return;

	QDateTime now = QDateTime::currentDateTime();
	QString sNow = now.toString("yyyy-MMM-dd_hh-mm-ss");
	QString sDestFilename = QString("nodes__%1").arg(sNow);

	int result = QMessageBox::question(
		this,
		"Save Nodes File?",
		QString("Save changes to the \"nodes\" file?\n\nNOTE: The current \"nodes\" file will be backed up to:\n   %2")
				.arg(sDestFilename),
		QMessageBox::No,
		QMessageBox::Yes
		);
	if (result == QMessageBox::No)
		return;

	if (m_mainWindow->m_Config_DataSource == 1) // if is "Remote host"
	{
		// since it's a remote "nodes" file, first update the local copy of "hosts" which is in the /tmp directory

		// open file for output
		QString nodesFilename("/tmp/nodes");
		QFile outputFile(nodesFilename);
		outputFile.open(QIODevice::WriteOnly);
		// opened OK?
		if(!outputFile.isOpen())
		{
		//	qDebug() << argv[0] << "- Error, unable to open" << outputFilename << "for output";
			QMessageBox::critical(0, "Error writing file", QString("Error, unable to open file '%1' for writing.")
				.arg(nodesFilename));
			return;
		}

		QTextStream outStream(&outputFile);
		outStream << ui->plainTextEdit_NodesFile->toPlainText();
		outputFile.close();
	}

    // refresh the pbs_server tab's copy of the "nodes" file with changes the user made in the EditNodesFileDlg
    ui->plainTextEdit_NodesFile->clear();
    QString contents = dlg.getPlainTextEdit_NodesFile_contents();
    QStringList lines = contents.split("\n");
    foreach (QString line, lines)
    {
        ui->plainTextEdit_NodesFile->appendPlainText(QString("%1").arg(line));
    }

	issueCmd_CopyNodesFile(sDestFilename);
	issueCmd_SaveNodesFile();
}

/*******************************************************************************
 *
*******************************************************************************/
bool PbsServerTab::loadLocalNodesFile( QString nodesFilename )
{
	// open "nodes" file for input
	QFile file(nodesFilename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		// don't show this msg, since it won't be needed when detecting root-user privileges
//		QMessageBox::critical(0, "Error reading file", QString("Error, unable to read file '%1'.")
//			.arg(nodesFilename));
		return false;
	}

	QTextStream in(&file);
	ui->plainTextEdit_NodesFile->clear();
	ui->plainTextEdit_NodesFile->setPlainText( in.readAll() );
	file.close();
	return true;
}


/*******************************************************************************
 *
*******************************************************************************/
void PbsServerTab::copyLocalNodesFile( QString fromFilename, QString toFilename )
{
//  QFile::copy("path1/file", "path2/file");
    if (QFile::copy(fromFilename, toFilename) == false) // if error copying..
    {
        QMessageBox::critical(0, "Error copying file", QString("Error, unable to copy file '%1' to: %2.")
            .arg(fromFilename).arg(toFilename));
        return;
    }
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsServerTab::saveLocalNodesFile( QString nodesFilename )
{
    // open file for output
//  QString nodesFilename("/tmp/nodes");
    QFile outputFile(nodesFilename);
    outputFile.open(QIODevice::WriteOnly);
    // opened OK?
    if(!outputFile.isOpen())
    {
    //	qDebug() << argv[0] << "- Error, unable to open" << outputFilename << "for output";
        QMessageBox::critical(0, "Error writing file", QString("Error, unable to open file '%1' for writing.")
            .arg(nodesFilename));
        return;
    }

    QTextStream outStream(&outputFile);
    outStream << ui->plainTextEdit_NodesFile->toPlainText();
    outputFile.close();
}


// ------------START OF LOAD NODES FILE ------------

/*******************************************************************************
 *
*******************************************************************************/
bool PbsServerTab::issueCmd_LoadNodesFile()  // execute a "load nodes file" command -- parse output data
{
    ui->plainTextEdit_NodesFile->clear();

//	// do a "cat" to display the contents of the "nodes" file
//	QString loadNodesFileCommand = QString("%1 %2%3")
//			.arg(m_mainWindow->m_Config_Cmd_Cat)
//			.arg(m_mainWindow->m_Config_PbsServerHomeDir)
//			.arg("/server_priv/nodes");

	if (m_mainWindow->m_Config_DataSource == 0) // if is "Local host"
	{
		// get the "nodes" filepath on the local host
		QString nodesFilename = QString("%1%2")
				.arg(m_mainWindow->m_Config_PbsServerHomeDir)
				.arg("/server_priv/nodes");
		return loadLocalNodesFile(nodesFilename);
	}

	// do an "scp" to copy the "nodes" file to the local "/tmp" directory
	QString loadNodesFileCommand = QString("%1:%2%3 /tmp")
			.arg(m_mainWindow->m_Config_Cmd_Scp_FromRemoteToLocal)
			.arg(m_mainWindow->m_Config_PbsServerHomeDir)
			.arg("/server_priv/nodes");


//	m_mainWindow->statusBar()->showMessage(QString("%1").arg(pbsServerStatusCommand), 10000);
//	ui->label_CmdExecuted->setText( pbsServerStatusCommand );

	// show "Wait cursor"
	QApplication::setOverrideCursor(Qt::WaitCursor);

	m_loadNodesFile_Stdout.clear();
	m_loadNodesFile_Stderr.clear();

	m_loadNodesFileProcess = new QProcess(this);
	connect (m_loadNodesFileProcess, SIGNAL(readyReadStandardOutput()),
			 this, SLOT(loadNodesFile_getStdout())); // connect process signals with your code
	connect (m_loadNodesFileProcess, SIGNAL(readyReadStandardError()),
			 this, SLOT(loadNodesFile_getStderr())); // same here
	m_loadNodesFileProcess->start(loadNodesFileCommand);

	if (!m_loadNodesFileProcess->waitForStarted())
	{
		QApplication::restoreOverrideCursor();
//		QMessageBox::critical(0, "Error trying to start qmgr process", "Process's waitForStarted() method returned error.\n");
		QMessageBox::critical(0, "Couldn't start 'loadNodesFile' process", "'loadNodesFile' process unable to start.  Torque is probably not running.\n");
		return false;
	}

	if (!m_loadNodesFileProcess->waitForFinished(8000)) //If msecs is -1, this function will not time out
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Error trying to complete loadNodesFile process", "Process's waitForFinished() method returned error.\n");
		return false;
	}

	delete m_loadNodesFileProcess;
	m_loadNodesFileProcess = NULL;


	bool bStatus = loadNodesFile_processStdout();

	// restore the original cursor
	QApplication::restoreOverrideCursor();


//	if (m_mainWindow->m_Config_ShowSTDERROutput)
//	{
		// see if there are any errors to display
		if (!m_loadNodesFile_Stderr.isEmpty())
		{
		//	QMessageBox::critical(0, "Error issuing loadNodesFile command",
		//						  QString("Error issuing loadNodesFile command.  Error message was: %1").arg(m_loadNodesFile_Stderr));
		}
//	}

//	ui->plainTextEdit_PbsServer->blockSignals(false);



//	QString nodesFilename = QString("%1/server_priv/nodes")
//			.arg(m_mainWindow->m_Config_PbsServerHomeDir);
//	m_nodesFilename = nodesFilename;

//	loadNodesFile( m_nodesFilename );    // load the "nodes" file from the "server_priv" directory

    return bStatus;
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_LoadNodesFile() - this gets called whenever the loadNodesFile process has something to say...
void PbsServerTab::loadNodesFile_getStdout()
{
	if (m_loadNodesFileProcess != NULL)
	{
		QByteArray s = m_loadNodesFileProcess->readAllStandardOutput(); // read normal output
		m_loadNodesFile_Stdout.append( s );  // if there's any stdout
	}
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_LoadNodesFile() - this gets called whenever the loadNodesFile process has something to say...
void PbsServerTab::loadNodesFile_getStderr()
{
	if (m_loadNodesFileProcess != NULL)
	{
		QByteArray s = m_loadNodesFileProcess->readAllStandardError(); // read error channel
		m_loadNodesFile_Stderr.append( s );  // if there's any stderr
	}
}

/*******************************************************************************
 *
*******************************************************************************/
bool PbsServerTab::loadNodesFile_processStdout() // parse the stdout data collected (above)
{
    // open file for input
    QString filename("/tmp/nodes");
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(0, "Error reading file", QString("Error, unable to read file '%1'.")
                              .arg(filename));
        return false;
    }

    QTextStream in(&file);
    ui->plainTextEdit_NodesFile->clear();
    ui->plainTextEdit_NodesFile->setPlainText( in.readAll() );
    file.close();
    return true;
}




// ------------START OF COPY NODES FILE ------------

/*******************************************************************************
 *
*******************************************************************************/
bool PbsServerTab::issueCmd_CopyNodesFile( QString sDestFilename )  // execute a "copy nodes file" command -- parse output data
{
	QString fromFilePath = QString("%1%2")
			.arg(m_mainWindow->m_Config_PbsServerHomeDir)
			.arg("/server_priv/nodes");
	QString toFilePath = QString("%1%2%3")
			.arg(m_mainWindow->m_Config_PbsServerHomeDir)
			.arg("/server_priv/")
			.arg(sDestFilename);

	QString copyNodesFileCommand = QString("%1 %2 %3")
			.arg(m_mainWindow->m_Config_Cmd_Cp)
			.arg(fromFilePath)
			.arg(toFilePath);

	if (m_mainWindow->m_Config_DataSource == 0) // if is "Local host"
	{
		// do a "cp" to copy the local "nodes" file (adds the current date/time to the end of the filename)
		copyLocalNodesFile(fromFilePath, toFilePath);
		return true;
	}

	// show "Wait cursor"
	QApplication::setOverrideCursor(Qt::WaitCursor);

	m_copyNodesFile_Stdout.clear();
	m_copyNodesFile_Stderr.clear();

	m_copyNodesFileProcess = new QProcess(this);
	connect (m_copyNodesFileProcess, SIGNAL(readyReadStandardOutput()),
			 this, SLOT(copyNodesFile_getStdout())); // connect process signals with your code
	connect (m_copyNodesFileProcess, SIGNAL(readyReadStandardError()),
			 this, SLOT(copyNodesFile_getStderr())); // same here
	m_copyNodesFileProcess->start(copyNodesFileCommand);

	if (!m_copyNodesFileProcess->waitForStarted())
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Couldn't start 'copyNodesFile' process", "'copyNodesFile' process unable to start.  Torque is probably not running.\n");
		return false;
	}

	if (!m_copyNodesFileProcess->waitForFinished(8000)) //If msecs is -1, this function will not time out
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Error trying to complete copyNodesFile process", "Process's waitForFinished() method returned error.\n");
		return false;
	}

	delete m_copyNodesFileProcess;
	m_copyNodesFileProcess = NULL;


	copyNodesFile_processStdout();

	// restore the original cursor
	QApplication::restoreOverrideCursor();


//	if (m_mainWindow->m_Config_ShowSTDERROutput)
//	{
		// see if there are any errors to display
		if (!m_copyNodesFile_Stderr.isEmpty())
		{
			QMessageBox::critical(0, "Error issuing copyNodesFile command",
								  QString("Error issuing copyNodesFile command.  Error message was: %1").arg(m_copyNodesFile_Stderr));
		}
//	}

//	ui->plainTextEdit_PbsServer->blockSignals(false);



//	QString nodesFilename = QString("%1/server_priv/nodes")
//			.arg(m_mainWindow->m_Config_PbsServerHomeDir);
//	m_nodesFilename = nodesFilename;

//	copyNodesFile( m_nodesFilename );    // load the "nodes" file from the "server_priv" directory

    return true;
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_copyNodesFile() - this gets called whenever the copyNodesFile process has something to say...
void PbsServerTab::copyNodesFile_getStdout()
{
	if (m_copyNodesFileProcess != NULL)
	{
		QByteArray s = m_copyNodesFileProcess->readAllStandardOutput(); // read normal output
		m_copyNodesFile_Stdout.append( s );  // if there's any stdout
	}
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_copyNodesFile() - this gets called whenever the copyNodesFile process has something to say...
void PbsServerTab::copyNodesFile_getStderr()
{
	if (m_copyNodesFileProcess != NULL)
	{
		QByteArray s = m_copyNodesFileProcess->readAllStandardError(); // read error channel
		m_copyNodesFile_Stderr.append( s );  // if there's any stderr
	}
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsServerTab::copyNodesFile_processStdout() // parse the stdout data collected (above)
{
//    // open file for input
//    QString filename("/tmp/nodes");
//    QFile file(filename);
//    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
//    {
//        QMessageBox::critical(0, "Error reading file", QString("Error, unable to read file '%1'.")
//                              .arg(filename));
//        return;
//    }

//    QTextStream in(&file);
//    ui->plainTextEdit_NodesFile->setPlainText( in.readAll() );
//    file.close();
}


// ------------START OF SAVE NODES FILE ------------

/*******************************************************************************
 *
*******************************************************************************/
bool PbsServerTab::issueCmd_SaveNodesFile()  // execute a "save nodes file" command -- parse output data
{
	if (m_mainWindow->m_Config_DataSource == 0) // if is "Local host"
	{
		// save out the "nodes" file
		QString nodesFilename = QString("%1%2")
				.arg(m_mainWindow->m_Config_PbsServerHomeDir)
				.arg("/server_priv/nodes");
		saveLocalNodesFile(nodesFilename);
		return true;
	}

	// do a "scp" to copy the "nodes" file back to the server
	QString saveNodesFileCommand = QString("%1 /tmp/nodes  %2:%3%4")
			.arg(m_mainWindow->m_Config_Cmd_Scp_FromLocalToRemote)
			.arg(m_mainWindow->m_Config_RemoteServer)
			.arg(m_mainWindow->m_Config_PbsServerHomeDir)
			.arg("/server_priv/nodes");


	// show "Wait cursor"
	QApplication::setOverrideCursor(Qt::WaitCursor);

	m_saveNodesFile_Stdout.clear();
	m_saveNodesFile_Stderr.clear();

	m_saveNodesFileProcess = new QProcess(this);
	connect (m_saveNodesFileProcess, SIGNAL(readyReadStandardOutput()),
			 this, SLOT(saveNodesFile_getStdout())); // connect process signals with your code
	connect (m_saveNodesFileProcess, SIGNAL(readyReadStandardError()),
			 this, SLOT(saveNodesFile_getStderr())); // same here
	m_saveNodesFileProcess->start(saveNodesFileCommand);

	if (!m_saveNodesFileProcess->waitForStarted())
	{
		QApplication::restoreOverrideCursor();
//		QMessageBox::critical(0, "Error trying to start qmgr process", "Process's waitForStarted() method returned error.\n");
		QMessageBox::critical(0, "Couldn't start 'saveNodesFile' process", "'saveNodesFile' process unable to start.  Torque is probably not running.\n");
		return false;
	}

	if (!m_saveNodesFileProcess->waitForFinished(8000)) //If msecs is -1, this function will not time out
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Error trying to complete saveNodesFile process", "Process's waitForFinished() method returned error.\n");
		return false;
	}

	delete m_saveNodesFileProcess;
	m_saveNodesFileProcess = NULL;


	saveNodesFile_processStdout();

	// restore the original cursor
	QApplication::restoreOverrideCursor();


//	if (m_mainWindow->m_Config_ShowSTDERROutput)
//	{
		// see if there are any errors to display
		if (!m_saveNodesFile_Stderr.isEmpty())
		{
			QMessageBox::critical(0, "Error issuing saveNodesFile command",
								  QString("Error issuing saveNodesFile command.  Error message was: %1").arg(m_saveNodesFile_Stderr));
		}
//	}

//	ui->plainTextEdit_PbsServer->blockSignals(false);



//	QString nodesFilename = QString("%1/server_priv/nodes")
//			.arg(m_mainWindow->m_Config_PbsServerHomeDir);
//	m_nodesFilename = nodesFilename;

//	saveNodesFile( m_nodesFilename );    // load the "nodes" file from the "server_priv" directory

    return true;
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_saveNodesFile() - this gets called whenever the saveNodesFile process has something to say...
void PbsServerTab::saveNodesFile_getStdout()
{
	if (m_saveNodesFileProcess != NULL)
	{
		QByteArray s = m_saveNodesFileProcess->readAllStandardOutput(); // read normal output
		m_saveNodesFile_Stdout.append( s );  // if there's any stdout
	}
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_saveNodesFile() - this gets called whenever the saveNodesFile process has something to say...
void PbsServerTab::saveNodesFile_getStderr()
{
	if (m_saveNodesFileProcess != NULL)
	{
		QByteArray s = m_saveNodesFileProcess->readAllStandardError(); // read error channel
		m_saveNodesFile_Stderr.append( s );  // if there's any stderr
	}
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsServerTab::saveNodesFile_processStdout() // parse the stdout data collected (above)
{
//    // open file for input
//    QString filename("/tmp/nodes");
//    QFile file(filename);
//    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
//    {
//        QMessageBox::critical(0, "Error reading file", QString("Error, unable to read file '%1'.")
//                              .arg(filename));
//        return;
//    }

//    QTextStream in(&file);
//    ui->plainTextEdit_NodesFile->setPlainText( in.readAll() );
//    file.close();
}

