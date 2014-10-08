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
