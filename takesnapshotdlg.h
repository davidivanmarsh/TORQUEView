/*
* The contents of this file are subject to the Mozilla Public License
* Version 1.1 (the "License"); you may not use this file except in
* compliance with the License. You may obtain a copy of the License at
* http://www.mozilla.org/MPL/

* Software distributed under the License is distributed on an "AS IS"
* basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
* License for the specific language governing rights and limitations
* under the License.

* The Original Code is: takesnapshotdlg.h.

* The Initial Developer of the Original Code is David I. Marsh.
* All Rights Reserved.

* Contributor(s): Ken Nielson.
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
