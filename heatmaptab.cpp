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


#include "heatmaptab.h"
#include "ui_heatmaptab.h"
#include "mainwindow.h"
#include "addnotedlg.h"
#include "logviewerdlg.h"
#include "errormsgdlg.h"
#include <QMessageBox>
#include <QTimer>
#include <QDateTime>
#include <QSettings>



/*******************************************************************************
 *
*******************************************************************************/
HeatMapTab::HeatMapTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HeatMapTab)
{
    ui->setupUi(this);
    CreateContextMenus();

	m_mainWindow = dynamic_cast<MainWindow*>(parent);
	if (m_mainWindow == NULL)
	{
		return;
	}

    ui->treeWidget_NodeInfo->header()->resizeSection(0, 195);	// Label
//	ui->treeWidget_NodeInfo->header()->resizeSection(1, 125);	// Value

    ui->treeWidget_MomCtl->header()->resizeSection(0, 215);	// Label
//	ui->treeWidget_MomCtl->header()->resizeSection(1, 125);	// Value

	// set default sizes for main splitter panes
	ui->splitter_3->setStretchFactor(0,26);  // splitter between legend and heat map
	ui->splitter_3->setStretchFactor(1,1);  // splitter between legend and heat map
    ui->splitter->setStretchFactor(0,1);  // splitter between nodeInfo and momctl lists
    ui->splitter->setStretchFactor(1,2);  // splitter between nodeInfo and momctl lists
    ui->splitter_4->setStretchFactor(0,2);
    ui->splitter_4->setStretchFactor(1,1);

    // at first, hide nodeInfo and momctl lists
    ui->checkBox_ShowLists->setChecked(false);  // uncheck "Show lists" checkbox
    // hide nodeInfo and momctl lists
    ui->splitter->hide();

    resetErrorMsgDlg();

}

/*******************************************************************************
 *
*******************************************************************************/
HeatMapTab::~HeatMapTab()
{
    delete ui;
}


/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::CreateContextMenus()
{
	QAction *separator1 = new QAction(this);
	separator1->setSeparator(true);
	QAction *separator2 = new QAction(this);
	separator2->setSeparator(true);
	QAction *separator3 = new QAction(this);
	separator3->setSeparator(true);

	ui->tableWidget_Nodes->addAction(ui->actionStart_MOM);
	ui->tableWidget_Nodes->addAction(ui->actionStart_MOM_Head_node);
	ui->tableWidget_Nodes->addAction(ui->actionStop_MOM);
	ui->tableWidget_Nodes->addAction(ui->actionStop_MOM_Head_node);
//	ui->tableWidget_Nodes->addAction(ui->actionMomctl_q_2);
	ui->tableWidget_Nodes->addAction(separator1);
	ui->tableWidget_Nodes->addAction(ui->actionMark_node_as_OFFLINE);
	ui->tableWidget_Nodes->addAction(ui->actionClear_OFFLINE_Node);
	ui->tableWidget_Nodes->addAction(separator2);
	ui->tableWidget_Nodes->addAction(ui->actionAdd_Note);
	ui->tableWidget_Nodes->addAction(ui->actionRemove_Note);
	ui->tableWidget_Nodes->addAction(separator3);
	ui->tableWidget_Nodes->addAction(ui->actionLog_Viewer);
	ui->tableWidget_Nodes->setContextMenuPolicy(Qt::ActionsContextMenu);
}

/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::resetErrorMsgDlg()
{
    m_showMomctl_d3_STDERROutput = true;
}

/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::checkShowNodeNamesCheckbox( bool bCheck )
{
    ui->checkBox_ShowNodeNames->blockSignals(true);	// prevent checkbox signals from being received (we don't want updateLists to be called AGAIN here)
    ui->checkBox_ShowNodeNames->setChecked( bCheck );
    ui->checkBox_ShowNodeNames->blockSignals(false);	// prevent checkbox signals from being received (we don't want updateLists to be called AGAIN here)
}

/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::hideLists()
{
    // at first, hide nodeInfo and momctl lists
    ui->checkBox_ShowLists->setChecked(false);  // uncheck "Show lists" checkbox
}

