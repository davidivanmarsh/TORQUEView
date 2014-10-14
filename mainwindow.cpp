/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: mainwindow.cpp.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
*/


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "heatmaptab.h"
#include "pbsnodestab.h"
#include "qstattab.h"
#include "qmgrtab.h"
#include "pbsservertab.h"
#include "takesnapshotdlg.h"
#include "configdlg.h"
#include "choosestylesheetdialog.h"
#include "logviewerdlg.h"
#include "replaysnapshotsdlg.h"
#include "aboutdlg.h"

#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>
#include <QTreeWidgetItem>
#include <QDateTime>
#include <QFormLayout>
#include <QTextEdit>
#include <QSettings>
#include <QFileDialog>
#include <QTimer>
#include <QDesktopWidget>



/*******************************************************************************
 *
*******************************************************************************/
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	QLocale::setDefault( QLocale(QLocale::English, QLocale::UnitedStates ));

//	QString s = QString("TORQUEView   (Version:  %1.%2.%3,  Built:  %4,  %5)")
//			.arg(TORQUEVIEW_MAJOR).arg(TORQUEVIEW_MINOR).arg(TORQUEVIEW_BUILD).arg(__DATE__).arg(__TIME__);
	QString s = QString("TORQUEView    Version:  %1.%2.%3")
			.arg(TORQUEVIEW_MAJOR).arg(TORQUEVIEW_MINOR).arg(TORQUEVIEW_BUILD);
	setWindowTitle(s);


    // add the application icon
    QIcon icon(":/icons/images/test-suite-wizard.png");
    setWindowIcon(icon);

    m_autoRefresh_timerID = 0;
    m_createSnapshot_timerID = 0;
    m_replaySnapshot_timerID = 0;

    setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is not currently executing a TORQUE client command"

	// add the Heat Map tab
	m_heatMapTab = new HeatMapTab(this);
	tabIndex_HeatMapTab = ui->tabWidget->addTab(m_heatMapTab, QIcon(":/icons/images/structured_data_32x32.ico"), tr("heat map"));
//	tabIndex_PbsNodesTab = ui->tabWidget->addTab(m_pbsNodesTab, QIcon(":/icons/images/structured_data_32x32.ico"), tr("PBSNodes"));

	// add the PbsNodes tab
	m_pbsNodesTab = new PbsNodesTab(this);
	tabIndex_PbsNodesTab = ui->tabWidget->addTab(m_pbsNodesTab, QIcon(":/icons/images/script_32x32.ico"), tr("nodes"));
//	tabIndex_PbsNodesTab = ui->tabWidget->addTab(m_pbsNodesTab, QIcon(":/icons/images/structured_data_32x32.ico"), tr("PBSNodes"));

	// add the Qstat tab
	m_qstatTab = new QstatTab(this);
	tabIndex_QstatTab = ui->tabWidget->addTab(m_qstatTab, QIcon(":/icons/images/preferences.png"), tr("jobs"));

	// add the Qmgr tab
	m_qmgrTab = new QmgrTab(this);
	tabIndex_QmgrTab = ui->tabWidget->addTab(m_qmgrTab, QIcon(":/icons/images/test-suite-wizard.png"), tr("qmgr"));

	// add the pbs_server tab
	m_pbsServerTab = new PbsServerTab(this);
	tabIndex_PbsServerTab = ui->tabWidget->addTab(m_pbsServerTab, QIcon(":/icons/images/categories.png"), tr("pbs_server"));


    connect(ui->checkBox_AutoRefresh, SIGNAL(stateChanged(int)),
        this, SLOT(on_checkBox_AutoRefresh_stateChanged()));


	SETTINGS_CMD_Pbsnodes = "pbsnodes";
	SETTINGS_CMD_Momctl_d3 = "momctl -d3 -h %1 -p %2";
	SETTINGS_CMD_Qstat_R = "qstat -R";
	SETTINGS_CMD_Qstat_f = "qstat -f %1";
	SETTINGS_CMD_Qmgr_c = "qmgr -c %1";
	// start MOM
	SETTINGS_CMD_StartMOM_HeadNode_Standard		= "pbs_mom";
	SETTINGS_CMD_StartMOM_ComputeNode_Standard	= "ssh -o BatchMode=yes root@%1 pbs_mom";		// the %1 is for the name of the node to start
	SETTINGS_CMD_StartMOM_HeadNode_Multimom		= "pbs_mom -A %1 -m -M %2 -R %3";
	SETTINGS_CMD_StartMOM_ComputeNode_Multimom	= "ssh -o BatchMode=yes root@%1 pbs_mom -A %1 -m -M %2 -R %3";	// the %1 is for the name of the node to start, and yes, it's duplicated twice in the string
	SETTINGS_CMD_StartMOM_HeadNode_Service		= "service pbs_mom start";
	SETTINGS_CMD_StartMOM_ComputeNode_Service	= "ssh -o BatchMode=yes root@%1 service pbs_mom start";	// the %1 is for the name of the node to start
	// stop MOM
	SETTINGS_CMD_StopMOM_HeadNode_Standard		= "momctl -s -h %1 -p %2";
	SETTINGS_CMD_StopMOM_ComputeNode_Standard	= "ssh -o BatchMode=yes root@%1 momctl -s -h %1 -p %2";	// the %1 is for the name of the node to stopt, and yes, it's duplicated twice
	SETTINGS_CMD_StopMOM_HeadNode_Service		= "service pbs_mom stop";
	SETTINGS_CMD_StopMOM_ComputeNode_Service	= "ssh -o BatchMode=yes root@%1 service pbs_mom stop";		// the %1 is for the name of the node to stop

	SETTINGS_CMD_MarkNodeAsOFFLINE = "pbsnodes -o %1";	// mark node as OFFLINE
	SETTINGS_CMD_ClearOFFLINEFromNode = "pbsnodes -c %1";	// clear OFFLINE from node
	SETTINGS_CMD_AddNote = "pbsnodes -N";	// add note -- do it this way because of the complexity of multiple levels of single/double quotes
	SETTINGS_CMD_RemoveNote = "pbsnodes -N n %1"; // remove note
	SETTINGS_CMD_Tail = "tail";				// tail log file
	SETTINGS_CMD_Grep = "grep";
	SETTINGS_CMD_Cat = "cat";				// cat a file
	SETTINGS_CMD_Scp_FromRemoteToLocal	= "scp";				// scp a file (remote to local)
	SETTINGS_CMD_Scp_FromLocalToRemote	= "scp";				// scp a file (local to remote)
	SETTINGS_CMD_Cp = "cp";					// copy a file
	SETTINGS_CMD_GetServerHome	 = "pbs_server --about";
//	SETTINGS_CMD_GetMOMHome		 = "pbs_mom --about";	// NOT NEEDED YET
	SETTINGS_CMD_ModifyJob		 = "qalter %1";
	SETTINGS_CMD_DeleteJob		 = "qdel %1";
	SETTINGS_CMD_PutJobOnHold	 = "qhold %1";
	SETTINGS_CMD_MoveJob		 = "qmove %1";
	SETTINGS_CMD_ReorderJob		 = "qorder %1";
	SETTINGS_CMD_RerunJob		 = "qrerun %1";
	SETTINGS_CMD_ReleaseHoldOnJob = "qrls %1";
	SETTINGS_CMD_RunJob			 = "qrun %1";
	SETTINGS_CMD_SelectJob		 = "qselect %1";
	SETTINGS_CMD_SendSignalToJob = "qsig %1";

	// init snapshot variables
	m_bTakingSnapshot = false;
	m_Snapshot_ScheduleMultiple = false;
	m_Snapshot_ScheduleSnapshots_Type = scheduleSnapshotsType_Minutes;  // init to Minutes (why not?)
	m_Snapshot_SnapshotSecondsVal = 0;
	m_Snapshot_SnapshotMinutesVal = 0;
	m_Snapshot_SnapshotHoursVal = 0;
	m_Snapshot_Iterations = 0;
	m_Snapshot_NoEnd = true;
	m_Snapshot_Filename = "";
	m_Snapshot_Comments = "";
	m_Snapshot_SaveIndividualNodeData = true;	// init to true
	m_Snapshot_SaveIndividualJobData = true;	// init to true

	setupStatusBar();

	readSettings();
	initAllTabs();

	CenterAppWindow();

}

