/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: qmgrtab.cpp.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
*/



#include "qmgrtab.h"
#include "ui_qmgrtab.h"
#include "mainwindow.h"
#include "errormsgdlg.h"
#include <QMessageBox>
#include <QStatusBar>


/*******************************************************************************
 *
*******************************************************************************/
QmgrTab::QmgrTab(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::QmgrTab)
{
	ui->setupUi(this);

	m_mainWindow = dynamic_cast<MainWindow*>(parent);
	if (m_mainWindow == NULL)
	{
		return;
	}

	connect(ui->checkBox_UseHelps, SIGNAL(stateChanged(int)),
		this, SLOT(on_checkBox_UseHelps_stateChanged()));
	connect(ui->lineEdit_Qmgr, SIGNAL(returnPressed(int)),
		this, SLOT(on_lineEdit_Qmgr_returnPressed()));

	initServerParamsCombobox();
	initQueueAttribsCombobox();


//	showHelps();	// no need to do this here -- these controls will be shown by default

	ui->lineEdit_Qmgr->installEventFilter(this);

    resetErrorMsgDlg();

	m_qmgrHistoryIndex = -1;

}

/*******************************************************************************
 *
*******************************************************************************/
QmgrTab::~QmgrTab()
{
	delete ui;
}

/*******************************************************************************
 *
*******************************************************************************/
void QmgrTab::resetErrorMsgDlg()
{
    m_showQmgrSTDERROutput = true;
}

/*******************************************************************************
 *
*******************************************************************************/
void QmgrTab::clearLists()
{
	ui->plainTextEdit_Qmgr->clear();		// clear out text edit field
}


/*******************************************************************************
 *
*******************************************************************************/
void QmgrTab::showHelps()
{
	ui->comboBox_QueueServer->show();
	if (ui->comboBox_QueueServer->currentText().contains("set server")) // either "set server" or "unset server"
	{
		ui->comboBox_ServerParams->show();
		ui->comboBox_QueueAttribs->hide();
		ui->comboBox_Queues->hide();
	}
	else
	{
		ui->comboBox_ServerParams->hide();
		ui->comboBox_QueueAttribs->show();
		ui->comboBox_Queues->show();
	}
	ui->comboBox_Operator->show();
	ui->lineEdit_Value->show();
	ui->btnInsert->show();
}

/*******************************************************************************
 *
*******************************************************************************/
void QmgrTab::hideHelps()
{
	ui->comboBox_QueueServer->hide();
	ui->comboBox_ServerParams->hide();
	ui->comboBox_QueueAttribs->hide();
	ui->comboBox_Queues->hide();
	ui->comboBox_Operator->hide();
	ui->lineEdit_Value->hide();
	ui->btnInsert->hide();
}

/*******************************************************************************
 *
*******************************************************************************/
void QmgrTab::on_checkBox_UseHelps_stateChanged()
{
	if (ui->checkBox_UseHelps->isChecked())
		showHelps();
	else
		hideHelps();
}

/*******************************************************************************
 *
*******************************************************************************/
void QmgrTab::on_comboBox_QueueServer_currentIndexChanged( const QString & text )
{
	if (text.contains("set server")) // either "set server" or "unset server"
    {
        ui->comboBox_ServerParams->show();
        ui->comboBox_QueueAttribs->hide();
        ui->comboBox_Queues->hide();
    }
	else // text == "set queue" or "unset queue"
    {
        ui->comboBox_ServerParams->hide();
        ui->comboBox_QueueAttribs->show();
        ui->comboBox_Queues->show();
    }
    ui->comboBox_Operator->show();
    ui->lineEdit_Value->show();
    ui->btnInsert->show();
}

/*******************************************************************************
 *
*******************************************************************************/
void QmgrTab::on_btnInsert_clicked()
{
//    QString sVal = ui->lineEdit_Value->currentText();
//    if (sVal.isEmpty())
//    {
//        // can't run this if the auto-refresh checkbox is checked
//        QMessageBox::critical(0, "Insert Command",
//                              "Command must contain a value in order to insert..");
//        return;
//    }

    QStringList sl;
    sl.append(ui->comboBox_QueueServer->currentText());

    if (ui->comboBox_QueueServer->currentText() == "set server")
    {
        sl.append(ui->comboBox_ServerParams->currentText());
    }
    else // is "set queue"
    {
		sl.append(ui->comboBox_Queues->currentText());
		sl.append(ui->comboBox_QueueAttribs->currentText());
    }
    sl.append(ui->comboBox_Operator->currentText());
    sl.append(ui->lineEdit_Value->text());

    QString sCmd = sl.join(" ");  // concat using " " space separator

//	QString qmgrCommand = m_mainWindow->m_Config_Cmd_Qmgr;
//	qmgrCommand.chop(1); // remove last char (the " char)
//	qmgrCommand = QString("%1 '%2'\"").arg(qmgrCommand).arg(sCmd);

    ui->lineEdit_Qmgr->setText( sCmd );

}