/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::clearLists()
{
	// re-init all GUI widgets
	ui->tableWidget_Nodes->setRowCount(0);
	ui->tableWidget_Nodes->setColumnCount(1);
	ui->tableWidget_Nodes->clear();  // clear out table
//	ui->tableWidget_Nodes->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//	ui->tableWidget_Nodes->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	if (m_mainWindow->m_Config_HeatMapShowNodeNames)
	{
		ui->tableWidget_Nodes->verticalHeader()->setDefaultSectionSize( m_mainWindow->m_Config_HeatMapRowHeight_WithNodeNames );
		ui->tableWidget_Nodes->horizontalHeader()->setDefaultSectionSize( m_mainWindow->m_Config_HeatMapColumnWidth_WithNodeNames );
	}
	else
	{
		ui->tableWidget_Nodes->verticalHeader()->setDefaultSectionSize( m_mainWindow->m_Config_HeatMapRowHeight_NoNodeNames );
		ui->tableWidget_Nodes->horizontalHeader()->setDefaultSectionSize( m_mainWindow->m_Config_HeatMapColumnWidth_NoNodeNames );
	}

	ui->tableWidget_Nodes->verticalHeader()->hide();
	ui->tableWidget_Nodes->horizontalHeader()->hide();

	ui->tableWidget_Legend->verticalHeader()->hide();
//	ui->tableWidget_Legend->verticalHeader()->setDefaultSectionSize( m_mainWindow->m_Config_HeatMapRowHeight * 3 / 4);
	QFontMetrics fm = ui->tableWidget_Nodes->fontMetrics();
//	int lineSpacing = fm.lineSpacing();  // base legend height on table's font metrics
	int height = fm.height();  // base legend height on table's font metrics
	ui->tableWidget_Legend->verticalHeader()->setDefaultSectionSize( height + 2 );
}

/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::clearLastHomeDirectory()
{
    m_lastHomeDirectory = "";
}

/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::refresh()
{
//  m_lastExecHosts = ""; // clear out
//  m_lastJobId = "";
    clearLastHomeDirectory();

    m_mainWindow->initAllTabs(false); // don't include qmgr - Ken says it doesn't need to be updated on refresh
}

/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::on_checkBox_ShowLists_stateChanged()
{
    if (ui->checkBox_ShowLists->isChecked())
    {
        // show nodeInfo and momctl lists
        ui->splitter->show();
    }
    else
    {
        // hide nodeInfo and momctl lists
        ui->splitter->hide();
    }
}

/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::on_checkBox_ShowNodeNames_stateChanged()
{
    bool bIsChecked = ui->checkBox_ShowNodeNames->isChecked();
    QSettings mysettings( "AdaptiveComputing", "TORQUEView" );
    mysettings.setValue( "HeatMapShowNodeNames", bIsChecked );

    m_mainWindow->m_Config_HeatMapShowNodeNames = bIsChecked;

    clearLists();
    updateLists();  // don't re-read the data from the local/remote server, just update the lists using current data

}

/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::on_spinBox_RowCount_valueChanged ( int i )
{
    QSettings mysettings( "AdaptiveComputing", "TORQUEView" );
    if (m_mainWindow->m_Config_HeatMapShowNodeNames)
    {
        m_mainWindow->m_Config_HeatMapRowCount_WithNodeNames = i;
        mysettings.setValue( "HeatMapRowCount_WithNodeNames", m_mainWindow->m_Config_HeatMapRowCount_WithNodeNames );
    }
    else
    {
        m_mainWindow->m_Config_HeatMapRowCount_NoNodeNames = i;
        mysettings.setValue( "HeatMapRowCount_NoNodeNames", m_mainWindow->m_Config_HeatMapRowCount_NoNodeNames );
    }

    clearLists();
    updateLists();  // don't re-read the data from the local/remote server, just update the lists using current data
}

/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::on_spinBox_RowHeight_valueChanged ( int i )
{
    QSettings mysettings( "AdaptiveComputing", "TORQUEView" );
    if (m_mainWindow->m_Config_HeatMapShowNodeNames)
    {
        m_mainWindow->m_Config_HeatMapRowHeight_WithNodeNames = i;
        mysettings.setValue( "HeatMapRowHeight_WithNodeNames", m_mainWindow->m_Config_HeatMapRowHeight_WithNodeNames );
        ui->tableWidget_Nodes->verticalHeader()->setDefaultSectionSize( m_mainWindow->m_Config_HeatMapRowHeight_WithNodeNames );
    }
    else
    {
        m_mainWindow->m_Config_HeatMapRowHeight_NoNodeNames = i;
        mysettings.setValue( "HeatMapRowHeight_NoNodeNames", m_mainWindow->m_Config_HeatMapRowHeight_NoNodeNames );
        ui->tableWidget_Nodes->verticalHeader()->setDefaultSectionSize( m_mainWindow->m_Config_HeatMapRowHeight_NoNodeNames );
    }

}