/*******************************************************************************
 *
*******************************************************************************/
MainWindow::~MainWindow()
{
	if (m_autoRefresh_timerID != 0)
	{
		killTimer(m_autoRefresh_timerID);
	}
	if (m_createSnapshot_timerID != 0)
	{
		killTimer(m_createSnapshot_timerID);
	}
	killSnapshotReplayTimer();

	delete ui;
}

/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::on_actionAbout_triggered()
{
	AboutDlg dlg(this);
	dlg.exec();
}

/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}

QString MainWindow::getLineEdit_SnapshotFilename_Text()
{
	return ui->lineEdit_SnapshotFilename->text();
}
int MainWindow::getComboBox_DataSource_CurrentIndex()
{
	return ui->comboBox_DataSource->currentIndex();
}
bool MainWindow::takingSnapshot()
{
	return m_bTakingSnapshot;
}
//bool MainWindow::isCheckBox_AutoRefresh_Checked()
//{
//	return ui->checkBox_AutoRefresh->isChecked();
//}
//void MainWindow::setCheckBox_AutoRefresh_Checked(bool bChecked)
//{
//	ui->checkBox_AutoRefresh->setChecked( bChecked );
//}

bool MainWindow::isCurrentlyExecutingCmd()  // is TORQUEView currently executing some TORQUE client command (i.e, Run Job, Start MOM, etc.)
{
    return m_bIsCurrentlyExecutingCmd;
}

void MainWindow::setIsCurrentlyExecutingCmd(bool bFlag)  // is TORQUEView currently executing some TORQUE client command (i.e, Run Job, Start MOM, etc.)
{
    m_bIsCurrentlyExecutingCmd = bFlag;
}


/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::setupStatusBar()
{
    m_statusText = new QLabel(this);
    m_statusText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    btnCancelSnapshotScheduling = new QPushButton(this);
    btnCancelSnapshotReplay = new QPushButton(this);
    btnPauseSnapshotReplay = new QPushButton(this);
    btnResumeSnapshotReplay = new QPushButton(this);
    btnNextSnapshotReplay = new QPushButton(this);
    btnBackSnapshotReplay = new QPushButton(this);

    btnCancelSnapshotScheduling->setText("Cancel");  // Cancel snapshot scheduling
    btnCancelSnapshotReplay->setText("Cancel");     // Cancel snapshot replay
    btnPauseSnapshotReplay->setText("Pause");
    btnResumeSnapshotReplay->setText("Resume");
    btnNextSnapshotReplay->setText("Next");
    btnBackSnapshotReplay->setText("Back");

    connect(btnCancelSnapshotScheduling, SIGNAL(clicked()),
        this, SLOT(on_btnCancelSnapshotScheduling_clicked()));
    connect(btnCancelSnapshotReplay, SIGNAL(clicked()),
        this, SLOT(on_btnCancelSnapshotReplay_clicked()));
    connect(btnPauseSnapshotReplay, SIGNAL(clicked()),
        this, SLOT(on_btnPauseSnapshotReplay_clicked()));
    connect(btnResumeSnapshotReplay, SIGNAL(clicked()),
        this, SLOT(on_btnResumeSnapshotReplay_clicked()));
    connect(btnNextSnapshotReplay, SIGNAL(clicked()),
        this, SLOT(on_btnNextSnapshotReplay_clicked()));
    connect(btnBackSnapshotReplay, SIGNAL(clicked()),
        this, SLOT(on_btnBackSnapshotReplay_clicked()));

    statusBar()->addPermanentWidget(m_statusText);
    statusBar()->addPermanentWidget(btnPauseSnapshotReplay);
    statusBar()->addPermanentWidget(btnResumeSnapshotReplay);
    statusBar()->addPermanentWidget(btnBackSnapshotReplay);
    statusBar()->addPermanentWidget(btnNextSnapshotReplay);
    statusBar()->addPermanentWidget(btnCancelSnapshotReplay);
    statusBar()->addPermanentWidget(btnCancelSnapshotScheduling);
    m_statusText->setVisible(false);
    btnCancelSnapshotScheduling->setVisible(false);
    btnCancelSnapshotReplay->setVisible(false);
    btnPauseSnapshotReplay->setVisible(false);
    btnResumeSnapshotReplay->setVisible(false);
    btnNextSnapshotReplay->setVisible(false);
    btnBackSnapshotReplay->setVisible(false);
}

/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::killSnapshotReplayTimer()
{
    if (m_replaySnapshot_timerID != 0)
    {
        killTimer(m_replaySnapshot_timerID);
        m_replaySnapshot_timerID = 0;
    }
}

/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::startSnapshotReplayTimer(int secs)
{
    int millisecs = secs * 1000;	// convert to millisecs
    m_replaySnapshot_timerID = startTimer(millisecs);	// restart the timer with the new value
}


/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::showCancelSnapshotSchedulingButton(QString msg)	// show the CancelSnapshotScheduling button (and message to the left of the Cancel button)
{
    btnCancelSnapshotScheduling->setVisible(true);

    m_statusText->setStyleSheet("QLabel { background-color : lightgreen; color : black; }");
    // give string some padding so it won't be so close to the edge of the statusbar box
    msg.prepend("  ");
    msg.append("  ");
    m_statusText->setVisible(true);
    m_statusText->setText(msg);
}

/*******************************************************************************
 *
*******************************************************************************/
// entering SnapshotReplay "Play" (or "Resume") state, so show SnapshotReplay buttons
// i.e, show the Pause, Resume, Next, Back, Cancel buttons, but only enable the
// Cancel and Pause buttons
void MainWindow::enteringSnapshotPlayState(QString msg)
{
    // show buttons
    btnCancelSnapshotReplay->setVisible(true);
    btnPauseSnapshotReplay->setVisible(true);
    btnResumeSnapshotReplay->setVisible(true);
    btnNextSnapshotReplay->setVisible(true);
    btnBackSnapshotReplay->setVisible(true);
    // enable/disable buttons
    btnCancelSnapshotReplay->setEnabled(true); // just enable Cancel and Pause buttons
    btnPauseSnapshotReplay->setEnabled(true);
    btnResumeSnapshotReplay->setEnabled(false);
    btnNextSnapshotReplay->setEnabled(false);
    btnBackSnapshotReplay->setEnabled(false);

    m_statusText->setStyleSheet("QLabel { background-color : violet; color : black; }");
    // give string some padding so it won't be so close to the edge of the statusbar box
    msg.prepend("  ");
    msg.append("  ");
    m_statusText->setVisible(true);
    m_statusText->setText(msg);
}

/*******************************************************************************
 *
*******************************************************************************/
// entering snapshotReplay "Pause" state, so enable all but the Pause button
void MainWindow::enteringSnapshotPauseState(QString msg)
{
    // enable/disable buttons
    btnCancelSnapshotReplay->setEnabled(true);
    btnPauseSnapshotReplay->setEnabled(false);
    btnResumeSnapshotReplay->setEnabled(true);
    btnNextSnapshotReplay->setEnabled(true);
    btnBackSnapshotReplay->setEnabled(true);

    m_statusText->setStyleSheet("QLabel { background-color : violet; color : black; }");
    // give string some padding so it won't be so close to the edge of the statusbar box
    msg.prepend("  ");
    msg.append("  ");
    m_statusText->setVisible(true);
    m_statusText->setText(msg);
}

