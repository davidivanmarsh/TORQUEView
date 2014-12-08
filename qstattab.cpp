/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: qstattab.cpp.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
*/

#include "qstattab.h"
#include "ui_qstattab.h"
#include "mainwindow.h"
#include "exechostdlg.h"
#include "errormsgdlg.h"
#include <QMessageBox>
#include <QStatusBar>



/*******************************************************************************
 *
*******************************************************************************/
QstatTab::QstatTab(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::QstatTab)
{
	ui->setupUi(this);
//	CreateContextMenus();  // at Michael Jackson's request, removing this context menu for now.  It could possibly go back in later...

	m_mainWindow = dynamic_cast<MainWindow*>(parent);
	if (m_mainWindow == NULL)
	{
		return;
	}

	ui->treeWidget_JobInfo->header()->resizeSection(0, 220);	// Node name
	ui->treeWidget_JobInfo->header()->resizeSection(1, 125);	// Node status

	ui->splitter->setStretchFactor(0,3);
	ui->splitter->setStretchFactor(1,2);

//	int height = ui->btnExecHost->height();
//	ui->label_CmdExecuted_Jobs->setFixedHeight(height - 3);

    m_lastJobIdSelected = ""; // init

    resetErrorMsgDlg();
}

/*******************************************************************************
 *
*******************************************************************************/
QstatTab::~QstatTab()
{
	delete ui;
}

/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::CreateContextMenus()
{
	ui->treeWidget_Jobs->addAction(ui->actionRun_job);
	ui->treeWidget_Jobs->addAction(ui->actionDelete_job);
	ui->treeWidget_Jobs->addAction(ui->actionPut_job_on_hold);
	ui->treeWidget_Jobs->addAction(ui->actionRelease_hold_on_job);
	ui->treeWidget_Jobs->addAction(ui->actionRerun_job);
	ui->treeWidget_Jobs->setContextMenuPolicy(Qt::ActionsContextMenu);

	ui->treeWidget_JobInfo->addAction(ui->actionShow_Nodes_Job_Is_Running_On);
	ui->treeWidget_JobInfo->addAction(ui->actionClear_Nodes_Job_Is_Running_On);
	ui->treeWidget_JobInfo->setContextMenuPolicy(Qt::ActionsContextMenu);
}

/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::resetErrorMsgDlg()
{
    m_showQStat_a_STDERROutput = true;
    m_showQStat_f_STDERROutput = true;
    m_showRunJobSTDERROutput = true;
    m_showDeleteJobSTDERROutput = true;
    m_showHoldJobSTDERROutput = true;
    m_showReleaseHoldOnJobSTDERROutput = true;
    m_showRerunJobSTDERROutput = true;

}

/*******************************************************************************
 *
*******************************************************************************/
QString QstatTab::getLastJobIdSelected()
{
    return m_lastJobIdSelected;
}

/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::clearLists()
{
	ui->treeWidget_Jobs->clear();		// clear out Job list
	ui->treeWidget_JobInfo->clear();	// clear out JobInfo list
}

/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::on_checkBox_ShowJobsByQueue_stateChanged()
{
    updateList();
}
/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::on_checkBox_Queued_stateChanged()  // show queued jobs
{
    updateList();
}
/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::on_checkBox_Running_stateChanged()  // show running jobs
{
    updateList();
}
/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::on_checkBox_Completed_stateChanged()   // show completed jobs
{
    updateList();
}
/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::on_checkBox_Hold_stateChanged()   // show hold jobs
{
    updateList();
}
/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::on_checkBox_Suspended_stateChanged()   // show suspended jobs
{
    updateList();
}

/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::on_checkBox_ExpandAll_stateChanged()   // expand all tree heirarchy
{
    updateList();
}

/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::initQstatFromFile( QTextStream &in )  // init the qstat tab from a file
{
    m_pbsJobs.clear();

//    bool bShowHeirarchy = false;
//    if (ui->checkBox_ShowJobsByQueue->isChecked())
//        bShowHeirarchy = true;

	bool bNodeNameSeen = false;  // have we come to the nodeName yet?

	while (!in.atEnd())
	{
		QString line = in.readLine().trimmed();  // remove leading and trailing spaces
		if (line.isEmpty())
			continue;

		// have we gotten to the next section after the qstat -a section?
		if (line.startsWith( "#--------" )) // each section will be terminated by a "#-------" string to indicate the start of the next section
		{
			break;
		}

		if (line.startsWith("Req'd") ||
				line.startsWith("Job ID") ||
				line.startsWith("----"))
		{
			continue;
		}
		else if (!bNodeNameSeen) // first line in output is node name
		{
			ui->label_Title_NodeName->setText(line);
			bNodeNameSeen = true;
		}
		else
		{
			QStringList fields = line.split(" ", QString::SkipEmptyParts);  // split on 'blank' boundaries
			int count = fields.size();
			if (count > 10)  // when user specifies "Show Heirarchy by Queue", first line will be just the quene name, i.e, "batch", so ignore it
			{
				QString jobID			= fields[0];
				QString username		= fields[1];
				QString queue			= fields[2];
				QString jobname			= fields[3];
				QString sessID			= fields[4];
				QString NDS				= fields[5];
				QString TSK				= fields[6];
				QString requiredMemory	= fields[7];
				QString requiredTime	= fields[8];
				QString state			= fields[9];
				QString elapsedTime		= fields[10];
//				QString BIG				= fields[9];
//				QString FAST			= fields[10];
//				QString PFS				= fields[11];

				PbsJob* job = new PbsJob(jobID, username, queue, jobname, sessID, NDS, TSK, requiredMemory, requiredTime, state, elapsedTime/*, BIG, FAST, PFS*/);
				m_pbsJobs.append(job);
			}
		}

	} // while not end of file

	updateList();


	// resize columns to contents
	int columnCount = ui->treeWidget_Jobs->header()->count();
	for(int i = 0; i < columnCount; i++)
	{
		ui->treeWidget_Jobs->resizeColumnToContents(i);
	}
	// ...but make sure first col (jobID) is wide enough
	ui->treeWidget_Jobs->header()->resizeSection(0, 240);	// Job ID column

}

/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::updateList()
{
    QMap<QString,QTreeWidgetItem*> heirarchyMap;

    // determine if user wants to show tree heirarchy based on the "Queue" field (ie. by "Queue")
	bool bShowJobsByQueue = false;

    bool bShowQueued = false;
    bool bShowRunning = false;
    bool bShowCompleted = false;
    bool bShowHold = false;
    bool bShowSuspended = false;
    bool bExpandAll = false;  // expand all tree heirarchy
    if (ui->checkBox_ShowJobsByQueue->isChecked())
		bShowJobsByQueue = true;
    if (ui->checkBox_Queued->isChecked())
        bShowQueued = true;
    if (ui->checkBox_Running->isChecked())
        bShowRunning = true;
    if (ui->checkBox_Completed->isChecked())
        bShowCompleted = true;
    if (ui->checkBox_Hold->isChecked())
        bShowHold = true;
    if (ui->checkBox_Suspended->isChecked())
        bShowSuspended = true;
    if (ui->checkBox_ExpandAll->isChecked())
        bExpandAll = true;


    ui->treeWidget_Jobs->clear();

    foreach (PbsJob* job, m_pbsJobs)
    {
        bool bAdd = false;
        if (!bShowQueued && !bShowRunning && !bShowCompleted && !bShowHold && !bShowSuspended)  // if none are checked, then assume we add ALL items
            bAdd = true;
        else if (bShowQueued && job->m_state == "Q")
            bAdd = true;
        else if (bShowRunning && job->m_state == "R")
            bAdd = true;
        else if (bShowCompleted && job->m_state == "C")
            bAdd = true;
        else if (bShowHold && job->m_state == "H")
            bAdd = true;
        else if (bShowSuspended && job->m_state == "S")
            bAdd = true;
        if (!bAdd)
            continue;

		QTreeWidgetItem* item = NULL;

		// first, see if this item is a job array item (i.e, if it's in format "6459[8].dmarsh.ac"), and if
		// so, create heirarchy beneath job array root
		QStringList slWholeID = job->m_jobID.split(".");
		QString sJobID = slWholeID[0];  // jobID is first part (just before first period)
		QStringList slJobIDParts = sJobID.split("[");  // look for a left square-bracket "[" ..
		// add a "[]" to the base JobArray string
		QString sJobArray = slJobIDParts[0] + "[]";
		if (slWholeID.count() > 1)
		{
			sJobArray += ".";
			sJobArray += slWholeID[1]; // tack on server name (e.g., ".dmarsh.ac")
		}

        bool bIsJobArrayItem = false;
        if (slJobIDParts.count() > 1) // see if it's a job array item
            bIsJobArrayItem = true;
        // create a new item and add it to the tree heirarchy
        // NOTE: most of the params to addHierarchyItem() are for displaying a jobArray head item
        item = addHierarchyItem( heirarchyMap, bShowJobsByQueue, job->m_queue, bIsJobArrayItem,
            sJobArray, job->m_username, job->m_jobname, job->m_sessID, job->m_NDS, job->m_TSK, bExpandAll );


        item->setText(0,  job->m_jobID);
        item->setText(1,  job->m_username);
        item->setText(2,  job->m_queue);
        item->setText(3,  job->m_jobname);
        item->setText(4,  job->m_sessID);
        item->setText(5,  job->m_NDS);
        item->setText(6,  job->m_TSK);
        item->setText(7,  job->m_requiredMemory);
        item->setText(8,  job->m_requiredTime);
        item->setText(9,  job->m_state);
        item->setText(10, job->m_elapsedTime);
//      item->setText(9,  job->m_BIG);
//      item->setText(10, job->m_FAST);
//      item->setText(11, job->m_PFS);

        QString detailedJobState;
        QIcon icon;
        QColor backgroundcolor;
        getJobInfo(job->m_state, detailedJobState, icon, backgroundcolor); 	// get job state enum
        item->setIcon(0, icon);
        item->setBackgroundColor(9, backgroundcolor);
        item->setToolTip(9, detailedJobState );
    }

    restoreExpandedState(); // expand any root-level items in Jobs list that previously were expanded
}

