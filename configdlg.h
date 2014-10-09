/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: configdlg.h.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
*/

#ifndef CONFIGDLG_H
#define CONFIGDLG_H

#include <QDialog>
#include <QTreeWidgetItem>


class MainWindow;


namespace Ui {
class ConfigDlg;
}

class ConfigDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDlg(QWidget *parent = 0);
    ~ConfigDlg();

    void init();

private slots:
	void on_checkBox_UsingMultiMoms_stateChanged();

	void on_treeWidget_Commands_itemActivated(QTreeWidgetItem * item, int column);
	void on_actionShow_Properties_triggered();

    void on_rbtn_Remote_toggled(bool checked);
    void on_comboBox_Remote_Server_editTextChanged(const QString & text);

    void on_btnEditList_clicked();

private:
    Ui::ConfigDlg *ui;

    MainWindow* m_mainWindow;

    void CreateContextMenus();

    void updateCommandListWithLocal();
    void updateCommandListWithRemote( QString remoteServer );

	void done( int result );
};

#endif // CONFIGDLG_H
