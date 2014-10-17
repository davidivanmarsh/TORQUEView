/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: configdlg.cpp.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
*/

#include "configdlg.h"
#include "ui_configdlg.h"
#include "mainwindow.h"
#include "remoteservercomboboxeditdlg.h"
#include "configitempropertiesdlg.h"
#include <QSettings>
#include <QMessageBox>


/*******************************************************************************
 *
*******************************************************************************/
ConfigDlg::ConfigDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDlg)
{
    ui->setupUi(this);
    CreateContextMenus();

	m_mainWindow = dynamic_cast<MainWindow*>(parent);
	if (m_mainWindow == NULL)
	{
		return;
	}

	connect(ui->checkBox_UsingMultiMoms, SIGNAL(stateChanged(int)),
		this, SLOT(on_checkBox_UsingMultiMoms_stateChanged()));
//	connect( comboboxEdit, SIGNAL( activated( const QString& ) ),
//			 this, SLOT( itemActivated( const QString& ) ) );

	// set column widths
	ui->treeWidget_Commands->header()->resizeSection(0, 280);
//	ui->treeWidget_Commands->header()->resizeSection(1, 285);
}

/*******************************************************************************
 *
*******************************************************************************/
ConfigDlg::~ConfigDlg()
{
    delete ui;
}

/*******************************************************************************
 *
*******************************************************************************/
QString ConfigDlg::getRemoteServer()
{
	return m_remoteServer;
}

/*******************************************************************************
 *
*******************************************************************************/
void ConfigDlg::CreateContextMenus()
{
	ui->treeWidget_Commands->addAction(ui->actionShow_Properties);
	ui->treeWidget_Commands->setContextMenuPolicy(Qt::ActionsContextMenu);
}

/*******************************************************************************
 *
*******************************************************************************/
void ConfigDlg::init()
{
	// get remote location
	if (! m_mainWindow->m_Config_RemoteServerList.isEmpty() )
	{
		foreach (QString s, m_mainWindow->m_Config_RemoteServerList)
		{
			ui->comboBox_Remote_Server->addItem(s);
		}
	}
	if (! m_mainWindow->m_Config_RemoteServer.isEmpty() )
	{
		int itemIndex = ui->comboBox_Remote_Server->findText( m_mainWindow->m_Config_RemoteServer );  // now try to find it in the combobox list
		if (itemIndex != -1)  // if we found it
			ui->comboBox_Remote_Server->setCurrentIndex(itemIndex); // select it
	}


	// get local/remote radiobutton state -- either "Remote" or "Local"
	if ( m_mainWindow->m_Config_DataSource == 0 )  // if is Local
	{
		ui->rbtn_Local->setChecked(true);
		ui->comboBox_Remote_Server->setEnabled(false);
		updateCommandListWithLocal();   // with local settings
	}
	else // else is Remote
	{
		ui->rbtn_Remote->setChecked(true);
		ui->comboBox_Remote_Server->setEnabled(true);
		QString remoteServer = ui->comboBox_Remote_Server->currentText();
		updateCommandListWithRemote(remoteServer);
	}

	ui->checkBox_UsingMultiMoms->setChecked( m_mainWindow->m_Config_UsingMultiMoms );
	ui->checkBox_UseServiceToStartStopMOMs->setChecked( m_mainWindow->m_Config_UseServiceToStartStopMOMs );
//	ui->checkBox_ShowSTDERROutput->setChecked( m_mainWindow->m_Config_ShowSTDERROutput );

//	ui->spinBox_RowCount->setValue( m_mainWindow->m_Config_HeatMapRowCount );
//	ui->spinBox_RowHeight->setValue( m_mainWindow->m_Config_HeatMapRowHeight );
//	ui->spinBox_ColumnWidth->setValue( m_mainWindow->m_Config_HeatMapColumnWidth );

}

