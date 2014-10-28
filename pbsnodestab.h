/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: pbsnodestab.h.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
*/


#ifndef PBSNODESTAB_H
#define PBSNODESTAB_H

#include <QWidget>
#include <QProcess>
#include <QHash>
#include <QMap>
#include <QTextStream>
#include <QTreeWidget>
#include <QLabel>


class MainWindow;


namespace Ui {
class PbsNodesTab;
}


enum NodeState
{
	pbs_free			= 0,
	pbs_offline			= 1,
	pbs_down			= 2,
	pbs_reserve			= 3,
	pbs_running			= 4,
	pbs_job_exclusive	= 5,
	pbs_job_sharing		= 6,
	pbs_busy			= 7,
	pbs_multiple		= 8,	// could be multiple states (e.g, "down,offline")
	pbs_state_unknown	= 9
};


struct MomCtlItem
{
public:
	QString m_nodeName;
	QStringList m_data;
};


class PbsNode
{
public:
	PbsNode(QString nodeName)
		: m_nodeName(nodeName)
		, m_nodeState(pbs_state_unknown)
		, m_momManagerPort("")
	{
		m_jobs.clear();
		m_data.clear();
		m_properties.clear();
		m_np				= -1; // init
		m_availableThreads	= -1; // init
		m_totalThreads		= -1; // init
		m_bErrorStatus		= false;
	}

	QString		m_nodeName;
	NodeState	m_nodeState;
	QString		m_sNodeState;	// string representation of node state (could be multiple states (e.g, "down,offline")
	QString		m_momServicePort;
	QString		m_momManagerPort;
	int			m_availableThreads; // NUMA
	int			m_totalThreads;		// NUMA
	int			m_np;	// number of procs (cores)
	QStringList m_properties;
	QStringList m_jobs;
	QStringList m_data;
	bool		m_bErrorStatus;
};



class PbsNodesTab : public QWidget
{
	Q_OBJECT

public:
	explicit PbsNodesTab(QWidget *parent = 0);
	~PbsNodesTab();

    QList< PbsNode* > m_pbsNodes;

	void clearLists();
    void resetErrorMsgDlg();
	void initPbsnodesFromFile(QTextStream &in);
	void initMomctlsFromFile(QTextStream &in);
	bool issueCmd_Pbsnodes();
    void issueCmd_Momctl_d3(QString nodeName, QString momManagerPort); // issue a "momctl -d3" command for that node -- parse output data
    void populateNodeInfo(PbsNode* node, QTreeWidget* treeWidgetNodeInfo);
    void populateMomctlFromFile(QString nodeName, QTreeWidget* treeWidgetMomCtl, QLabel* labelTitleMomCtl); // called when parsing momctl data obtained from a file (and stored in m_momctlMap)
	bool selectLastPbsnode();  // select (highlight) the last pbsnode the user selected - used by auto-refresh

	bool issueCmd_GetServerHome();  // get pbs_server's "serverhome" dir  (issue "pbs_server --about" command and parse it)
	bool issueCmd_AccessServerPrivDir();  // to find out if running as root or not
	bool issueCmd_StartMOM(PbsNode* node, bool bIsHeadNode);  // issue "Start MOM" (pbs_mom)command
	bool issueCmd_StopMOM(PbsNode* node, bool bIsHeadNode);  // issue "Stop MOM" (momctl -s) command

	void writeDataToFile(QTextStream& out);
	PbsNode* getLastNodeSelected();

	void showNodesRunningJob( QString jobId/*, QString execHosts */);
	void setJobID(QString jobID);  // set the jobID value in pbsNodesTab's spinBox_JobID control
	void checkShowNodesRunningJobIDCheckbox(bool bCheck);  // check/uncheck the checkbox

	void getNodeInfo(NodeState state, QString sMultipleStates, QString& sNodeState, bool bJobsPresent,
		 bool bErrorStatus, QIcon& icon, QColor& backgroundcolor);

    void enableControls();
    void disableControls();


public slots:
//	void on_btnClear_clicked();

	void on_actionStart_MOM_triggered();				// for nodes tree
	void on_actionStart_MOM_Head_node_triggered();		// for nodes tree
	void on_actionStop_MOM_triggered();					// for nodes tree
	void on_actionStop_MOM_Head_node_triggered();		// for nodes tree

	void on_actionMark_node_as_OFFLINE_triggered();
	void on_actionClear_OFFLINE_Node_triggered();
	void on_actionAdd_Note_triggered();
	void on_actionRemove_Note_triggered();
	void on_actionLog_Viewer_triggered();


private slots:
	void on_treeWidget_Nodes_itemSelectionChanged();
	void on_treeWidget_Nodes_itemDoubleClicked(QTreeWidgetItem* item, int column);
	void on_actionHighlight_all_occurances_of_JobID_triggered();
	void on_spinbox_JobID_valueChanged(int i);
	void on_checkBox_ShowNodesRunningJobID_stateChanged();