/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::hideCancelSnapshotSchedulingButton()	// hide the message and button
{
    btnCancelSnapshotScheduling->setVisible(false);
    m_statusText->setVisible(false);
}

/*******************************************************************************
 *
*******************************************************************************/
// exiting SnapshotReplay "Play" state, so hide all the SnapshotReplay buttons
void MainWindow::exitingSnapshotPlayState()
{
    btnCancelSnapshotReplay->setVisible(false);
    btnPauseSnapshotReplay->setVisible(false);
    btnResumeSnapshotReplay->setVisible(false);
    btnNextSnapshotReplay->setVisible(false);
    btnBackSnapshotReplay->setVisible(false);

    m_statusText->setVisible(false);
}

/*******************************************************************************
 *
*******************************************************************************/
// exiting snapshotReplay "Pause" state, so show all the buttons, but only enable the
// Cancel and Pause buttons
void MainWindow::exitingSnapshotPauseState(QString msg)
{
    // enable/disable buttons
    btnCancelSnapshotReplay->setEnabled(true); // just enable Cancel and Pause buttons
    btnPauseSnapshotReplay->setEnabled(true);
    btnResumeSnapshotReplay->setEnabled(false);
    btnNextSnapshotReplay->setEnabled(false);
    btnBackSnapshotReplay->setEnabled(false);

    m_statusText->setStyleSheet("QLabel { background-color : violet; color : black; }");
    // give string some padding so it won't be so close to the edge of the statusbar box
    msg.prepend("  ");
    msg.append("  ");
    m_statusText->setVisible(true);
    m_statusText->setText(msg);
}

/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::on_btnCancelSnapshotScheduling_clicked()
{
	if (m_createSnapshot_timerID != 0)
	{
		killTimer(m_createSnapshot_timerID);
		m_createSnapshot_timerID = 0;
	}
	hideCancelSnapshotSchedulingButton();
}

/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::on_btnCancelSnapshotReplay_clicked()
{
    killSnapshotReplayTimer();
    exitingSnapshotPlayState();

    m_SnapshotReplay_Filelist_Index = 0;  // reset
    m_SnapshotReplay_Filelist.clear(); // reset
}

/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::on_btnPauseSnapshotReplay_clicked()
{
    killSnapshotReplayTimer();
    // change the text message on the statusbar
    int count = m_SnapshotReplay_Filelist.count();
    QString text = QString("Snapshot Replay: PAUSED,  Showing snapshot: %1 of %2")
            .arg(m_SnapshotReplay_Filelist_Index + 1) // (is 0-indexed)
            .arg(count);

    enteringSnapshotPauseState(text);
}

/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::on_btnResumeSnapshotReplay_clicked()
{
    int count = m_SnapshotReplay_Filelist.count();
    QString text = QString("Snapshot Replay: RESUMED,  Total count: %1, Frequency: every %2 secs")
            .arg(count)
            .arg(m_SnapshotReplay_TimeIntervalInSecs);

    exitingSnapshotPauseState(text);
    // restart the timer with the saved value (using the saved filelist index)
    startSnapshotReplayTimer(m_SnapshotReplay_TimeIntervalInSecs);
}

/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::on_btnNextSnapshotReplay_clicked()
{
    int count = m_SnapshotReplay_Filelist.count();
    if (m_SnapshotReplay_Filelist_Index >= (count - 1))
        return; // we're already at the end of the list, so nothing to do

    m_SnapshotReplay_Filelist_Index++;  // inc index (m_SnapshotReplay_Filelist_Index always needs to have something valid)
    doReplaySnapshot();

    QString msg = QString("Snapshot Replay: STEPPED FORWARD;  Showing snapshot: %1 of %2")
            .arg(m_SnapshotReplay_Filelist_Index + 1) // (is 0-indexed)
            .arg(count);
    m_statusText->setStyleSheet("QLabel { background-color : violet; color : black; }");
    // give string some padding so it won't be so close to the edge of the statusbar box
    msg.prepend("  ");
    msg.append("  ");
    m_statusText->setVisible(true);
    m_statusText->setText(msg);
}

/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::on_btnBackSnapshotReplay_clicked()
{
    if (m_SnapshotReplay_Filelist_Index == 0)
        return; // we're already at the beginning of the list, so nothing to do

    m_SnapshotReplay_Filelist_Index--;  // dec index (m_SnapshotReplay_Filelist_Index always needs to have something valid)
    doReplaySnapshot();

    int count = m_SnapshotReplay_Filelist.count();
    QString msg = QString("Snapshot Replay: STEPPED BACK;  Showing snapshot: %1 of %2")
            .arg(m_SnapshotReplay_Filelist_Index + 1) // (is 0-indexed)
            .arg(count);
    m_statusText->setStyleSheet("QLabel { background-color : violet; color : black; }");
    // give string some padding so it won't be so close to the edge of the statusbar box
    msg.prepend("  ");
    msg.append("  ");
    m_statusText->setVisible(true);
    m_statusText->setText(msg);
}


/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::readSettings()
{
	// get the settings
	QSettings settings( "AdaptiveComputing", "TORQUEView" );

	// load the style sheet
	QString stylesheetname = settings.value( "StylesheetName" ).toString( );
	if ( !stylesheetname.isEmpty( ) )
	{
		LoadStyleSheet( stylesheetname );
	}
	else
	{
		LoadStyleSheet("Default");
	}

	QString filename = settings.value( "TORQUEViewSnapshotFileName" ).toString( );
	ui->lineEdit_SnapshotFilename->setText(filename);

//    settings.remove("HeatMapRowCount");   // TESTING ONLY
//    settings.remove("HeatMapRowHeight");    // TESTING ONLY
//    settings.remove("HeatMapColumnWidth");  // TESTING ONLY

    m_Config_HeatMapShowNodeNames = settings.value( "HeatMapShowNodeNames", true ).toBool( );

    m_Config_HeatMapRowCount_NoNodeNames = settings.value( "HeatMapRowCount_NoNodeNames", "12" ).toInt( );
    m_Config_HeatMapRowHeight_NoNodeNames = settings.value( "HeatMapRowHeight_NoNodeNames", "23" ).toInt( );
    m_Config_HeatMapColumnWidth_NoNodeNames = settings.value( "HeatMapColumnWidth_NoNodeNames", "22" ).toInt( );

    m_Config_HeatMapRowCount_WithNodeNames = settings.value( "HeatMapRowCount_WithNodeNames", "8" ).toInt( );
    m_Config_HeatMapRowHeight_WithNodeNames = settings.value( "HeatMapRowHeight_WithNodeNames", "22" ).toInt( );
    m_Config_HeatMapColumnWidth_WithNodeNames = settings.value( "HeatMapColumnWidth_WithNodeNames", "72" ).toInt( );

	m_Config_DataSource = settings.value( "DataSource", 0 ).toInt( );  // 0="Local host", 1="Remote host", or 2="Snapshot file"
	m_Config_RemoteServer = settings.value( "RemoteServer" ).toString( );
	m_Config_RemoteServerList = settings.value( "RemoteServerList" ).toStringList( );
	m_Config_UsingMultiMoms = settings.value( "UsingMultiMoms", false ).toBool( );
	m_Config_UseServiceToStartStopMOMs = settings.value( "UseServiceToStartStopMOMs", false ).toBool( );
//	m_Config_ShowSTDERROutput = settings.value( "ShowSTDERROutput", true ).toBool( );

	// just for init purposes, make sure remoteServer is in list
	if (! m_Config_RemoteServerList.contains(m_Config_RemoteServer))
		m_Config_RemoteServerList.append(m_Config_RemoteServer);


	getConfigCmds();
}