/*******************************************************************************
 *
*******************************************************************************/
void ConfigDlg::updateCommandListWithLocal()
{
    ui->treeWidget_Commands->clear();

    QTreeWidgetItem* item = NULL;
    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("List nodes and node states"));
    item->setText(1, m_mainWindow->SETTINGS_CMD_Pbsnodes);

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Show MOM diagnostics below level 2"));
    item->setText(1, m_mainWindow->SETTINGS_CMD_Momctl_d3);

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Show job info for all jobs"));
    item->setText(1, m_mainWindow->SETTINGS_CMD_Qstat_R);

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Show detailed job info"));
    item->setText(1, m_mainWindow->SETTINGS_CMD_Qstat_f);

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("PBS Queue Manager interface"));
    item->setText(1, m_mainWindow->SETTINGS_CMD_Qmgr_c);

    QTreeWidgetItem* startMOMitem = new QTreeWidgetItem(ui->treeWidget_Commands);
    startMOMitem->setText(0, QString("Start MOM"));
    startMOMitem->setExpanded(true);

    QTreeWidgetItem* subitem = new QTreeWidgetItem(startMOMitem);
    subitem->setText(0, QString("Standard format - Head node"));
    subitem->setText(1, m_mainWindow->SETTINGS_CMD_StartMOM_HeadNode_Standard);
    subitem = new QTreeWidgetItem(startMOMitem);
    subitem->setText(0, QString("Standard format - Compute node"));
    subitem->setText(1, m_mainWindow->SETTINGS_CMD_StartMOM_ComputeNode_Standard);

    subitem = new QTreeWidgetItem(startMOMitem);
    subitem->setText(0, QString("Multimom format - Head node"));
    subitem->setText(1, m_mainWindow->SETTINGS_CMD_StartMOM_HeadNode_Multimom);
    subitem = new QTreeWidgetItem(startMOMitem);
    subitem->setText(0, QString("Multimom format - Compute node"));
    subitem->setText(1, m_mainWindow->SETTINGS_CMD_StartMOM_ComputeNode_Multimom);

    subitem = new QTreeWidgetItem(startMOMitem);
    subitem->setText(0, QString("Service format - Head node"));
    subitem->setText(1, m_mainWindow->SETTINGS_CMD_StartMOM_HeadNode_Service);
    subitem = new QTreeWidgetItem(startMOMitem);
    subitem->setText(0, QString("Service format - Compute node"));
    subitem->setText(1, m_mainWindow->SETTINGS_CMD_StartMOM_ComputeNode_Service);


    QTreeWidgetItem* stopMOMitem = new QTreeWidgetItem(ui->treeWidget_Commands);
    stopMOMitem->setText(0, QString("Stop MOM"));
    stopMOMitem->setExpanded(true);

    subitem = new QTreeWidgetItem(stopMOMitem);
    subitem->setText(0, QString("Standard & Multimom format - Head node"));
    subitem->setText(1, m_mainWindow->SETTINGS_CMD_StopMOM_HeadNode_Standard);
    subitem = new QTreeWidgetItem(stopMOMitem);
    subitem->setText(0, QString("Standard & Multimom format - Compute node"));
    subitem->setText(1, m_mainWindow->SETTINGS_CMD_StopMOM_ComputeNode_Standard);

    subitem = new QTreeWidgetItem(stopMOMitem);
    subitem->setText(0, QString("Service format - Head node"));
    subitem->setText(1, m_mainWindow->SETTINGS_CMD_StopMOM_HeadNode_Service);
    subitem = new QTreeWidgetItem(stopMOMitem);
    subitem->setText(0, QString("Service format - Compute node"));
    subitem->setText(1, m_mainWindow->SETTINGS_CMD_StopMOM_ComputeNode_Service);


    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Mark node as OFFLINE"));
    item->setText(1, m_mainWindow->SETTINGS_CMD_MarkNodeAsOFFLINE);

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Clear OFFLINE from node"));
    item->setText(1, m_mainWindow->SETTINGS_CMD_ClearOFFLINEFromNode);

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Add note"));
    item->setText(1, m_mainWindow->SETTINGS_CMD_AddNote);

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Remove note"));
    item->setText(1, m_mainWindow->SETTINGS_CMD_RemoveNote);

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Tail log file"));
    item->setText(1, m_mainWindow->SETTINGS_CMD_Tail);

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Grep a log file"));
    item->setText(1, m_mainWindow->SETTINGS_CMD_Grep);

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Cat a file"));
    item->setText(1, m_mainWindow->SETTINGS_CMD_Cat);

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Scp a file (remote to local)"));
    item->setText(1, m_mainWindow->SETTINGS_CMD_Scp_FromRemoteToLocal);

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Scp a file (local to remote)"));
    item->setText(1, m_mainWindow->SETTINGS_CMD_Scp_FromLocalToRemote);

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Copy a file"));
    item->setText(1, m_mainWindow->SETTINGS_CMD_Cp);

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Get pbs_server home dir"));
    item->setText(1, m_mainWindow->SETTINGS_CMD_GetServerHome);

    // NOT NEEDED YET:
