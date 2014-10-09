/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: mainwindow.h.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
*/


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>

class HeatMapTab;
class PbsNodesTab;
class QstatTab;
class QmgrTab;
class PbsServerTab;



namespace Ui {
class MainWindow;
}

enum ScheduleSnapshotsType
{
	scheduleSnapshotsType_Seconds	= 0,
	scheduleSnapshotsType_Minutes	= 1,
	scheduleSnapshotsType_Hours		= 2
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	QString SETTINGS_CMD_Pbsnodes;
	QString SETTINGS_CMD_Momctl_d3;
	QString SETTINGS_CMD_Qstat_R;
	QString SETTINGS_CMD_Qstat_f;
	QString SETTINGS_CMD_Qmgr_c;

	QString SETTINGS_CMD_StartMOM_HeadNode_Standard;		// StartMOM standard version (for head node)
	QString SETTINGS_CMD_StartMOM_ComputeNode_Standard;		// StartMOM standard version (for compute node)
	QString SETTINGS_CMD_StartMOM_HeadNode_Multimom;		// StartMOM when using multi-moms (for head node)
	QString SETTINGS_CMD_StartMOM_ComputeNode_Multimom;		// StartMOM when using multi-moms (for compute node)
	QString SETTINGS_CMD_StartMOM_HeadNode_Service;			// StartMOM using a service to start the MOM (For head node)
	QString SETTINGS_CMD_StartMOM_ComputeNode_Service;		// StartMOM using a service to start the MOM (for compute node)

	QString SETTINGS_CMD_StopMOM_HeadNode_Standard;			// StopMOM standard version, also for multi-moms (for head node)
	QString SETTINGS_CMD_StopMOM_ComputeNode_Standard;		// StopMOM standard version, also for multi-moms (for compute node)
	QString SETTINGS_CMD_StopMOM_HeadNode_Service;			// StopMOM using a service to stop the MOM (for head node)
	QString SETTINGS_CMD_StopMOM_ComputeNode_Service;		// StopMOM using a service to stop the MOM (for compute node)

	QString SETTINGS_CMD_MarkNodeAsOFFLINE;
	QString SETTINGS_CMD_ClearOFFLINEFromNode;
	QString SETTINGS_CMD_AddNote;
	QString SETTINGS_CMD_RemoveNote;
	QString SETTINGS_CMD_Tail;			// tail log file
	QString SETTINGS_CMD_Grep;			// grep a log file
	QString SETTINGS_CMD_Cat;			// cat a file
	QString SETTINGS_CMD_Scp_FromRemoteToLocal;// scp a file (from remote src to local dest)
	QString SETTINGS_CMD_Scp_FromLocalToRemote;// scp a file (from local src to remote dest)
	QString SETTINGS_CMD_Cp;			// copya file
	QString SETTINGS_CMD_GetServerHome;
//	QString SETTINGS_CMD_GetMOMHome;  // NOT NEEDED YET

	QString SETTINGS_CMD_ModifyJob;
	QString SETTINGS_CMD_DeleteJob;
	QString SETTINGS_CMD_PutJobOnHold;
	QString SETTINGS_CMD_MoveJob;
	QString SETTINGS_CMD_ReorderJob;
	QString SETTINGS_CMD_RerunJob;
	QString SETTINGS_CMD_ReleaseHoldOnJob;
	QString SETTINGS_CMD_RunJob;
	QString SETTINGS_CMD_SelectJob;
	QString SETTINGS_CMD_SendSignalToJob;


	QString getLineEdit_SnapshotFilename_Text();
	int getComboBox_DataSource_CurrentIndex();
//	bool isCheckBox_AutoRefresh_Checked();
//	void setCheckBox_AutoRefresh_Checked(bool bChecked);
	bool isCurrentlyExecutingCmd();  // is TORQUEView currently executing some TORQUE client command (i.e, Run Job, Start MOM, etc.)
	void setIsCurrentlyExecutingCmd(bool bFlag);  // is TORQUEView currently executing some TORQUE client command (i.e, Run Job, Start MOM, etc.)

	bool takingSnapshot();

	void switchToPbsNodesTab();
	void showNodesRunningJob( QString jobId );

	QString formatForTooltip(QString text, QString separator); // split out string into separate lines for better tooltip output

	void initAllTabs(bool bIncludeQmgr=true);


	// Config dlg settings
	QString m_Config_Cmd_Pbsnodes;
	QString m_Config_Cmd_Momctl_d3;
	QString m_Config_Cmd_Qstat_R;
	QString m_Config_Cmd_Qstat_f;
	QString m_Config_Cmd_Qmgr_c;

	QString m_Config_Cmd_StartMOM_HeadNode_Standard;
	QString m_Config_Cmd_StartMOM_ComputeNode_Standard;
	QString m_Config_Cmd_StartMOM_HeadNode_Multimom;
	QString m_Config_Cmd_StartMOM_ComputeNode_Multimom;
	QString m_Config_Cmd_StartMOM_HeadNode_Service;
	QString m_Config_Cmd_StartMOM_ComputeNode_Service;

	QString m_Config_Cmd_StopMOM_HeadNode_Standard;
	QString m_Config_Cmd_StopMOM_ComputeNode_Standard;
	QString m_Config_Cmd_StopMOM_HeadNode_Service;
	QString m_Config_Cmd_StopMOM_ComputeNode_Service;