/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::getConfigCmds()
{
    if (m_Config_DataSource == 0) // if "Local host"
    {
        m_Config_Cmd_Pbsnodes = SETTINGS_CMD_Pbsnodes;
        m_Config_Cmd_Momctl_d3 = SETTINGS_CMD_Momctl_d3;
        m_Config_Cmd_Qstat_R = SETTINGS_CMD_Qstat_R;
        m_Config_Cmd_Qstat_f = SETTINGS_CMD_Qstat_f;
        m_Config_Cmd_Qmgr_c = SETTINGS_CMD_Qmgr_c;

        m_Config_Cmd_StartMOM_HeadNode_Standard     = SETTINGS_CMD_StartMOM_HeadNode_Standard;
        m_Config_Cmd_StartMOM_ComputeNode_Standard  = SETTINGS_CMD_StartMOM_ComputeNode_Standard;
        m_Config_Cmd_StartMOM_HeadNode_Multimom     = SETTINGS_CMD_StartMOM_HeadNode_Multimom;
        m_Config_Cmd_StartMOM_ComputeNode_Multimom  = SETTINGS_CMD_StartMOM_ComputeNode_Multimom;
        m_Config_Cmd_StartMOM_HeadNode_Service      = SETTINGS_CMD_StartMOM_HeadNode_Service;
        m_Config_Cmd_StartMOM_ComputeNode_Service   = SETTINGS_CMD_StartMOM_ComputeNode_Service;

        m_Config_Cmd_StopMOM_HeadNode_Standard      = SETTINGS_CMD_StopMOM_HeadNode_Standard;
        m_Config_Cmd_StopMOM_ComputeNode_Standard   = SETTINGS_CMD_StopMOM_ComputeNode_Standard;
        m_Config_Cmd_StopMOM_HeadNode_Service       = SETTINGS_CMD_StopMOM_HeadNode_Service;
        m_Config_Cmd_StopMOM_ComputeNode_Service    = SETTINGS_CMD_StopMOM_ComputeNode_Service;

        m_Config_Cmd_MarkNodeAsOFFLINE = SETTINGS_CMD_MarkNodeAsOFFLINE;
        m_Config_Cmd_ClearOFFLINEFromNode = SETTINGS_CMD_ClearOFFLINEFromNode;
        m_Config_Cmd_AddNote = SETTINGS_CMD_AddNote;
        m_Config_Cmd_RemoveNote = SETTINGS_CMD_RemoveNote;
        m_Config_Cmd_Tail = SETTINGS_CMD_Tail;
        m_Config_Cmd_Grep = SETTINGS_CMD_Grep;
        m_Config_Cmd_Cat = SETTINGS_CMD_Cat;
        m_Config_Cmd_Scp_FromRemoteToLocal = SETTINGS_CMD_Scp_FromRemoteToLocal;
        m_Config_Cmd_Scp_FromLocalToRemote = SETTINGS_CMD_Scp_FromLocalToRemote;
        m_Config_Cmd_Cp = SETTINGS_CMD_Cp;
        m_Config_Cmd_GetServerHome = SETTINGS_CMD_GetServerHome;
//      m_Config_Cmd_GetMOMHome = SETTINGS_CMD_GetMOMHome;  // NOT NEEDED YET

		m_Config_Cmd_ModifyJob = SETTINGS_CMD_ModifyJob;
		m_Config_Cmd_DeleteJob = SETTINGS_CMD_DeleteJob;
		m_Config_Cmd_PutJobOnHold = SETTINGS_CMD_PutJobOnHold;
		m_Config_Cmd_MoveJob = SETTINGS_CMD_MoveJob;
		m_Config_Cmd_ReorderJob = SETTINGS_CMD_ReorderJob;
		m_Config_Cmd_RerunJob = SETTINGS_CMD_RerunJob;
		m_Config_Cmd_ReleaseHoldOnJob = SETTINGS_CMD_ReleaseHoldOnJob;
		m_Config_Cmd_RunJob = SETTINGS_CMD_RunJob;
		m_Config_Cmd_SelectJob = SETTINGS_CMD_SelectJob;
		m_Config_Cmd_SendSignalToJob = SETTINGS_CMD_SendSignalToJob;

    }
    else // else Remote host (or snapshot file, but set Remote values anyway...)
    {
        m_Config_Cmd_Pbsnodes = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_Pbsnodes);
        m_Config_Cmd_Momctl_d3 = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_Momctl_d3);
        m_Config_Cmd_Qstat_R = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_Qstat_R);
        m_Config_Cmd_Qstat_f = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_Qstat_f);
        m_Config_Cmd_Qmgr_c = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_Qmgr_c);

        m_Config_Cmd_StartMOM_HeadNode_Standard     = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_StartMOM_HeadNode_Standard);
        m_Config_Cmd_StartMOM_ComputeNode_Standard  = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_StartMOM_ComputeNode_Standard);
        m_Config_Cmd_StartMOM_HeadNode_Multimom     = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_StartMOM_HeadNode_Multimom);
        m_Config_Cmd_StartMOM_ComputeNode_Multimom  = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_StartMOM_ComputeNode_Multimom);
        m_Config_Cmd_StartMOM_HeadNode_Service      = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_StartMOM_HeadNode_Service);
        m_Config_Cmd_StartMOM_ComputeNode_Service   = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_StartMOM_ComputeNode_Service);

        m_Config_Cmd_StopMOM_HeadNode_Standard      = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_StopMOM_HeadNode_Standard);
        m_Config_Cmd_StopMOM_ComputeNode_Standard   = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_StopMOM_ComputeNode_Standard);
        m_Config_Cmd_StopMOM_HeadNode_Service       = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_StopMOM_HeadNode_Service);
        m_Config_Cmd_StopMOM_ComputeNode_Service    = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_StopMOM_ComputeNode_Service);

        m_Config_Cmd_MarkNodeAsOFFLINE = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_MarkNodeAsOFFLINE);
        m_Config_Cmd_ClearOFFLINEFromNode = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_ClearOFFLINEFromNode);
        m_Config_Cmd_AddNote = QString("ssh -o BatchMode=yes %1 \"%2\"").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_AddNote);
        m_Config_Cmd_RemoveNote = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_RemoveNote);
        m_Config_Cmd_Tail = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_Tail);
        m_Config_Cmd_Grep = QString("ssh -o BatchMode=yes %1 %2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_Grep);
        m_Config_Cmd_Cat = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_Cat);
        m_Config_Cmd_Scp_FromRemoteToLocal = QString("%1 %2").arg(SETTINGS_CMD_Scp_FromRemoteToLocal).arg(m_Config_RemoteServer);
        m_Config_Cmd_Scp_FromLocalToRemote = QString("%1").arg(SETTINGS_CMD_Scp_FromLocalToRemote);
        m_Config_Cmd_Cp = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_Cp);
        m_Config_Cmd_GetServerHome = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_GetServerHome);
  //    m_Config_Cmd_GetMOMHome = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_GetMOMHome);

        m_Config_Cmd_ModifyJob        = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_ModifyJob);
        m_Config_Cmd_DeleteJob        = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_DeleteJob);
        m_Config_Cmd_PutJobOnHold     = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_PutJobOnHold);
        m_Config_Cmd_MoveJob          = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_MoveJob);
        m_Config_Cmd_ReorderJob       = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_ReorderJob);
        m_Config_Cmd_RerunJob         = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_RerunJob);
        m_Config_Cmd_ReleaseHoldOnJob = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_ReleaseHoldOnJob);
        m_Config_Cmd_SelectJob        = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_SelectJob);
        m_Config_Cmd_SendSignalToJob  = QString("ssh -o BatchMode=yes %1 \"%2").arg(m_Config_RemoteServer).arg(SETTINGS_CMD_SendSignalToJob);

    }
}