//  item = new QTreeWidgetItem(ui->treeWidget_Commands);
//  item->setText(0, QString("Get pbs_mom home dir"));
//  item->setText(1, m_mainWindow->SETTINGS_CMD_GetMOMHome);


    // job commands:
    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Modify job"));
    item->setText(1, m_mainWindow->SETTINGS_CMD_ModifyJob);

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Delete job"));
    item->setText(1, m_mainWindow->SETTINGS_CMD_DeleteJob);

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Put job on hold"));
    item->setText(1, m_mainWindow->SETTINGS_CMD_PutJobOnHold);

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Move job to another queue"));
    item->setText(1, m_mainWindow->SETTINGS_CMD_MoveJob);

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Change order of job"));
    item->setText(1, m_mainWindow->SETTINGS_CMD_ReorderJob);

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Rerun job"));
    item->setText(1, m_mainWindow->SETTINGS_CMD_RerunJob);

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Select job"));
    item->setText(1, m_mainWindow->SETTINGS_CMD_SelectJob);

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Send signal to job"));
    item->setText(1, m_mainWindow->SETTINGS_CMD_SendSignalToJob);

}

/*******************************************************************************
 *
*******************************************************************************/
void ConfigDlg::updateCommandListWithRemote( QString remoteServer )  // with remote settings
{
    ui->treeWidget_Commands->clear();

    QTreeWidgetItem* item = NULL;
    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("List nodes and node states"));
    item->setText(1, QString("ssh -o BatchMode=yes %1 \"%2\"").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_Pbsnodes));

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Show MOM diagnostics below level 2"));
    item->setText(1, QString("ssh -o BatchMode=yes %1 \"%2").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_Momctl_d3));

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Show job info for all jobs"));
    item->setText(1, QString("ssh -o BatchMode=yes %1 \"%2\"").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_Qstat_R));

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Show detailed job info"));
    item->setText(1, QString("ssh -o BatchMode=yes %1 \"%2").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_Qstat_f));

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("PBS Queue Manager interface"));
    item->setText(1, QString("ssh -o BatchMode=yes %1 \"%2\"").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_Qmgr_c));


    QTreeWidgetItem* startMOMitem = new QTreeWidgetItem(ui->treeWidget_Commands);
    startMOMitem->setText(0, QString("Start MOM"));
    startMOMitem->setExpanded(true);

    QTreeWidgetItem* subitem = new QTreeWidgetItem(startMOMitem);
    subitem->setText(0, QString("Standard format - Head node"));
    subitem->setText(1, QString("ssh -o BatchMode=yes %1 \"%2").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_StartMOM_HeadNode_Standard));
    subitem = new QTreeWidgetItem(startMOMitem);
    subitem->setText(0, QString("Standard format - Compute node"));
    subitem->setText(1, QString("ssh -o BatchMode=yes %1 \"%2").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_StartMOM_ComputeNode_Standard));