	QString m_Config_Cmd_MarkNodeAsOFFLINE;
	QString m_Config_Cmd_ClearOFFLINEFromNode;
	QString m_Config_Cmd_AddNote;
	QString m_Config_Cmd_RemoveNote;
	QString m_Config_Cmd_Tail;			// tail log file
	QString m_Config_Cmd_Grep;			// grep a log file
	QString m_Config_Cmd_Cat;			// cat a file
	QString m_Config_Cmd_Scp_FromRemoteToLocal;			// scp a file (remote to local)
	QString m_Config_Cmd_Scp_FromLocalToRemote;			// scp a file (local to remote)
	QString m_Config_Cmd_Cp;			// copy a file
	QString m_Config_Cmd_GetServerHome;
//	QString m_Config_Cmd_GetMOMHome;  // NOT NEEDED YET

	QString m_Config_Cmd_ModifyJob;
	QString m_Config_Cmd_DeleteJob;
	QString m_Config_Cmd_PutJobOnHold;
	QString m_Config_Cmd_MoveJob;
	QString m_Config_Cmd_ReorderJob;
	QString m_Config_Cmd_RerunJob;
	QString m_Config_Cmd_ReleaseHoldOnJob;
	QString m_Config_Cmd_RunJob;
	QString m_Config_Cmd_SelectJob;
	QString m_Config_Cmd_SendSignalToJob;


	bool m_Config_HeatMapShowNodeNames;

	int m_Config_HeatMapRowCount_NoNodeNames;
	int m_Config_HeatMapRowHeight_NoNodeNames;
	int m_Config_HeatMapColumnWidth_NoNodeNames;

	int m_Config_HeatMapRowCount_WithNodeNames;
	int m_Config_HeatMapRowHeight_WithNodeNames;
	int m_Config_HeatMapColumnWidth_WithNodeNames;
//	int m_Config_NodeColorLegendRowHeight;

	QString m_Config_PbsServerHomeDir;
//	QString m_Config_PbsMOMHomeDir;  // NOT NEEDED YET

	QString m_Config_RemoteServer;
	QStringList m_Config_RemoteServerList;
	int  m_Config_DataSource;  // 0="Local host", 1="Remote host", or 2="Snapshot file"
	bool m_Config_UsingMultiMoms;
	bool m_Config_UseServiceToStartStopMOMs;
//	bool m_Config_ShowSTDERROutput;


private slots:
    void on_actionAbout_triggered();
    void on_actionAbout_Qt_triggered();

	void on_checkBox_AutoRefresh_stateChanged();

	void on_comboBox_DataSource_activated ( int index );
	void on_comboBox_Remote_Servers_currentIndexChanged ( const QString & text );

	void on_actionChoose_Stylesheet_triggered();

	void on_btnConfig_clicked();
	void on_actionConfiguration_triggered();

    void on_btnSnapshot_clicked();
    void on_btnCancelSnapshotScheduling_clicked();
    void on_btnCancelSnapshotReplay_clicked();
    void on_btnPauseSnapshotReplay_clicked();
    void on_btnResumeSnapshotReplay_clicked();
    void on_btnNextSnapshotReplay_clicked();
    void on_btnBackSnapshotReplay_clicked();

	void on_btnRefresh_clicked();
	void on_actionRefresh_triggered();
	void on_btnBrowse_clicked();
	void on_btnReplaySnapshots_clicked();
	void on_actionReplay_Snapshots_triggered();
	void on_actionOpen_triggered();
	void on_actionExit_triggered();

protected:
    int m_autoRefresh_timerID;
    int m_createSnapshot_timerID;
    int m_replaySnapshot_timerID;

    void timerEvent(QTimerEvent *event);

private:
    Ui::MainWindow *ui;

public:
    HeatMapTab* m_heatMapTab;
    PbsNodesTab* m_pbsNodesTab;
private:
	QstatTab* m_qstatTab;
	QmgrTab* m_qmgrTab;
	PbsServerTab* m_pbsServerTab;

	bool m_bIsCurrentlyExecutingCmd;

	int tabIndex_HeatMapTab;
	int tabIndex_PbsNodesTab;
	int tabIndex_QstatTab;
	int tabIndex_QmgrTab;
	int tabIndex_PbsServerTab;

	// Take Snapshot
	bool	m_bTakingSnapshot;
	bool	m_Snapshot_ScheduleMultiple;
	ScheduleSnapshotsType m_Snapshot_ScheduleSnapshots_Type;  // either Seconds, Minutes, or Hours
	int		m_Snapshot_SnapshotSecondsVal;
	int		m_Snapshot_SnapshotMinutesVal;
	int		m_Snapshot_SnapshotHoursVal;
	int		m_Snapshot_Iterations;
	bool    m_Snapshot_NoEnd;
	QString m_Snapshot_Filename;
	QString m_Snapshot_Comments;
	bool doTakeSnapshot();

    // Replay Snapshot
    QStringList m_SnapshotReplay_Filelist;
    int m_SnapshotReplay_Filelist_Index;
    int m_SnapshotReplay_TimeIntervalInSecs;
    void doReplaySnapshot();

    // Schedule Snapshot / Replay Snapshot status bar controls:
    QLabel* m_statusText;
    QPushButton* btnCancelSnapshotScheduling;

    QPushButton* btnCancelSnapshotReplay;
    QPushButton* btnPauseSnapshotReplay;
    QPushButton* btnResumeSnapshotReplay;
    QPushButton* btnNextSnapshotReplay;
    QPushButton* btnBackSnapshotReplay;

    void setupStatusBar();
    void killSnapshotReplayTimer();
    void startSnapshotReplayTimer(int secs);
    void showCancelSnapshotSchedulingButton(QString msg);
    void hideCancelSnapshotSchedulingButton();
    void enteringSnapshotPlayState(QString msg);
    void exitingSnapshotPlayState();
    void enteringSnapshotPauseState(QString msg);
    void exitingSnapshotPauseState(QString msg);


	void readSettings();
	void getConfigCmds();
	void LoadStyleSheet(const QString &sheetName);
	void CenterAppWindow();

};

#endif // MAINWINDOW_H