/*******************************************************************************
 *
*******************************************************************************/
QTreeWidgetItem* QstatTab::addHierarchyItem( QMap<QString,QTreeWidgetItem*>& heirarchyMap,
	bool bShowJobsByQueue, QString sQueue, bool bIsJobArrayItem, QString sJobArray,
	QString sUsername, QString sJobname, QString sSessionID, QString sNDS, QString sTSK, bool bExpandAll )
{
	QTreeWidgetItem* newItem = NULL;
	QTreeWidgetItem* queueParent = NULL;

	if (bShowJobsByQueue)
	{
		if (heirarchyMap.contains(sQueue))
		{
			queueParent = heirarchyMap.value(sQueue);
		}
		else
		{
			queueParent = new QTreeWidgetItem(ui->treeWidget_Jobs);  // ..create it at root level of tree
			queueParent->setText(0, sQueue);
			// attach a 0 to the item (indicates it's NOT a jobID)
			queueParent->setData(0, Qt::UserRole, QVariant::fromValue(0));
			if (bExpandAll)
				queueParent->setExpanded(true);
			heirarchyMap[sQueue] = queueParent;
		}
	}

	if (bIsJobArrayItem) // if this is a job array item
	{
		QTreeWidgetItem* jobArrayHeadItem = NULL;
		if (heirarchyMap.contains(sJobArray))
		{
			jobArrayHeadItem = heirarchyMap.value(sJobArray);
			newItem = new QTreeWidgetItem(jobArrayHeadItem); // create the new item under the job array parent
			// attach a 1 to the item (indicates it's a jobID)
			newItem->setData(0, Qt::UserRole, QVariant::fromValue(1));
		}
		else
		{
			if (bShowJobsByQueue)  // if showing queue heirarchy..
				jobArrayHeadItem = new QTreeWidgetItem(queueParent);  // create it below the queue parent
			else
				jobArrayHeadItem = new QTreeWidgetItem(ui->treeWidget_Jobs);  // create it at root level of tree

			QStringList slJobname = sJobname.split("-");
			QString sTruncatedJobname = slJobname[0];  // truncate off the part following the "-" dash char (e.g, "STDIN_11" would become just "STDIN")

			jobArrayHeadItem->setText(0, sJobArray);
			jobArrayHeadItem->setText(1, sUsername);
			jobArrayHeadItem->setText(2, sQueue);  // jobs for this jobArray might be split across multiple queues, but this is rare...
			jobArrayHeadItem->setText(3, sTruncatedJobname);
			jobArrayHeadItem->setText(4, sSessionID);
			jobArrayHeadItem->setText(5, sNDS);
			jobArrayHeadItem->setText(6, sTSK);
			// attach a 0 to the item (indicates it's NOT a jobID)
			jobArrayHeadItem->setData(0, Qt::UserRole, QVariant::fromValue(0));
			if (bExpandAll)
				jobArrayHeadItem->setExpanded(true);
			heirarchyMap[sJobArray] = jobArrayHeadItem;
			newItem = new QTreeWidgetItem(jobArrayHeadItem);  // create the new item under the job array parent
			// attach 1 to the item (indicates it's a jobID)
			newItem->setData(0, Qt::UserRole, QVariant::fromValue(1));
		}
	}
	else // it's not a job array item, so just create a regular item (if bShowJobsByQueue is true, under the queue item)
	{
		if (bShowJobsByQueue)
			newItem = new QTreeWidgetItem(queueParent);
		else
			newItem = new QTreeWidgetItem(ui->treeWidget_Jobs);

		// attach 1 to the item (indicates it's a jobID)
		newItem->setData(0, Qt::UserRole, QVariant::fromValue(1));
	}

    return newItem;
}

/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::enableControls()
{
    // data is "live" (i.e, from either a local or remote host instead of a snapshot file), so enable all context menu items
    ui->actionRun_job->setEnabled(true);
    ui->actionDelete_job->setEnabled(true);
    ui->actionPut_job_on_hold->setEnabled(true);
    ui->actionRelease_hold_on_job->setEnabled(true);
    ui->actionRerun_job->setEnabled(true);
}

/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::disableControls()
{
    // data is NOT "live" (i.e, it's from a snapshot file rather than a local or remote host), so disable all context menu items
    ui->actionRun_job->setEnabled(false);
    ui->actionDelete_job->setEnabled(false);
    ui->actionPut_job_on_hold->setEnabled(false);
    ui->actionRelease_hold_on_job->setEnabled(false);
    ui->actionRerun_job->setEnabled(false);
}

/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::initQstats_f_FromFile( QTextStream &in )
{
//	ui->treeWidget_Jobs->clear();		// clear out Job list
//	ui->treeWidget_JobInfo->clear();	// clear out JobInfo list

	m_jobInfoMap.clear();

	JobInfo* jobInfoItem = NULL;
	QString sJobName;


	while (!in.atEnd())
	{
		QString line = in.readLine();
		if (line.isEmpty())
			continue;

		// have we gotten to the next section after the momctl section?
		if (line.startsWith( "#--------" )) // each section will be terminated by a "#-------" string to indicate the start of the next section
		{
			break;
		}


		if ((! line.startsWith("   ") && (! line.startsWith(("\t"))) ))	// if line starts with a jobIDName (instead of blanks)..
		{
			if (jobInfoItem != NULL) // if there's momctl data waiting to be added..
			{
				m_jobInfoMap[sJobName] = jobInfoItem;  // add the jobInfoItem item to the map (IMPORTANT: use the old sJobName!!)
				jobInfoItem = NULL;	// re-init
			}
			jobInfoItem = new JobInfo();	// start a new jobInfo item


			// now update sJobName - get the job name from this line -- (will be used the next iteration - when we do the m_jobInfoHash[sJobName] above)
			QStringList sl = line.split(":");
			if (sl.count() > 1)
			{
				sJobName = sl[1].trimmed();  // remove leading and trailing spaces
			}
		}
		else
		{
			jobInfoItem->m_data.append(line);
		}
	} // while not end of file


	// OK, we're at the end of the section -- see if one last item needs to be added
	if (jobInfoItem != NULL) // if there's data waiting to be added to the momctl map
	{
		m_jobInfoMap[sJobName] = jobInfoItem;  // add the jobInfo item to the hash
		jobInfoItem = NULL;	// re-init
	}
}