/*******************************************************************************
 *
*******************************************************************************/
void QmgrTab::on_btnX_clicked()  // clear the Submit edit line
{
    ui->lineEdit_Qmgr->clear();

    clearSelection();
}

/*******************************************************************************
 *
*******************************************************************************/
void QmgrTab::on_btnRefresh_clicked()  // submit qmgr "p  s" command -- essentially does a refresh
{
	QString sCmd = "p s";
	m_qmgrHistoryList.insert(0, sCmd );
	m_qmgrHistoryIndex = -1;

	issueCmd_Qmgr( sCmd );  // execute "qmgr -c 'p s'" command -- parse output data
}

/*******************************************************************************
 *
*******************************************************************************/
void QmgrTab::on_plainTextEdit_Qmgr_cursorPositionChanged ()
{
    // first, clear the last selection
    QTextCursor cursor = ui->plainTextEdit_Qmgr->textCursor();
    cursor.select(QTextCursor::LineUnderCursor);
    ui->plainTextEdit_Qmgr->setTextCursor(cursor);   // select the current line

    QString text = cursor.selectedText();  // get selected text
    text = text.trimmed(); // strip off leading/trailing white space
    if (text.startsWith("#"))  // if it's a comment
        ui->lineEdit_Qmgr->clear();
    else
        ui->lineEdit_Qmgr->setText( text );

}


/*******************************************************************************
 *
*******************************************************************************/
void QmgrTab::clearSelection()
{
     QTextCursor cursor = ui->plainTextEdit_Qmgr->textCursor();
     if ( cursor.hasSelection())
     {
         cursor.clearSelection();
         ui->plainTextEdit_Qmgr->setTextCursor(cursor);
     }
}

/*******************************************************************************
 *
*******************************************************************************/
void QmgrTab::on_lineEdit_Qmgr_returnPressed()
{
    on_btnSubmit_clicked();
}

/*******************************************************************************
 *
*******************************************************************************/
void QmgrTab::enableControls()
{
    // data is "live" (i.e, from either a local or remote host instead of a snapshot file), so enable Submit functionality
    ui->btnRefresh->setEnabled(true);
    ui->btnClear->setEnabled(true);

	if (m_mainWindow->m_runningState == runningAs_AdminUser)  // if running With admin privileges
	{
		ui->btnSubmit->setEnabled(true);
	}
	else
	{
		ui->btnSubmit->setEnabled(false);
	}
}

/*******************************************************************************
 *
*******************************************************************************/
void QmgrTab::disableControls()
{
    // data is NOT "live" (i.e, it's from a snapshot file rather than a local or remote host), so disable Submit functionality
    ui->btnSubmit->setEnabled(false);
    ui->btnRefresh->setEnabled(false);
    ui->btnClear->setEnabled(false);
}