/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::switchToPbsNodesTab()
{
    ui->tabWidget->setCurrentIndex( tabIndex_PbsNodesTab );	// switch to PbsNodes tab
}

/*******************************************************************************
 *  tell PbsNodes tab to show all nodes running job <jobID>
*******************************************************************************/
void MainWindow::showNodesRunningJob( QString jobId )
{
    m_pbsNodesTab->showNodesRunningJob( jobId ); // also pass in exec_host list
}


/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::initAllTabs(bool bIncludeQmgr)
{
	bool bStatus = true; // success

	ui->comboBox_DataSource->blockSignals(true);	// prevent currentIndexChanged signals from being received
	ui->comboBox_Remote_Servers->blockSignals(true);	// prevent currentIndexChanged signals from being received


	if (m_Config_DataSource == 0 || m_Config_DataSource == 1) //if datasource is either 0="Local host" or 1="Remote host"
	{
		ui->label_SnapshotFilename->hide();
		ui->lineEdit_SnapshotFilename->hide();
		ui->btnBrowse->hide();

		m_pbsNodesTab->issueCmd_GetServerHome();	// get the pbs_server's "serverhome" directory
//		m_pbsNodesTab->issueCmd_GetMOMHome();		// get the pbs_mom's "serverhome" directory  -- NOT NEEDED YET

		if (m_Config_DataSource == 1) // if is "Remote"
		{
			ui->comboBox_DataSource->setCurrentIndex(1);	// 1 == Remote

			ui->comboBox_Remote_Servers->clear();
			foreach (QString s, m_Config_RemoteServerList)
				ui->comboBox_Remote_Servers->addItem(s);
			if (! m_Config_RemoteServer.isEmpty() )
			{
				int itemIndex = ui->comboBox_Remote_Servers->findText( m_Config_RemoteServer );  // now try to find it in the combobox list
				if (itemIndex != -1)  // if we found it
					ui->comboBox_Remote_Servers->setCurrentIndex(itemIndex); // select it
			}

			ui->label_RemoteServers->show();
			ui->comboBox_Remote_Servers->show();  // do this last so it resizes the combobox to fit the longest string
		}
		else  // else is "Local"
		{
			ui->label_RemoteServers->hide();
			ui->comboBox_DataSource->setCurrentIndex(0);	// 0 == Local
			ui->label_RemoteServers->hide();
			ui->comboBox_Remote_Servers->hide();
		}


		m_pbsNodesTab->clearLists();
		m_qstatTab->clearLists();
		if (bIncludeQmgr)
			m_qmgrTab->clearLists();
		m_pbsServerTab->clearLists();
		m_heatMapTab->clearLists();
		// data is "live" (i.e, from either a local or remote host instead of a snapshot file), so
		// enable all applicable context menu items and buttons
		m_heatMapTab->enableControls();
		m_pbsNodesTab->enableControls();
		m_qstatTab->enableControls();
		m_qmgrTab->enableControls();
		m_pbsServerTab->enableControls();

//		QString lastJobId = m_qstatTab->getLastJobId();


		bStatus = m_pbsNodesTab->issueCmd_Pbsnodes();	// re-init the pbsnodes lists from the cmdline "pbnodes" command
		if (bStatus)
			bStatus = m_pbsNodesTab->selectLastPbsnode();  // select last pbsnode in the Node list (to populate the other pbsnodes lists)
		if (bStatus)
			bStatus = m_heatMapTab->updateLists();  // also use the data in m_pbsNodesTab to update the heat map
		if (bStatus)
			bStatus = m_qstatTab->issueCmd_Qstat_R();	// re-init the qstat lists from the cmdline "qstat" command
		if (bStatus)
			bStatus = m_qstatTab->selectLastJobId();  // select last JobId in the Job list (to populate the other qstat lists)
		if (bStatus)
		{
			if (bIncludeQmgr)
			{
				m_qmgrTab->issueCmd_Qmgr("p s");
			}
		}
		if (bStatus)
			bStatus = m_pbsServerTab->issueCmd_PbsServer();
		if (bStatus)
			bStatus = m_pbsServerTab->issueCmd_LoadNodesFile();


//		ui->btnBrowse->setEnabled(false);
//		ui->label_SnapshotFilename->setEnabled(false);
//		ui->lineEdit_SnapshotFilename->setEnabled(false);
	}
	else  // else dataSource is from a snapshot file
	{
		ui->label_SnapshotFilename->show();
		ui->lineEdit_SnapshotFilename->show();
		ui->btnBrowse->show();
		ui->label_RemoteServers->hide();
		ui->comboBox_Remote_Servers->hide();

		// read in file and init all tabs from the one file

		QString filename = ui->lineEdit_SnapshotFilename->text();
		if (filename.isEmpty())
			return;
		QFileInfo info(filename);
		if (!info.exists())
		{
			QMessageBox::information(NULL, "TORQUEView Snapshot file not found",
				QString("The snapshot file: '%1' ' does not exist.").arg(filename), QMessageBox::Ok,  QMessageBox::Ok);
			return;
		}

		m_pbsNodesTab->clearLists();
		m_qstatTab->clearLists();
		m_qmgrTab->clearLists();
		m_pbsServerTab->clearLists();
		m_heatMapTab->clearLists();
		// data is NOT "live" (i.e, it's from a snapshot file rather than a local or remote host), so
		// disable all applicable context menu items and buttons
		m_heatMapTab->disableControls();
		m_pbsNodesTab->disableControls();
		m_qstatTab->disableControls();
		m_qmgrTab->disableControls();
		m_pbsServerTab->disableControls();

		ui->comboBox_DataSource->setCurrentIndex(2);  // 2=data is from Snapshot file

		QFile file(filename);
		if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QTextStream in(&file);
			while (!in.atEnd())
			{
				QString line = in.readLine();
				if (line.isEmpty())
					continue;

				// parse the TORQUEView data ----------------------

				// skip header lines
				if (line.startsWith("#---------") ||  // if it starts with a line of "-----"'s
					//	line.startsWith("TORQUEView") ||
						line.startsWith("TORQUEView Version:") ||
						line.startsWith("Date:") ||
						line.startsWith("Comments:"))
					continue;

				if (line.startsWith("TORQUEView -- Pbsnode Output"))
				{
					// read in all "pbsnodes" output lines from file
					line = in.readLine(); // read in next line and discard -- it's the "#------" line
					m_pbsNodesTab->initPbsnodesFromFile(in);
					m_heatMapTab->updateLists();  // also use the data in m_pbsNodesTab to update the heat map
				}
				else if (line.startsWith("TORQUEView -- MomCtl Output"))
				{
					// read in all "momctl -d3" output lines from file
					line = in.readLine(); // read in next line and discard -- it's the "#------" line
					m_pbsNodesTab->initMomctlsFromFile(in);
				}
				else if (line.startsWith("TORQUEView -- Qstat -R Output"))
				{
					// read in all "qstat -R" output lines from file
					line = in.readLine(); // read in next line and discard -- it's the "#------" line
					m_qstatTab->initQstatFromFile(in);
				}
				else if (line.startsWith("TORQUEView -- Qstat -f Output"))
				{
					// read in all "qstat -f" output lines from file
					line = in.readLine(); // read in next line and discard -- it's the "#------" line
					m_qstatTab->initQstats_f_FromFile(in);
				}
				else if (line.startsWith("TORQUEView -- Qmgr Output"))
				{
					// read in all "qmgr" output lines from file
					line = in.readLine(); // read in next line and discard -- it's the "#------" line
					m_qmgrTab->initQmgrFromFile(in);
				}
				else if (line.startsWith("TORQUEView -- Pbs_server Output"))
				{
					// read in all "pbs_server" output lines from file
					line = in.readLine(); // read in next line and discard -- it's the "#------" line
					m_pbsServerTab->initPbsServerFromFile(in);
				}
				else if (line.startsWith("TORQUEView -- Nodes File Contents"))
				{
					// read in all "nodes" file contents from file
					line = in.readLine(); // read in next line and discard -- it's the "#------" line
					m_pbsServerTab->initNodesContentsFromFile(in);
				}

			}
		}

//		ui->btnBrowse->setEnabled(true);
//		ui->label_SnapshotFilename->setEnabled(true);
//		ui->lineEdit_SnapshotFilename->setEnabled(true);
    }

    if (m_Config_HeatMapShowNodeNames)
        m_heatMapTab->checkShowNodeNamesCheckbox(true);
    else
        m_heatMapTab->checkShowNodeNamesCheckbox(false);

    ui->comboBox_DataSource->blockSignals(false);	// allow currentIndexChanged signals to be received
    ui->comboBox_Remote_Servers->blockSignals(false);	// allow currentIndexChanged signals to be received
}