/*******************************************************************************
 *
*******************************************************************************/
bool QstatTab::issueCmd_Qstat_a()  // init the qstat tab from cmdline "qstat -a" command
{
	// re-init all GUI widgets
	ui->treeWidget_Jobs->clear();	// clear out list
//  ui->treeWidget_JobInfo->clear();	// clear out list

	m_qstat_a_Stdout.clear();
	m_qstat_a_Stderr.clear();
	m_pbsJobs.clear();

	QString qstatCommand;
	if (m_mainWindow->m_Config_Call_Qstat_with_T_Flag)  // the "qstat -t" flag is for supporting job arrays
	{
		// do a "qstat -a' as the default display here, using the "-t" flag (supports job arrays)
		qstatCommand = m_mainWindow->m_Config_Cmd_Qstat_a_with_T;
	}
	else
	{
		// do a "qstat -a' as the default display here, but without the "-t" flag
		qstatCommand = m_mainWindow->m_Config_Cmd_Qstat_a_without_T;
	}


	// show "Wait cursor"
	QApplication::setOverrideCursor(Qt::WaitCursor);


	m_qstat_a_Process = new QProcess(this);
	connect (m_qstat_a_Process, SIGNAL(readyReadStandardOutput()),
			 this, SLOT(qstat_a_getStdout())); // connect process signals with code
	connect (m_qstat_a_Process, SIGNAL(readyReadStandardError()),
			 this, SLOT(qstat_a_getStderr())); // same here
	m_qstat_a_Process->start(qstatCommand);

	if (!m_qstat_a_Process->waitForStarted())
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Couldn't start 'qstat -a' process", "'qstat -a' process unable to start.  Torque is probably not running.\n");
		return false;
	}

	if (!m_qstat_a_Process->waitForFinished(-1)) //If msecs is -1, this function will not time out
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Error trying to complete qstat process", "Process's waitForFinished() method returned error.\n");
		return false;
	}

	delete m_qstat_a_Process;
	m_qstat_a_Process = NULL;


	qstat_a_processStdout(); // parse the stdout data collected (above)

	// restore the original cursor
	QApplication::restoreOverrideCursor();


	if (m_showQStat_a_STDERROutput)
	{
		if (!m_qstat_a_Stderr.isEmpty())
		{
//			QMessageBox::critical(0, "Error issuing 'qstat -a' command",
//								  QString("Error issuing 'qstat -a' command.  Error message was: %1").arg(m_qstat_a_Stderr));
            ErrorMsgDlg dlg("TORQUEView Error", "Error issuing 'qstat -a' command. Error message:", m_qstat_a_Stderr);
            dlg.exec();
            if (dlg.isDontShowThisAgainChecked())  // see if the "Don't show this dialog again" checkbox was checked
                m_showQStat_a_STDERROutput = false;
            m_lastJobIdSelected = ""; // re-init
			return false;
		}
	}

	return true; // success
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueQstatCmd() - this gets called whenever the qstat process has something to say...
void QstatTab::qstat_a_getStdout()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	if (m_qstat_a_Process != NULL)
	{
		QByteArray s = m_qstat_a_Process->readAllStandardOutput(); // read normal output
		m_qstat_a_Stdout.append( s );  // if there's any stdout
	}
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueQstatCmd() - this gets called whenever the qstat process has something to say...
void QstatTab::qstat_a_getStderr()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	if (m_qstat_a_Process != NULL)
	{
		QByteArray s = m_qstat_a_Process->readAllStandardError(); // read error channel
		m_qstat_a_Stderr.append( s );  // if there's any stderr
	}
}


/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::qstat_a_processStdout() // parse the stdout data collected (above)
{
    m_pbsJobs.clear();

    // determine if user wants to show tree heirarchy based on the "Queue" field
//    bool bShowHeirarchy = false;
//    if (ui->checkBox_ShowJobsByQueue->isChecked())
//        bShowHeirarchy = true;

	bool bNodeNameSeen = false;

	QStringList lines = m_qstat_a_Stdout.split("\n");
	foreach (QString line, lines)
	{
		line = line.trimmed(); // strip off leading and trailing spaces
		if (line.isEmpty())
		{
			continue;
		}
		else if (line.startsWith("Req'd") ||
				 line.startsWith("Job ID") ||
				 line.startsWith("----"))
		{
			continue;
		}
		else if (!bNodeNameSeen) // first line in output is node name
		{
			ui->label_Title_NodeName->setText(line);
			bNodeNameSeen = true;
		}
		else
		{
			QStringList fields = line.split(" ", QString::SkipEmptyParts);  // split on 'blank' boundaries
			QString jobID			= fields[0];
			QString username		= fields[1];
			QString queue			= fields[2];
			QString jobname			= fields[3];
			QString sessID			= fields[4];
			QString NDS				= fields[5];
			QString TSK				= fields[6];
			QString requiredMemory	= fields[7];
			QString requiredTime	= fields[8];
			QString state			= fields[9];
			QString elapsedTime		= fields[10];
//			QString BIG				= fields[9];
//			QString FAST			= fields[10];
//			QString PFS				= fields[11];


			PbsJob* job = new PbsJob(jobID, username, queue, jobname, sessID, NDS, TSK, requiredMemory, requiredTime, state, elapsedTime/*, BIG, FAST, PFS*/);
			m_pbsJobs.append(job);
		}

	}  // foreach line

	updateList();


	// resize columns to contents
	int columnCount = ui->treeWidget_Jobs->header()->count();
	for(int i = 0; i < columnCount; i++)
	{
		  ui->treeWidget_Jobs->resizeColumnToContents(i);
	}
	// ...but make sure first col (jobID) is wide enough
	ui->treeWidget_Jobs->header()->resizeSection(0, 240);	// Job ID column
}

/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::populateQstat_f_FromFile( QString jobID )  // get "qstat -f" output from file and populate jobInfo tree
{
	ui->treeWidget_JobInfo->clear();	// clear out list

	QTreeWidgetItem * rootitem = NULL;

	bool bInVariableList = false;
	QString sVariableList;

	bool bInExecHost = false;
	QString sExecHost;

	bool bInExecPort = false;
	QString sExecPort;

	m_lastExecHost = ""; // clear
	m_lastExecPort = ""; // clear

	JobInfo* jobInfoItem = m_jobInfoMap.value(jobID);
	if (jobInfoItem == NULL)
		return;

	ui->label_Title_JobInfo->setText(jobID);


	QStringList lines = jobInfoItem->m_data;
    foreach (QString line, lines)
    {
		if (bInVariableList)  // make this check BEFORE trimming 'line' -- need to find out if it starts with a tab char
		{
			if (line.startsWith("\t"))
			{
				// still in Variable List section, so continue adding lines to sVariableList
				sVariableList += line.trimmed();
				continue;
			}
			else  // this is the first line that doesn't start with a TAB char
			{
				// finished with Variable List section, so process those lines
				bInVariableList = false;

				// first, split on comma-boundaries
				QStringList parts = sVariableList.split(",", QString::SkipEmptyParts);
				foreach (QString s, parts)
				{
					QTreeWidgetItem * item = new QTreeWidgetItem(rootitem); // (use last rootitem seen)

					QStringList pieces = s.split("=");  // second, split on "=" boundaries
					item->setText(0,pieces[0]);
				//	item->setIcon(0, QIcon(":/icons/images/icon_green.bmp"));
					if (pieces.count() > 1)
					{
						item->setText(1, pieces[1]);
						item->setToolTip( 1, pieces[1] );
					}
				}
				// DON'T continue here!  -- keep on going with code (below) -- need to process line just found
			}
		}
		else if (bInExecHost)  // make this check BEFORE trimming 'line' -- need to find out if it starts with tab char
		{
			if (line.startsWith("\t"))
			{
				// still in exec_host section, so continue adding lines to sExecHost
				sExecHost += line.trimmed();
				continue;
			}
			else  // this is the first line that doesn't start with a TAB char
			{
				// finished with exec_host section, so process those lines
				bInExecHost = false;
				// first, split on "+" boundaries
				QStringList parts = sExecHost.split("+", QString::SkipEmptyParts);
				foreach (QString s, parts)
				{
					QTreeWidgetItem * item = new QTreeWidgetItem(rootitem); // (use last rootitem seen)
					item->setText(0, s);
				//	item->setToolTip( 0, s );
				}

				m_lastExecHost = sExecHost;
			}
		}
		else if (bInExecPort)  // make this check BEFORE trimming 'line' -- need to find out if it starts with tab char
		{
			if (line.startsWith("\t"))
			{
				// still in exec_port section, so continue adding lines to sExecPort
				sExecPort += line.trimmed();
				continue;
			}
			else  // this is the first line that doesn't start with a TAB char
			{
				// finished with exec_port section, so process those lines
				bInExecPort = false;

				// first, split on "+" boundaries
				QStringList parts = sExecPort.split("+", QString::SkipEmptyParts);
				foreach (QString s, parts)
				{
					QTreeWidgetItem * item = new QTreeWidgetItem(rootitem); // (use last rootitem seen)
					item->setText(0, s);
				//	item->setToolTip( 0, s );
				}

				m_lastExecPort = sExecPort;
			}
		}

//      if (line.isEmpty())
//          continue;

		line = line.trimmed();	// strip off white space at beginning and end
		QStringList fields = line.split("=");
		QString sLabel = fields[0];
		QString sValue;
		if (fields.size() > 1)
		{
			fields.removeAt(0);	// remove the first item in the stringlist (the label)
			sValue = fields.join("=");	// concat all the rest --put the equals back in
		}
		sLabel = sLabel.trimmed();
		sValue = sValue.trimmed();

		rootitem = new QTreeWidgetItem(ui->treeWidget_JobInfo); // put it under rootitem
		rootitem->setText(0, sLabel);
	//	rootitem->setIcon(0, QIcon(":/icons/images/icon_green.bmp"));
	//	rootitem->setExpanded(true);

		// special case 1:  if it's the Variable_List, parse out each part
		// and add as subitems
		if (sLabel.startsWith("Variable_List"))
		{
			bInVariableList = true;
			// in Variable List section, so start adding lines to sVariableList
			sVariableList = sValue; // add the value part of the name/value pair

			// (value will be added to new rootitem just created in code (see 'if (bInVariableList)' code above)
			continue;
		}
		// special case 2:  if it's the exec_host, save off list
		else if (sLabel.startsWith("exec_host"))
		{
			bInExecHost = true;
			// in exec_host section, so start adding lines to sExecHost
			sExecHost = sValue; // add the value part of the name/value pair

			// (value will be added to new rootitem just created in code (see 'if (bInExecHost)' code above)
			continue;
		}
		// special case 3:  if it's the exec_port, proocess list
		else if (sLabel.startsWith("exec_port"))
		{
			bInExecPort = true;
			// in exec_port section, so start adding lines to sExecPort
			sExecPort = sValue; // add the value part of the name/value pair

			// (value will be added to new rootitem just created in code (see 'if (bInExecPort)' code above)
			continue;
		}


		rootitem->setText(1, sValue);
		rootitem->setToolTip( 1, sValue );
    }
}