/*******************************************************************************
 *
*******************************************************************************/
void QmgrTab::issueCmd_Qmgr(QString sCmd)  // execute a qmgr command -- parse output data
{
    ui->plainTextEdit_Qmgr->blockSignals(true);

    clearLists();

	if ( m_mainWindow->m_Config_DataSource == 1)  // 1=remote nodes
	{
		sCmd.insert(0, "'");
		sCmd.append("'");
	}
	else // if ( m_mainWindow->m_Config_DataSource == 0)  // 0=local nodes
	{
		sCmd.insert(0, "\"");
		sCmd.append("\"");
	}
	QString qmgrCommand = QString(m_mainWindow->m_Config_Cmd_Qmgr_c)
			.arg(sCmd);

//	ui->label_Submitted->setText(QString("%1").arg(qmgrCommand));
	m_mainWindow->statusBar()->showMessage(QString("%1").arg(qmgrCommand), 10000);

	// show "Wait cursor"
	QApplication::setOverrideCursor(Qt::WaitCursor);

	m_qmgr_Stdout.clear();
	m_qmgr_Stderr.clear();

	m_qmgrProcess = new QProcess(this);
	connect (m_qmgrProcess, SIGNAL(readyReadStandardOutput()),
			 this, SLOT(qmgr_getStdout())); // connect process signals with your code
	connect (m_qmgrProcess, SIGNAL(readyReadStandardError()),
			 this, SLOT(qmgr_getStderr())); // same here
	m_qmgrProcess->start(qmgrCommand);

	if (!m_qmgrProcess->waitForStarted())
	{
		QApplication::restoreOverrideCursor();
//		QMessageBox::critical(0, "Error trying to start qmgr process", "Process's waitForStarted() method returned error.\n");
		QMessageBox::critical(0, "Couldn't start 'qmgr' process", "'qmgr' process unable to start.  Torque is probably not running.\n");
		return;
	}

	if (!m_qmgrProcess->waitForFinished(5000)) //If msecs is -1, this function will not time out
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Error trying to complete qmgr process", "Process's waitForFinished() method returned error.\n");
		return;
	}

	delete m_qmgrProcess;
	m_qmgrProcess = NULL;


	qmgr_processStdout();

	// restore the original cursor
	QApplication::restoreOverrideCursor();


    if (m_showQmgrSTDERROutput)
	{
		// see if there are any errors to display
		if (!m_qmgr_Stderr.isEmpty())
		{
//			QMessageBox::critical(0, "Error issuing qmgr command",
//								  QString("Error issuing qmgr -c 'p s' command.  Error message was: %1").arg(m_qmgr_Stderr));
            ErrorMsgDlg dlg("TORQUEView Error", "Error issuing \"qmgr -c 'p s'\" command. Error message:", m_qmgr_Stderr);
            dlg.exec();
            if (dlg.isDontShowThisAgainChecked())  // see if the "Don't show this dialog again" checkbox was checked
                m_showQmgrSTDERROutput = false;
        }
	}

    ui->plainTextEdit_Qmgr->blockSignals(false);

}

/*******************************************************************************
 *
*******************************************************************************/
// called from populateQmgrFromCmd() - this gets called whenever the qmgr process has something to say...
void QmgrTab::qmgr_getStdout()
{
	QByteArray s = m_qmgrProcess->readAllStandardOutput(); // read normal output
	m_qmgr_Stdout.append( s );  // if there's any stdout
}

/*******************************************************************************
 *
*******************************************************************************/
// called from populateQmgrFromCmd() - this gets called whenever the qmgr process has something to say...
void QmgrTab::qmgr_getStderr()
{
	QByteArray s = m_qmgrProcess->readAllStandardError(); // read error channel
	m_qmgr_Stderr.append( s );  // if there's any stderr
}

/*******************************************************************************
 *
*******************************************************************************/
void QmgrTab::qmgr_processStdout() // parse the stdout data collected (above)
{
	QStringList slQueues;
	QString sCreateAndDefine = "# Create and define queue ";

	ui->plainTextEdit_Qmgr->insertPlainText(QString("\n"));


	QStringList lines = m_qmgr_Stdout.split("\n");
	foreach (QString line, lines)
	{
		if (line.startsWith( sCreateAndDefine ))
		{
			QString queue = line.mid( sCreateAndDefine.size() );
			slQueues.append( queue );
		}
		ui->plainTextEdit_Qmgr->appendPlainText(QString("%1").arg(line));
	}

	// add all queues we found
	ui->comboBox_Queues->clear();
	ui->comboBox_Queues->addItems( slQueues );

	ui->plainTextEdit_Qmgr->setFocus();

}

/*******************************************************************************
 *
*******************************************************************************/
void QmgrTab::on_btnSubmit_clicked()  // submit qmgr command
{
	QString sCmd = ui->lineEdit_Qmgr->text();
	if (sCmd.isEmpty())
		return;

	m_qmgrHistoryList.insert(0, sCmd );
	m_qmgrHistoryIndex = -1;

	issueCmd_Qmgr(sCmd);

	// do a qmgr 'p s' to refresh the screen (but don't add it to the history list)
	QString sRefreshCmd = "p s";
	issueCmd_Qmgr(sRefreshCmd);
}