/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::on_spinBox_ColumnWidth_valueChanged ( int i )
{
    QSettings mysettings( "AdaptiveComputing", "TORQUEView" );
    if (m_mainWindow->m_Config_HeatMapShowNodeNames)
    {
        m_mainWindow->m_Config_HeatMapColumnWidth_WithNodeNames = i;
        mysettings.setValue( "HeatMapColumnWidth_WithNodeNames", m_mainWindow->m_Config_HeatMapColumnWidth_WithNodeNames );
        ui->tableWidget_Nodes->horizontalHeader()->setDefaultSectionSize( m_mainWindow->m_Config_HeatMapColumnWidth_WithNodeNames );
    }
    else
    {
        m_mainWindow->m_Config_HeatMapColumnWidth_NoNodeNames = i;
        mysettings.setValue( "HeatMapColumnWidth_NoNodeNames", m_mainWindow->m_Config_HeatMapColumnWidth_NoNodeNames );
        ui->tableWidget_Nodes->horizontalHeader()->setDefaultSectionSize( m_mainWindow->m_Config_HeatMapColumnWidth_NoNodeNames );
    }
}

/*******************************************************************************
 *  if m_execHost isn't empty, highlight those hosts (showing hosts specified job is running on)
*******************************************************************************/
bool HeatMapTab::updateLists()
{
    int num_table_rows = 0;
    // is the heatMap displaying with node names, or without?
    if (m_mainWindow->m_Config_HeatMapShowNodeNames)
        num_table_rows = m_mainWindow->m_Config_HeatMapRowCount_WithNodeNames;
    else
        num_table_rows = m_mainWindow->m_Config_HeatMapRowCount_NoNodeNames;

    int lastCol = 0;

//  QStringList slExecHosts = getExecHost(); // get the list of hosts the specified job is running on


    // pre-init table with rows
    for (int i=0; i<num_table_rows; i++)
    {
        ui->tableWidget_Nodes->insertRow(i);
    }

    // get the m_pbsNodes from pbsNodesTab
    for (int i=0; i < m_mainWindow->m_pbsNodesTab->m_pbsNodes.size(); i++)
    {
        // insert data into table
        QTableWidgetItem *tableItem = new QTableWidgetItem();
        int row = i % num_table_rows;
        int col = i / num_table_rows;
        if (col > lastCol)
        {
            ui->tableWidget_Nodes->insertColumn(col);
            lastCol = col;
        }
        ui->tableWidget_Nodes->setItem(row, col, tableItem);
        // attach pbsnode index to the item
        tableItem->setData(Qt::UserRole, QVariant::fromValue(i));

        if ( m_mainWindow->m_Config_HeatMapShowNodeNames )  // show the node names in each cell?
            tableItem->setText(m_mainWindow->m_pbsNodesTab->m_pbsNodes[i]->m_nodeName);

        QString sNodeState;
        QIcon icon;
        QColor backgroundcolor;

		bool bJobsPresent = true;
		if (m_mainWindow->m_pbsNodesTab->m_pbsNodes[i]->m_jobs.isEmpty())
			bJobsPresent = false;

        // get node state
        m_mainWindow->m_pbsNodesTab->getNodeInfo(
                  m_mainWindow->m_pbsNodesTab->m_pbsNodes[i]->m_nodeState,
                  m_mainWindow->m_pbsNodesTab->m_pbsNodes[i]->m_sNodeState,
                  sNodeState, bJobsPresent,
                  m_mainWindow->m_pbsNodesTab->m_pbsNodes[i]->m_bErrorStatus,
                  icon, backgroundcolor);

		if (m_mainWindow->m_pbsNodesTab->m_pbsNodes[i]->m_availableThreads != -1) // is this a NUMA node?
		{		// for NUMA:
			float percent = 0.0f;
			if (m_mainWindow->m_pbsNodesTab->m_pbsNodes[i]->m_totalThreads > 0)
				percent = (m_mainWindow->m_pbsNodesTab->m_pbsNodes[i]->m_availableThreads * 100.0) /
						m_mainWindow->m_pbsNodesTab->m_pbsNodes[i]->m_totalThreads;
//			tableItem->setBackground(QBrush(Qt::blue, Qt::DiagCrossPattern));
			if (percent <= 20.0)
				tableItem->setBackground(QBrush(backgroundcolor, Qt::DiagCrossPattern));
			else if (percent <= 40.0)
				tableItem->setBackground(QBrush(backgroundcolor, Qt::Dense6Pattern));
			else if (percent <= 60.0)
				tableItem->setBackground(QBrush(backgroundcolor, Qt::Dense4Pattern));
			else if (percent <= 80.0)
				tableItem->setBackground(QBrush(backgroundcolor, Qt::Dense2Pattern));
			else
				tableItem->setBackground(backgroundcolor);
		}
		else
		{
			tableItem->setBackground(backgroundcolor);
		}

//        if (slExecHosts.contains(m_pbsNodes[i]->m_nodeName)) // is this one of the nodes job is running on?
//        {
//            tableItem->setIcon(QIcon(":/icons/images/icon_yellow.bmp"));
//            tableItem->setToolTip(QString("Node: %1\nState: %2\nIs running job:  %3").arg(m_pbsNodes[i]->m_nodeName).arg(sNodeState).arg(m_lastJobId));

//            item->setText(2,  QString("%1").arg(m_lastJobId)); // show job currently running
//            item->setBackgroundColor(2,  QColor("lightyellow"));
//        }
//        else
//        {
            if (m_mainWindow->m_pbsNodesTab->m_pbsNodes[i]->m_jobs.count() > 0)
            {
                if (m_mainWindow->m_pbsNodesTab->m_pbsNodes[i]->m_availableThreads >= 0)  // m_availableThreads needs to be initialized to -1
                    tableItem->setToolTip(QString("Node: %1\nState: %2\nAvailable threads: %3\nTotal threads: %4\n\nJobs Running:\n%5")
                                          .arg(m_mainWindow->m_pbsNodesTab->m_pbsNodes[i]->m_nodeName)
                                          .arg(sNodeState)
                                          .arg(m_mainWindow->m_pbsNodesTab->m_pbsNodes[i]->m_availableThreads)
                                          .arg(m_mainWindow->m_pbsNodesTab->m_pbsNodes[i]->m_totalThreads)
                                          .arg(m_mainWindow->m_pbsNodesTab->m_pbsNodes[i]->m_jobs.join("\n")));
                else
                    tableItem->setToolTip(QString("Node: %1\nState: %2\n\nJobs Running:\n%3")
                                          .arg(m_mainWindow->m_pbsNodesTab->m_pbsNodes[i]->m_nodeName)
                                          .arg(sNodeState)
                                          .arg(m_mainWindow->m_pbsNodesTab->m_pbsNodes[i]->m_jobs.join("\n")));

            }
            else
            {
                if (m_mainWindow->m_pbsNodesTab->m_pbsNodes[i]->m_availableThreads >= 0)  // m_availableThreads needs to be initialized to -1
                    tableItem->setToolTip(QString("Node: %1\nState: %2\nAvailable threads: %3\nTotal threads: %4")
                                          .arg(m_mainWindow->m_pbsNodesTab->m_pbsNodes[i]->m_nodeName)
                                          .arg(sNodeState)
                                          .arg(m_mainWindow->m_pbsNodesTab->m_pbsNodes[i]->m_availableThreads)
                                          .arg(m_mainWindow->m_pbsNodesTab->m_pbsNodes[i]->m_totalThreads));
                else
                    tableItem->setToolTip(QString("Node: %1\nState: %2")
                                          .arg(m_mainWindow->m_pbsNodesTab->m_pbsNodes[i]->m_nodeName)
                                          .arg(sNodeState));

             }
//        }

    }

    QFontMetrics fm = ui->tableWidget_Nodes->fontMetrics();
//	int width = fm.width("WW");  // base width on table's font metrics
//	ui->tableWidget_Nodes->setVisible(false);
//	ui->tableWidget_Nodes->resizeColumnsToContents();

    // (this code works but is not needed:)
//	for (int j =0; j < ui->tableWidget_Nodes->columnCount(); j++)
//		ui->tableWidget_Nodes->horizontalHeader()->resizeSection(j, width);

//	ui->tableWidget_Nodes->setVisible(true);

    // update row count, etc
    ui->spinBox_RowCount->blockSignals(true);	// prevent spinbox signals from being received (we don't want updateLists to be called AGAIN here)
    ui->spinBox_RowHeight->blockSignals(true);	// prevent spinbox signals from being received (we don't want updateLists to be called AGAIN here)
    ui->spinBox_ColumnWidth->blockSignals(true);	// prevent spinbox signals from being received (we don't want updateLists to be called AGAIN here)

    if (m_mainWindow->m_Config_HeatMapShowNodeNames)
    {
        ui->spinBox_RowCount->setValue( m_mainWindow->m_Config_HeatMapRowCount_WithNodeNames );
        ui->spinBox_RowHeight->setValue( m_mainWindow->m_Config_HeatMapRowHeight_WithNodeNames );
        ui->spinBox_ColumnWidth->setValue( m_mainWindow->m_Config_HeatMapColumnWidth_WithNodeNames );
    }
    else
    {
        ui->spinBox_RowCount->setValue( m_mainWindow->m_Config_HeatMapRowCount_NoNodeNames );
        ui->spinBox_RowHeight->setValue( m_mainWindow->m_Config_HeatMapRowHeight_NoNodeNames );
        ui->spinBox_ColumnWidth->setValue( m_mainWindow->m_Config_HeatMapColumnWidth_NoNodeNames );
    }

    ui->spinBox_RowCount->blockSignals(false);	// allow spinbox signals to be received
    ui->spinBox_RowHeight->blockSignals(false);	// allow spinbox signals to be received
    ui->spinBox_ColumnWidth->blockSignals(false);	// allow spinbox signals to be received

    return true;
}

