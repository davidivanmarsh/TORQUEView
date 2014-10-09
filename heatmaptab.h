/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: heatmaptab.h.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
*/


#ifndef HEATMAPTAB_H
#define HEATMAPTAB_H

#include <QWidget>
#include <QProcess>
#include "pbsnodestab.h"

class MainWindow;


namespace Ui {
class HeatMapTab;
}

class HeatMapTab : public QWidget
{
    Q_OBJECT

public:
    explicit HeatMapTab(QWidget *parent = 0);
    ~HeatMapTab();

    void clearLists();
    void checkShowNodeNamesCheckbox( bool bCheck ); // check the checkbox or not
    void clearLastHomeDirectory();
    void refresh();
    void hideLists();
    void resetErrorMsgDlg();
//  PbsNode* getLastNodeSelected();
    void issueCmd_Momctl_d3(QString nodeName, QString momManagerPort); // issue a "momctl -d3" command for that node -- parse output data
    bool updateLists();

    void enableControls();
    void disableControls();

private slots:
    void on_tableWidget_Nodes_itemSelectionChanged ();

    void on_checkBox_ShowLists_stateChanged();
    void on_checkBox_ShowNodeNames_stateChanged();

    void on_spinBox_RowCount_valueChanged ( int i );
    void on_spinBox_RowHeight_valueChanged ( int i );
    void on_spinBox_ColumnWidth_valueChanged ( int i );

    void momctl_getStdout();
    void momctl_getStderr();
    void momctl_processStdout();

    void on_actionStart_MOM_triggered();
    void on_actionStart_MOM_Head_node_triggered();
    void on_actionStop_MOM_triggered();
    void on_actionStop_MOM_Head_node_triggered();
    void on_actionMark_node_as_OFFLINE_triggered();
    void on_actionClear_OFFLINE_Node_triggered();
    void on_actionAdd_Note_triggered();
    void on_actionRemove_Note_triggered();
    void on_actionLog_Viewer_triggered();

private:
    Ui::HeatMapTab *ui;

    MainWindow* m_mainWindow;
    PbsNode* m_lastNodeSelected;	// save off last node user clicked on (in table or tree list) - used for auto-refreshing last node


    bool m_showMomctl_d3_STDERROutput;

    QString m_momCtl_Stdout;
    QString m_momCtl_Stderr;
    QProcess* m_momctlProcess;
    QStringList m_momCtl_Lines;  // used when taking a snapshot and writing out momctl lines
    QString m_lastHomeDirectory;

    void CreateContextMenus();

};

#endif // HEATMAPTAB_H
