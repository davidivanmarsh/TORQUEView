/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: pbsservertab.h.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
*/



#ifndef PBSSERVERTAB_H
#define PBSSERVERTAB_H

#include <QWidget>
#include <QProcess>
#include <QTextStream>



class MainWindow;

namespace Ui {
class PbsServerTab;
}

class PbsServerTab : public QWidget
{
    Q_OBJECT

public:
    explicit PbsServerTab(QWidget *parent = 0);
    ~PbsServerTab();\

    void clearLists();
    bool issueCmd_PbsServer();
    bool issueCmd_LoadNodesFile();
    bool issueCmd_CopyNodesFile( QString sDestFilename );
    bool issueCmd_SaveNodesFile();
    void writeDataToFile(QTextStream& out);
	void writeNodesContentsToFile(QTextStream& out);
	void initPbsServerFromFile( QTextStream &in );  // init the pbs_server tab from a file
	int  issueCmd_getLocalPbsServerPID();  // execute a command to get the local pbs_server PID
	void initNodesContentsFromFile( QTextStream &in );  // init the "nodes" file contents from a file

    void enableControls();
    void disableControls();

private slots:
	void on_btnViewServerLog_clicked();
	void on_btnEditNodesFile_clicked();

	void on_btnRefresh_clicked();
	void on_btnStartServer_clicked();
	void on_btnStopServer_clicked();

	void pbsServerStatus_getStdout();
	void pbsServerStatus_getStderr();
	void pbsServerStatus_processStdout();

	void getLocalPbsServerPID_getStdout();
	void getLocalPbsServerPID_getStderr();
	int  getLocalPbsServerPID_processStdout();

	void loadNodesFile_getStdout();
	void loadNodesFile_getStderr();
	void loadNodesFile_processStdout();

	void copyNodesFile_getStdout();
	void copyNodesFile_getStderr();
	void copyNodesFile_processStdout();

	void saveNodesFile_getStdout();
	void saveNodesFile_getStderr();
	void saveNodesFile_processStdout();


private:
    Ui::PbsServerTab *ui;

    MainWindow* m_mainWindow;

	QString m_pbsServerStatus_Stdout;
	QString m_pbsServerStatus_Stderr;
	QString m_getLocalPbsServerPID_Stdout;
	QString m_getLocalPbsServerPID_Stderr;
	QString m_loadNodesFile_Stdout;
	QString m_loadNodesFile_Stderr;
	QString m_copyNodesFile_Stdout;
	QString m_copyNodesFile_Stderr;
	QString m_saveNodesFile_Stdout;
	QString m_saveNodesFile_Stderr;

	QProcess* m_pbsServerStatusProcess;
	QProcess* m_getLocalPbsServerPIDProcess;
	QProcess* m_loadNodesFileProcess;
	QProcess* m_copyNodesFileProcess;
	QProcess* m_saveNodesFileProcess;

	void loadLocalNodesFile( QString nodesFilename );
	void copyLocalNodesFile( QString fromFilename, QString toFilename );
	void saveLocalNodesFile( QString nodesFilename );

};

#endif // PBSSERVERTAB_H