/*******************************************************************************
 *
*******************************************************************************/
bool QstatTab::issueCmd_Qstat_f( QString jobID )
{
//	ui->treeWidget_JobInfo->clear();	// clear out list

	m_qstat_f_Stdout.clear();
	m_qstat_f_Stderr.clear();

	QString qstat_f_Command;
	if (m_mainWindow->m_Config_Call_Qstat_with_T_Flag)  // the "qstat -t" flag is for supporting job arrays
	{
		// do a "qstat -f' as the default display here, using the "-t" flag (supports job arrays)
		qstat_f_Command = qstat_f_Command = QString (m_mainWindow->m_Config_Cmd_Qstat_f_with_T).arg(jobID);
	}
	else
	{
		// do a "qstat -f' as the default display here, but without the "-t" flag
		qstat_f_Command = qstat_f_Command = QString (m_mainWindow->m_Config_Cmd_Qstat_f_without_T).arg(jobID);
	}


	// show "Wait cursor"
	QApplication::setOverrideCursor(Qt::WaitCursor);


	m_qstat_f_Process = new QProcess(this);
	connect (m_qstat_f_Process, SIGNAL(readyReadStandardOutput()),
			 this, SLOT(qstat_f_getStdout())); // connect process signals with your code
	connect (m_qstat_f_Process, SIGNAL(readyReadStandardError()),
			 this, SLOT(qstat_f_getStderr())); // same here
	m_qstat_f_Process->start(qstat_f_Command);

	if (!m_qstat_f_Process->waitForStarted())
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Couldn't start 'qstat -f' process", "'qstat -f' process unable to start.  Torque is probably not running.\n");
		return false;
	}

	if (!m_qstat_f_Process->waitForFinished(-1)) //If msecs is -1, this function will not time out
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Error trying to complete qstat -f process", "Process's waitForFinished() method returned error.\n");
		return false;
	}

	delete m_qstat_f_Process;
	m_qstat_f_Process = NULL;


	qstat_f_processStdout(); // parse the stdout data collected (above)

	// restore the original cursor
	QApplication::restoreOverrideCursor();

	if (!m_qstat_f_Stderr.isEmpty())
	{
		// this error happens usually when job is not longer found (is finished running)--
		// not really an error, but we need to clear out lastJobId so it doesn't try to select a job in the job list (on auto-refresh)
        if (m_showQStat_f_STDERROutput)
		{
//			QMessageBox::critical(0, QString("Error issuing '%1' command").arg(qstat_f_Command),
//								  QString("Error issuing '%1' command.  Error message was: %2").arg(m_qstat_f_Stderr).arg(m_qstat_f_Stderr));
			ErrorMsgDlg dlg("TORQUEView Error", "Error issuing 'qstat -f' command. Error message:",
							QString("%1\nReason: Job \"%2\"  no longer exists.").arg(m_qstat_f_Stderr).arg(jobID),
							false, true);  // don't show checkbox, show only OK button
            dlg.exec();
            if (dlg.isDontShowThisAgainChecked())  // see if the "Don't show this dialog again" checkbox was checked
                m_showQStat_f_STDERROutput = false;
        }
        m_lastJobIdSelected = ""; // re-init
		return false;
	}
	return true; // success
}

/*******************************************************************************
 *
*******************************************************************************/
// called from getQstat_f_FromCmd() - this gets called whenever the qstat process has something to say...
void QstatTab::qstat_f_getStdout()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	if (m_qstat_f_Process != NULL)
	{
		QByteArray s = m_qstat_f_Process->readAllStandardOutput(); // read normal output
		m_qstat_f_Stdout.append( s );  // if there's any stdout
	}
}

/*******************************************************************************
 *
*******************************************************************************/
// called from getQstat_f_FromCmd() - this gets called whenever the qstat process has something to say...
void QstatTab::qstat_f_getStderr()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	if (m_qstat_f_Process != NULL)
	{
		QByteArray s = m_qstat_f_Process->readAllStandardError(); // read error channel
		m_qstat_f_Stderr.append( s );  // if there's any stderr
	}
}


/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::qstat_f_processStdout() // parse the stdout data collected (above)
{
	ui->treeWidget_JobInfo->clear();	// clear out list


	bool bFirstLine = true;

	bool bInVariableList = false;
	QString sVariableList;

	bool bInExecHost = false;
	QString sExecHost;

	bool bInExecPort = false;
	QString sExecPort;

	QTreeWidgetItem * rootitem = NULL;
	m_lastExecHost = ""; // clear
	m_lastExecPort = ""; // clear

	QStringList lines = m_qstat_f_Stdout.split("\n");
	foreach (QString line, lines)
	{
		// if we're in the middle of taking a snapshot, then save out each line to m_momctlrs list
		if (m_mainWindow->takingSnapshot())
			m_jobInfo_Lines.append(line);


		if (bInVariableList)  // make this check BEFORE trimming 'line' -- need to find out if it starts with a tab char
		{
			if (line.startsWith("\t"))
			{
				// still in Variable List section, so continue adding lines to sVariableList
				sVariableList += line.trimmed();
				continue;
			}
			else  // this is the first line that doesn't start with a TAB char
			{
				// finished with Variable List section, so process those lines
				bInVariableList = false;

				// first, split on comma-boundaries
				QStringList parts = sVariableList.split(",", QString::SkipEmptyParts);
				foreach (QString s, parts)
				{
					QTreeWidgetItem * item = new QTreeWidgetItem(rootitem); // (use last rootitem seen)

					QStringList pieces = s.split("=");  // second, split on "=" boundaries
					item->setText(0,pieces[0]);
				//	item->setIcon(0, QIcon(":/icons/images/icon_green.bmp"));
					if (pieces.count() > 1)
					{
						item->setText(1, pieces[1]);
						item->setToolTip( 1, pieces[1] );
					}
				}
				// DON'T continue here!  -- keep on going with code (below) -- need to process line just found
			}
		}
		else if (bInExecHost)  // make this check BEFORE trimming 'line' -- need to find out if it starts with 8 space chars
		{
			if (line.startsWith("\t") )
			{
				// still in Host List section, so continue adding lines to sExecHost
				sExecHost += line.trimmed();
				continue;
			}
			else  // this is the first line that doesn't start with a TAB char
			{
				// finished with exec_host section, so process those lines
				bInExecHost = false;

			//	rootitem->setIcon(0, QIcon(":/icons/images/icon_green.bmp"));
				rootitem->setText(1, sExecHost);
				rootitem->setToolTip( 1, m_mainWindow->formatForTooltip(sExecHost, "+") );

				m_lastExecHost = sExecHost;
			}
		}
		else if (bInExecPort)  // make this check BEFORE trimming 'line' -- need to find out if it starts with 8 space chars
		{
			if (line.startsWith("\t"))
			{
				// still in exec_port section, so continue adding lines to sExecPort
				sExecPort += line.trimmed();
				continue;
			}
			else  // this is the first line that doesn't start with a TAB char
			{
				// finished with Host List section, so process those lines
				bInExecPort = false;

			//	rootitem->setIcon(0, QIcon(":/icons/images/icon_green.bmp"));
				rootitem->setText(1, sExecPort);
				rootitem->setToolTip( 1, m_mainWindow->formatForTooltip(sExecPort, "+") );

				m_lastExecPort = sExecPort;
			}
		}



		line = line.trimmed(); // strip off leading and trailing spaces
		if (line.isEmpty())
			continue;

		if (bFirstLine)
		{
			ui->label_Title_JobInfo->setText(line);
			bFirstLine = false;
			continue;
		}

        QStringList fields = line.split("=");
        QString sLabel = fields[0];
        QString sValue;
        if (fields.size() > 1)
        {
            fields.removeAt(0);	// remove the first item in the stringlist (the label)
            sValue = fields.join("=");	// concat all the rest --put the "=" back in (REASON: some of the fields have multiple "=" signs)
        }
        sLabel = sLabel.trimmed();
        sValue = sValue.trimmed();

		rootitem = new QTreeWidgetItem(ui->treeWidget_JobInfo);
        rootitem->setText(0, sLabel);
	//  rootitem->setIcon(0, QIcon(":/icons/images/icon_green.bmp"));
     // rootitem->setExpanded(true);

        // special case 1:  if it's the Variable_List, parse out each part
        // and add as subitems
        if (sLabel.startsWith("Variable_List"))
        {
            bInVariableList = true;
			// in Variable List section, so start adding lines to sVariableList
			sVariableList = sValue; // add the value part of the name/value pair

			// (value will be added to new rootitem just created in code (see 'if (bInVariableList)' code above)
            continue;
        }

		// special case 2:  if it's the exec_host, save off list
		if (sLabel.startsWith("exec_host"))
		{
			bInExecHost = true;
			// in host List section, so start adding lines to sExecHost
			sExecHost = sValue; // add the value part of the name/value pair

			// (value will be added to new rootitem just created in code (see 'if (bInExecHost)' code above)
			continue;
		}

		// special case 3:  if it's the exec_port, process it
		if (sLabel.startsWith("exec_port"))
		{
			bInExecPort = true;
			// in host port section, so start adding lines to sExecPort
			sExecPort = sValue; // add the value part of the name/value pair

			// (value will be added to new rootitem just created in code (see 'if (bInExecPort)' code above)
			continue;
		}

        rootitem->setText(1, sValue);
        rootitem->setToolTip( 1, sValue );
    }  // foreach line


//	// resize columns to contents
//	int columnCount = ui->treeWidget_JobInfo->header()->count();
//	for(int i = 0; i < columnCount; i++)
//	{
//		  ui->treeWidget_JobInfo->resizeColumnToContents(i);
//	}
}