/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::enableControls()
{
    // data is "live" (i.e, from either a local or remote host instead of a snapshot file), so enable all context menu items
    ui->actionStart_MOM->setEnabled(true);
    ui->actionStart_MOM_Head_node->setEnabled(true);
    ui->actionStop_MOM->setEnabled(true);
    ui->actionStop_MOM_Head_node->setEnabled(true);
    ui->actionMark_node_as_OFFLINE->setEnabled(true);
    ui->actionClear_OFFLINE_Node->setEnabled(true);
    ui->actionAdd_Note->setEnabled(true);
    ui->actionRemove_Note->setEnabled(true);
    ui->actionLog_Viewer->setEnabled(true);
}

/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::disableControls()
{
    // data is NOT "live" (i.e, it's from a snapshot file rather than a local or remote host), so disable all context menu items
    ui->actionStart_MOM->setEnabled(false);
    ui->actionStart_MOM_Head_node->setEnabled(false);
    ui->actionStop_MOM->setEnabled(false);
    ui->actionStop_MOM_Head_node->setEnabled(false);
    ui->actionMark_node_as_OFFLINE->setEnabled(false);
    ui->actionClear_OFFLINE_Node->setEnabled(false);
    ui->actionAdd_Note->setEnabled(false);
    ui->actionRemove_Note->setEnabled(false);
    ui->actionLog_Viewer->setEnabled(false);
}

