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


#include "takesnapshotdlg.h"
#include "ui_takesnapshotdlg.h"
#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QCoreApplication>


/*******************************************************************************
 *
*******************************************************************************/
TakeSnapshotDlg::TakeSnapshotDlg(MainWindow* mainWindow, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TakeSnapshotDlg)
{
    ui->setupUi(this);

    m_mainWindow = mainWindow;

    // fill in snapshot file's edit field
    QString dirPath;
    QString snapshotfile = m_mainWindow->getLineEdit_SnapshotFilename_Text();
    if (snapshotfile.isEmpty())
    {
        dirPath = QDir::homePath();  // returns absolute path of user's home directory
    }
    else
    {
        QFileInfo fi = QFileInfo(snapshotfile);
        dirPath = fi.absolutePath();
    }

//  QDateTime now = QDateTime::currentDateTime();
//  QString sDateTime = now.toString("yyyy-MMM-dd_hh-mm-ss");
//	QString filename = QString("TORQUEViewSnapshot__%1.txt").arg(sDateTime);
    QString filename = QString("TORQUEViewSnapshot.txt");
    QString path = QDir(dirPath).filePath(filename); // combine two paths
    ui->lineEdit_SnapshotFilename->setText(path);

	// show/hide controls for "Take Single Snapshot" configuration
	takeSingleSnapshot_Setup();

}

/*******************************************************************************
 *
*******************************************************************************/
TakeSnapshotDlg::~TakeSnapshotDlg()
{
    delete ui;
}

bool TakeSnapshotDlg::isScheduleMultipleChecked()
{
	return ui->checkBox_ScheduleMultipleSnapshots->isChecked();
}
bool TakeSnapshotDlg::isScheduleSnapshots_Seconds_Checked()	// true=take snapshots each "seconds"
{
	if (ui->radioButton_ScheduleSnapshots_Seconds->isChecked())
		return true;
	else
		return false;
}
bool TakeSnapshotDlg::isScheduleSnapshots_Minutes_Checked()	// true=take snapshots each "minutes"
{
	if (ui->radioButton_ScheduleSnapshots_Minutes->isChecked())
		return true;
	else
		return false;
}
bool TakeSnapshotDlg::isScheduleSnapshots_Hours_Checked()	// true=take snapshots each "hours"
{
	if (ui->radioButton_ScheduleSnapshots_Hours->isChecked())
		return true;
	else
		return false;
}
int TakeSnapshotDlg::SnapshotSecondsVal()
{
	return ui->spinBox_ScheduleSnapshots_Seconds->value();
}
int TakeSnapshotDlg::SnapshotSecondsIterationsVal()
{
	return ui->spinBox_EndSnapshots_Seconds_Iterations->value();
}
int TakeSnapshotDlg::SnapshotMinutesVal()
{
	return ui->spinBox_ScheduleSnapshots_Minutes->value();
}
int TakeSnapshotDlg::SnapshotMinutesIterationsVal()
{
	return ui->spinBox_EndSnapshots_Minutes_Iterations->value();
}
int TakeSnapshotDlg::SnapshotHoursVal()
{
	return ui->spinBox_ScheduleSnapshots_Hours->value();
}
int TakeSnapshotDlg::SnapshotHoursIterationsVal()
{
	return ui->spinBox_EndSnapshots_Hours_Iterations->value();
}
QString TakeSnapshotDlg::SnapshotFilename()
{
    return ui->lineEdit_SnapshotFilename->text();
}
QString TakeSnapshotDlg::Comment()
{
    return ui->plainTextEdit_Comment->toPlainText();
}
bool TakeSnapshotDlg::isRefreshListsChecked()
{
	return ui->checkBox_RefreshLists->isChecked();
}

/*******************************************************************************
 *
*******************************************************************************/
void TakeSnapshotDlg::on_btnBrowse_clicked()
{
	QString snapshotfile = m_mainWindow->getLineEdit_SnapshotFilename_Text();
	QFileInfo fi = QFileInfo(snapshotfile);
	QString dirPath = fi.absolutePath();

	QString filename = QFileDialog::getSaveFileName(
		this,
		tr("Choose TORQUEView snapshot filename"),
		dirPath);

	if (!filename.isEmpty())	// if Cancel wasn't pressed
	{
		ui->lineEdit_SnapshotFilename->setText(filename);
	}
}