/*******************************************************************************
 *
*******************************************************************************/
bool QstatTab::selectLastJobId()
{
	ui->treeWidget_Jobs->clearSelection();  // important -- clear any previous selection
	if (! m_lastJobIdSelected.isEmpty())
	{
		// iterate through the whole tree and see which item was previously
		// selected -- then select it (Note: can't use the findItems() method as
		// we do above, since we don't know what level the item might be at -- rootitem or childitem)
		for ( QTreeWidgetItemIterator it( ui->treeWidget_Jobs ); (*it) != NULL; ++it )
		{
			QString lastJobId = (*it)->text(0);
			// NOTE From Ken:  qstat truncates the job name. You cannot use the truncated name to look up a job id.
			// You must use the job id number itself or you will need the full job id name.
			// See on_treeWidget_Jobs_itemSelectionChanged() below.)

			// Example: user gets this back from qstat: 420689.cottos.rq xuxiaofe courte dis_fluent.pbs 31251 2 -- -- 20:00 R 11:14
			// but the real jobID is:  420689.cottos.rqchp.qc.ca
			// So I need to truncate jobID returned from qstat ("420689.cottos.rq") and grab just the numeric part
			// before the first dot ("420689").
			QStringList sl = lastJobId.split(".");
			QString jobIDNumber = sl[0];
			if (m_lastJobIdSelected == jobIDNumber)
			{
				(*it)->setSelected( true );
				return true;
			}
		}
	}
	return true;
}

/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::writeDataToFile(QTextStream& out)
{
	// WRITE OUT QSTAT DATA TO FILE:

	// if list is empty, bail out
	if (ui->treeWidget_Jobs->topLevelItemCount() == 0)
		return;

	m_jobInfo_Lines.clear();  // clear out momctl list

	int dataSource = m_mainWindow->getComboBox_DataSource_CurrentIndex();


	out << "\n";
	out << "\n";
	out << "\n";
	out << "#---------------------------------------------------------------" << "\n";
	out << "TORQUEView -- Qstat -a Output" << "\n";
	out << "#---------------------------------------------------------------" << "\n";
	out << "\n";
	out << "\n";

	QString sNodeName = ui->label_Title_NodeName->text();
	out << sNodeName << "\n";

	// write out header lines
	out << "                                                          Req'd  Req'd       Elap\n";
	out << "Job ID                  Username    Queue    NDS   TSK    Memory Time      S Time       BIG  FAST   PFS\n";
	out << "----   ---------------- ----------- -------- ----- ------ ------ --------- - --------- ----- ----- -----\n";


	// iterate through the job tree and write out each job item
	// (there are only top-level items so far, no child items, so this iterator should work OK)
	QTreeWidgetItemIterator it( ui->treeWidget_Jobs );
	while (*it)
	{
		int columnCount = ui->treeWidget_Jobs->columnCount();
		QStringList sl;
		for (int i=0; i < columnCount; i++)
		{
			sl.append( (*it)->text(i) );
		}
		QString line = sl.join("   ");
		out << line << "\n";

		++it;
	}

	if (dataSource == 0 || dataSource == 1)  // if data is coming from either Local host or Remote host
	{
		if (m_mainWindow->m_Snapshot_SaveIndividualJobData)  // only do this if user wants to include qstat -f data in snapshot file
		{
			issueCmd_Qstat_f(); // issue a single "qstat -f" command -- will get all qstat -f data
			// NOTE: since m_bTakingSnapshot is true, will add it to m_jobInfo_Lines
		}
	}


	if (dataSource == 0 || dataSource == 1)  // if data is coming from either Local host or Remote host
	{
		// finally, if there are "qstat -f" lines, add each line to end of file
		int lineCount = m_jobInfo_Lines.count();
		if (lineCount > 0)
		{
			out << "\n";
			out << "\n";
			out << "\n";
			out << "#---------------------------------------------------------------" << "\n";
			out << "TORQUEView -- Qstat -f Output" << "\n";
			out << "#---------------------------------------------------------------" << "\n";
			out << "\n";
			out << "\n";

			foreach (QString s, m_jobInfo_Lines)
			{
				out << s << "\n";
			}
		}
	}
	else // data is coming from a snapshot file
	{
		if (m_jobInfoMap.size() > 0)
		{
			out << "\n";
			out << "\n";
			out << "\n";
			out << "#---------------------------------------------------------------" << "\n";
			out << "TORQUEView -- Qstat -f Output" << "\n";
			out << "#---------------------------------------------------------------" << "\n";

			// iterate through the m_jobInfoMap and write out each item
			QMapIterator<QString, JobInfo*> i(m_jobInfoMap);
			while (i.hasNext())
			{
				i.next();
				QString sNodeName = i.key();
				JobInfo* jobInfoItem = i.value();

				// first, add a NodeName line to the stringlist
				QString s = QString("\n\nJob Id:  %1\n\n").arg(sNodeName);
				out << s;

				QStringList sl = jobInfoItem->m_data;
				foreach (QString line, sl)
				{
					out << line << endl;
				}

			   // cout << i.key() << ": " << i.value() << endl;
			}
		}
	}

}



