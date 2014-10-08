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


#ifndef TAKESNAPSHOTDLG_H
#define TAKESNAPSHOTDLG_H

#include <QDialog>

class MainWindow;


namespace Ui {
class TakeSnapshotDlg;
}

class TakeSnapshotDlg : public QDialog
{
    Q_OBJECT

public:
    explicit TakeSnapshotDlg(MainWindow* mainWindow, QWidget *parent = 0);
    ~TakeSnapshotDlg();

	bool isScheduleMultipleChecked();
	bool isScheduleSnapshots_Seconds_Checked();
	bool isScheduleSnapshots_Minutes_Checked();
	bool isScheduleSnapshots_Hours_Checked();
	int SnapshotSecondsVal();
	int SnapshotSecondsIterationsVal();
	int SnapshotMinutesVal();
	int SnapshotMinutesIterationsVal();
	int SnapshotHoursVal();
	int SnapshotHoursIterationsVal();
	QString SnapshotFilename(); // if "ScheduleMultipleSnapshots" is checked, this is the baseline filename (we add date/time to it)
    QString Comment();
	bool isRefreshListsChecked();


private slots:
    void on_btnBrowse_clicked();
	void on_checkBox_ScheduleMultipleSnapshots_toggled(bool checked);

private:
    Ui::TakeSnapshotDlg *ui;
    MainWindow* m_mainWindow;

	void scheduleMultipleSnapshots_Setup();
	void takeSingleSnapshot_Setup();

    void done( int result );
};

#endif // TAKESNAPSHOTDLG_H