/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::on_tableWidget_Nodes_itemSelectionChanged ()
{
    // called when user clicks on an item in the Nodes tree
    QList<QTableWidgetItem*> selectedItems = ui->tableWidget_Nodes->selectedItems();
    if ( selectedItems.size() == 0)
    {
        ui->treeWidget_NodeInfo->clear();
        ui->treeWidget_MomCtl->clear();
    }
    else
    {
        // show nodeInfo and momctl lists
//        ui->splitter->setStretchFactor(0,1);  // splitter between nodeInfo and momctl lists
//        ui->splitter->setStretchFactor(1,2);  // splitter between nodeInfo and momctl lists
//        ui->splitter_4->setStretchFactor(0,2);
//        ui->splitter_4->setStretchFactor(1,1);
//        ui->label_Title_NodeInfo->show();
//        ui->label_Title_MomCtl->show();
//        ui->treeWidget_NodeInfo->show();
//        ui->treeWidget_MomCtl->show();

//        ui->checkBox_ShowLists->setChecked(true);  // check "Show lists" checkbox


        ui->treeWidget_NodeInfo->clear();
        ui->treeWidget_MomCtl->clear();

        QTableWidgetItem* item = selectedItems[0];	// list is single-select
        // get index from item's data
        QVariant qv = item->data(Qt::UserRole);
        int index = qv.value<int>();
        PbsNode* node = m_mainWindow->m_pbsNodesTab->m_pbsNodes.at(index);

        ui->label_Title_NodeInfo->setText( QString("Node name:  %1").arg(node->m_nodeName) );
        m_mainWindow->m_pbsNodesTab->populateNodeInfo(node, ui->treeWidget_NodeInfo);

        // now update the momctl groupbox and treeewidget
        ui->label_Title_MomCtl->setText("");	// clear out title label
//		ui->label_CmdExecuted_Momctl->setText("");	// clear out "cmd executed" label at bottom
        ui->treeWidget_MomCtl->clear();	// clear out list

        m_lastNodeSelected = node;	// save off node

        // if state is "down", "offline", etc. don't go any further
        switch (node->m_nodeState) {
        case pbs_down:
        case pbs_offline:
        case pbs_multiple:
        case pbs_state_unknown:
            return;
        default:
            break; // keep on truckin..
        }

        int dataSource = m_mainWindow->getComboBox_DataSource_CurrentIndex();
        if (dataSource == 0 || dataSource == 1)  // if data is coming from either Local host or Remote host
        {
            issueCmd_Momctl_d3(node->m_nodeName, node->m_momManagerPort);
        }
        else  // else coming from snapshot file
        {
            // get the momctrl stringlist from the hash
            m_mainWindow->m_pbsNodesTab->populateMomctlFromFile(node->m_nodeName, ui->treeWidget_MomCtl, ui->label_Title_MomCtl);
        }
    }
}