/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::on_checkBox_AutoRefresh_stateChanged()
{
	if (ui->checkBox_AutoRefresh->isChecked())
	{
		int secs = ui->spinBox_AutoRefresh->value();	// value in secs
		int millisecs = secs * 1000;	// convert to millisecs

		if (m_autoRefresh_timerID != 0)
		{
			killTimer(m_autoRefresh_timerID);	// kill the timer
			m_autoRefresh_timerID = 0;
		}
		m_autoRefresh_timerID = startTimer(millisecs);	// restart the timer with the new value

		// when auto-refresh is checked, disable the Refresh button (so as to avoid contention)
		ui->btnRefresh->setEnabled(false);

		on_btnCancelSnapshotReplay_clicked();  // make sure SnapshotReplay isn't running
	}
	else
	{
		if (m_autoRefresh_timerID != 0)
		{
			killTimer(m_autoRefresh_timerID);
			m_autoRefresh_timerID = 0;
		}

		// when auto-refresh is not checked, re-enable the Refresh button
		ui->btnRefresh->setEnabled(true);
		statusBar()->clearMessage();	// clear off any "List Updated" message on statusbar

	}
}


/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::on_comboBox_DataSource_activated ( int index )
{
	m_Config_DataSource = index;

	switch(index) {
	case 0: // "Local host"
	{
		ui->label_RemoteServers->hide();
		ui->comboBox_Remote_Servers->hide();
		ui->label_SnapshotFilename->hide();
		ui->lineEdit_SnapshotFilename->hide();
		ui->btnBrowse->hide();

		on_btnCancelSnapshotReplay_clicked();  // make sure SnapshotReplay isn't running
		break;
	}
	case 1: // "Remote host"
	{
		ui->label_RemoteServers->show();
		ui->comboBox_Remote_Servers->show();
		ui->label_SnapshotFilename->hide();
		ui->lineEdit_SnapshotFilename->hide();
		ui->btnBrowse->hide();

		QString remoteServer = ui->comboBox_Remote_Servers->currentText();
		m_Config_RemoteServer = remoteServer;

		on_btnCancelSnapshotReplay_clicked();  // make sure SnapshotReplay isn't running
		break;
	}
	case 2: // "Snapshot file"
	{
		ui->label_RemoteServers->hide();
		ui->comboBox_Remote_Servers->hide();
		ui->label_SnapshotFilename->show();
		ui->lineEdit_SnapshotFilename->show();
		ui->btnBrowse->show();
		break;
	}
	} // switch

//	m_pbsNodesTab->on_btnClear_clicked();	// clear out any "show nodes running current job" stuff
//	m_pbsNodesTab->clearNodesRunningJob();	// clear out any "show nodes running current job" stuff

	ui->checkBox_AutoRefresh->setChecked(false);	// since data source changed, turn off Auto=Refresh

	// also, turn off ShowNodesRunningJobId checkbox (otherwise the nodes list will
	// usually be blank and the user won't know why)
	m_pbsNodesTab->checkShowNodesRunningJobIDCheckbox(false);

	QSettings mysettings( "AdaptiveComputing", "TORQUEView" );
	mysettings.setValue( "DataSource", index );  // do this BEFORE calling readSettings! -- it depends on it

	// reset the "DontShowThisAgain" checkbox in all STDERR error output dialogs (show the checkboxes)
	m_heatMapTab->resetErrorMsgDlg();
	m_pbsNodesTab->resetErrorMsgDlg();
	m_qstatTab->resetErrorMsgDlg();
	m_qmgrTab->resetErrorMsgDlg();

	readSettings();
	initAllTabs( true );  // include qmgr
}

/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::on_comboBox_Remote_Servers_currentIndexChanged ( const QString & text )
{
    m_Config_RemoteServer = text;

//	m_pbsNodesTab->on_btnClear_clicked();	// clear out any "show nodes running current job" stuff
//	m_pbsNodesTab->clearNodesRunningJob();	// clear out any "show nodes running current job" stuff

	getConfigCmds();// important -- update Config command variables!
	initAllTabs( true ); // include qmgr

    // reset the "DontShowThisAgain" checkbox in all STDERR error output dialogs (show the checkboxes)
    m_heatMapTab->resetErrorMsgDlg();
    m_pbsNodesTab->resetErrorMsgDlg();
    m_qstatTab->resetErrorMsgDlg();
    m_qmgrTab->resetErrorMsgDlg();

	// save off settings
	QSettings mysettings( "AdaptiveComputing", "TORQUEView" );
	mysettings.setValue( "RemoteServer", m_Config_RemoteServer );
}

/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::timerEvent(QTimerEvent *event)
{
    int timerId = event->timerId();
    QDateTime now = QDateTime::currentDateTime();
//  QString sNow = now.toString("hh:mm:ss");
    QString sNow = now.toString("yyyy/MM/dd  hh:mm:ss");

    // if TORQUEView is currently executing a TORQUE client command, then just return
    if (isCurrentlyExecutingCmd())
        return;

	if (timerId == m_autoRefresh_timerID)
	{
	//	qDebug() << "Timer ID:" << event->timerId();

		// before we clear the Job list in the Jobs tab (in initAllTabs()), remember which Job list
		// items were expanded (so we can re-expanded them later)
		m_qstatTab->getExpandedState();
		initAllTabs(false); // don't include qmgr - Ken says it doesn't need to be updated on refresh

	//  QString s = QString("Update: %1;  refreshed lists at %2...").arg(m_nUpdateCount).arg(sNow);
		QString s = QString("%1  --  Lists have been updated").arg(sNow);
		int secs = ui->spinBox_AutoRefresh->value();	// value in secs

		int millisecs = (secs * 1000) - 500;
		millisecs = qMax(millisecs, 4000);  // show for at least 4 secs
		statusBar()->showMessage(s, millisecs);  // so there's a small gap inbetween showing time of last update
	}
	else if (timerId == m_createSnapshot_timerID)
	{
		// if still more iterations remaining (or if endless)
		if (m_Snapshot_NoEnd || m_Snapshot_Iterations > 0)
		{
			doTakeSnapshot();
			QString s;
			if (m_Snapshot_NoEnd)
			{
				s = QString("%1  --  Snapshot was taken").arg(sNow);
			}
			else
			{
				m_Snapshot_Iterations--;
				s = QString("%1  --  Snapshot was taken;  %2  iterations remaining")
					.arg(sNow)
					.arg(m_Snapshot_Iterations);
				if (m_Snapshot_Iterations == 0)
				{
					on_btnCancelSnapshotScheduling_clicked();   // kill timerID and clear out button and message
				}
			}
			statusBar()->showMessage(s, 12000);
		}
	}
	else if (timerId == m_replaySnapshot_timerID)
	{
		int count = m_SnapshotReplay_Filelist.count();
		if (m_SnapshotReplay_Filelist_Index >= (count-1))
		{
			// we're done - we've replayed all the snapshot files in the list
			on_btnCancelSnapshotReplay_clicked();
			return;
		}
		doReplaySnapshot();
		if (m_SnapshotReplay_Filelist_Index < (count - 1))
			m_SnapshotReplay_Filelist_Index++;  // inc index (m_SnapshotReplay_Filelist_Index always needs to have something valid)
	}
}