/*******************************************************************************
 *
*******************************************************************************/
void TakeSnapshotDlg::scheduleMultipleSnapshots_Setup()
{
	// show/hide all the appropriate controls
    ui->groupBox_TakeSnapshot->setTitle("Schedule snapshots:");

//	ui->gridLayout_SnapshotFrequency->show();

	ui->radioButton_ScheduleSnapshots_Seconds->show();
	ui->spinBox_ScheduleSnapshots_Seconds->show();
	ui->label_ScheduleSnapshots_Seconds->show();
	ui->spinBox_EndSnapshots_Seconds_Iterations->show();
	ui->label_EndSnapshots_Seconds->show();

	ui->radioButton_ScheduleSnapshots_Minutes->show();
	ui->spinBox_ScheduleSnapshots_Minutes->show();
	ui->label_ScheduleSnapshots_Minutes->show();
	ui->spinBox_EndSnapshots_Minutes_Iterations->show();
	ui->label_EndSnapshots_Minutes->show();

	ui->radioButton_ScheduleSnapshots_Hours->show();
	ui->spinBox_ScheduleSnapshots_Hours->show();
	ui->label_ScheduleSnapshots_Hours->show();
	ui->spinBox_EndSnapshots_Hours_Iterations->show();
	ui->label_EndSnapshots_Hours->show();

//	ui->label_SaveSnapshotFileAs->setText("Baseline snapshot filename (will append date/time to filename):");
}

/*******************************************************************************
 *
*******************************************************************************/
void TakeSnapshotDlg::takeSingleSnapshot_Setup()
{
	// show/hide all the appropriate controls
	ui->groupBox_TakeSnapshot->setTitle("Take single snapshot:");

//	ui->gridLayout_SnapshotFrequency->hide();

	ui->radioButton_ScheduleSnapshots_Seconds->hide();
	ui->spinBox_ScheduleSnapshots_Seconds->hide();
	ui->label_ScheduleSnapshots_Seconds->hide();
	ui->spinBox_EndSnapshots_Seconds_Iterations->hide();
	ui->label_EndSnapshots_Seconds->hide();

	ui->radioButton_ScheduleSnapshots_Minutes->hide();
	ui->spinBox_ScheduleSnapshots_Minutes->hide();
	ui->label_ScheduleSnapshots_Minutes->hide();
	ui->spinBox_EndSnapshots_Minutes_Iterations->hide();
	ui->label_EndSnapshots_Minutes->hide();

	ui->radioButton_ScheduleSnapshots_Hours->hide();
	ui->spinBox_ScheduleSnapshots_Hours->hide();
	ui->label_ScheduleSnapshots_Hours->hide();
	ui->spinBox_EndSnapshots_Hours_Iterations->hide();
	ui->label_EndSnapshots_Hours->hide();

//	ui->label_SaveSnapshotFileAs->setText("Save snapshot file as:");
}

/*******************************************************************************
 *
*******************************************************************************/
void TakeSnapshotDlg::on_checkBox_ScheduleMultipleSnapshots_toggled(bool checked)
{
	if (checked) // schedule multiple snapshots
	{
		// show/hide controls for "Schedule Multiple Snapshots" configuration
		scheduleMultipleSnapshots_Setup();
	}
	else // take single snapshot
	{
		// show/hide controls for "Take Single Snapshot" configuration
		takeSingleSnapshot_Setup();
	}

}

/*******************************************************************************
 *
*******************************************************************************/
// the done() function is reimplemented from QDialog. It is called when the user
// clicks OK or Cancel
void TakeSnapshotDlg::done( int result )
{
	if ( result == QDialog::Accepted ) // if user clicked OK
	{
		if (ui->lineEdit_SnapshotFilename->text().isEmpty())
		{
			QMessageBox::information( this,
									  "Take Snapshot",
									  "Filename is empty. Must have a filename to continue.",
									  QMessageBox::Ok,
									  QMessageBox::Ok);
			return;  // stay in this dialog -- don't exit
		}
	}

	QDialog::done( result );  // exit dialog
}

