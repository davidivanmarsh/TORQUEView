/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: qmgrtab.h.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
*/

#ifndef QMGRTAB_H
#define QMGRTAB_H

#include <QWidget>
#include <QProcess>
#include <QComboBox>
#include <QTextStream>

class MainWindow;



namespace Ui {
class QmgrTab;
}

class QmgrTab : public QWidget
{
	Q_OBJECT

public:
	explicit QmgrTab(QWidget *parent = 0);
	~QmgrTab();

	void clearLists();
    void resetErrorMsgDlg();
	void initQmgrFromFile( QTextStream &in );  // init the qmgr tab from a file
	void issueCmd_Qmgr(QString sCmd); // execute "qmgr -c 'p s'" command -- parse output data

	void writeDataToFile(QTextStream& out);

    void enableControls();
    void disableControls();

protected:
	bool eventFilter(QObject* obj, QEvent *event);

private slots:
	void on_checkBox_UseHelps_stateChanged();
	void on_comboBox_QueueServer_currentIndexChanged ( const QString & text );

	void on_plainTextEdit_Qmgr_cursorPositionChanged ();

	void on_btnSubmit_clicked();  // submit qmgr command
	void on_btnRefresh_clicked(); // submit qmgr "p  s" command -- essentially does a refresh
	void on_btnClear_clicked();
	void on_btnX_clicked();  // clear the Submit edit line

	void on_btnInsert_clicked();
	void on_lineEdit_Qmgr_returnPressed();

	void qmgr_getStdout();
	void qmgr_getStderr();
	void qmgr_processStdout();

private:
	Ui::QmgrTab *ui;

	MainWindow* m_mainWindow;

    bool m_showQmgrSTDERROutput;

	QString m_qmgr_Stdout;
	QString m_qmgr_Stderr;

	QProcess* m_qmgrProcess;

	QStringList m_qmgrHistoryList;
	int m_qmgrHistoryIndex;

	void clearSelection();
	void showHelps();
	void hideHelps();

	void initServerParamsCombobox();
	void initQueueAttribsCombobox();


};

#endif // QMGRTAB_H