/*******************************************************************************
 *
*******************************************************************************/
void QmgrTab::on_btnClear_clicked()  // clear lists
{
	clearLists();
}

/*******************************************************************************
 *
*******************************************************************************/
void QmgrTab::initQmgrFromFile( QTextStream &in )  // init the qmgr tab from a file
{
	ui->plainTextEdit_Qmgr->clear();

	while (!in.atEnd())
	{
		QString line = in.readLine();

		// have we gotten to the next section after the Qmgr section?
		if (line.startsWith( "#--------" )) // each section will be terminated by a "#-------" string to indicate the start of the next section
		{
			break;
		}

		ui->plainTextEdit_Qmgr->appendPlainText(line);

	} // while not end of file
}

/*******************************************************************************
 *
*******************************************************************************/
void QmgrTab::writeDataToFile(QTextStream& out)
{
	// WRITE OUT QMGR DATA TO FILE:

	// get all the text
	QString s = ui->plainTextEdit_Qmgr->document()->toPlainText();
	if (s.isEmpty())
		return;  // bail out

	out << "\n";
	out << "\n";
	out << "\n";
	out << "#---------------------------------------------------------------" << "\n";
	out << "TORQUEView -- Qmgr Output" << "\n";
	out << "#---------------------------------------------------------------" << "\n";
	out << "\n";
	out << "\n";

	out << s << endl;

}

/*******************************************************************************
 *
*******************************************************************************/
bool QmgrTab::eventFilter(QObject* obj, QEvent *event)
{
	// this implements a "history" list, which the user can cycle through by
	// clicking either down-arrow or up-arrow keys

	if (obj == ui->lineEdit_Qmgr)
	{
		if (event->type() == QEvent::KeyPress)
		{
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
			if (keyEvent->key() == Qt::Key_Up)
			{
			//	qDebug() << "lineEdit -> Qt::Key_Up";
				if (m_qmgrHistoryList.size() == 0)
					return true;
				if (m_qmgrHistoryIndex == -1)
					return true;
				if (m_qmgrHistoryIndex > 0)
					m_qmgrHistoryIndex--;
				ui->lineEdit_Qmgr->setText(m_qmgrHistoryList[ m_qmgrHistoryIndex ]);
				return true;
			}
			else if(keyEvent->key() == Qt::Key_Down)
			{
			//	qDebug() << "lineEdit -> Qt::Key_Down";
				if (m_qmgrHistoryList.size() == 0)
					return true;
				if (m_qmgrHistoryIndex < m_qmgrHistoryList.size()-1)
					m_qmgrHistoryIndex++;
				ui->lineEdit_Qmgr->setText(m_qmgrHistoryList[m_qmgrHistoryIndex]);
				return true;
			}
		}
		return false;
	}
	return QmgrTab::eventFilter(obj, event);
}