/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::issueCmd_Momctl_d3(QString nodeName, QString momManagerPort)  // execute "momctl -d3" command call for that node -- parse output data
{
    QString momctlCommand = QString (m_mainWindow->m_Config_Cmd_Momctl_d3)
            .arg(nodeName)
            .arg(momManagerPort);


    // show "Wait cursor"
    QApplication::setOverrideCursor(Qt::WaitCursor);

    m_momCtl_Stdout.clear();
    m_momCtl_Stderr.clear();

    m_momctlProcess = new QProcess(this);
    connect (m_momctlProcess, SIGNAL(readyReadStandardOutput()),
             this, SLOT(momctl_getStdout())); // connect process signals with your code
    connect (m_momctlProcess, SIGNAL(readyReadStandardError()),
             this, SLOT(momctl_getStderr())); // same here
    m_momctlProcess->start(momctlCommand);

    if (!m_momctlProcess->waitForStarted())
    {
        QMessageBox::critical(0, "Error trying to start momctl process", "Process's waitForStarted() method returned error.\n");
        QApplication::restoreOverrideCursor();
        return;
    }

    if (!m_momctlProcess->waitForFinished(5000)) //If msecs is -1, this function will not time out
    {
        QMessageBox::critical(0, "Error trying to complete momctl process", "Process's waitForFinished() method returned error.\n");
        QApplication::restoreOverrideCursor();
        return;
    }

    delete m_momctlProcess;
    m_momctlProcess = NULL;


    momctl_processStdout();

    // restore the original cursor
    QApplication::restoreOverrideCursor();


    if (m_showMomctl_d3_STDERROutput)
    {
        // see if there are any errors to display
        if (!m_momCtl_Stderr.isEmpty())
        {
//            QMessageBox::critical(0, "Error issuing momctl command",
//                                  QString("Error issuing 'momctl -d3'' command.  Error message was: %1").arg(m_momCtl_Stderr));
            ErrorMsgDlg dlg("TORQUEView Error", "Error issuing 'momctl -d3' command. Error message:", m_momCtl_Stderr);
            dlg.exec();
            if (dlg.isDontShowThisAgainChecked())  // see if the "Don't show this dialog again" checkbox was checked
                m_showMomctl_d3_STDERROutput = false;
            m_lastNodeSelected = NULL;
        }
    }

}

/*******************************************************************************
 *
*******************************************************************************/
// called from momctlFromCmd() - this gets called whenever the momctl process has something to say...
void HeatMapTab::momctl_getStdout()
{
    QByteArray s = m_momctlProcess->readAllStandardOutput(); // read normal output
    m_momCtl_Stdout.append( s );  // if there's any stdout
}

/*******************************************************************************
 *
*******************************************************************************/
// called from momctlFromCmd() - this gets called whenever the momctl process has something to say...
void HeatMapTab::momctl_getStderr()
{
    QByteArray s = m_momctlProcess->readAllStandardError(); // read error channel
    m_momCtl_Stderr.append( s );  // if there's any stderr
}

/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::momctl_processStdout() // parse the stdout data collected (above)
{
    ui->treeWidget_MomCtl->clear();	// clear out list

    bool bFirstLine = true;


    QStringList lines = m_momCtl_Stdout.split("\n");
    foreach (QString line, lines)
    {
        // if we're in the middle of taking a snapshot, then save out each line to m_momctlrs list
        if (m_mainWindow->takingSnapshot())
            m_momCtl_Lines.append(line);


        if (line.isEmpty())
            continue;

        // continue adding fields to existing node
        line = line.trimmed();	// strip off white space at beginning and end

        if (bFirstLine)
        {
            // get rid of any lines before the:
            // "Host: dmarsh-cray-simulator/dmarsh-cray-simulator   Version: 4.2.6.1   PID: 1395" line
            if (!line.startsWith("Host"))
                continue;

            // first line of momctl output is something like:
            // Host: dmarsh-cray-simulator/dmarsh-cray-simulator   Version: 4.2.6.1   PID: 1395
            ui->label_Title_MomCtl->setText( line );
            bFirstLine = false;
            continue;
        }

        QStringList fields = line.split(":");
        QString sLabel = fields[0];
        QString sValue;
        if (fields.size() > 1)
        {
            fields.removeAt(0);	// remove the first item in the stringlist (the label)
            sValue = fields.join(":");	// concat all the rest --put the colon back in
        }
        sLabel = sLabel.trimmed();
        sValue = sValue.trimmed();

        // special case 1: if label starts with "job[", as in:
        // "job[3801.kmn.ac]  state=RUNNING cput=0 mem=2568 vmem=37060 sidlist=6269"
        if (sLabel.startsWith("job["))
        {
            QStringList parts = sLabel.split("  ");
            sLabel = parts[0];
            if (parts.size() > 1)
                sValue = parts[1];
        }

    //	int length = sValue.length();
        QTreeWidgetItem * rootitem = new QTreeWidgetItem(ui->treeWidget_MomCtl);
        rootitem->setText(0, sLabel);
    //	rootitem->setIcon(0, QIcon(":/icons/images/icon_green.bmp"));
    //	rootitem->setExpanded(true);

        // special case 2:  if it's the Trusted Client List, parse out each client
        // and add as subitems
        if (sLabel.startsWith("Trusted Client List"))
        {
            QStringList addrs = sValue.split(",");
            foreach (QString s, addrs)
            {
                QTreeWidgetItem * item = new QTreeWidgetItem(rootitem);
                item->setText(0, s);
            //	item->setIcon(0, QIcon(":/icons/images/icon_green.bmp"));
            //	item->setText(1, sValue);	// (don't put anything in column 1)
            }
            continue;
        }
        // special case 3:  if it's the HomeDirectory, get the directory
        else if (sLabel.startsWith("HomeDirectory"))
        {
            m_lastHomeDirectory = sValue;

    //		QDateTime now = QDateTime::currentDateTime();
    //		QDateTime eventDateTime = it.key();
    //		int instance_count = it.value();

    //		QString sDate = now.toString("yyyyMMdd");
    //		QString sDate = now.toString("yyyy-MM-dd hh:mm:ss a");
    //		QString sTime = now.toString("hh:mm:ss a");

			// NOTE: fix up the path. It will no longer be the "HomeDirectory" but will be the path of the
			// "/mom_logs/" directory, which is a sibling to the /mom_priv/ directory.  (Maybe we should rename it??)
			QString mom_priv("mom_priv");
            if (m_lastHomeDirectory.endsWith(mom_priv))
            {
                m_lastHomeDirectory.chop(mom_priv.size());
                m_lastHomeDirectory.append("mom_logs");
                m_lastHomeDirectory.append("/");
            }
        }

        rootitem->setText(1, sValue);
        rootitem->setToolTip( 1, sValue );
    }

}