	void on_radioButton_ShowCores_toggled();
	void on_checkBox_ResizeColumnsToContents_stateChanged();
	void on_checkBox_ShowNodeDetailsLists_stateChanged();
	void on_checkBox_OnlyShowCoresRunningJobs_stateChanged();

	void getServerHome_getStdout();
	void getServerHome_getStderr();
	bool getServerHome_processStdout();

	void accessServerPrivDir_getStdout();
	void accessServerPrivDir_getStderr();
	bool accessServerPrivDir_processStdout();

	void pbsNodes_getStdout();
	void pbsNodes_getStderr();
	void pbsNodes_processStdout();

	void momctl_getStdout();
	void momctl_getStderr();
	void momctl_processStdout();

	void startMOM_getStdout();
	void startMOM_getStderr();
	void startMOM_processStdout();

	void stopMOM_getStdout();
	void stopMOM_getStderr();
	void stopMOM_processStdout();

	void markNodeAsOFFLINE_getStdout();
	void markNodeAsOFFLINE_getStderr();
	void markNodeAsOFFLINE_processStdout();

	void clearOFFLINENode_getStdout();
	void clearOFFLINENode_getStderr();
	void clearOFFLINENode_processStdout();

	void addNote_getStdout();
	void addNote_getStderr();
	void addNote_processStdout();

	void removeNote_getStdout();
	void removeNote_getStderr();
	void removeNote_processStdout();
	void on_actionNo_op_triggered();

private:
	Ui::PbsNodesTab *ui;

	MainWindow* m_mainWindow;
	PbsNode* m_lastNodeSelected;	// save off last node user clicked on (in table or tree list) - used for auto-refreshing last node
	QString m_lastHomeDirectory;

	bool m_showPbsnodesSTDERROutput;
	bool m_showMomctlSTDERROutput;
	bool m_showStartMomSTDERROutput;
	bool m_showStopMomSTDERROutput;
	bool m_showMarkNodeAsOfflineSTDERROutput;
	bool m_showClearOfflineNodeSTDERROutput;
	bool m_showAddNoteSTDERROutput;
	bool m_showRemoveNoteSTDERROutput;
	bool m_showGetServerHomeSTDERROutput;

	void refresh();
	void CreateContextMenus();
	void updateLists();
//	QStringList getExecHost();

	QString extractField(QString line, int& index, QString fieldLabel, QString terminalStr);

public:
	bool issueCmd_MarkNodeAsOFFLINE(PbsNode* node);
	bool issueCmd_ClearOFFLINENode(PbsNode* node);
	bool issueCmd_AddNote(PbsNode* node, QString sAnnotation);
	bool issueCmd_RemoveNote(PbsNode* node);
private:
	QString m_getServerHome_Stdout;
	QString m_getServerHome_Stderr;
	QString m_accessServerPrivDir_Stdout;
	QString m_accessServerPrivDir_Stderr;
	QString m_pbsNode_Stdout;
	QString m_pbsNode_Stderr;
	QString m_momCtl_Stdout;
	QString m_momCtl_Stderr;
	QString m_startMOM_Stdout;
	QString m_startMOM_Stderr;
	QString m_stopMOM_Stdout;
	QString m_stopMOM_Stderr;
	QString m_markNodeAsOFFLINE_Stdout;
	QString m_markNodeAsOFFLINE_Stderr;
	QString m_clearOFFLINENode_Stdout;
	QString m_clearOFFLINENode_Stderr;
	QString m_addNote_Stdout;
	QString m_addNote_Stderr;
	QString m_removeNote_Stdout;
	QString m_removeNote_Stderr;

	QStringList m_momCtl_Lines;  // used when taking a snapshot and writing out momctl lines
	QMap<QString, MomCtlItem*> m_momctlMap;  // used when reading in a combined pbsnodes/momctl data text file, and when updating the MomCtl treewidget

	QProcess* m_getServerHomeProcess;
	QProcess* m_accessServerPrivDirProcess;
	QProcess* m_pbsNodesProcess;
	QProcess* m_momctlProcess;
	QProcess* m_startMOMProcess;
	QProcess* m_stopMOMProcess;
	QProcess* m_markNodeAsOFFLINEProcess;
	QProcess* m_clearOFFLINENodeProcess;
	QProcess* m_addNoteProcess;
	QProcess* m_removeNoteProcess;
};

#endif // PBSNODESTAB_H