//	QColor backgroundcolor = QColor("lightgreen");
//	item->setBackgroundColor(1, backgroundcolor);

    subitem = new QTreeWidgetItem(startMOMitem);
    subitem->setText(0, QString("Multimom format - Head node"));
    subitem->setText(1, QString("ssh -o BatchMode=yes %1 \"%2").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_StartMOM_HeadNode_Multimom));
    subitem = new QTreeWidgetItem(startMOMitem);
    subitem->setText(0, QString("Multimom format - Compute node"));
    subitem->setText(1, QString("ssh -o BatchMode=yes %1 \"%2").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_StartMOM_ComputeNode_Multimom));

    subitem = new QTreeWidgetItem(startMOMitem);
    subitem->setText(0, QString("Service format - Head node"));
    subitem->setText(1, QString("ssh -o BatchMode=yes %1 \"%2").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_StartMOM_HeadNode_Service));
    subitem = new QTreeWidgetItem(startMOMitem);
    subitem->setText(0, QString("Service format - Compute node"));
    subitem->setText(1, QString("ssh -o BatchMode=yes %1 \"%2").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_StartMOM_ComputeNode_Service));


    QTreeWidgetItem* stopMOMitem = new QTreeWidgetItem(ui->treeWidget_Commands);
    stopMOMitem->setText(0, QString("Stop MOM"));
    stopMOMitem->setExpanded(true);

    subitem = new QTreeWidgetItem(stopMOMitem);
    subitem->setText(0, QString("Standard & Multimom format - Head node"));
    subitem->setText(1, QString("ssh -o BatchMode=yes %1 \"%2").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_StopMOM_HeadNode_Standard));
    subitem = new QTreeWidgetItem(stopMOMitem);
    subitem->setText(0, QString("Standard & Multimom format - Compute node"));
    subitem->setText(1, QString("ssh -o BatchMode=yes %1 \"%2").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_StopMOM_ComputeNode_Standard));

    subitem = new QTreeWidgetItem(stopMOMitem);
    subitem->setText(0, QString("Service format - Head node"));
    subitem->setText(1, QString("ssh -o BatchMode=yes %1 \"%2").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_StopMOM_HeadNode_Service));
    subitem = new QTreeWidgetItem(stopMOMitem);
    subitem->setText(0, QString("Service format - Compute node"));
    subitem->setText(1, QString("ssh -o BatchMode=yes %1 \"%2").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_StopMOM_ComputeNode_Service));


    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Mark node as OFFLINE"));
    item->setText(1, QString("ssh -o BatchMode=yes %1 \"%2\"").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_MarkNodeAsOFFLINE));

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Clear OFFLINE from node"));
    item->setText(1, QString("ssh -o BatchMode=yes %1 \"%2\"").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_ClearOFFLINEFromNode));

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Add note"));
    item->setText(1, QString("ssh -o BatchMode=yes %1 \"%2\"").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_AddNote));

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Remove note"));
    item->setText(1, QString("ssh -o BatchMode=yes %1 \"%2\"").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_RemoveNote));

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Tail log file"));
    item->setText(1, QString("ssh -o BatchMode=yes %1 \"%2\"").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_Tail));

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Grep a log file"));
    item->setText(1, QString("ssh -o BatchMode=yes %1 \"%2\"").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_Grep));

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Cat a file"));
    item->setText(1, QString("ssh -o BatchMode=yes %1 \"%2\"").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_Cat));

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Scp a file (remote to local)"));
    item->setText(1, QString("%1 %2").arg(m_mainWindow->SETTINGS_CMD_Scp_FromRemoteToLocal).arg(m_mainWindow->m_Config_RemoteServer));

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Scp a file (local to remote)"));
    item->setText(1, QString("%1 %2").arg(m_mainWindow->SETTINGS_CMD_Scp_FromLocalToRemote).arg(m_mainWindow->m_Config_RemoteServer));

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Copy a file"));
    item->setText(1, QString("ssh -o BatchMode=yes %1 \"%2\"").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_Cp));

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Get pbs_server home dir"));
    item->setText(1, QString("ssh -o BatchMode=yes %1 \"%2\"").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_GetServerHome));

    // NOT NEEDED YET:
//  item = new QTreeWidgetItem(ui->treeWidget_Commands);
//  item->setText(0, QString("Get pbs_mom home dir"));
//  item->setText(1, QString("ssh -o BatchMode=yes %1 \"%2\"").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_GetMOMHome));


    // job commands:
    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Modify job"));
    item->setText(1, QString("ssh -o BatchMode=yes %1 \"%2\"").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_ModifyJob));

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Delete job"));
    item->setText(1, QString("ssh -o BatchMode=yes %1 \"%2\"").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_DeleteJob));

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Put job on hold"));
    item->setText(1, QString("ssh -o BatchMode=yes %1 \"%2\"").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_PutJobOnHold));

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Move job to another queue"));
    item->setText(1, QString("ssh -o BatchMode=yes %1 \"%2\"").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_MoveJob));

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Change order of job"));
    item->setText(1, QString("ssh -o BatchMode=yes %1 \"%2\"").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_ReorderJob));

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Rerun job"));
    item->setText(1, QString("ssh -o BatchMode=yes %1 \"%2\"").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_RerunJob));

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Select job"));
    item->setText(1, QString("ssh -o BatchMode=yes %1 \"%2\"").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_SelectJob));

    item = new QTreeWidgetItem(ui->treeWidget_Commands);
    item->setText(0, QString("Send signal to job"));
    item->setText(1, QString("ssh -o BatchMode=yes %1 \"%2\"").arg(remoteServer).arg(m_mainWindow->SETTINGS_CMD_SendSignalToJob));


}

