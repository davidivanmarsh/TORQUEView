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


#ifndef LOGVIEWERDLG_H
#define LOGVIEWERDLG_H

#include <QDialog>
#include <QProcess>


class MainWindow;


namespace Ui {
class LogViewerDlg;
}

class LogViewerDlg : public QDialog
{
	Q_OBJECT

public:
	explicit LogViewerDlg(MainWindow* mainWindow, const QString& logfilename, const QString& command, QWidget *parent = 0);
	~LogViewerDlg();

protected:
	void closeEvent(QCloseEvent *event);

private slots:
	void on_btnClose_clicked();
	void on_btnBrowse_clicked();
	void on_btnClear_clicked();

	void on_btnFindNext_clicked();
	void on_btnFindAll_clicked();
	void on_btnGrep_clicked();

	void on_checkBox_RunTail_f_toggled(bool checked);
	void on_checkBox_WrapLines_toggled(bool checked);
	void on_checkBox_UseGrep_toggled(bool checked);
	void on_spinBox_MaxLineCount_valueChanged(int val);

	void on_btnSaveAs_clicked();

	void tailLogfile_getStdout();
	void tailLogfile_getStderr();
	void tailLogfile_processStdout();

	void grep_getStdout();
	void grep_getStderr();
	void grep_processStdout();


private:
	Ui::LogViewerDlg *ui;

	MainWindow* m_mainWindow;

	QString m_logViewerCommand;

    bool m_showGrepSTDERROutput;

	QString m_grep_Stdout;
	QString m_grep_Stderr;
	QString m_tailLogfile_Stdout;
	QString m_tailLogfile_Stderr;

	bool issueCmd_tailLogfile(QString logfilename, bool bStart);  // either Start or Stop
	bool issueCmd_grep(QString logfilename);
	void startTailLogfile();
	void stopTailLogfile();

	QProcess* m_tailLogfileProcess;
	QProcess* m_grepProcess;

};

#endif // LOGVIEWERDLG_H