/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::on_actionStart_MOM_triggered()
{
    //  starting MOM.... (user clicked on tableWidget_Nodes list)

    // called when user clicks on an item in the Nodes tree
    QList<QTableWidgetItem*> selectedItems = ui->tableWidget_Nodes->selectedItems();
    foreach (QTableWidgetItem* item, selectedItems)	// table is extended-select
    {
        // get index from item's data
        QVariant qv = item->data(Qt::UserRole);
        int index = qv.value<int>();
        PbsNode* node = m_mainWindow->m_pbsNodesTab->m_pbsNodes.at(index);

        m_mainWindow->m_pbsNodesTab->issueCmd_StartMOM( node, false );  // false=for a standard compute node
    }

//	QThread::sleep(3);	// doesn't compile on Ken's box
    // sleep 3 secs -- starting a MOM seems to need several secs to start (why is this, Ken?)
    QEventLoop loop;
    QTimer::singleShot(3000, &loop, SLOT(quit()));
    loop.exec();

    refresh();	// refresh all lists
}

/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::on_actionStart_MOM_Head_node_triggered()
{
    //  starting MOM.... (user clicked on tableWidget_Nodes list)

    // called when user clicks on an item in the Nodes tree
    QList<QTableWidgetItem*> selectedItems = ui->tableWidget_Nodes->selectedItems();
    foreach (QTableWidgetItem* item, selectedItems)	// table is extended-select
    {
        // get index from item's data
        QVariant qv = item->data(Qt::UserRole);
        int index = qv.value<int>();
        PbsNode* node = m_mainWindow->m_pbsNodesTab->m_pbsNodes.at(index);

        m_mainWindow->m_pbsNodesTab->issueCmd_StartMOM( node, true ); // true=for a head node
    }

//	QThread::sleep(3);	// doesn't compile on Ken's box
    // sleep 3 secs -- starting a MOM seems to need several secs to start (why is this, Ken?)
    QEventLoop loop;
    QTimer::singleShot(3000, &loop, SLOT(quit()));
    loop.exec();

    refresh();	// refresh all lists
}

/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::on_actionStop_MOM_triggered()
{
    // stopping MOM...(user clicked on tableWidget_Nodes list)

    QList<QTableWidgetItem*> selectedItems = ui->tableWidget_Nodes->selectedItems();
    foreach (QTableWidgetItem* item, selectedItems)	// list is extended-select
    {
        // get index from item's data
        QVariant qv = item->data(Qt::UserRole);
        int index = qv.value<int>();
        PbsNode* node = m_mainWindow->m_pbsNodesTab->m_pbsNodes.at(index);

        m_mainWindow->m_pbsNodesTab->issueCmd_StopMOM( node, false );  // false=for a standard compute node
    }

    refresh();	// refresh all lists
}

/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::on_actionStop_MOM_Head_node_triggered()
{
    // stopping MOM...(user clicked on tableWidget_Nodes list)

    QList<QTableWidgetItem*> selectedItems = ui->tableWidget_Nodes->selectedItems();
    foreach (QTableWidgetItem* item, selectedItems)	// list is extended-select
    {
        // get index from item's data
        QVariant qv = item->data(Qt::UserRole);
        int index = qv.value<int>();
        PbsNode* node = m_mainWindow->m_pbsNodesTab->m_pbsNodes.at(index);

        m_mainWindow->m_pbsNodesTab->issueCmd_StopMOM( node, true ); // true=for a head node
    }

    refresh();	// refresh all lists
}

