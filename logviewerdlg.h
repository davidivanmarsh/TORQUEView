/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: logviewerdlg.h.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
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