/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::on_treeWidget_Jobs_itemSelectionChanged ()
{
	// called when user clicks on an item in the Jobs tree
	QList<QTreeWidgetItem*> selectedItems = ui->treeWidget_Jobs->selectedItems();
	if ( selectedItems.size() > 0)
	{
		ui->treeWidget_JobInfo->clear();  // clear out the qstat -f list
		ui->label_Title_JobInfo->setText("");  // clear out the qstat -f list's title field
		QTreeWidgetItem* item = selectedItems[0];	// list is single-select

		// get the "is this a jobID?" flag from item's data -- 0==is NOT a jobID, 1==is a jobID
		QVariant qv = item->data(0, Qt::UserRole);
		int iFlag = qv.value<int>();
		if (iFlag == 0)  // if not a jobID, just clear the list on the right and bail out
			return;

		QString jobID = item->text(0);  // get text from column 0 (jobID)
		// NOTE From Ken:  qstat truncates the job name. You cannot use the truncated name to look up a job id.
		// You must use the job id number itself or you will need the full job id name.

		// Example: user gets this back from qstat: 420689.cottos.rq xuxiaofe courte dis_fluent.pbs 31251 2 -- -- 20:00 R 11:14
		// but the real jobID is:  420689.cottos.rqchp.qc.ca
		// So I need to truncate jobID returned from qstat ("420689.cottos.rq") and grab just the numeric part
		// before the first dot ("420689").
		QStringList sl = jobID.split(".");
		QString jobIDNumber = sl[0];

		int dataSource = m_mainWindow->getComboBox_DataSource_CurrentIndex();
		if (dataSource == 0 || dataSource == 1)  // if data is coming from either Local host or Remote host
		{
			issueCmd_Qstat_f(jobIDNumber);
		}
		else // else data is coming from snapshot file
		{
			// get the "qstat -f" output stringlist from the hash
			populateQstat_f_FromFile(jobIDNumber);
		}

		m_lastJobIdSelected = jobIDNumber;  // save off jobID
	}
}

/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::getExpandedState()   // remember which Job list items were expanded (so we can restore those states below) -- needed for auto-refresh
{
	m_expandedMap.clear();

	if (ui->checkBox_ShowJobsByQueue->isChecked()) // only applies if "ShowJobsByQueue" checkbox is checked
	{
		// Iterate through treewidget, getting all top-level items, and see which ones are expanded.
		// Add them all to the m_expandedMap.
		int rootItemCount = ui->treeWidget_Jobs->topLevelItemCount();
		for (int i=0; i<rootItemCount; i++)
		{
			QTreeWidgetItem* rootItem = ui->treeWidget_Jobs->topLevelItem(i);
			QString sQueue = rootItem->text(0);
			bool bIsExpanded = rootItem->isExpanded();
			m_expandedMap[sQueue] = bIsExpanded;
		}
	}
}

/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::restoreExpandedState() // expand any root-level items in Jobs list that previously were expanded
{
//	int count = m_expandedMap.count();
	// iterate through the m_expandedItemMap and expand all items that previously were expanded
	QMap<QString, bool>::iterator i;
	for (i = m_expandedMap.begin(); i != m_expandedMap.end(); ++i)
	{
		QString sQueue = i.key();
		bool bIsExpanded = i.value();
		// list is single-select, so should only be one item in itemList
		QList<QTreeWidgetItem *> itemList = ui->treeWidget_Jobs->findItems ( sQueue, Qt::MatchExactly | Qt::MatchWrap );
		if (itemList.count() > 0)
		{
			itemList[0]->setExpanded(bIsExpanded); // expand the item (or not)
		}
	}
}

/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::on_btnShowNodesRunningThisJob_clicked()
{
	if (m_lastJobIdSelected.isEmpty())
	{
		QMessageBox::information(0, "No Job ID specified", "No job selected in job list.  Couldn't display nodes running this job.\n");
		return;
	}
	if (m_lastExecHost.isEmpty())
	{
		QMessageBox::information(0, "No 'exec_host' field found", "'exec_host' field was not found in 'qstat_f'' output.  No nodes are currently running this job.\n");
		return;
	}

	m_mainWindow->switchToPbsNodesTab();	// switch to PbsNodes tab
	m_mainWindow->showNodesRunningJob( m_lastJobIdSelected );
}

/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::on_btnExecHost_clicked()
{
	if (m_lastExecHost.isEmpty())
	{
		QMessageBox::information(0, "No 'exec_host' field found", "'exec_host' field was not found in 'qstat_f'' output.\n");
		return;
	}

	ExecHostDlg dlg( m_lastExecHost, m_lastExecPort );
	if (!dlg.exec())
		return;

}

/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::getJobInfo(QString jobState, QString& detailedJobState, QIcon& icon, QColor& backgroundcolor)
{
	// get node state
	if (jobState == "C")   // Job is completed after having run
	{
//		icon = QIcon(":/icons/images/icon_green.bmp");
//		backgroundcolor = QColor("lightgreen");
		icon = QIcon(":/icons/images/icon_gray.bmp");
		backgroundcolor = QColor("lightgray");
		detailedJobState = "Job is completed";
	}
	else if (jobState == "E")	// Job is exiting after having run
	{
		icon = QIcon(":/icons/images/icon_purple.bmp");
		backgroundcolor = QColor(Qt::magenta).lighter(140);
		detailedJobState = "Job is exiting";
	}
	else if (jobState == "H")	// Job is on hold
	{
//		icon = QIcon(":/icons/images/icon_red.bmp");
//		backgroundcolor = QColor("red");
		icon = QIcon(":/icons/images/icon_yellow.bmp");
		backgroundcolor = QColor("lightyellow");
		detailedJobState = "Job is on hold";
	}
	else if (jobState == "Q")	// Job is queued, eligible to run or routed
	{
		icon = QIcon(":/icons/images/icon_blue.bmp");
		backgroundcolor = QColor("cyan");
		detailedJobState = "Job is queued";
	}
	else if (jobState == "R")	// Job is running
	{
//		icon = QIcon(":/icons/images/icon_orange.bmp");
//		backgroundcolor = QColor("orange");
		icon = QIcon(":/icons/images/icon_green.bmp");
		backgroundcolor = QColor("green").lighter(190);
		detailedJobState = "Job is running";
	}
	else if (jobState == "T")	// Job is being moved to new location
	{
		icon = QIcon(":/icons/images/icon_white.bmp");
	//	backgroundcolor = QColor("white");
		detailedJobState = "Job is being moved to new location";
	}
	else if (jobState == "W")	// Job is waiting for its execution time (-a option) to be reached
	{
		icon = QIcon(":/icons/images/icon_white.bmp");
	//	backgroundcolor = QColor("white");
		detailedJobState = "Job is waiting for its execution time (-a option) to be reached";
	}
	else if (jobState == "S")	// (Unicos only) Job is suspended
	{
//		icon = QIcon(":/icons/images/icon_gray.bmp");
//		backgroundcolor = QColor("gray");
		icon = QIcon(":/icons/images/icon_orange.bmp");
		backgroundcolor = QColor("orange");
		detailedJobState = "Job is suspended";
	}
	else
	{
		icon = QIcon(":/icons/images/icon_red.bmp");
		backgroundcolor = QColor("red");
		detailedJobState = "Job state is unknown";
	}
}

/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::on_actionRun_job_triggered()
{
    resetErrorMsgDlg();  // clear all "don't show this message again" flags

    QList<QTreeWidgetItem*> selectedItems = ui->treeWidget_Jobs->selectedItems();
    foreach (QTreeWidgetItem* item, selectedItems)	// list is extended-select
    {
        QString jobID = item->text(0);  // get text from column 0 (jobID)
        issueCmd_RunJob( jobID );
    }

    issueCmd_Qstat_a();	// refresh qstat list

}

/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::on_actionDelete_job_triggered()
{
    resetErrorMsgDlg();  // clear all "don't show this message again" flags

    QList<QTreeWidgetItem*> selectedItems = ui->treeWidget_Jobs->selectedItems();
    foreach (QTreeWidgetItem* item, selectedItems)	// list is extended-select
    {
        QString jobID = item->text(0);  // get text from column 0 (jobID)
        issueCmd_DeleteJob( jobID );
    }

    issueCmd_Qstat_a();	// refresh qstat list
}

/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::on_actionPut_job_on_hold_triggered()
{
    resetErrorMsgDlg();  // clear all "don't show this message again" flags

    QList<QTreeWidgetItem*> selectedItems = ui->treeWidget_Jobs->selectedItems();
    foreach (QTreeWidgetItem* item, selectedItems)	// list is extended-select
    {
        QString jobID = item->text(0);  // get text from column 0 (jobID)
        issueCmd_HoldJob( jobID );
    }

    issueCmd_Qstat_a();	// refresh qstat list
}

/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::on_actionRelease_hold_on_job_triggered()
{
    resetErrorMsgDlg();  // clear all "don't show this message again" flags

    QList<QTreeWidgetItem*> selectedItems = ui->treeWidget_Jobs->selectedItems();
    foreach (QTreeWidgetItem* item, selectedItems)	// list is extended-select
    {
        QString jobID = item->text(0);  // get text from column 0 (jobID)
        issueCmd_ReleaseHoldOnJob( jobID );
    }

    issueCmd_Qstat_a();	// refresh qstat list
}