/*******************************************************************************
 * sNow has the current date/time
*******************************************************************************/
void MainWindow::doReplaySnapshot()
{
    // (we've already checked the index to make sure it's not out of range)

    QDateTime now = QDateTime::currentDateTime();
    QString sNow = now.toString("yyyy/MM/dd  hh:mm:ss");

    QString sSnapshotFilename = m_SnapshotReplay_Filelist[m_SnapshotReplay_Filelist_Index]; // (m_SnapshotReplay_Filelist_Index always needs to have something valid)
    ui->lineEdit_SnapshotFilename->setText(sSnapshotFilename);
    initAllTabs(false); // don't include qmgr - Ken says it doesn't need to be updated on refresh

    QString s = QString("%1  --  Snapshot '%2' was replayed").arg(sNow).arg(sSnapshotFilename);
    statusBar()->showMessage(s, 12000);
}

/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::on_btnRefresh_clicked()
{
    on_actionRefresh_triggered();
}

/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::on_actionRefresh_triggered()
{
	on_btnCancelSnapshotReplay_clicked();  // make sure SnapshotReplay isn't running

	initAllTabs(false); // don't include qmgr - Ken says it doesn't need to be updated on refresh

	QString s = "Lists have been refreshed...";
	statusBar()->showMessage(s, 2000);

}

/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::on_btnReplaySnapshots_clicked()
{
    // make sure auto-refresh is OFF
    if (ui->checkBox_AutoRefresh->isChecked())
        ui->checkBox_AutoRefresh->setChecked(false);  // turn off Auto-Refresh

	ReplaySnapshotsDlg dlg(this);
	if (!dlg.exec())
		return;

    m_SnapshotReplay_Filelist_Index = 0;  // reset index
    m_SnapshotReplay_Filelist = dlg.SnapshotFilelist();
    if (m_SnapshotReplay_Filelist.count() == 0)
        return;

    // no matter what it was, switch data source to 2 (Snapshot file)
//  ui->comboBox_DataSource->blockSignals(true);	// prevent currentIndexChanged signals from being received
    ui->comboBox_DataSource->setCurrentIndex(2);  // 2=data is from Snapshot file
//  ui->comboBox_DataSource->blockSignals(false);	// allow currentIndexChanged signals to be received
    m_Config_DataSource = 2;
    QSettings mysettings( "AdaptiveComputing", "TORQUEView" );
    mysettings.setValue( "DataSource", m_Config_DataSource );


    on_btnCancelSnapshotScheduling_clicked();  // make sure Snapshot Scheduling is turned off

    int secs = dlg.TimeIntervalBetweenReplays();
    m_SnapshotReplay_TimeIntervalInSecs = secs;


    killSnapshotReplayTimer();
    doReplaySnapshot();
    int count = m_SnapshotReplay_Filelist.count();
    if (m_SnapshotReplay_Filelist_Index < (count - 1))
        m_SnapshotReplay_Filelist_Index++;  // inc index (m_SnapshotReplay_Filelist_Index always needs to have something valid)
    startSnapshotReplayTimer(m_SnapshotReplay_TimeIntervalInSecs); // restart the timer with the new value
    // show the "Cancel Snapshot Replay" button on the statusbar
    QString text = QString("Snapshot Replay: PLAYING,  Total count: %1, Frequency: every %2 secs")
            .arg(count)
            .arg(secs);
    enteringSnapshotPlayState( text );
}

/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::on_actionReplay_Snapshots_triggered()
{
    on_btnReplaySnapshots_clicked();
}


/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::on_btnBrowse_clicked()
{
    on_actionOpen_triggered();

}

/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::on_actionOpen_triggered()
{
	QString filename = ui->lineEdit_SnapshotFilename->text();
	filename = QFileDialog::getOpenFileName(
		this,
		tr("Select PbsNodes Data file to analyze"),
		filename);

	if (!filename.isEmpty())	// if Cancel wasn't pressed
	{
		ui->lineEdit_SnapshotFilename->setText(filename);

		// save off the filename in QSettings
		QSettings mysettings( "AdaptiveComputing", "TORQUEView" );
		mysettings.setValue( "TORQUEViewSnapshotFileName", filename );

//		m_pbsNodesTab->on_btnClear_clicked();	// clear out any "show nodes running current job" stuff
//		m_pbsNodesTab->clearNodesRunningJob();	// clear out any "show nodes running current job" stuff

		on_btnCancelSnapshotReplay_clicked();  // make sure SnapshotReplay isn't running

		initAllTabs();
	}
}

/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::on_btnConfig_clicked()
{
    on_actionConfiguration_triggered();
}


/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::on_actionConfiguration_triggered()
{
    ConfigDlg dlg( this );
    dlg.init();

    if (dlg.exec())
    {

        initAllTabs(false); // don't include qmgr - Ken says it doesn't need to be updated on refresh

    }

}

/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::on_actionExit_triggered()   // exit app
{
    close();
}

/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::LoadStyleSheet(const QString &sheetName)
{
	QString sheetname = ":/icons/images/" + sheetName.toLower() + ".qss";
	QFile file(sheetname);
	file.open(QFile::ReadOnly);
	QString styleSheet = QLatin1String(file.readAll());

	qApp->setStyleSheet(styleSheet);
}

/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::on_actionChoose_Stylesheet_triggered()
{
	ChooseStylesheetDialog dlg;
	if (dlg.exec())
	{
		QString sheetName = dlg.iv_CurrentText;
		LoadStyleSheet(sheetName);

		// save off the stylesheet in QSettings
		QSettings mysettings( "AdaptiveComputing", "TORQUEView" );
		mysettings.setValue( "StylesheetName", sheetName );
	}
}


