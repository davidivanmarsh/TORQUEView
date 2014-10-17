/*
* The contents of this file are subject to the Mozilla Public License
* Version 1.1 (the "License"); you may not use this file except in
* compliance with the License. You may obtain a copy of the License at
* http://www.mozilla.org/MPL/

* Software distributed under the License is distributed on an "AS IS"
* basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
* License for the specific language governing rights and limitations
* under the License.

* The Original Code is: remoteservercomboboxeditdlg.h.

* The Initial Developer of the Original Code is David I. Marsh.
* All Rights Reserved.

* Contributor(s): Ken Nielson.
*/


#ifndef REMOTESERVERCOMBOBOXEDITDLG_H
#define REMOTESERVERCOMBOBOXEDITDLG_H

#include <QDialog>

namespace Ui {
class RemoteServerComboboxEditDlg;
}

class RemoteServerComboboxEditDlg : public QDialog
{
	Q_OBJECT

public:
	explicit RemoteServerComboboxEditDlg(QStringList remoteServerList, QWidget *parent = 0);
	~RemoteServerComboboxEditDlg();

	QStringList m_List;
	int m_listIndex;

private slots:
	void on_btnAdd_clicked();
	void on_btnRemove_clicked();
	void on_listWidget_itemSelectionChanged();


private:
	Ui::RemoteServerComboboxEditDlg *ui;

	QStringList m_remoteServerList;

	void done( int result );
};

#endif // REMOTESERVERCOMBOBOXEDITDLG_H
