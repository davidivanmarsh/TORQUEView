/*
* DISCLAIMER OF WARRANTY
*
* THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT
* ARE EXPRESSLY DISCLAIMED.
*
* IN NO EVENT SHALL DAVID I. MARSH BE LIABLE FOR ANY DIRECT OR INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* This license will be governed by the laws of the State of Utah,
* without reference to its choice of law rules.
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