/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::on_actionRerun_job_triggered()
{
    resetErrorMsgDlg();  // clear all "don't show this message again" flags

    QList<QTreeWidgetItem*> selectedItems = ui->treeWidget_Jobs->selectedItems();
    foreach (QTreeWidgetItem* item, selectedItems)	// list is extended-select
    {
        QString jobID = item->text(0);  // get text from column 0 (jobID)
        issueCmd_RerunJob( jobID );
    }

    issueCmd_Qstat_a();	// refresh qstat list

}



/*******************************************************************************
 *
*******************************************************************************/
bool QstatTab::issueCmd_RunJob( QString jobID)
{
	m_mainWindow->setIsCurrentlyExecutingCmd(true);  // indicate "TORQUEView IS currently executing a TORQUE client command"

	m_runJob_Stdout.clear();
	m_runJob_Stderr.clear();


	QString runJobCommand = QString(m_mainWindow->m_Config_Cmd_RunJob)
			.arg(jobID);

	m_mainWindow->statusBar()->showMessage(QString("%1").arg(runJobCommand), 10000);
	// show "Wait cursor"
	QApplication::setOverrideCursor(Qt::WaitCursor);


	m_runJobProcess = new QProcess(this);
	connect (m_runJobProcess, SIGNAL(readyReadStandardOutput()),
			 this, SLOT(runJob_getStdout())); // connect process signals with my code
	connect (m_runJobProcess, SIGNAL(readyReadStandardError()),
			 this, SLOT(runJob_getStderr())); // same here
	m_runJobProcess->start( runJobCommand );

	if (!m_runJobProcess->waitForStarted())
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Couldn't run job", "'qrun' process unable to start.  Torque is probably not running.\n");
		m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
		return false;
	}

	if (!m_runJobProcess->waitForFinished(-1)) //If msecs is -1, this function will not time out
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Error trying to run job", "Process's waitForFinished() method returned error.\n");
		m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
		return false;
	}

	delete m_runJobProcess;
	m_runJobProcess = NULL;

	runJob_processStdout(); // parse the stdout data collected (above)
	// restore the original cursor
	QApplication::restoreOverrideCursor();


    if (m_showRunJobSTDERROutput)
	{
		// see if there's any errors to display
		if (!m_runJob_Stderr.isEmpty())
		{
//			QMessageBox::critical(0, "Error issuing 'run job' command",
//								  QString("Error issuing 'run job' command.  Error message was: %1").arg(m_runJob_Stderr));
            ErrorMsgDlg dlg("TORQUEView Error", "Error issuing 'Run Job' command. Error message:", m_runJob_Stderr);
            dlg.exec();
            if (dlg.isDontShowThisAgainChecked())  // see if the "Don't show this dialog again" checkbox was checked
                m_showRunJobSTDERROutput = false;
            m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"

            return false;
		}
	}

	m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
	return true; // success

}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_RunJob() - this gets called whenever the RunJob process has something to say...
void QstatTab::runJob_getStdout()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	if (m_runJobProcess != NULL)
	{
		QByteArray s = m_runJobProcess->readAllStandardOutput(); // read normal output
		m_runJob_Stdout.append( s );  // if there's any stdout
	}
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_RunJob() - this gets called whenever the RunJob process has something to say...
void QstatTab::runJob_getStderr()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	if (m_runJobProcess != NULL)
	{
		QByteArray s = m_runJobProcess->readAllStandardError(); // read error channel
		m_runJob_Stderr.append( s );  // if there's any stderr
	}
}


/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::runJob_processStdout() // parse the stdout data collected (above)
{
//  QMessageBox::information(0, "Run job", m_runJob_Stdout);
}



/*******************************************************************************
 *
*******************************************************************************/
bool QstatTab::issueCmd_DeleteJob( QString jobID)
{
	m_mainWindow->setIsCurrentlyExecutingCmd(true);  // indicate "TORQUEView IS currently executing a TORQUE client command"

	m_deleteJob_Stdout.clear();
	m_deleteJob_Stderr.clear();


	QString deleteJobCommand = QString(m_mainWindow->m_Config_Cmd_DeleteJob)
			.arg(jobID);

	m_mainWindow->statusBar()->showMessage(QString("%1").arg(deleteJobCommand), 10000);
	// show "Wait cursor"
	QApplication::setOverrideCursor(Qt::WaitCursor);


	m_deleteJobProcess = new QProcess(this);
	connect (m_deleteJobProcess, SIGNAL(readyReadStandardOutput()),
			 this, SLOT(deleteJob_getStdout())); // connect process signals with my code
	connect (m_deleteJobProcess, SIGNAL(readyReadStandardError()),
			 this, SLOT(deleteJob_getStderr())); // same here
	m_deleteJobProcess->start( deleteJobCommand );

	if (!m_deleteJobProcess->waitForStarted())
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Couldn't delete job", "'qdel' process unable to start.  Torque is probably not running.\n");
		m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
		return false;
	}

	if (!m_deleteJobProcess->waitForFinished(-1)) //If msecs is -1, this function will not time out
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Error trying to delete job", "Process's waitForFinished() method returned error.\n");
		m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
		return false;
	}

	delete m_deleteJobProcess;
	m_deleteJobProcess = NULL;

	deleteJob_processStdout(); // parse the stdout data collected (above)
	// restore the original cursor
	QApplication::restoreOverrideCursor();


    if (m_showDeleteJobSTDERROutput)
	{
		// see if there's any errors to display
		if (!m_deleteJob_Stderr.isEmpty())
		{
//			QMessageBox::critical(0, "Error issuing 'delete job' command",
//								  QString("Error issuing 'delete job' command.  Error message was: %1").arg(m_deleteJob_Stderr));
            ErrorMsgDlg dlg("TORQUEView Error", "Error issuing 'Delete Job' command. Error message:", m_deleteJob_Stderr);
            dlg.exec();
            if (dlg.isDontShowThisAgainChecked())  // see if the "Don't show this dialog again" checkbox was checked
                m_showDeleteJobSTDERROutput = false;
            m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
            return false;
		}
	}

	m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
	return true; // success

}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_DeleteJob() - this gets called whenever the DeleteJob process has something to say...
void QstatTab::deleteJob_getStdout()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	if (m_deleteJobProcess != NULL)
	{
		QByteArray s = m_deleteJobProcess->readAllStandardOutput(); // read normal output
		m_deleteJob_Stdout.append( s );  // if there's any stdout
	}
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_DeleteJob() - this gets called whenever the DeleteJob process has something to say...
void QstatTab::deleteJob_getStderr()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	if (m_deleteJobProcess != NULL)
	{
		QByteArray s = m_deleteJobProcess->readAllStandardError(); // read error channel
		m_deleteJob_Stderr.append( s );  // if there's any stderr
	}
}


/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::deleteJob_processStdout() // parse the stdout data collected (above)
{
//  QMessageBox::information(0, "Delete job", m_deleteJob_Stdout);
}



/*******************************************************************************
 *
*******************************************************************************/
bool QstatTab::issueCmd_HoldJob( QString jobID)
{
	m_mainWindow->setIsCurrentlyExecutingCmd(true);  // indicate "TORQUEView IS currently executing a TORQUE client command"

	m_holdJob_Stdout.clear();
	m_holdJob_Stderr.clear();


	QString holdJobCommand = QString(m_mainWindow->m_Config_Cmd_PutJobOnHold)
			.arg(jobID);

	m_mainWindow->statusBar()->showMessage(QString("%1").arg(holdJobCommand), 10000);
	// show "Wait cursor"
	QApplication::setOverrideCursor(Qt::WaitCursor);


	m_holdJobProcess = new QProcess(this);
	connect (m_holdJobProcess, SIGNAL(readyReadStandardOutput()),
			 this, SLOT(holdJob_getStdout())); // connect process signals with my code
	connect (m_holdJobProcess, SIGNAL(readyReadStandardError()),
			 this, SLOT(holdJob_getStderr())); // same here
	m_holdJobProcess->start( holdJobCommand );

	if (!m_holdJobProcess->waitForStarted())
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Couldn't put job on hold", "'qhold' process unable to start.  Torque is probably not running.\n");
		m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
		return false;
	}

	if (!m_holdJobProcess->waitForFinished(-1)) //If msecs is -1, this function will not time out
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Error trying to put job on hold", "Process's waitForFinished() method returned error.\n");
		m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
		return false;
	}

	delete m_holdJobProcess;
	m_holdJobProcess = NULL;

	holdJob_processStdout(); // parse the stdout data collected (above)
	// restore the original cursor
	QApplication::restoreOverrideCursor();


    if (m_showHoldJobSTDERROutput)
	{
		// see if there's any errors to display
		if (!m_holdJob_Stderr.isEmpty())
		{
//			QMessageBox::critical(0, "Error issuing 'put job on hold' command",
//								  QString("Error issuing 'put job on hold' command.  Error message was: %1").arg(m_holdJob_Stderr));
            ErrorMsgDlg dlg("TORQUEView Error", "Error issuing 'Put Job On Hold' command. Error message:", m_holdJob_Stderr);
            dlg.exec();
            if (dlg.isDontShowThisAgainChecked())  // see if the "Don't show this dialog again" checkbox was checked
                m_showHoldJobSTDERROutput = false;
            m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
            return false;
		}
	}

	m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
	return true; // success

}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_HoldJob() - this gets called whenever the HoldJob process has something to say...
void QstatTab::holdJob_getStdout()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	if (m_holdJobProcess != NULL)
	{
		QByteArray s = m_holdJobProcess->readAllStandardOutput(); // read normal output
		m_holdJob_Stdout.append( s );  // if there's any stdout
	}
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_HoldJob() - this gets called whenever the holdJob process has something to say...
void QstatTab::holdJob_getStderr()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	if (m_holdJobProcess != NULL)
	{
		QByteArray s = m_holdJobProcess->readAllStandardError(); // read error channel
		m_holdJob_Stderr.append( s );  // if there's any stderr
	}
}