/*******************************************************************************
 *
*******************************************************************************/
void ConfigDlg::on_checkBox_UsingMultiMoms_stateChanged()
{
	if (ui->checkBox_UsingMultiMoms->isChecked())
	{
		ui->checkBox_UseServiceToStartStopMOMs->setChecked(false);
		ui->checkBox_UseServiceToStartStopMOMs->setEnabled(false);
	}
	else
	{
		ui->checkBox_UseServiceToStartStopMOMs->setEnabled(true);
	}
}

/*******************************************************************************
 *
*******************************************************************************/
void ConfigDlg::on_rbtn_Remote_toggled(bool checked)
{
	if (checked)  // if "Remote" is checked
	{
		ui->comboBox_Remote_Server->setEnabled(true);
		QString remoteServer = ui->comboBox_Remote_Server->currentText();
		updateCommandListWithRemote(remoteServer);
	}
	else  // else "Local" is checked
	{
		ui->comboBox_Remote_Server->setEnabled(false);
		updateCommandListWithLocal();   // with local settings
	}
}

/*******************************************************************************
 *
*******************************************************************************/
void ConfigDlg::on_comboBox_Remote_Server_editTextChanged(const QString & text)
{
    updateCommandListWithRemote(text);
}

/*******************************************************************************
 *
*******************************************************************************/
void ConfigDlg::on_btnEditList_clicked()
{
	// lets the user edit the RemoteServer list -- add, remove, etc.
	RemoteServerComboboxEditDlg dlg( m_mainWindow->m_Config_RemoteServerList, this );
	if (!dlg.exec())
		return;

	QStringList remoteServerList = dlg.m_List;

	m_mainWindow->m_Config_RemoteServerList = remoteServerList;

	// add combobox items to ui->comboBox_Remote_Server
	ui->comboBox_Remote_Server->clear();
	ui->comboBox_Remote_Server->addItems(remoteServerList);
}

/*******************************************************************************
 *
*******************************************************************************/
// the done() function is reimplemented from QDialog. It is called when the user
// clicks OK or Cancel
void ConfigDlg::done( int result )
{
	if ( result == QDialog::Accepted )
	{
		if (ui->rbtn_Remote->isChecked())
			m_mainWindow->m_Config_DataSource = 1;
		else
			m_mainWindow->m_Config_DataSource = 0;

		m_mainWindow->m_Config_RemoteServer = ui->comboBox_Remote_Server->currentText();
		m_mainWindow->m_Config_UsingMultiMoms = ui->checkBox_UsingMultiMoms->isChecked();
		m_mainWindow->m_Config_UseServiceToStartStopMOMs = ui->checkBox_UseServiceToStartStopMOMs->isChecked();
	}
	QDialog::done( result );
}

/*******************************************************************************
 *
*******************************************************************************/
void ConfigDlg::on_treeWidget_Commands_itemActivated(QTreeWidgetItem * item, int column)
{
	Q_UNUSED(column); // prevents warning
	// (so user can copy the text if desired)
//	QMessageBox::information( this, "Item properties", QString("%1\n\n%2").arg(item->text(0)).arg(item->text(1)),
//							  QMessageBox::Ok, QMessageBox::Ok);
    ConfigItemPropertiesDlg dlg(item->text(0), item->text(1), this);
    dlg.exec();
}

/*******************************************************************************
 *
*******************************************************************************/
void ConfigDlg::on_actionShow_Properties_triggered()
{
	// called when user right-clicks on an item in the Commands list
	QList<QTreeWidgetItem*> selectedItems = ui->treeWidget_Commands->selectedItems();
	if ( selectedItems.size() > 0)
	{
		QTreeWidgetItem* item = selectedItems[0];	// list is single-select
		on_treeWidget_Commands_itemActivated(item, 0);
	}

}