/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::on_actionMark_node_as_OFFLINE_triggered()
{
    // Marking Node as OFFLINE...(for nodes tableWidget)

    QList<QTableWidgetItem*> selectedItems = ui->tableWidget_Nodes->selectedItems();
    foreach (QTableWidgetItem* item, selectedItems)	// list is extended-select
    {
        // get index from item's data
        QVariant qv = item->data(Qt::UserRole);
        int index = qv.value<int>();
        PbsNode* node = m_mainWindow->m_pbsNodesTab->m_pbsNodes.at(index);

        m_mainWindow->m_pbsNodesTab->issueCmd_MarkNodeAsOFFLINE( node );
    }

    refresh();	// refresh all lists
}

/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::on_actionClear_OFFLINE_Node_triggered()
{
    //	Clearing OFFLINE Node...(from nodes tableWidget)

    QList<QTableWidgetItem*> selectedItems = ui->tableWidget_Nodes->selectedItems();
    foreach (QTableWidgetItem* item, selectedItems)	// list is extended-select
    {
        // get index from item's data
        QVariant qv = item->data(Qt::UserRole);
        int index = qv.value<int>();
        PbsNode* node = m_mainWindow->m_pbsNodesTab->m_pbsNodes.at(index);

        m_mainWindow->m_pbsNodesTab->issueCmd_ClearOFFLINENode( node );
    }

    refresh();	// refresh all lists
}

/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::on_actionAdd_Note_triggered()
{
    //  add a note to the node.... (user clicked on tableWidget_Nodes list)
    AddNoteDlg dlg;
//	dlg.setAnnotation("This is some sample text, etc.");
    if (!dlg.exec())
        return;
    QString sAnnotation = dlg.getAnnotation();


    // called when user clicks on an item in the Nodes tree
    QList<QTableWidgetItem*> selectedItems = ui->tableWidget_Nodes->selectedItems();
    foreach (QTableWidgetItem* item, selectedItems)	// table is extended-select
    {
        // get index from item's data
        QVariant qv = item->data(Qt::UserRole);
        int index = qv.value<int>();
        PbsNode* node = m_mainWindow->m_pbsNodesTab->m_pbsNodes.at(index);

        m_mainWindow->m_pbsNodesTab->issueCmd_AddNote( node, sAnnotation );
    }

    refresh();	// refresh all lists
}

/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::on_actionRemove_Note_triggered()
{
    // remove a note from the node...(user clicked on tableWidget_Nodes list)

    QList<QTableWidgetItem*> selectedItems = ui->tableWidget_Nodes->selectedItems();
    foreach (QTableWidgetItem* item, selectedItems)	// list is extended-select
    {
        // get index from item's data
        QVariant qv = item->data(Qt::UserRole);
        int index = qv.value<int>();
        PbsNode* node = m_mainWindow->m_pbsNodesTab->m_pbsNodes.at(index);

        m_mainWindow->m_pbsNodesTab->issueCmd_RemoveNote( node );
    }

    refresh();	// refresh all lists
}

/*******************************************************************************
 *
*******************************************************************************/
void HeatMapTab::on_actionLog_Viewer_triggered()
{
    // bring up the node's log viewer...(user clicked on tableWidget_Nodes list)

    QList<QTableWidgetItem*> selectedItems = ui->tableWidget_Nodes->selectedItems();
//	foreach (QTableWidgetItem* item, selectedItems)	// list is extended-select
    if (selectedItems.count() > 0)
    {
        QTableWidgetItem* item = selectedItems[0];  // only get first one (for now)
        // get index from item's data
        QVariant qv = item->data(Qt::UserRole);
        int index = qv.value<int>();
        PbsNode* node = m_mainWindow->m_pbsNodesTab->m_pbsNodes.at(index);

        // need to get the node's logfilename here and pass it into logViewerDlg call (below) --------

        QString cmd = QString("%1 -f").arg(m_mainWindow->m_Config_Cmd_Tail);

        QString logfilename = m_lastHomeDirectory;
        QDateTime now = QDateTime::currentDateTime();
        QString sDate = now.toString("yyyyMMdd");
//		QString sDate = now.toString("yyyy-MM-dd hh:mm:ss a");
//		QString sTime = now.toString("hh:mm:ss a");

        // NOTE: since according to Ken this directory path isn't right, fix it up:
        logfilename.append(sDate);

        if (m_mainWindow->m_Config_UsingMultiMoms)	// if using MultiMoms, tack on service port number
        {
            QString sServicePort = QString("%1").arg(node->m_momServicePort);
            logfilename.append(".");
            logfilename.append(sServicePort);
        }

        LogViewerDlg* logViewerDlg = new LogViewerDlg(m_mainWindow, logfilename, cmd, this);
        logViewerDlg->show();
    }
}