/*******************************************************************************
 *
*******************************************************************************/
void QmgrTab::initServerParamsCombobox()
{
	ui->comboBox_ServerParams->clear();
	int idx=0;
	ui->comboBox_ServerParams->addItem( "acl_hosts" );
//	 m_mainWindow->formatForTooltip(sExecHost, " ") );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: <HOST>[,<HOST>]... or <HOST>[range] or <HOST*> where the asterisk (*) can appear anywhere in the host name\n\nSpecifies a list of hosts from which jobs may be submitted.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "acl_host_enable" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nWhen set to TRUE, specifies that the acl_hosts value is enabled.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "acl_logic_or" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nWhen set to TRUE,the user and group queue ACL's are logically OR'd. When set to FALSE, they are AND'd.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "allow_node_submit" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nWhen set to TRUE, specifies that users can submit jobs directly from any trusted compute host directly or from within batch jobs.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "allow_proxy_user" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nWhen set to TRUE, specifies that users can proxy from one user to another. Proxy requests will be either validated by ruserok() or by the scheduler.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "auto_node_np" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nWhen set to TRUE, automatically configures a node's np (number of processors) value based on the ncpus value from the status update. Requires full manager privilege to set or alter.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "automatic_requeue_exit_code" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: LONG\n\nThis is an exit code, defined by the admin, that tells pbs_server to requeue the job instead of considering it as completed. This allows the user to add some additional checks that the job can run meaningfully, and if not, then the job script exits with the specified code to be requeued.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "checkpoint_defaults" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: STRING\n\nSpecifies for a queue the default checkpoint values for a job that does not have checkpointing specified. The checkpoint_defaults parameter only takes effect on execution queues.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "clone_batch_delay" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nSpecifies the delay (in seconds) between clone batches", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "clone_batch_size" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nJob arrays are created in batches of size X. X jobs are created, and after the clone_batch_delay, X more are created. This repeats until all are created.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "cray_enabled" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nWhen set to TRUE, specifies that this instance of pbs_server has Cray hardware that reports to it.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "default_queue" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: STRING\n\nIndicates the queue to assign to a job if no queue is explicitly specified by the submitter.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "disable_server_id_check" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nWhen set to TRUE, makes it so the user for the job doesn't have to exist on the server. The user must still exist on all the compute nodes or the job will fail when it tries to execute.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "display_job_server_suffix" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nWhen set to TRUE, TORQUE will display both the job ID and the host name. When set to FALSE, only the job ID will be displayed.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "interactive_jobs_can_roam" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nBy default, interactive jobs run from the login node that they submitted from. When TRUE, interactive jobs may run on login nodes other than the one where the jobs were submitted to. ", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "job_force_cancel_time" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nIf a job has been deleted and is still in the system after x seconds, the job will be purged from the system. This is mostly useful when a job is running on a large number of nodes and one node goes down. The job cannot be deleted because the MOM cannot be contacted. The qdel fails and none of the other nodes can be reused. This parameter can used to remedy such situations.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "job_log_file_max_size" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nThis specifies a soft limit (in kilobytes) for the job log's maximum size. The file size is checked every five minutes and if the current day file size is greater than or equal to this value, it is rolled from <filename> to <filename.1> and a new empty log is opened. If the current day file size exceeds the maximum size a second time, the <filename.1> log file is rolled to <filename.2>, the current log is rolled to <filename.1>, and a new empty log is opened. Each new log causes all other logs to roll to an extension that is one greater than its current number. Any value less than 0 is ignored by pbs_server (meaning the log will not be rolled).", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "job_log_file_roll_depth" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nThis sets the maximum number of new log files that are kept in a day if the job_log_file_max_size parameter is set. For example, if the roll depth is set to 3, no file can roll higher than <filename.3>. If a file is already at the specified depth, such as <filename.3>, the file is deleted so it can be replaced by the incoming file roll, <filename.2>.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "job_log_keep_days" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nThis maintains logs for the number of days designated. If set to 4, any log file older than 4 days old is deleted.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "job_nanny" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nWhen set to TRUE, enables the experimental \"job deletion nanny\" feature. All job cancels will create a repeating task that will resend KILL signals if the initial job cancel failed. Further job cancels will be rejected with the message \"job cancel in progress.\" This is useful for temporary failures with a job's execution node during a job delete request.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "job_stat_rate" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nSpecifies the maximum age of MOM level job data which is allowed when servicing a qstat request. If data is older than this value, the pbs_server daemon will contact the MOMs with stale data to request an update. For large systems, this value should be increased to 5 minutes or higher.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "job_start_timeout" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nSpecifies the pbs_server to pbs_mom TCP socket timeout in seconds that is used when the pbs_server sends a job start to the pbs_mom. It is useful when the MOM has extra overhead involved in starting jobs. If not specified, then the tcp_timeout parameter is used.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "job_sync_timeout" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nWhen a stray job is reported on multiple nodes, the server sends a kill signal to one node at a time. This timeout determines how long the server waits between kills if the job is still being reported on any nodes.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "keep_completed" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nThe amount of time a job will be kept in the queue after it has entered the completed state. keep_completed must be set for job dependencies to work.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "lock_file" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: STRING\n\nSpecifies the name and location of the lock file used to determine which high availability server should be active. If a full path is specified, it is used verbatim by TORQUE. If a relative path is specified, TORQUE will prefix it with torque/server_priv.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "lock_file_update_time" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nSpecifies how often (in seconds) the thread will update the lock file.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "lock_file_check_time" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nSpecifies how often (in seconds) a high availability server will check to see if it should become active.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "log_events" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: Bitmap\n\nBy default, all events are logged. However, you can customize things so that only certain events show up in the log file. See TORQUE documentation to see the bitmaps for the different kinds of logs.	If you want to log only error, system, and job information, use qmgr to set log_events to 11.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "log_file_max_size" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nSpecifies a soft limit, in kilobytes, for the server's log file. The file size is checked every 5 minutes, and if the current day file size is greater than or equal to this value then it will be rolled from X to X.1 and a new empty log will be opened. Any value less than or equal to 0 will be ignored by pbs_server (the log will not be rolled).", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "log_file_roll_depth" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nControls how deep the current day log files will be rolled, if log_file_max_size is set, before they are deleted.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "log_keep_days" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nSpecifies how long (in days) a server or MOM log should be kept.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "log_level" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nSpecifies the pbs_server logging verbosity. Maximum value is 7.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "mail_body_fmt" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: A printf-like format string\n\nOverride the default format for the body of outgoing mail messages. A number of printf-like format specifiers and escape sequences can be used.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "mail_domain" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: STRING\n\nOverride the default domain for outgoing mail messages. If set, emails will be addressed to <user>@<hostdomain>. If unset, the job's Job_Owner attribute will be used. If set to never, TORQUE will never send emails.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "mail_from" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: STRING\n\nSpecify the name of the sender when TORQUE sends emails.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "mail_subject_fmt" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: A printf-like format string\n\nOverride the default format for the subject of outgoing mail messages. A number of printf-like format specifiers and escape sequences can be used.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "managers" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: <user>@<host.sub.domain>[,<user>@<host.sub.domain>...]\n\nList of users granted batch administrator privileges. The host, sub-domain, or domain name may be wildcarded by the use of an asterisk character (*). Requires full manager privilege to set or alter.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "max_job_array_size" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nSets the maximum number of jobs that can be in a single job array.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "max_slot_limit" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nThis is the maximum number of jobs that can run concurrently in any job array. Slot limits can be applied at submission time with qsub, or it can be modified with qalter.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "max_threads" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nThis is the maximum number of threads that should exist in the thread pool at any time.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "max_user_queuable" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nWhen set, max_user_queuable places a system-wide limit on the amount of jobs that an individual user can queue.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "min_threads" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nThis is the minimum number of threads that should exist in the thread pool at any time.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "moab_array_compatible" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nThis parameter places a hold on jobs that exceed the slot limit in a job array. When one of the active jobs is completed or deleted, one of the held jobs goes to a queued state.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "mom_job_sync" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nWhen set to TRUE, specifies that the pbs_server will synchronize its view of the job queue and resource allocation with compute nodes as they come online. If a job exists on a compute node, it will be automatically cleaned up and purged. (Enabled by default in TORQUE 2.2.0 and higher.)\n\nJobs that are no longer reported by the mother superior are automatically purged by pbs_server. Jobs that pbs_server instructs the MOM to cancel have their processes killed in addition to being deleted (instead of leaving them running as in versions of TORQUE prior to 4.1.1).", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "next_job_number" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nSpecifies the ID number of the next job. If you set your job number too low and TORQUE repeats a job number that it has already used, the job will fail. Before setting next_job_number to a number lower than any number that TORQUE has already used, you must clear out your .e and .o files.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "node_check_rate" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nSpecifies the minimum duration (in seconds) that a node can fail to send a status update before being marked down by the pbs_server daemon.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "node_pack" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nControls how multiple processor nodes are allocated to jobs. If this attribute is set to TRUE, jobs will be assigned to the multiple processor nodes with the fewest free processors. This packs jobs into the fewest possible nodes leaving multiple processor nodes free for jobs which need many processors on a node. If set to false, jobs will be scattered across nodes reducing conflicts over memory between jobs. If unset, the jobs are packed on nodes in the order that the nodes are declared to the server (in the nodes file). Default value: unset - assigned to nodes as nodes in order that were declared.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "node_ping_rate" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nSpecifies the maximum interval (in seconds) between successive \"pings\" sent from the pbs_server daemon to the pbs_mom daemon to determine node/daemon health.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "no_mail_force" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nWhen set to TRUE, eliminates all e-mails when mail_options (see qsub) is set to \"n\". The job owner won't receive e-mails when a job is deleted by a different user or a job failure occurs. If no_mail_force is unset or is FALSE, then the job owner receives e-mails when a job is deleted by a different user or a job failure occurs.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "np_default" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nAllows the administrator to unify the number of processors (np) on all nodes. The value can be dynamically changed. A value of 0 tells pbs_server to use the value of np found in the nodes file. The maximum value is 32767.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "operators" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: <user>@<host.sub.domain>[,<user>@<host.sub.domain>...]\n\nList of users granted batch operator privileges. Requires full manager privilege to set or alter.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "poll_jobs" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nIf set to TRUE, pbs_server will poll job info from MOMs over time and will not block on handling requests which require this job information. If set to FALSE, no polling will occur and if requested job information is stale, pbs_server may block while it attempts to update this information. For large systems, this value should be set to TRUE.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "query_other_jobs" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nWhen set to TRUE, specifies whether or not non-admin users may view jobs they do not own.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "record_job_info" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nThis must be set to TRUE in order for job logging to be enabled.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "record_job_script" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nIf set to TRUE, this adds the contents of the script executed by a job to the log.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "resources_available" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: STRING\n\nAllows overriding of detected resource quantity limits. pbs_server must be restarted for changes to take effect. Also, resources_available is constrained by the smallest of queue.resources_available and the server.resources_available.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "scheduling" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nAllows pbs_server to be scheduled. When FALSE, pbs_server is a resource manager that works on its own. When TRUE, TORQUE allows a scheduler, such as Moab or Maui, to dictate what pbs_server should do.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "submit_hosts" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: \"<HOSTNAME>[,<HOSTNAME>]...\"\n\nIndicates which hosts included in the server nodes file located at $TORQUE/server_priv/nodes can submit batch or interactive jobs. For more information on adding hosts that are not included in the first nodes file, see the acl_hosts parameter.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "tcp_timeout" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nSpecifies the timeout for idle TCP connections. If no communication is received by the server on the connection after the timeout, the server closes the connection. There is an exception for connections made to the server on port 15001 (default); timeout events are ignored on the server for such connections established by a client utility or scheduler. Responsibility rests with the client to close the connection first.", " "), Qt::ToolTipRole);
	ui->comboBox_ServerParams->addItem( "thread_idle_seconds" );
	ui->comboBox_ServerParams->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nThis is the number of seconds a thread can be idle in the thread pool before it is deleted. If threads should not be deleted, set to -1. TORQUE will always maintain at least min_threads number of threads, even if all are idle.", " "), Qt::ToolTipRole);

}

