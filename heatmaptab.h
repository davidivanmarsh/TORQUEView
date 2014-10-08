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
