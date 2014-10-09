/*
* The contents of this file are subject to the Mozilla Public License
* Version 1.1 (the "License"); you may not use this file except in
* compliance with the License. You may obtain a copy of the License at
* http://www.mozilla.org/MPL/

* Software distributed under the License is distributed on an "AS IS"
* basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
* License for the specific language governing rights and limitations
* under the License.

* The Original Code is: qstattab.h.

* The Initial Developer of the Original Code is David I. Marsh.
* All Rights Reserved.

* Contributor(s): Ken Nielson.
*/


#ifndef QSTATTAB_H
#define QSTATTAB_H

#include <QWidget>
#include <QProcess>
#include <QTextStream>
#include <QMap>
#include <QTreeWidgetItem>


class MainWindow;


namespace Ui {
class QstatTab;
}

struct JobInfo
{
public:
	QStringList m_data;
};

class PbsJob // "qstat -R" row item structure
{
public:
	PbsJob(QString jobID, QString username, QString queue, QString NDS, QString TSK, QString requiredMemory,
			QString requiredTime, QString state, QString elapsedTime, QString BIG, QString FAST, QString PFS)
		: m_jobID(jobID)
		, m_username(username)
		, m_queue(queue)
		, m_NDS(NDS)
		, m_TSK(TSK)
		, m_requiredMemory(requiredMemory)
		, m_requiredTime(requiredTime)
		, m_state(state)
		, m_elapsedTime(elapsedTime)
		, m_BIG(BIG)
		, m_FAST(FAST)
		, m_PFS(PFS)
	{
	}

	QString m_jobID;
	QString m_username;
	QString m_queue;
	QString m_NDS;
	QString m_TSK;
	QString m_requiredMemory;
	QString m_requiredTime;
	QString m_state;
	QString m_elapsedTime;
	QString m_BIG;
	QString m_FAST;
	QString m_PFS;

};


class QstatTab : public QWidget
{
	Q_OBJECT

public:
	explicit QstatTab(QWidget *parent = 0);
	~QstatTab();

	QList< PbsJob* > m_pbsJobs;

	void clearLists();
	void getExpandedState();
    void resetErrorMsgDlg();
	bool issueCmd_Qstat_R();
	void initQstatFromFile( QTextStream &in );
	void initQstats_f_FromFile( QTextStream &in );
	void writeDataToFile(QTextStream& out);
	bool issueCmd_Qstat_f( QString jobID="" );

//	bool issueCmd_ModifyJob( QString jobID );
	bool issueCmd_DeleteJob( QString jobID );
	bool issueCmd_HoldJob( QString jobID );
//	bool issueCmd_MoveJob( QString jobID );
//	bool issueCmd_ReorderJob( QString jobID );
	bool issueCmd_RerunJob( QString jobID );
	bool issueCmd_ReleaseHoldOnJob( QString jobID );
	bool issueCmd_RunJob( QString jobID );
//	bool issueCmd_SelectJob( QString jobID );
//	bool issueCmd_SendSignalToJob( QString jobID );

    void enableControls();
    void disableControls();

	bool selectLastJobId();  // select (highlight) the last JobId the user selected - used by auto-refresh
	QString getLastJobIdSelected();

private slots:
	void qstat_R_getStdout();
	void qstat_R_getStderr();
	void qstat_R_processStdout();

	void qstat_f_getStdout();
	void qstat_f_getStderr();
	void qstat_f_processStdout();

	void runJob_getStdout();
	void runJob_getStderr();
	void runJob_processStdout();

	void deleteJob_getStdout();
	void deleteJob_getStderr();
	void deleteJob_processStdout();

	void holdJob_getStdout();
	void holdJob_getStderr();
	void holdJob_processStdout();

	void releaseHoldOnJob_getStdout();
	void releaseHoldOnJob_getStderr();
	void releaseHoldOnJob_processStdout();

	void rerunJob_getStdout();
	void rerunJob_getStderr();
	void rerunJob_processStdout();


	void on_treeWidget_Jobs_itemSelectionChanged();

	void on_btnShowNodesRunningThisJob_clicked();
	void on_btnExecHost_clicked();

	void on_actionRun_job_triggered();
	void on_actionDelete_job_triggered();
	void on_actionPut_job_on_hold_triggered();
	void on_actionRelease_hold_on_job_triggered();
	void on_actionRerun_job_triggered();

    void on_checkBox_ShowJobsByQueue_stateChanged();
    void on_checkBox_Queued_stateChanged();
    void on_checkBox_Running_stateChanged();
    void on_checkBox_Completed_stateChanged();
    void on_checkBox_Hold_stateChanged();
    void on_checkBox_Suspended_stateChanged();
    void on_checkBox_ExpandAll_stateChanged();

private:
	Ui::QstatTab *ui;

	MainWindow* m_mainWindow;
	QString m_lastJobIdSelected;
	QString m_lastExecHost;
	QString m_lastExecPort;

    bool m_showQStat_R_STDERROutput;
    bool m_showQStat_f_STDERROutput;
    bool m_showRunJobSTDERROutput;
    bool m_showDeleteJobSTDERROutput;
    bool m_showHoldJobSTDERROutput;
    bool m_showReleaseHoldOnJobSTDERROutput;
    bool m_showRerunJobSTDERROutput;

	QProcess* m_qstat_R_Process;
	QProcess* m_qstat_f_Process;
	QProcess* m_runJobProcess;
	QProcess* m_deleteJobProcess;
	QProcess* m_holdJobProcess;
	QProcess* m_releaseHoldOnJobProcess;
	QProcess* m_rerunJobProcess;

	QString m_qstat_R_Stdout;
	QString m_qstat_R_Stderr;
	QString m_qstat_f_Stdout;
	QString m_qstat_f_Stderr;

	QString m_runJob_Stdout;
	QString m_runJob_Stderr;
	QString m_deleteJob_Stdout;
	QString m_deleteJob_Stderr;
	QString m_holdJob_Stdout;
	QString m_holdJob_Stderr;
	QString m_releaseHoldOnJob_Stdout;
	QString m_releaseHoldOnJob_Stderr;
	QString m_rerunJob_Stdout;
	QString m_rerunJob_Stderr;

	QStringList m_jobInfo_Lines;  // used when taking a snapshot and writing out qstat -f (i.e, jobInfo) lines
	QMap<QString, JobInfo*> m_jobInfoMap;  // used when reading in a combined qstat_R/qstat_f data text file, and when updating the JobInfo treewidget
	QMap<QString, bool> m_expandedMap;  // used to determine each item's "expanded"/"not expanded" state (in the job list)

	void CreateContextMenus();

	void populateQstat_f_FromFile( QString jobID );
	void updateList();

	void restoreExpandedState(); // expand any root-level items in Jobs list that previously were expanded

	void getJobInfo(QString jobState,  QString& detailedJobState, QIcon& icon, QColor& backgroundcolor);
};

#endif // QSTATTAB_H