/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::on_btnSnapshot_clicked()
{
    TakeSnapshotDlg dlg(this);
    if (!dlg.exec())
        return;

    on_btnCancelSnapshotReplay_clicked();  // make sure SnapshotReplay isn't running

	if (dlg.isRefreshListsChecked())
	{
		// refresh all lists before taking snapshot
		initAllTabs();
	}

	m_Snapshot_ScheduleMultiple = dlg.isScheduleMultipleChecked();

	if (dlg.isScheduleSnapshots_Seconds_Checked())
		m_Snapshot_ScheduleSnapshots_Type = scheduleSnapshotsType_Seconds;
	else if (dlg.isScheduleSnapshots_Minutes_Checked())
		m_Snapshot_ScheduleSnapshots_Type = scheduleSnapshotsType_Minutes;
	else // if (dlg.isScheduleSnapshots_Minutes_Checked())
		m_Snapshot_ScheduleSnapshots_Type = scheduleSnapshotsType_Hours;

	m_Snapshot_SnapshotSecondsVal = dlg.SnapshotSecondsVal();
	m_Snapshot_SnapshotMinutesVal = dlg.SnapshotMinutesVal();
	m_Snapshot_SnapshotHoursVal = dlg.SnapshotHoursVal();
	m_Snapshot_NoEnd = true;

	m_Snapshot_Filename = dlg.SnapshotFilename();
	m_Snapshot_Comments = dlg.Comment();
	m_Snapshot_SaveIndividualNodeData = dlg.isSaveIndividualNodeDataChecked();
	m_Snapshot_SaveIndividualJobData = dlg.isSaveIndividualJobDataChecked();

    if (m_Snapshot_ScheduleMultiple)
    {
        QString text;
        int secs = 0;	// value in secs

        switch (m_Snapshot_ScheduleSnapshots_Type) {
        case scheduleSnapshotsType_Seconds:  // if value is in seconds...
        {
            secs = m_Snapshot_SnapshotSecondsVal;
            m_Snapshot_Iterations = dlg.SnapshotSecondsIterationsVal();
            if (m_Snapshot_Iterations != 0)
            {
                text = QString("Snapshot Scheduling is ON:  Taking snapshot every %1 seconds, lasting %2 iterations")
                        .arg(m_Snapshot_SnapshotSecondsVal)
                        .arg(m_Snapshot_Iterations);
                m_Snapshot_NoEnd = false;
            }
            else
            {
                text = QString("Snapshot Scheduling is ON:  Taking snapshot every %1 seconds")
                        .arg(m_Snapshot_SnapshotSecondsVal);
                m_Snapshot_NoEnd = true;
            }
            break;
        }
        case scheduleSnapshotsType_Minutes:  // if value is in minutes...
        {
            secs = m_Snapshot_SnapshotMinutesVal * 60;
            m_Snapshot_Iterations = dlg.SnapshotMinutesIterationsVal();
            if (m_Snapshot_Iterations != 0)
            {
                text = QString("Snapshot Scheduling is ON:  Taking snapshot every %1 minutes, lasting %2 iterations")
                        .arg(m_Snapshot_SnapshotMinutesVal)
                        .arg(m_Snapshot_Iterations);
                m_Snapshot_NoEnd = false;
            }
            else
            {
                text = QString("Snapshot Scheduling is ON:  Taking snapshot every %1 minutes")
                        .arg(m_Snapshot_SnapshotMinutesVal);
                m_Snapshot_NoEnd = true;
            }
            break;
        }
        default:  // else value is in hours
        {
            secs = m_Snapshot_SnapshotHoursVal * 60 * 60;
            m_Snapshot_Iterations = dlg.SnapshotHoursIterationsVal();
            if (m_Snapshot_Iterations != 0)
            {
                text = QString("Snapshot Scheduling is ON:  Taking snapshot every %1 hours, lasting %2 iterations")
                        .arg(m_Snapshot_SnapshotHoursVal)
                        .arg(m_Snapshot_Iterations);
                m_Snapshot_NoEnd = false;
            }
            else
            {
                text = QString("Snapshot Scheduling is ON:  Taking snapshot every %1 hours")
                        .arg(m_Snapshot_SnapshotHoursVal);
                m_Snapshot_NoEnd = true;
            }
        }
        } // switch
        int millisecs = secs * 1000;	// convert to millisecs

        if (m_createSnapshot_timerID != 0)
        {
            killTimer(m_createSnapshot_timerID);	// kill the timer
        }
        m_createSnapshot_timerID = startTimer(millisecs);	// restart the timer with the new value
        // show the "Cancel Snapshot Scheduling" button on the statusbar
        showCancelSnapshotSchedulingButton( text );
    }
    else
    {
        doTakeSnapshot();
    }

}

/*******************************************************************************
 *
*******************************************************************************/
bool MainWindow::doTakeSnapshot()
{
    m_bTakingSnapshot = true; // used inside each of the "writeDataToFile()" methods below to indicate we're in a "taking snapshot" mode

	QDateTime now = QDateTime::currentDateTime();
	QString sNow = now.toString("yyyy/MM/dd hh:mm:ss");
	QString sNow_toAppendToFilename = now.toString("yyyy-MMM-dd_hh-mm-ss");

	QFileInfo fi(m_Snapshot_Filename);
	QString dirPath = fi.path();
	QString baseName = fi.baseName();
	QString extension = fi.suffix();  // see if filename had a suffix (extension) -- if so, save it

	QString filename = QString("%1__%2.%3")
			.arg(baseName)
			.arg(sNow_toAppendToFilename)
			.arg(extension);
	QString filepath = QDir(dirPath).filePath(filename); // combine two paths


	QFile file( filepath );
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QMessageBox::critical(0, "Can't create snapshot file",
			   QString("Unable to open snapshot file '%1' for writing.").arg(filepath));
		return false;
	}
	QTextStream out(&file);

	out << "#---------------------------------------------------------------" << "\n";
	out << "TORQUEView Version:  1.0" << "\n";
//	out << "Torque Version:  4.2" << "\n";
	out << "Date:  " << sNow << "\n";
	out << "Comments:  " << m_Snapshot_Comments << "\n";
	out << "#---------------------------------------------------------------" << "\n";
	out << "\n";
	out << "\n";

	m_pbsNodesTab->writeDataToFile(out);	// write out pbsnodes/momctl data to file
	m_qstatTab->writeDataToFile(out);	// write out qstat data to file
	m_qmgrTab->writeDataToFile(out);	// write out qmgr data to file
	m_pbsServerTab->writeDataToFile(out);	// write out pbs_server data to file
	m_pbsServerTab->writeNodesContentsToFile(out);  // write out "nodes" file contents to file

	file.close();

	m_bTakingSnapshot = false;

	return true; // success
}


/*******************************************************************************
 *
*******************************************************************************/
QString MainWindow::formatForTooltip(QString text, QString separator) // split out string into separate lines for better tooltip output
{
    QString output;
    QString line;
    int lineLen = 0;
    const int TOOLTIP_MAXLINELEN = 90;

    QStringList sl = text.split( separator );
    foreach (QString s, sl)
    {
        line += s;
        line += separator;

		lineLen = line.size();
		if (lineLen >= TOOLTIP_MAXLINELEN)
		{
			line += "\n";
			output += line;
			lineLen = 0;
			line.clear();
		}
	}
	output += line; // append last line onto output
	output.chop ( 1 );  // remove last 'separator' char
	return output;
}

/*******************************************************************************
 *
*******************************************************************************/
void MainWindow::CenterAppWindow() // unfortunately, doesn't account for multi-screen systems -- centers app in between the two screens
{
	QDesktopWidget *desktop = QApplication::desktop();

	int screenCount = desktop->screenCount();
	int screenWidth, width;
	int screenHeight, height;
	int x, y;
//	QSize windowSize;

	if (screenCount == 1)
		screenWidth = desktop->width(); // get width of screen
	else
		screenWidth = desktop->width() / 2; // get width of screen
	screenHeight = desktop->height(); // get height of screen

//	windowSize = size(); // size of our application window
//	width = windowSize.width();
//	height = windowSize.height();
	width = (screenWidth * 2) / 3;
	height = (screenHeight * 4) / 5;

	// little computations
	x = (screenWidth - width) / 2;
	y = (screenHeight - height) / 2;
	y -= 50;

	// move window to desired coordinates
	resize(width, height);	// set app's initial window size
	move ( x, y );
}