/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::holdJob_processStdout() // parse the stdout data collected (above)
{
//  QMessageBox::information(0, "Run job", m_holdJob_Stdout);
}


/*******************************************************************************
 *
*******************************************************************************/
bool QstatTab::issueCmd_ReleaseHoldOnJob( QString jobID)
{
	m_mainWindow->setIsCurrentlyExecutingCmd(true);  // indicate "TORQUEView IS currently executing a TORQUE client command"

	m_releaseHoldOnJob_Stdout.clear();
	m_releaseHoldOnJob_Stderr.clear();


	QString releaseHoldOnJobCommand = QString(m_mainWindow->m_Config_Cmd_ReleaseHoldOnJob)
			.arg(jobID);

	m_mainWindow->statusBar()->showMessage(QString("%1").arg(releaseHoldOnJobCommand), 10000);
	// show "Wait cursor"
	QApplication::setOverrideCursor(Qt::WaitCursor);


	m_releaseHoldOnJobProcess = new QProcess(this);
	connect (m_releaseHoldOnJobProcess, SIGNAL(readyReadStandardOutput()),
			 this, SLOT(releaseHoldOnJob_getStdout())); // connect process signals with my code
	connect (m_releaseHoldOnJobProcess, SIGNAL(readyReadStandardError()),
			 this, SLOT(releaseHoldOnJob_getStderr())); // same here
	m_releaseHoldOnJobProcess->start( releaseHoldOnJobCommand );

	if (!m_releaseHoldOnJobProcess->waitForStarted())
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Couldn't release hold on job", "'qrls' process unable to start.  Torque is probably not running.\n");
		m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
		return false;
	}

	if (!m_releaseHoldOnJobProcess->waitForFinished(-1)) //If msecs is -1, this function will not time out
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Error trying to release hold on job", "Process's waitForFinished() method returned error.\n");
		m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
		return false;
	}

	delete m_releaseHoldOnJobProcess;
	m_releaseHoldOnJobProcess = NULL;

	releaseHoldOnJob_processStdout(); // parse the stdout data collected (above)
	// restore the original cursor
	QApplication::restoreOverrideCursor();


    if (m_showReleaseHoldOnJobSTDERROutput)
	{
		// see if there's any errors to display
		if (!m_releaseHoldOnJob_Stderr.isEmpty())
		{
//			QMessageBox::critical(0, "Error issuing 'release hold on job' command",
//								  QString("Error issuing 'release hold on job' command.  Error message was: %1").arg(m_releaseHoldOnJob_Stderr));
            ErrorMsgDlg dlg("TORQUEView Error", "Error issuing 'Release Hold on Job' command. Error message:", m_releaseHoldOnJob_Stderr);
            dlg.exec();
            if (dlg.isDontShowThisAgainChecked())  // see if the "Don't show this dialog again" checkbox was checked
                m_showReleaseHoldOnJobSTDERROutput = false;
            m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
            return false;
		}
	}

	m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
	return true; // success

}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_releaseHoldOnJob() - this gets called whenever the releaseHoldOnJob process has something to say...
void QstatTab::releaseHoldOnJob_getStdout()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	if (m_releaseHoldOnJobProcess != NULL)
	{
		QByteArray s = m_releaseHoldOnJobProcess->readAllStandardOutput(); // read normal output
		m_releaseHoldOnJob_Stdout.append( s );  // if there's any stdout
	}
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_releaseHoldOnJob() - this gets called whenever the releaseHoldOnJob process has something to say...
void QstatTab::releaseHoldOnJob_getStderr()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	if (m_releaseHoldOnJobProcess != NULL)
	{
		QByteArray s = m_releaseHoldOnJobProcess->readAllStandardError(); // read error channel
		m_releaseHoldOnJob_Stderr.append( s );  // if there's any stderr
	}
}


/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::releaseHoldOnJob_processStdout() // parse the stdout data collected (above)
{
//  QMessageBox::information(0, "Run job", m_releaseHoldOnJob_Stdout);
}


/*******************************************************************************
 *
*******************************************************************************/
bool QstatTab::issueCmd_RerunJob( QString jobID)
{
	m_mainWindow->setIsCurrentlyExecutingCmd(true);  // indicate "TORQUEView IS currently executing a TORQUE client command"

	m_rerunJob_Stdout.clear();
	m_rerunJob_Stderr.clear();


	QString rerunJobCommand = QString(m_mainWindow->m_Config_Cmd_RerunJob)
			.arg(jobID);

	m_mainWindow->statusBar()->showMessage(QString("%1").arg(rerunJobCommand), 10000);
	// show "Wait cursor"
	QApplication::setOverrideCursor(Qt::WaitCursor);


	m_rerunJobProcess = new QProcess(this);
	connect (m_rerunJobProcess, SIGNAL(readyReadStandardOutput()),
			 this, SLOT(rerunJob_getStdout())); // connect process signals with my code
	connect (m_rerunJobProcess, SIGNAL(readyReadStandardError()),
			 this, SLOT(rerunJob_getStderr())); // same here
	m_rerunJobProcess->start( rerunJobCommand );

	if (!m_rerunJobProcess->waitForStarted())
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Couldn't rerun job", "'qrerun' process unable to start.  Torque is probably not running.\n");
		m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
		return false;
	}

	if (!m_rerunJobProcess->waitForFinished(-1)) //If msecs is -1, this function will not time out
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Error trying to rerun job", "Process's waitForFinished() method returned error.\n");
		m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
		return false;
	}

	delete m_rerunJobProcess;
	m_rerunJobProcess = NULL;

	rerunJob_processStdout(); // parse the stdout data collected (above)
	// restore the original cursor
	QApplication::restoreOverrideCursor();


    if (m_showRerunJobSTDERROutput)
	{
		// see if there's any errors to display
		if (!m_rerunJob_Stderr.isEmpty())
		{
//			QMessageBox::critical(0, "Error issuing 'rerun job' command",
//								  QString("Error issuing 'rerun job' command.  Error message was: %1").arg(m_rerunJob_Stderr));
            ErrorMsgDlg dlg("TORQUEView Error", "Error issuing 'Rerun Job' command. Error message:", m_rerunJob_Stderr);
            dlg.exec();
            if (dlg.isDontShowThisAgainChecked())  // see if the "Don't show this dialog again" checkbox was checked
                m_showRerunJobSTDERROutput = false;
            m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
            return false;
		}
	}

	m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
	return true; // success

}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_RerunJob() - this gets called whenever the RerunJob process has something to say...
void QstatTab::rerunJob_getStdout()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	if (m_rerunJobProcess != NULL)
	{
		QByteArray s = m_rerunJobProcess->readAllStandardOutput(); // read normal output
		m_rerunJob_Stdout.append( s );  // if there's any stdout
	}
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_RerunJob() - this gets called whenever the RerunJob process has something to say...
void QstatTab::rerunJob_getStderr()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	if (m_rerunJobProcess != NULL)
	{
		QByteArray s = m_rerunJobProcess->readAllStandardError(); // read error channel
		m_rerunJob_Stderr.append( s );  // if there's any stderr
	}
}


/*******************************************************************************
 *
*******************************************************************************/
void QstatTab::rerunJob_processStdout() // parse the stdout data collected (above)
{
//  QMessageBox::information(0, "Run job", m_rerunJob_Stdout);
}