/*******************************************************************************
 *
*******************************************************************************/
void QmgrTab::initQueueAttribsCombobox()
{
	ui->comboBox_QueueAttribs->clear();
	int idx=0;
	ui->comboBox_QueueAttribs->addItem( "acl_groups" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: <GROUP>[@<HOST>][+<USER>[@<HOST>]]...\n\nSpecifies the list of groups which may submit jobs to the queue. If acl_group_enable is set to true, only users with a primary group listed in acl_groups may utilize the queue.", " "), Qt::ToolTipRole);
	ui->comboBox_QueueAttribs->addItem( "acl_group_enable" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nIf TRUE, constrains TORQUE to only allow jobs submitted from groups specified by the acl_groups parameter.", " "), Qt::ToolTipRole);
	ui->comboBox_QueueAttribs->addItem( "acl_group_sloppy" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nIf TRUE, acl_groups will be checked against all groups of which the job users is a member.", " "), Qt::ToolTipRole);
	ui->comboBox_QueueAttribs->addItem( "acl_hosts" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: <HOST>[+<HOST>]...\n\nSpecifies the list of hosts that may submit jobs to the queue.", " "), Qt::ToolTipRole);
	ui->comboBox_QueueAttribs->addItem( "acl_host_enable" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nIf TRUE, constrains TORQUE to only allow jobs submitted from hosts specified by the acl_hosts parameter.", " "), Qt::ToolTipRole);
	ui->comboBox_QueueAttribs->addItem( "acl_logic_or" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nIf TRUE, user and group acls are logically OR'd together, meaning that either acl may be met to allow access. If FALSE or unset, then both acls are AND'd, meaning that both acls must be satisfied.", " "), Qt::ToolTipRole);
	ui->comboBox_QueueAttribs->addItem( "acl_users" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: <USER>[@<HOST>][+<USER>[@<HOST>]]...\n\nSpecifies the list of users who may submit jobs to the queue. If acl_user_enable is set to TRUE, only users listed in acl_users may use the queue.", " "), Qt::ToolTipRole);
	ui->comboBox_QueueAttribs->addItem( "acl_user_enable" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nIf TRUE, constrains TORQUE to only allow jobs submitted from users specified by the acl_users parameter.", " "), Qt::ToolTipRole);
	ui->comboBox_QueueAttribs->addItem( "disallowed_types" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: <type>[+<type>]...\n\nSpecifies classes of jobs that are not allowed to be submitted to this queue. Valid types are interactive, batch, rerunable, nonrerunable, fault_tolerant (as of version 2.4.0 and later), fault_intolerant (as of version 2.4.0 and later), and job_array (as of version 2.4.1 and later).", " "), Qt::ToolTipRole);
	ui->comboBox_QueueAttribs->addItem( "enabled" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nSpecifies whether the queue accepts new job submissions.", " "), Qt::ToolTipRole);
	ui->comboBox_QueueAttribs->addItem( "features_required" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: feature1[feature2[,feature3...]]\n\nSpecifies that all jobs in this queue will require these features in addition to any they may have requested. A feature is a synonym for a property.", " "), Qt::ToolTipRole);
	ui->comboBox_QueueAttribs->addItem( "keep_completed" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nSpecifies the number of seconds jobs should be held in the Completed state after exiting. ", " "), Qt::ToolTipRole);
	ui->comboBox_QueueAttribs->addItem( "kill_delay" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nSpecifies the number of seconds between sending a SIGTERM and a SIGKILL to a job in a specific queue that you want to cancel. It is possible that the job script, and any child processes it spawns, can receive several SIGTERM signals before the SIGKILL signal is received.", " "), Qt::ToolTipRole);
	ui->comboBox_QueueAttribs->addItem( "max_queuable" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nSpecifies the maximum number of jobs allowed in the queue at any given time (includes idle, running, and blocked jobs).", " "), Qt::ToolTipRole);
	ui->comboBox_QueueAttribs->addItem( "max_running" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nSpecifies the maximum number of jobs in the queue allowed to run at any given time.", " "), Qt::ToolTipRole);
	ui->comboBox_QueueAttribs->addItem( "max_user_queuable" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nSpecifies the maximum number of jobs, per user, allowed in the queue at any given time (includes idle, running, and blocked jobs). Version 2.1.3 and greater.", " "), Qt::ToolTipRole);
	ui->comboBox_QueueAttribs->addItem( "max_user_run" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nSpecifies the maximum number of jobs, per user, in the queue allowed to run at any given time.", " "), Qt::ToolTipRole);
	ui->comboBox_QueueAttribs->addItem( "priority" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: INTEGER\n\nSpecifies the priority value associated with the queue.", " "), Qt::ToolTipRole);
	ui->comboBox_QueueAttribs->addItem( "queue_type" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: One of e, execution, r, or route\n\nSpecifies the queue type.", " "), Qt::ToolTipRole);
	ui->comboBox_QueueAttribs->addItem( "required_login_property" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: STRING\n\nAdds the specified login property as a requirement for all jobs in this queue.", " "), Qt::ToolTipRole);
	ui->comboBox_QueueAttribs->addItem( "resources_available" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: STRING\n\nSpecifies to cumulative resources available to all jobs running in the queue.", " "), Qt::ToolTipRole);
	ui->comboBox_QueueAttribs->addItem( "resources_default" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: STRING\n\nSpecifies default resource requirements for jobs submitted to the queue.", " "), Qt::ToolTipRole);
	ui->comboBox_QueueAttribs->addItem( "resources_max" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: STRING\n\nSpecifies the maximum resource limits for jobs submitted to the queue.", " "), Qt::ToolTipRole);
	ui->comboBox_QueueAttribs->addItem( "resources_min" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: STRING\n\nSpecifies the minimum resource limits for jobs submitted to the queue.", " "), Qt::ToolTipRole);
	ui->comboBox_QueueAttribs->addItem( "route_destinations" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: <queue>[@<host>]\n\nSpecifies the potential destination queues for jobs submitted to the associated routing queue.", " "), Qt::ToolTipRole);
	ui->comboBox_QueueAttribs->addItem( "started" );
	ui->comboBox_QueueAttribs->setItemData(idx++, m_mainWindow->formatForTooltip(
		"FORMAT: BOOLEAN\n\nSpecifies whether jobs in the queue are allowed to execute.", " "), Qt::ToolTipRole);
}
