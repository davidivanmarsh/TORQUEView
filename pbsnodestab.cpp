/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: pbsnodestab.cpp.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
*/

#include "pbsnodestab.h"
#include "ui_pbsnodestab.h"
#include "mainwindow.h"
#include "addnotedlg.h"
#include "logviewerdlg.h"
#include "heatmaptab.h"
#include "percentrect.h"
#include "errormsgdlg.h"
#include <QProgressBar>
#include <QSettings>
#include <QMessageBox>
#include <QFileInfo>
#include <QTextStream>
#include <QTextEdit>
#include <QFileDialog>
#include <QDateTime>
#include <QDate>
#include <QThread>
#include <QStatusBar>
#include <QTimer>


/*******************************************************************************
 *
*******************************************************************************/
PbsNodesTab::PbsNodesTab(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PbsNodesTab)
{
	ui->setupUi(this);
	CreateContextMenus();

	m_mainWindow = dynamic_cast<MainWindow*>(parent);
	if (m_mainWindow == NULL)
	{
		return;
	}

	ui->treeWidget_Nodes->header()->resizeSection(0, 185);	// Node name
	ui->treeWidget_Nodes->header()->resizeSection(1, 105);	// Node state
	ui->treeWidget_Nodes->header()->resizeSection(2, 95);	// Job currently running
	ui->treeWidget_Nodes->header()->resizeSection(3, 95);	// Available threads (NUMA)
	ui->treeWidget_Nodes->header()->resizeSection(4, 95);	// Total threads (NUMA)
//	ui->treeWidget_Nodes->header()->resizeSection(5, 95);	// Percent Available threads / Total threads

	ui->treeWidget_NodeInfo->header()->resizeSection(0, 195);	// Label
//	ui->treeWidget_NodeInfo->header()->resizeSection(1, 125);	// Value

	ui->treeWidget_MomCtl->header()->resizeSection(0, 215);	// Label
//	ui->treeWidget_MomCtl->header()->resizeSection(1, 125);	// Value

	connect(ui->spinBox_JobID, SIGNAL(valueChanged(int)),
		this, SLOT(on_spinbox_JobID_valueChanged(int)));
//	// set column widths
//	ui->tableWidget_Nodes->horizontalHeader()->resizeSection(0, 85);
//	ui->tableWidget_Nodes->horizontalHeader()->resizeSection(1, 85);
//	ui->tableWidget_Nodes->horizontalHeader()->resizeSection(2, 85);
//	ui->tableWidget_Nodes->horizontalHeader()->resizeSection(3, 85);
//	ui->tableWidget_Nodes->horizontalHeader()->resizeSection(4, 85);
//	ui->tableWidget_Nodes->horizontalHeader()->resizeSection(5, 85);


//	// set row heights
//	ui->tableWidget_Nodes->verticalHeader()->setUpdatesEnabled(false);
//	int rowHeight = ui->tableWidget->rowHeight(0);  // returns 30
//	int rowHeight = 24;
//	int numRows = ui->tableWidget_Nodes->rowCount();
//	for (unsigned int i = 0; i < numRows; i++)
//		ui->tableWidget_Nodes->verticalHeader()->resizeSection(i, rowHeight);
//	ui->tableWidget_Nodes->verticalHeader()->setUpdatesEnabled(true);

    // set default sizes for main splitter panes
    ui->splitter_2->setStretchFactor(0,3);
    ui->splitter_2->setStretchFactor(1,2);

	m_lastNodeSelected = NULL;

    resetErrorMsgDlg();
}

/*******************************************************************************
 *
*******************************************************************************/
PbsNodesTab::~PbsNodesTab()
{
	delete ui;
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::CreateContextMenus()
{
	QAction *separator1 = new QAction(this);
	separator1->setSeparator(true);
	QAction *separator2 = new QAction(this);
	separator2->setSeparator(true);
	QAction *separator3 = new QAction(this);
	separator3->setSeparator(true);

	ui->treeWidget_Nodes->addAction(ui->actionStart_MOM);
	ui->treeWidget_Nodes->addAction(ui->actionStart_MOM_Head_node);
	ui->treeWidget_Nodes->addAction(ui->actionStop_MOM);
	ui->treeWidget_Nodes->addAction(ui->actionStop_MOM_Head_node);
//	ui->treeWidget_Nodes->addAction(ui->actionMomctl_q);
	ui->treeWidget_Nodes->addAction(separator1);
	ui->treeWidget_Nodes->addAction(ui->actionMark_node_as_OFFLINE);
	ui->treeWidget_Nodes->addAction(ui->actionClear_OFFLINE_Node);
	ui->treeWidget_Nodes->addAction(ui->actionAdd_Note);
	ui->treeWidget_Nodes->addAction(ui->actionRemove_Note);
	ui->treeWidget_Nodes->addAction(separator2);
	ui->treeWidget_Nodes->addAction(ui->actionLog_Viewer);
	ui->treeWidget_Nodes->addAction(separator3);
	ui->treeWidget_Nodes->addAction(ui->actionHighlight_all_occurances_of_JobID);
	ui->treeWidget_Nodes->setContextMenuPolicy(Qt::ActionsContextMenu);

	ui->treeWidget_MomCtl->addAction(ui->actionShow_Nodes_Job_Is_Running_On);
	ui->treeWidget_MomCtl->addAction(ui->actionClear_Nodes_Job_Is_Running_On);
	ui->treeWidget_MomCtl->setContextMenuPolicy(Qt::ActionsContextMenu);
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::resetErrorMsgDlg()
{
    m_showPbsnodesSTDERROutput = true;
    m_showMomctlSTDERROutput = true;
    m_showStartMomSTDERROutput = true;
    m_showStopMomSTDERROutput = true;
    m_showMarkNodeAsOfflineSTDERROutput = true;
    m_showClearOfflineNodeSTDERROutput = true;
    m_showAddNoteSTDERROutput = true;
    m_showRemoveNoteSTDERROutput = true;
    m_showGetServerHomeSTDERROutput = true;
}

/*******************************************************************************
 *
*******************************************************************************/
PbsNode* PbsNodesTab::getLastNodeSelected()
{
    return m_lastNodeSelected;
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::clearLists()
{
	// re-init all GUI widgets
	ui->treeWidget_Nodes->clear();	// clear out list
	ui->treeWidget_NodeInfo->clear();  // clear out tree -- Ken: should we really do this here?  Or would the user want to keep on seeing the old values until he clicks on another item in nodes tree?
	ui->treeWidget_MomCtl->clear();
}

/*******************************************************************************
 *
*******************************************************************************/
bool PbsNodesTab::issueCmd_Pbsnodes()  // init app from "pbsnodes" command call
{
	m_pbsNode_Stdout.clear();
	m_pbsNode_Stderr.clear();
	m_pbsNodes.clear();

//	QString pbsNodesCommand = "pbsnodes";
	QString pbsNodesCommand = m_mainWindow->m_Config_Cmd_Pbsnodes;

	// show "Wait cursor"
	QApplication::setOverrideCursor(Qt::WaitCursor);

	m_pbsNodesProcess = new QProcess(this);
	connect (m_pbsNodesProcess, SIGNAL(readyReadStandardOutput()),
			 this, SLOT(pbsNodes_getStdout())); // connect process signals with your code
	connect (m_pbsNodesProcess, SIGNAL(readyReadStandardError()),
			 this, SLOT(pbsNodes_getStderr())); // same here
	m_pbsNodesProcess->start(pbsNodesCommand);

	if (!m_pbsNodesProcess->waitForStarted())
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Couldn't start 'pbsnodes' process", "'pbsnodes' process unable to start.  Torque is probably not running.\n");
		return false;
	}

	if (!m_pbsNodesProcess->waitForFinished(-1)) //If msecs is -1, this function will not time out
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Error trying to complete pbsnodes process", "Process's waitForFinished() method returned error.\n");
		return false;
	}

	delete m_pbsNodesProcess;
	m_pbsNodesProcess = NULL;


	pbsNodes_processStdout(); // parse the stdout data collected (above)

	// restore the original cursor
	QApplication::restoreOverrideCursor();


	if (m_showPbsnodesSTDERROutput)
	{
		// see if there's any errors to display
		if (!m_pbsNode_Stderr.isEmpty())
		{
//			QMessageBox::critical(0, "Error issuing pbsnodes command",
//								  QString("Error issuing 'pbsnodes' command.  Error message was: %1").arg(m_pbsNode_Stderr));
			ErrorMsgDlg dlg("TORQUEView Error", "Error issuing pbsnodes command. Error message:", m_pbsNode_Stderr);
			dlg.exec();
			if (dlg.isDontShowThisAgainChecked())  // see if the "Don't show this dialog again" checkbox was checked
				m_showPbsnodesSTDERROutput = false;
			return false;
		}
	}
	return true; // success

}

/*******************************************************************************
 *
*******************************************************************************/
// called from pbsnodesFromCmd() - this gets called whenever the pbsnodes process has something to say...
void PbsNodesTab::pbsNodes_getStdout()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	QByteArray s = m_pbsNodesProcess->readAllStandardOutput(); // read normal output
	m_pbsNode_Stdout.append( s );  // if there's any stdout
}

/*******************************************************************************
 *
*******************************************************************************/
// called from pbsnodesFromCmd() - this gets called whenever the pbsnodes process has something to say...
void PbsNodesTab::pbsNodes_getStderr()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	QByteArray s = m_pbsNodesProcess->readAllStandardError(); // read error channel
	m_pbsNode_Stderr.append( s );  // if there's any stderr
}


/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::pbsNodes_processStdout() // parse the stdout data collected (above)
{
	m_pbsNodes.clear();

	PbsNode* node = NULL;


	QStringList lines = m_pbsNode_Stdout.split("\n");
	foreach (QString line, lines)
	{
		if (line.isEmpty())
			continue;

		if (!line.startsWith("   "))	// if line starts with a nodeName (instead of blanks)..
		{
			// we're starting a new node
			if (node != NULL) // if there's data waiting to be added to qlist
			{
				m_pbsNodes.append(node);
				node = NULL;	// re-init
			}
			QString nodeName = line.trimmed();	// strip off white space at beginning and end
			node = new PbsNode( nodeName );
		}
		else
		{
			QString sLine = line.trimmed();	// strip off white space at beginning and end
			int length = sLine.length();

			int index = 0;
			index = sLine.indexOf("=",0);  // look for first "=" in line
			if(index == -1)	// if NOT found, punch out
				continue;
			QString sLabel = sLine.left(index-1);
			QString sValue = sLine.right(length - index - 2);
			sLabel = sLabel.trimmed();
			sValue = sValue.trimmed();

			NodeState nodeState = pbs_state_unknown;

			// special case 1:  if it's the 'state', set the nodeState
			if (sLabel.compare("state") == 0)	// if this is the "state" field
			{
				if (sValue.compare("free") == 0)
					nodeState = pbs_free;
				else if (sValue.compare("busy") == 0)
					nodeState = pbs_busy;
				else if (sValue.compare("reserve") == 0)
					nodeState = pbs_reserve;
				else if (sValue.compare("running") == 0)
					nodeState = pbs_running;
				else if (sValue.compare("down") == 0)
					nodeState = pbs_down;
				else if (sValue.compare("offline") == 0)
					nodeState = pbs_offline;
				else if (sValue.compare("job-exclusive") == 0)
					nodeState = pbs_job_exclusive;
				else if (sValue.compare("job-sharing") == 0)
					nodeState = pbs_job_sharing;
				else // else check for multiple states
				{
					QStringList slStates = sValue.split(",");
					if (slStates.count() > 1)	// could be multiple states (e.g, "down,offline")
						nodeState = pbs_multiple;
				}

				node->m_nodeState = nodeState;
				node->m_sNodeState = sValue;	// save off string representation of node state (could be multiple states)

			}
			// special case 2:  if it's 'status'
			else if (sLabel.compare("status") == 0)	// if this is the "status" field
			{
				// look for "message=ERROR" in the status line -- if found, set bErrorStatus to true (will display node icon in red)
				bool bErrorStatus = false;
				QStringList sl = sValue.split(",");
				foreach (QString sField, sl)
				{
					QStringList parts = sField.split("=");
					if (parts.count() > 1)
					{
						if (parts[0] == "message" && parts[1].startsWith("ERROR"))
						{
							bErrorStatus = true;
							break;
						}
					}
				}
				if (bErrorStatus)
					node->m_bErrorStatus = true;
				else
					node->m_bErrorStatus = false;

			}
			// special case 3:  if it's 'properties'
			else if (sLabel.compare("properties") == 0)	// if this is the "properties" field
			{
				node->m_properties = sValue.split(","); // split on commas
			}
			// special case 4:  if it's 'np', save it
			else if (sLabel.compare("np") == 0)	// if this is the "np" (number of procs) field
			{
				node->m_np = sValue.toInt();
			}
			// special case 5:  if it's 'jobs', save it in a jobs list
			else if (sLabel.compare("jobs") == 0)	// if this is the "jobs" field
			{
				node->m_jobs = sValue.split(","); // split on commas
			}
			// special case 6:  if it's 'mom_service_port', save it in the node->m_momServicePort
			else if (sLabel.compare("mom_service_port") == 0)	// if this is the "mom_service_port " field
			{
				node->m_momServicePort = sValue;	// get the mom_service_port
			}
			// special case 7:  if it's 'mom_manager_port', save it in the node->m_momManagerPort
			else if (sLabel.compare("mom_manager_port") == 0)	// if this is the "mom_manager_port " field
			{
				node->m_momManagerPort = sValue;	// get the mom_manager_port
			}
			// special case 8:  if it's 'available_threads', save it in the node->m_availableThreads
			else if (sLabel.compare("available_threads") == 0)	// if this is the "available_threads " field (NUMA)
			{
				node->m_availableThreads = sValue.toInt();
			}
			// special case 9:  if it's 'total_threads', save it in the node->m_totalThreads
			else if (sLabel.compare("total_threads") == 0)	// if this is the "total_threads " field (NUMA)
			{
				node->m_totalThreads = sValue.toInt();
			}
			node->m_data.append(line); // append the original line (not-stripped) to the data QStringList
		} // if line DOESN'T start with a nodeName

	}  // foreach line


	// add last node to list
	if (node != NULL) // if there's data waiting to be added to qlist
	{
		m_pbsNodes.append(node);
		node = NULL;	// re-init
	}

	ui->treeWidget_MomCtl->addAction(ui->actionShow_Nodes_Job_Is_Running_On);
	ui->treeWidget_MomCtl->addAction(ui->actionClear_Nodes_Job_Is_Running_On);
	ui->treeWidget_MomCtl->setContextMenuPolicy(Qt::ActionsContextMenu);

	updateLists();
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::on_spinbox_JobID_valueChanged(int i)
{
    Q_UNUSED(i); // to prevent warning

    // if user changed spinbox value, uncheck "checkBox_ShowNodesRunningJobID"
    // REASON: we don't want to update Node list in the middle of user changing JobID spinbox value.
    // Only change it when he's through changing JobID value and THEN has checked the spinbox_JobID checkbox.
    ui->checkBox_ShowNodesRunningJobID->setChecked(false);
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::on_checkBox_ShowNodesRunningJobID_stateChanged()
{
//  if (ui->checkBox_ShowNodesRunningJobID->isChecked())
//  {
        // re-init Nodes list
        ui->treeWidget_Nodes->clear();	// clear out list
        updateLists();  // update Node list, but only show nodes where job is running "jobID" on one or more of its cores
//  }
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::on_actionHighlight_all_occurances_of_JobID_triggered()
{
	resetErrorMsgDlg();  // clear all "don't show this message again" flags

	// called when user clicks on an item in the Nodes tree
	QList<QTreeWidgetItem*> selectedItems = ui->treeWidget_Nodes->selectedItems();
	if ( selectedItems.size() > 0)
	{
		QTreeWidgetItem* item = selectedItems[0];	// list is single-select
		int column = ui->treeWidget_Nodes->currentColumn();
		on_treeWidget_Nodes_itemDoubleClicked(item, column);
	}
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::on_treeWidget_Nodes_itemDoubleClicked(QTreeWidgetItem* item, int column)
{
	// get the jobID from the item in the row the user double-clicked on, and
	// highlight all occurances of that jobID (in all nodes) in yellow background

	if (ui->radioButton_ShowProperties->isChecked()) // don't try to highlight jobID's if user is looking at Properties, not Cores!
		return;

	QString jobIDText;
	if (column > 1)	// disregard Node or State columns
	{
		jobIDText = item->text(column);
		ui->spinBox_JobID->setValue(jobIDText.toInt());  // display value in the JobID spinbox at the bottom of the screen
	}

	// iterate through treewidget list, getting all top-level items
	int rootItemCount = ui->treeWidget_Nodes->topLevelItemCount();
	for (int i=0; i<rootItemCount; i++)
	{
		QTreeWidgetItem* rootItem = ui->treeWidget_Nodes->topLevelItem(i);
		int colCount = rootItem->columnCount();
		for (int jj=0; jj<colCount; jj++)  // go through all the columns, looking for matches to jobIDText
		{
			if (jj > 1) // don't change background color for Node or State columns
			{
				QString itemText = rootItem->text(jj);
				if (itemText.compare(jobIDText) == 0)
				{
					rootItem->setBackgroundColor(jj,  QColor("yellow"));
				}
				else
				{
					rootItem->setBackgroundColor(jj,  QColor("white"));  // else reset background color to white
				}
			}
		}
	}
//	item->setSelected(false);  // turn off selection so user can see the yellow highlight
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::on_actionNo_op_triggered()
{
   // this is a no-op
}

/*******************************************************************************
 *
*******************************************************************************/
//QStringList PbsNodesTab::getExecHost()  // get the "exec_host" list
//{
//	QStringList slExecHosts;

//	if (! m_lastExecHosts.isEmpty())
//	{
//		QStringList slHostNames = m_lastExecHosts.split("+"); // split on "+" separator chars
//		foreach (QString s, slHostNames)
//		{
//			int index = s.indexOf("/");
//			if (index != -1)
//			{
//				int len = s.size();
//				s.remove(index, len);  // truncate rest of string after the "/" char ('len' will probably be even longer that needed, but that's fine)
//				if (! slExecHosts.contains(s))
//					slExecHosts.append(s);
//			}
//		}

//	}
//	return slExecHosts;
//}

/*******************************************************************************
 *  if m_execHost isn't empty, highlight those hosts (showing hosts specified job is running on)
*******************************************************************************/
void PbsNodesTab::updateLists()
{
	//	QStringList slExecHosts = getExecHost(); // get the list of hosts the specified job is running on

	if (ui->radioButton_ShowCores->isChecked())  // if user checked the "Show cores" radiobutton..
	{
		// first, determine the maxCores value by looking through all the nodes
		// (this will be the number of new columns we need to add for procs (i.e, cores)
		int nMaxCores = 0;
		//	QList<int> allCores;
		//	QList<bool> allCoresRunningJobs;

		for (int j=0; j < m_pbsNodes.size(); j++)
		{
			int nCores = m_pbsNodes[j]->m_np;
			if (nCores > nMaxCores)
				nMaxCores = nCores;
		}

		// now add the new colummns (for the cores) -- (have to add all the columns again)
		int nTotalColumns = 2 + nMaxCores;  // the first 2 are for:  Nodes and State columns
		ui->treeWidget_Nodes->setColumnCount( nTotalColumns );
		QStringList slHeaderLabels;
		slHeaderLabels.append("Nodes");
		slHeaderLabels.append("State");
		for (int k = 0; k < nMaxCores; ++k)
		{
			// make field width 2, with 0 as the fill character
			const QChar fillchar('0');
			int fillwidth = 2;
			int base = 10;
			QString sNum = QString("%1").arg(k, fillwidth, base, fillchar);
			slHeaderLabels.append(sNum);  // fieldwidth=2, fillChar="0"
			//		allCores.append(k);
			//		allCoresRunningJobs.append(false);  // init each one to false
		}
		ui->treeWidget_Nodes->setHeaderLabels(slHeaderLabels);


		// now go through the list of nodes and add each one to the tableWidget
		for (int i=0; i < m_pbsNodes.size(); i++)
		{
			QList<int> coreList;	// list of cores running jobs on this node
			QList<QString> jobIDList;	// list of the JobIDs that are running on this node
			QString targetJobID = QString("%1").arg(ui->spinBox_JobID->value());  // convert to string

			bool bJobsPresent = true;
			if (m_pbsNodes[i]->m_jobs.isEmpty())
				bJobsPresent = false;

			// first, get a list of jobs running on each proc (core) on each node -- see if we really do need to add each one
			if (bJobsPresent) // any jobs running on this node?
			{
				foreach (QString job, m_pbsNodes[i]->m_jobs)
				{
					// For TORQUE 4.2, jobs will be in the format:
					//	"0/2844.c04a.ac"  -- the first value (0 here) is which core is running the job
					//  "1/2844.c04a.ac"  -- the first value (1 here) is which core is running the job
					//	"2/2844.c04a.ac"  -- the first value (2 here) is which core is running the job
					// For TORQUE 5.x, jobs will be in the format:
					//	"0-7/2844.c04a.ac"  -- the first values (0-7 here) is the range for cores running the job

					QStringList parts = job.split("/");
					int core = parts[0].toInt();
					QStringList coreRange = parts[0].split("-");  // split on a "-" (dash)
					QStringList sl = parts[1].split(".");
					QString sJobID = sl[0];
					if (coreRange.count() > 1)
					{
						int firstCore = coreRange[0].toInt();
						int lastCore  = coreRange[1].toInt();
						for (int k=firstCore; k<=lastCore; k++)
						{
						//	item->setText(core + 2 + k, sJobID); // show job currently running on this core (on this node)
							coreList.append(k);
							jobIDList.append(sJobID);
						}
					}
					else
					{
					//	item->setText(core + 2, sJobID); // show job currently running on this core (on this node)
						coreList.append(core);
						jobIDList.append(sJobID);
					}

				}
			}

			// Does this node needs to be added? If we're only showing nodes runnning the target jobID and if
			// this JobID is one of the jobs running on this node, add the node; otherwise, don't add it.
			// (The default case is to add all nodes.)
			if (ui->checkBox_ShowNodesRunningJobID->isChecked())
			{
				if (! jobIDList.contains(targetJobID))
					continue;  // don't add it; just go on to next node
			}

			// insert data into tree
			QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget_Nodes);
			item->setText(0, m_pbsNodes[i]->m_nodeName);
			// attach pbsnode index to the item
			item->setData(0, Qt::UserRole, QVariant::fromValue(i));

			QString sNodeState;
			QIcon icon;
			QColor backgroundcolor;


			// get node state
			getNodeInfo(m_pbsNodes[i]->m_nodeState, m_pbsNodes[i]->m_sNodeState, sNodeState, bJobsPresent,
						m_pbsNodes[i]->m_bErrorStatus, icon, backgroundcolor);
			item->setIcon(0, icon);
			item->setBackgroundColor(1, backgroundcolor);
			item->setText(1, sNodeState);

			// show the jobs running on each proc (core) on each node
			if (bJobsPresent) // any jobs running on this node?
			{
				int coreListSize = coreList.size();
				for (int jj=0; jj<coreListSize; jj++)
				{
					int core = coreList[jj];
					QString sJobID = jobIDList[jj];
					item->setText(core + 2, sJobID); // show job currently running on this core (on this node) -- add 2 to move past Node and State columns

					if (targetJobID.compare(sJobID) == 0)
					{
						item->setBackgroundColor(core + 2,  QColor("yellow"));
					}
					else
					{
						item->setBackgroundColor(core + 2,  QColor("white"));  // else reset background color to white
					}
				}

//				foreach (QString job, m_pbsNodes[i]->m_jobs)
//				{
//					// For TORQUE 4.2, jobs will be in the format:
//					//	"0/2844.c04a.ac"  -- the first value (0 here) is which core is running the job
//					//  "1/2844.c04a.ac"  -- the first value (1 here) is which core is running the job
//					//	"2/2844.c04a.ac"  -- the first value (2 here) is which core is running the job
//					// For TORQUE 5.x, jobs will be in the format:
//					//	"0-7/2844.c04a.ac"  -- the first values (0-7 here) is the range for cores running the job

//					QStringList parts = job.split("/");
//					int core = parts[0].toInt();
//					QStringList coreRange = parts[0].split("-");  // split on a "-" (dash)
//					QStringList sl = parts[1].split(".");
//					QString sJobID = sl[0];
//					if (coreRange.count() > 1)
//					{
//						int firstCore = coreRange[0].toInt();
//						int lastCore  = coreRange[1].toInt();
//						for (int k=firstCore; k<=lastCore; k++)
//						{
//							item->setText(core + 2 + k, sJobID); // show job currently running on this core (on this node)
//						}
//					}
//					else
//					{
//						item->setText(core + 2, sJobID); // show job currently running on this core (on this node)
//					}

//					//	item->setBackgroundColor(2,  QColor("lightyellow"));
//				}
				// tooltip for column 0 (Node)
				item->setToolTip(0, QString("Node: %1\nState: %2\n\nJobs Running:\n%3")
								 .arg(m_pbsNodes[i]->m_nodeName)
								 .arg(sNodeState)
								 .arg(m_pbsNodes[i]->m_jobs.join("\n")));
				// tooltip for column 1 (State)
				item->setToolTip(1, QString("Node: %1\nState: %2\n\nJobs Running:\n%3")
								 .arg(m_pbsNodes[i]->m_nodeName)
								 .arg(sNodeState)
								 .arg(m_pbsNodes[i]->m_jobs.join("\n")));
			}
			else
			{
				// tooltip for column 0 (Node)
				item->setToolTip(0, QString("Node: %1\nState: %2")
								 .arg(m_pbsNodes[i]->m_nodeName)
								 .arg(sNodeState));
				// tooltip for column 1 (State)
				item->setToolTip(1, QString("Node: %1\nState: %2")
								 .arg(m_pbsNodes[i]->m_nodeName)
								 .arg(sNodeState));
			}
			//		if (slExecHosts.contains(m_pbsNodes[i]->m_nodeName)) // is this one of the nodes job is running on?
			//		{
			//			item->setText(2,  QString("%1").arg(m_lastJobId)); // show job currently running
			//			item->setBackgroundColor(2,  QColor("yellow"));
			//		}
			//		if (m_pbsNodes[i]->m_availableThreads != -1) // is this a NUMA node?
			//		{
			//			item->setText(3,  QString("%1").arg(m_pbsNodes[i]->m_availableThreads));
			//			item->setText(4,  QString("%1").arg(m_pbsNodes[i]->m_totalThreads));

			//			QRect rect = ui->treeWidget_Nodes->visualItemRect ( item );
			//			int rowHeight = rect.height() - 1;
			//			float percent = 0.0f;
			//			if (m_pbsNodes[i]->m_totalThreads > 0)
			//				percent = (m_pbsNodes[i]->m_availableThreads * 100.0) / m_pbsNodes[i]->m_totalThreads;
			//			PercentRect* pr = new PercentRect(/*25.0f */ percent, rowHeight, 100);
			//			ui->treeWidget_Nodes->setItemWidget(item, 5, pr);
			//		}
		}  // end of "for (int i=0; i < m_pbsNodes.size(); i++)"

		ui->treeWidget_Nodes->header()->resizeSection(0, 185);	// Node name
		ui->treeWidget_Nodes->header()->resizeSection(1, 105);	// Node state
		//	ui->treeWidget_Nodes->header()->resizeSection(2, 95);	// Job currently running
		//	ui->treeWidget_Nodes->header()->resizeSection(3, 95);	// Available threads (NUMA)
		//	ui->treeWidget_Nodes->header()->resizeSection(4, 95);	// Total threads (NUMA)
		for (int k = 0; k < nMaxCores; k++)
		{
			ui->treeWidget_Nodes->header()->resizeSection(k+2, 60);	// core
		}

	} // end of "if (ui->radioButton_ShowCores->isChecked())"
// BEGIN---------------------

//	else if (ui->radioButton_ShowNUMA->isChecked())  // if user checked the "Show NUMA" radiobutton..
//	{
//		// create all the column headers
//		ui->treeWidget_Nodes->setColumnCount( 5 );
//		QStringList slHeaderLabels;
//		slHeaderLabels.append("Nodes");
//		slHeaderLabels.append("State");
//		slHeaderLabels.append("Available threads (NUMA)");
//		slHeaderLabels.append("Total threads (NUMA)");
//		slHeaderLabels.append("Percent Available threads");
//		ui->treeWidget_Nodes->setHeaderLabels(slHeaderLabels);

//		// now go through the list of nodes and add each one to the tableWidget
//		for (int i=0; i < m_pbsNodes.size(); i++)
//		{
//			// insert data into tree
//			QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget_Nodes);
//			item->setText(0, m_pbsNodes[i]->m_nodeName);
//			// attach pbsnode index to the item
//			item->setData(0, Qt::UserRole, QVariant::fromValue(i));

//			QString sNodeState;
//			QIcon icon;
//			QColor backgroundcolor;

//			bool bJobsPresent = true;
//			if (m_pbsNodes[i]->m_jobs.isEmpty())
//				bJobsPresent = false;

//			// get node state
//			getNodeInfo(m_pbsNodes[i]->m_nodeState, m_pbsNodes[i]->m_sNodeState, sNodeState, bJobsPresent,
//						m_pbsNodes[i]->m_bErrorStatus, icon, backgroundcolor);
//			item->setIcon(0, icon);
//			item->setBackgroundColor(1, backgroundcolor);
//			item->setText(1, sNodeState);


////			// show the properties for each node
////			foreach (QString sProperty, m_pbsNodes[i]->m_properties)
////			{
////				// get the list index in slAllProperties --
////				// we use it to determine which column this property goes in
////				int index = slAllProperties.indexOf(sProperty);
////				// now add it to the correct location (column) in the row
////				item->setText(2 + index, sProperty); // just use the property name for the column's value
////			}

//			// show tooltips:
//			// tooltip for column 0 (Node)
//			item->setToolTip(0, QString("Node: %1\nState: %2\n\nJobs Running:\n%3")
//							 .arg(m_pbsNodes[i]->m_nodeName)
//							 .arg(sNodeState)
//							 .arg(m_pbsNodes[i]->m_jobs.join("\n")));
//			// tooltip for column 1 (State)
//			item->setToolTip(1, QString("Node: %1\nState: %2\n\nJobs Running:\n%3")
//							 .arg(m_pbsNodes[i]->m_nodeName)
//							 .arg(sNodeState)
//							 .arg(m_pbsNodes[i]->m_jobs.join("\n")));

//			if (m_pbsNodes[i]->m_availableThreads != -1) // is this a NUMA node?
//			{
//				item->setText(2,  QString("%1").arg(m_pbsNodes[i]->m_availableThreads));
//				item->setText(3,  QString("%1").arg(m_pbsNodes[i]->m_totalThreads));

////				QRect rect = ui->treeWidget_Nodes->visualItemRect ( item );
////				int rowHeight = rect.height() - 1;
////				float percent = 0.0f;
////				if (m_pbsNodes[i]->m_totalThreads > 0)
////					percent = (m_pbsNodes[i]->m_availableThreads * 100.0) / m_pbsNodes[i]->m_totalThreads;
////				PercentRect* pr = new PercentRect(/*25.0f */ percent, rowHeight, 100);
////				ui->treeWidget_Nodes->setItemWidget(item, 4, pr);
//			}

//		}
//		ui->treeWidget_Nodes->header()->resizeSection(0, 185);	// Node name
//		ui->treeWidget_Nodes->header()->resizeSection(1, 105);	// Node state
//		ui->treeWidget_Nodes->header()->resizeSection(2, 145);	//
//		ui->treeWidget_Nodes->header()->resizeSection(3, 145);	//
//		ui->treeWidget_Nodes->header()->resizeSection(4, 200);	//
//	}



// END-----------------------
	else  // else user checked the "Show properties (features)" radiobutton..
	{
		// parse through list of properties for each node, and build a QStringList of all
		// properties for all nodes (removing duplicates)
		QStringList slAllProperties;
		for (int j=0; j < m_pbsNodes.size(); j++)
		{
			foreach (QString property, m_pbsNodes[j]->m_properties)
			{
				if (!slAllProperties.contains(property))
					slAllProperties.append(property);
			}
		}

		// now add the new colummns (for the properties) -- (have to add all the columns again)
		int nTotalProperties = slAllProperties.count();
		int nTotalColumns = 2 + nTotalProperties; // the first 2 are for:  Nodes and State columns
		ui->treeWidget_Nodes->setColumnCount( nTotalColumns );
		QStringList slHeaderLabels;
		slHeaderLabels.append("Nodes");
		slHeaderLabels.append("State");
		// now create all the column headers for the properties
		foreach (QString prop, slAllProperties)
		{
			slHeaderLabels.append(prop);
		}
		ui->treeWidget_Nodes->setHeaderLabels(slHeaderLabels);

		// now go through the list of nodes and add each one to the tableWidget
		for (int i=0; i < m_pbsNodes.size(); i++)
		{
			// insert data into tree
			QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget_Nodes);
			item->setText(0, m_pbsNodes[i]->m_nodeName);
			// attach pbsnode index to the item
			item->setData(0, Qt::UserRole, QVariant::fromValue(i));

			QString sNodeState;
			QIcon icon;
			QColor backgroundcolor;

			bool bJobsPresent = true;
			if (m_pbsNodes[i]->m_jobs.isEmpty())
				bJobsPresent = false;

			// get node state
			getNodeInfo(m_pbsNodes[i]->m_nodeState, m_pbsNodes[i]->m_sNodeState, sNodeState, bJobsPresent,
						m_pbsNodes[i]->m_bErrorStatus, icon, backgroundcolor);
			item->setIcon(0, icon);
			item->setBackgroundColor(1, backgroundcolor);
			item->setText(1, sNodeState);


			// show the properties for each node
			foreach (QString sProperty, m_pbsNodes[i]->m_properties)
			{
				// get the list index in slAllProperties --
				// we use it to determine which column this property goes in
				int index = slAllProperties.indexOf(sProperty);
				// now add it to the correct location (column) in the row
				item->setText(2 + index, sProperty); // just use the property name for the column's value
			}

			// show tooltips:
			// tooltip for column 0 (Node)
			item->setToolTip(0, QString("Node: %1\nState: %2\n\nJobs Running:\n%3")
							 .arg(m_pbsNodes[i]->m_nodeName)
							 .arg(sNodeState)
							 .arg(m_pbsNodes[i]->m_jobs.join("\n")));
			// tooltip for column 1 (State)
			item->setToolTip(1, QString("Node: %1\nState: %2\n\nJobs Running:\n%3")
							 .arg(m_pbsNodes[i]->m_nodeName)
							 .arg(sNodeState)
							 .arg(m_pbsNodes[i]->m_jobs.join("\n")));

		}
		ui->treeWidget_Nodes->header()->resizeSection(0, 185);	// Node name
		ui->treeWidget_Nodes->header()->resizeSection(1, 105);	// Node state
		for (int k = 0; k < nTotalProperties; k++)
		{
			ui->treeWidget_Nodes->header()->resizeSection(k+2, 60);	// core
		}
	}

	//	QFontMetrics fm = ui->tableWidget_Nodes->fontMetrics();
	//	int width = fm.width("WW");  // base width on table's font metrics
	//	ui->tableWidget_Nodes->setVisible(false);

	if (ui->checkBox_ResizeColumnsToContents->isChecked())
	{
		ui->treeWidget_Nodes->hide();  // for better performance

		// resize columns to contents (i.e, compress column widths down to size of column contents)
		int columnCount = ui->treeWidget_Nodes->header()->count();
		for(int i = 0; i < columnCount; i++)
		{
			ui->treeWidget_Nodes->resizeColumnToContents(i);
		}
		ui->treeWidget_Nodes->show();  // for better performance
	}

	// (this code works but is not needed:)
	//	for (int j =0; j < ui->tableWidget_Nodes->columnCount(); j++)
	//		ui->tableWidget_Nodes->horizontalHeader()->resizeSection(j, width);

	//	ui->tableWidget_Nodes->setVisible(true);
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::on_checkBox_ResizeColumnsToContents_stateChanged()
{
	// re-init Nodes list
	ui->treeWidget_Nodes->clear();	// clear out list
	updateLists();
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::on_radioButton_ShowCores_toggled()
{
	// re-init Nodes list
	ui->treeWidget_Nodes->clear();	// clear out list
	updateLists();
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::on_checkBox_ShowNodeDetailsLists_stateChanged()
{
	if (ui->checkBox_ShowNodeDetailsLists->isChecked())
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
void PbsNodesTab::on_checkBox_OnlyShowCoresRunningJobs_stateChanged()
{
	// re-init Nodes list
	ui->treeWidget_Nodes->clear();	// clear out list
	updateLists();
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::initPbsnodesFromFile(QTextStream &in)
{
	// re-init all GUI widgets
	ui->treeWidget_Nodes->clear();	// clear out list
	ui->treeWidget_NodeInfo->clear();  // clear out tree -- Ken: should we really do this here?  Or would the user want to keep on seeing the old values until he clicks on another item in nodes tree?

	m_pbsNodes.clear();
	PbsNode* node = NULL;

	while (!in.atEnd())
	{
		QString line = in.readLine();
		if (line.isEmpty())
			continue;

		// have we gotten to the next section after the pbsnodes section?
		if (line.startsWith( "#--------" )) // each section will be terminated by a "#-------" string to indicate the start of the next section
		{
			// if there's one last node to be added, add it
			if (node != NULL)
			{
				m_pbsNodes.append(node);
				node = NULL;	// re-init
			}
			break;
		}


		// parse pbsnodes data ----------------------

		// skip header lines
		if (line.startsWith("#---------") ||
				line.startsWith("TORQUEView") ||
				line.startsWith("TORQUEView Version:") ||
				line.startsWith("Date:") ||
				line.startsWith("Comments:"))
			continue;

		if (!line.startsWith("   "))	// if line starts with a nodeName (instead of 4 blanks)..
		{
			// we're starting a new node
			if (node != NULL) // if there's data waiting to be added to qlist
			{
				m_pbsNodes.append(node);
				node = NULL;	// re-init
			}
			QString nodeName = line.trimmed();	// strip off white space at beginning and end
			node = new PbsNode( nodeName );
		}
		else
		{
            QString sLine = line.trimmed();	// strip off white space at beginning and end
            int length = sLine.length();

            int index = 0;
            index = sLine.indexOf("=",0);  // look for first "=" in line
            if(index == -1)	// if NOT found, punch out
                continue;
            QString sLabel = sLine.left(index-1);
            QString sValue = sLine.right(length - index - 2);
            sLabel = sLabel.trimmed();
            sValue = sValue.trimmed();

            NodeState nodeState = pbs_state_unknown;

            // special case 1:  if it's 'state', process it
            if (sLabel.compare("state") == 0)	// if this is the "state" field
            {
                if (sValue.compare("free") == 0)
                    nodeState = pbs_free;
                else if (sValue.compare("busy") == 0)
                    nodeState = pbs_busy;
                else if (sValue.compare("reserve") == 0)
                    nodeState = pbs_reserve;
                else if (sValue.compare("running") == 0)
                    nodeState = pbs_running;
                else if (sValue.compare("down") == 0)
                    nodeState = pbs_down;
                else if (sValue.compare("offline") == 0)
                    nodeState = pbs_offline;
                else if (sValue.compare("job-exclusive") == 0)
                    nodeState = pbs_job_exclusive;
                else if (sValue.compare("job-sharing") == 0)
                    nodeState = pbs_job_sharing;
                else // else check for multiple states
                {
                    QStringList slStates = sValue.split(",");
                    if (slStates.count() > 1)	// could be multiple states (e.g, "down,offline")
                        nodeState = pbs_multiple;
                }

                node->m_nodeState = nodeState;
                node->m_sNodeState = sValue;	// save off string representation of node state (could be multiple states)
            }
			// special case 2:  if it's 'properties'
			else if (sLabel.compare("properties") == 0)	// if this is the "properties" field
			{
				node->m_properties = sValue.split(","); // split on commas
			}
			// special case 3:  if it's 'status'
            else if (sLabel.compare("status") == 0)	// if this is the "status" field
            {
                // look for "message=ERROR" in the status line -- if found, set bErrorStatus to true (will display node icon in red)
                bool bErrorStatus = false;
                QStringList sl = sValue.split(",");
                foreach (QString sField, sl)
                {
                    QStringList parts = sField.split("=");
                    if (parts.count() > 1)
                    {
                        if (parts[0] == "message" && parts[1].startsWith("ERROR"))
                        {
                            bErrorStatus = true;
                            break;
                        }
                    }
                }
                if (bErrorStatus)
                    node->m_bErrorStatus = true;
                else
                    node->m_bErrorStatus = false;

            }
			// special case 4:  if it's 'np', save it
            else if (sLabel.compare("np") == 0)	// if this is the "np" (number of procs) field
            {
                node->m_np = sValue.toInt();
            }
			// special case 5:  if it's 'jobs', save it in a jobs list
            else if (sLabel.compare("jobs") == 0)	// if this is the "jobs" field
            {
                node->m_jobs = sValue.split(","); // split on commas
            }
			// special case 6:  if it's 'available_threads', save it in the node->m_availableThreads
            else if (sLabel.compare("available_threads") == 0)	// if this is the "available_threads " field (NUMA)
            {
                node->m_availableThreads = sValue.toInt();
            }
			// special case 7:  if it's 'total_threads', save it in the node->m_totalThreads
            else if (sLabel.compare("total_threads") == 0)	// if this is the "total_threads " field (NUMA)
            {
                node->m_totalThreads = sValue.toInt();
            }
			node->m_data.append(line); // append the original line (not-stripped) to the data QStringList

		} // else if line starts with a nodeName
	} // while not end of file


	// OK, we're at the end of the section -- see if one last node needs to be added
	if (node != NULL) // if there's node data waiting to be added to the node qlist
	{
		m_pbsNodes.append(node);
		node = NULL;	// re-init
	}

	updateLists();
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::enableControls()
{
	if (m_mainWindow->m_bRunningState == runningState_Admin)  // if running With admin privileges
	{
		ui->actionStart_MOM->setEnabled(true);
		ui->actionStart_MOM_Head_node->setEnabled(true);
		ui->actionStop_MOM->setEnabled(true);
		ui->actionStop_MOM_Head_node->setEnabled(true);
	}
	else // else don't enable the start/stop MOM commands
	{
		ui->actionStart_MOM->setEnabled(false);
		ui->actionStart_MOM_Head_node->setEnabled(false);
		ui->actionStop_MOM->setEnabled(false);
		ui->actionStop_MOM_Head_node->setEnabled(false);
	}
    ui->actionMark_node_as_OFFLINE->setEnabled(true);
    ui->actionClear_OFFLINE_Node->setEnabled(true);
    ui->actionAdd_Note->setEnabled(true);
    ui->actionRemove_Note->setEnabled(true);
    ui->actionLog_Viewer->setEnabled(true);
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::disableControls()
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
void PbsNodesTab::initMomctlsFromFile(QTextStream &in) // read in momctl items from a TORQUEView data file
{
	// re-init all GUI widgets
//	ui->treeWidget_Nodes->clear();	// clear out list
	m_momctlMap.clear();

	MomCtlItem* momCtlItem = NULL;
	QString sNodeName;


	while (!in.atEnd())
	{
		QString line = in.readLine();
		if (line.isEmpty())
			continue;

		// have we gotten to the next section after the momctl section?
		if (line.startsWith( "#--------" )) // each section will be terminated by a "#-------" string to indicate the start of the next section
		{
			break;
		}

		if (line.startsWith("TORQUEView - Momctl Node Name:"))  // this signals the start of a new momctl item
		{
			if (momCtlItem != NULL) // if there's momctl data waiting to be added..
			{
				m_momctlMap[sNodeName] = momCtlItem;  // add the momctl item to the map (IMPORTANT: use the old sNodeName!!)
				momCtlItem = NULL;	// re-init
			}
			momCtlItem = new MomCtlItem();	// start a new momctl item


			// now update sNodeName - get the node name from this line -- (will be used the next iteration - when we do the m_momctlMap[sNodeName] above)
			QStringList sl = line.split(":");
			if (sl.count() > 1)
			{
				sNodeName = sl[1].trimmed();  // remove leading and trailing spaces
			}
		}
		else
		{
			momCtlItem->m_data.append(line);
		}
	} // while not end of file


	// OK, we're at the end of the section -- see if one last item needs to be added
	if (momCtlItem != NULL) // if there's data waiting to be added to the momctl map
	{
		m_momctlMap[sNodeName] = momCtlItem;  // add the momctl item to the map
		momCtlItem = NULL;	// re-init
	}

//	updateLists();
}


/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::issueCmd_Momctl_d3(QString nodeName, QString momManagerPort)  // execute "momctl -d3" command call for that node -- parse output data
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


	if (m_showMomctlSTDERROutput)
	{
		// see if there are any errors to display
		if (!m_momCtl_Stderr.isEmpty())
		{
//			QMessageBox::critical(0, "Error issuing momctl command",
//								  QString("Error issuing 'momctl -d3'' command.  Error message was: %1").arg(m_momCtl_Stderr));
			ErrorMsgDlg dlg("TORQUEView Error", "Error issuing 'momctl -d3' command. Error message:", m_momCtl_Stderr);
			dlg.exec();
			if (dlg.isDontShowThisAgainChecked())  // see if the "Don't show this dialog again" checkbox was checked
				m_showMomctlSTDERROutput = false;
			m_lastNodeSelected = NULL;
		}
	}

}

/*******************************************************************************
 *
*******************************************************************************/
// called from momctlFromCmd() - this gets called whenever the momctl process has something to say...
void PbsNodesTab::momctl_getStdout()
{
	QByteArray s = m_momctlProcess->readAllStandardOutput(); // read normal output
	m_momCtl_Stdout.append( s );  // if there's any stdout
}

/*******************************************************************************
 *
*******************************************************************************/
// called from momctlFromCmd() - this gets called whenever the momctl process has something to say...
void PbsNodesTab::momctl_getStderr()
{
	QByteArray s = m_momctlProcess->readAllStandardError(); // read error channel
	m_momCtl_Stderr.append( s );  // if there's any stderr
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::momctl_processStdout() // parse the stdout data collected (above)
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
void PbsNodesTab::populateMomctlFromFile(QString nodeName, QTreeWidget* treeWidgetMomCtl, QLabel* labelTitleMomCtl) // called when parsing momctl data obtained from a file (and stored in m_momctlHash)
{
    bool bFirstLine = true;

	MomCtlItem* momCtlItem = m_momctlMap.value(nodeName);
	if (momCtlItem == NULL)
		return;

	QStringList lines = momCtlItem->m_data;
    foreach (QString line, lines)
    {
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
            labelTitleMomCtl->setText( line );
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
        QTreeWidgetItem * rootitem = new QTreeWidgetItem(treeWidgetMomCtl);
        rootitem->setText(0, sLabel);
	//  rootitem->setIcon(0, QIcon(":/icons/images/icon_green.bmp"));
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
			//  item->setIcon(0, QIcon(":/icons/images/icon_green.bmp"));
                //	item->setText(1, sValue);	// (don't put anything in column 1)
            }
            continue;
        }
        rootitem->setText(1, sValue);
        rootitem->setToolTip( 1, sValue );
    }
}


/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::writeDataToFile(QTextStream& out)
{
	// WRITE OUT PBSNODES/MOMCTL DATA TO FILE:

	m_momCtl_Lines.clear();  // clear out momctl list

	int dataSource = m_mainWindow->getComboBox_DataSource_CurrentIndex();

	out << "#---------------------------------------------------------------" << "\n";
	out << "TORQUEView -- Pbsnode Output" << "\n";
	out << "#---------------------------------------------------------------" << "\n";
	out << "\n";
	out << "\n";

	int nodeCount = m_pbsNodes.count();
	for (int i=0; i<nodeCount; i++)
	{
		PbsNode* node = m_pbsNodes[i];
		QString nodeName = node->m_nodeName;
		out << nodeName << "\n";

		// add each of node's data lines
		int dataCount = node->m_data.count();
		for (int j=0; j<dataCount; j++)
		{
			QString line = node->m_data[j];
			out << line << "\n";
		}
		out << "\n";

		if (dataSource == 0 || dataSource == 1)	// if data is coming from pbsnodes command (either 0="Local host", or 1="Remote host")
		{
			// first, add a special "TORQUEView" Node Name line to the stringlist
			m_momCtl_Lines.append(QString("TORQUEView - Momctl Node Name:  %1\n").arg(node->m_nodeName));

            if (m_mainWindow->m_Snapshot_SaveIndividualNodeData)  // only do this if user wants to include momctl -d3 data in snapshot file
            {
				if (m_mainWindow->m_bRunningState == runningState_Admin)  // if running With admin privileges
				{
					issueCmd_Momctl_d3(node->m_nodeName, node->m_momManagerPort); // execute "momctl -d3" command call for that node -- parse output data
				}
                // NOTE: since m_bTakingSnapshot is true, will add it to m_momCtl_Lines
            }
		}
	}

	if (dataSource == 0 || dataSource == 1)	// if data is coming from pbsnodes command (either 0="Local host", or 1="Remote host")
	{
		// finally, if there are momctl lines, add each line to end of file
		if (m_momCtl_Lines.count() > 0)
		{
			out << "\n";
			out << "\n";
			out << "\n";
			out << "#---------------------------------------------------------------" << "\n";
			out << "TORQUEView -- MomCtl Output" << "\n";
			out << "#---------------------------------------------------------------" << "\n";
			out << "\n";
			out << "\n";

			foreach (QString s, m_momCtl_Lines)
			{
				out << s << "\n";
			}
		}
	}
	else // data is coming from a snapshot file
	{
		if (m_momctlMap.size() > 0)
		{
			out << "\n";
			out << "\n";
			out << "\n";
			out << "#---------------------------------------------------------------" << "\n";
			out << "TORQUEView -- MomCtl Output" << "\n";
			out << "#---------------------------------------------------------------" << "\n";

			// iterate through the m_momctlMap and write out each item
			QMapIterator<QString, MomCtlItem*> i(m_momctlMap);
			while (i.hasNext())
			{
				i.next();
				QString sNodeName = i.key();
				MomCtlItem* momCtlItem = i.value();

				// first, add a special "TORQUEView" Node Name line to the stringlist
				QString s = QString("\n\nTORQUEView - Momctl Node Name:  %1\n\n").arg(sNodeName);
				out << s;

				QStringList sl = momCtlItem->m_data;
				foreach (QString line, sl)
				{
					out << line << endl;
				}

			   // cout << i.key() << ": " << i.value() << endl;
			}
		}
	}
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::on_treeWidget_Nodes_itemSelectionChanged ()
{
	// called when user clicks on an item in the Nodes tree
	QList<QTreeWidgetItem*> selectedItems = ui->treeWidget_Nodes->selectedItems();
	if ( selectedItems.size() > 0)
	{
		ui->treeWidget_NodeInfo->clear();

		QTreeWidgetItem* item = selectedItems[0];	// list is single-select
		// get index from item's data
		QVariant qv = item->data(0, Qt::UserRole);
		int index = qv.value<int>();
		PbsNode* node = m_pbsNodes.at(index);

//		ui->label_Title_NodeInfo->setText( QString("Node name:  %1").arg(node->m_nodeName) );
		ui->label_Title_NodeInfo->setText( QString("%1").arg(node->m_nodeName) );
        populateNodeInfo(node, ui->treeWidget_NodeInfo);

		// now update the momctl groupbox treewidget
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
			if (m_mainWindow->m_bRunningState == runningState_Admin)  // if running With admin privileges
			{
				issueCmd_Momctl_d3(node->m_nodeName, node->m_momManagerPort);
			}
			else
			{
				// show msg: "(Because user does not have admin rights, "momctl -d3" values are unavailable.)"
				QString sText1 = QString("(Not running with admin");
				QString sText2 = QString("rights. Values unavailable.)");
				ui->treeWidget_MomCtl->clear();	// clear out list
				QTreeWidgetItem * itemLine1 = new QTreeWidgetItem(ui->treeWidget_MomCtl);
				QTreeWidgetItem * itemLine2 = new QTreeWidgetItem(ui->treeWidget_MomCtl);
				itemLine1->setText(0, sText1);
				itemLine2->setText(0, sText2);
			}
		}
		else  // else coming from snapshot file
		{
			// get the momctrl stringlist from the hash
            populateMomctlFromFile(node->m_nodeName, ui->treeWidget_MomCtl, ui->label_Title_MomCtl);
		}

		// is user selected a jobID, copy it to the spinbox at the bottom
		if (ui->radioButton_ShowCores->isChecked()) // don't try to copy if user is looking at Properties, not Cores!
		{
			int column = ui->treeWidget_Nodes->currentColumn();
			if (column > 1)	// disregard Node or State columns
			{
				QString jobIDText = item->text(column);
				ui->spinBox_JobID->setValue(jobIDText.toInt());  // display value in the JobID spinbox at the bottom of the screen
			}
		}

	}
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::populateNodeInfo(PbsNode* node, QTreeWidget* treeWidgetNodeInfo)  // populate the NodeInfo tree
{
	for (int j=0; j<node->m_data.count(); j++)
	{

		// parse out fields from node's QString line
		QString line = node->m_data[j].trimmed();	// strip off white space at beginning and end
		int length = line.length();

		int index = 0;
		index = line.indexOf("=",0);  // look for first "=" in line
		if(index == -1)	// if NOT found, just continue
			continue;
		QString sLabel = line.left(index-1);
		QString sValue = line.right(length - index - 2);
		sLabel = sLabel.trimmed();
		sValue = sValue.trimmed();


		// now add data to tree ----------

		// special case 1:  "status"
		if (sLabel.compare("status") == 0)  // if "status", insert items as sub-items under the parent "status" item
		{
            QTreeWidgetItem* item = new QTreeWidgetItem(treeWidgetNodeInfo);
			item->setText(0, sLabel);	// "status" label
	//		item->setText(1, sValue);
	//		item->setIcon(0, QIcon(":/icons/images/icon_green.bmp"));
			item->setExpanded(true);
			QStringList sl = sValue.split(",");
			for (int i=0; i<sl.length(); i++)
			{
				QStringList parts = sl[i].split("=");
				QTreeWidgetItem* subitem = new QTreeWidgetItem(item);
				subitem->setText(0, parts[0]);
				if (parts.size()>1)
				{
					subitem->setText(1, parts[1]);
					subitem->setToolTip( 1, parts[1] );
				}
			}
		}
		// special case 2:  if it's 'jobs', parse it out (from Jason Booth)
		else if (sLabel.compare("jobs") == 0)	// if this is the "jobs" field
		{
            QTreeWidgetItem* item = new QTreeWidgetItem(treeWidgetNodeInfo);
			item->setText(0, sLabel);
	//		item->setText(1, sValue);
	//		item->setIcon(0, QIcon(":/icons/images/icon_green.bmp"));
			item->setExpanded(true);

			QStringList sl = sValue.split(","); // split on commas
			if (sl.count() == 1)
			{
				item->setText(1, sValue);	// just one job, so set it as the value of 'item'
			}
			else
			{
				foreach (QString s, sl)
				{
					QTreeWidgetItem* subitem = new QTreeWidgetItem(item);
					subitem->setText(0, s);
				//	subitem->setIcon(0, QIcon(":/icons/images/icon_green.bmp"));

				}
			}
		}
		// special case 3:  if it's 'properties', parse it out (from Jason Booth)
		else if (sLabel.compare("properties") == 0)	// if this is the "properties" field
		{
            QTreeWidgetItem* item = new QTreeWidgetItem(treeWidgetNodeInfo);
			item->setText(0, sLabel);	// 'properties'
	//		item->setText(1, sValue);
	//		item->setIcon(0, QIcon(":/icons/images/icon_green.bmp"));

			QStringList sl = sValue.split(","); // split on commas
			if (sl.count() == 1)
			{
				item->setText(1, sValue);	// just one property, so set it as the value of 'item'
			}
			else
			{
				foreach (QString s, sl)
				{
					QTreeWidgetItem* subitem = new QTreeWidgetItem(item);
					subitem->setText(0, s);
				//	subitem->setIcon(0, QIcon(":/icons/images/icon_green.bmp"));

				}
			}
		}
		// special case 4:  "gpu_status"
		else if (sLabel.compare("gpu_status") == 0)  // if "gpu_status", insert items as sub-items under the parent "status" item
		{
            QTreeWidgetItem* item = new QTreeWidgetItem(treeWidgetNodeInfo);
			item->setText(0, sLabel);	// "gpu_status" label
	//		item->setText(1, sValue);
	//		item->setIcon(0, QIcon(":/icons/images/icon_green.bmp"));
			item->setExpanded(true);
			QStringList slCommaPieces = sValue.split(",");	// first, split using "," comma chars
			foreach (QString sPiece, slCommaPieces)
			{
				int len = sPiece.length();

				QTreeWidgetItem* subitem = new QTreeWidgetItem(item);
			//	subitem->setExpanded(true);


				int index = sPiece.indexOf("=");
				if (index != -1)
				{
					QString sName = sPiece.left(index);
					subitem->setText(0, sName);
				//	subitem->setIcon(0, QIcon(":/icons/images/icon_green.bmp"));
				}

				if (sPiece.startsWith("gpu["))
				{
					QString sVal = sPiece.right(len - index - 1);

					QStringList slSemicolonPieces = sVal.split(";");	// second, split using ";" semicolon chars
					foreach (QString sPart, slSemicolonPieces)
					{
						QStringList parts = sPart.split("=");
						QTreeWidgetItem* myItem = new QTreeWidgetItem(subitem);
						myItem->setText(0, parts[0]);
						if (parts.length()>1)
							myItem->setText(1, parts[1]);
					//	myItem->setIcon(0, QIcon(":/icons/images/icon_green.bmp"));

					}
				}
				else
				{
					QString sVal = sPiece.right(len - index - 1);
					subitem->setText(1, sVal);

				}
			}
		}
		else
		{
            QTreeWidgetItem* item = new QTreeWidgetItem(treeWidgetNodeInfo);
			item->setText(0, sLabel);
			item->setText(1, sValue);
		//	item->setIcon(0, QIcon(":/icons/images/icon_green.bmp"));
		//	if (sLabel.compare("jobs") == 0)
		//		item->setToolTip( 1, m_mainWindow->formatForTooltip(sValue, ",") ); // split on commas
		//	else if (sLabel.compare("properties") == 0)
		//		item->setToolTip( 1, m_mainWindow->formatForTooltip(sValue, ",") ); // split on commas
		//	else
				item->setToolTip( 1, sValue );
			item->setExpanded(true);
		}
	}
}


/*******************************************************************************
 *
*******************************************************************************/
bool PbsNodesTab::selectLastPbsnode()
{
	if ( (m_lastNodeSelected) && (! m_lastNodeSelected->m_nodeName.isEmpty()) )
	{
		QList<QTreeWidgetItem *> itemList = ui->treeWidget_Nodes->findItems ( m_lastNodeSelected->m_nodeName,
			Qt::MatchExactly );
		if (itemList.count() == 0)
		{
			m_lastNodeSelected = NULL;
			return true; // this is still successful
		}
		itemList[0]->setSelected(true); // select the item
	}
	return true;
}


/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::getNodeInfo(NodeState state, QString sMultipleStates, QString& sNodeState, bool bJobsPresent,
	  bool bErrorStatus, QIcon& icon, QColor& backgroundcolor)
{
	// get node state
	switch (state) {
	case pbs_free:
		if (bErrorStatus)  // first, check if node has error status set
		{
			sNodeState = "free (error)";
			icon = QIcon(":/icons/images/icon_red.bmp");
			backgroundcolor = QColor("red");
			return;
		}
		if (bJobsPresent)
		{
			sNodeState = "free (jobs running)";
			icon = QIcon(":/icons/images/icon_green.bmp");
			backgroundcolor = QColor("lightgreen");
		}
		else
		{
			sNodeState = "free (idle)";
			icon = QIcon(":/icons/images/icon_blue.bmp");
		//	backgroundcolor = QColor("blue").lighter(190);
			backgroundcolor = QColor("cyan");
		}
		break;
	case pbs_offline:
		sNodeState = "offline";
		icon = QIcon(":/icons/images/icon_orange.bmp");
		backgroundcolor = QColor("orange");
		break;
	case pbs_down:
		sNodeState = "down";
//		icon = QIcon(":/icons/images/icon_red.bmp");
//		backgroundcolor = QColor("red");
		icon = QIcon(":/icons/images/icon_gray.bmp");
		backgroundcolor = QColor("lightgray");
		break;
//	case pbs_reserve:
//		sNodeState = "reserve";
//		icon = QIcon(":/icons/images/icon_white.bmp");
//		backgroundcolor = QColor(Qt::white);
//		break;
	case pbs_job_exclusive:
		sNodeState = "job-exclusive";
		icon = QIcon(":/icons/images/icon_purple.bmp");
		backgroundcolor = QColor(Qt::magenta).lighter(140);
		break;
	case pbs_job_sharing:
		sNodeState = "job-sharing";
		icon = QIcon(":/icons/images/icon_white.bmp");
		backgroundcolor = QColor(Qt::white);
		break;
	case pbs_busy:
		sNodeState = "busy";
		icon = QIcon(":/icons/images/icon_yellow.bmp");
		backgroundcolor = QColor("yellow");
		break;
//	case pbs_running:
//		sNodeState = "running";
//		icon = QIcon(":/icons/images/icon_blue.bmp");
//		backgroundcolor = QColor("lightblue");
//		break;
	case pbs_multiple:
	//	sNodeState = "multiple";
		sNodeState = sMultipleStates;	// get saved off string value of state (only used in case of multiple simultaneous states)
		icon = QIcon(":/icons/images/icon_gray.bmp");
		backgroundcolor = QColor("gray");
		break;
	case pbs_state_unknown:
	default:
		sNodeState = "state unknown";
//		icon = QIcon(":/icons/images/icon_gray.bmp");
//		backgroundcolor = QColor("gray");
		icon = QIcon(":/icons/images/icon_red.bmp");
		backgroundcolor = QColor("red");
		break;
	} // switch
}


/*******************************************************************************
 *  show all nodes running job <jobID>
*******************************************************************************/
void PbsNodesTab::showNodesRunningJob( QString jobId/*, QString execHosts*/ )
{
    m_mainWindow->m_pbsNodesTab->setJobID(jobId);  // set the jobID value in pbsNodesTab's spinBox_JobID control
    m_mainWindow->m_pbsNodesTab->checkShowNodesRunningJobIDCheckbox( true );
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::setJobID( QString jobID )
{
    // set the jobID value in pbsNodesTab's spinBox_JobID control
    // jobID will be in the format:  23097.acueo.ac
    QStringList sl = jobID.split(".");
    QString sJobID = sl[0];
    ui->spinBox_JobID->setValue(sJobID.toInt());
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::checkShowNodesRunningJobIDCheckbox(bool bCheck)
{
    ui->checkBox_ShowNodesRunningJobID->setChecked(bCheck);   // check/uncheck the checkbox
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::refresh()
{
    m_lastHomeDirectory = "";
    m_mainWindow->m_heatMapTab->clearLastHomeDirectory();

    m_mainWindow->initAllTabs(false); // don't include qmgr - Ken says it doesn't need to be updated on refresh
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::on_actionStart_MOM_triggered()
{
	//  starting MOM.... (user clicked on treeWidget_Nodes list)

	resetErrorMsgDlg();  // clear all "don't show this message again" flags

	QList<QTreeWidgetItem*> selectedItems = ui->treeWidget_Nodes->selectedItems();
	foreach (QTreeWidgetItem* item, selectedItems)	// list is extended-select
	{
		// get index from item's data
		QVariant qv = item->data(0, Qt::UserRole);
		int index = qv.value<int>();
		PbsNode* node = m_pbsNodes.at(index);

		issueCmd_StartMOM( node, false ); // false=for a standard compute node
	}

//	QThread::sleep(3);	// doesn't compile on Ken's box
    // sleep 3 secs -- starting a MOM seems to need several secs to start (why is this, Ken?)
    QEventLoop loop;
    QTimer::singleShot(2000, &loop, SLOT(quit()));
    loop.exec();

	refresh();	// refresh all lists

}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::on_actionStart_MOM_Head_node_triggered()
{
	//  start MOM (for a head node).... (user clicked on treeWidget_Nodes list)

	resetErrorMsgDlg();  // clear all "don't show this message again" flags

	QList<QTreeWidgetItem*> selectedItems = ui->treeWidget_Nodes->selectedItems();
	foreach (QTreeWidgetItem* item, selectedItems)	// list is extended-select
	{
		// get index from item's data
		QVariant qv = item->data(0, Qt::UserRole);
		int index = qv.value<int>();
		PbsNode* node = m_pbsNodes.at(index);

		issueCmd_StartMOM( node, true );  // true=for a head node
	}

//	QThread::sleep(3);	// doesn't compile on Ken's box
    // sleep 3 secs -- starting a MOM seems to need several secs to start (why is this, Ken?)
    QEventLoop loop;
    QTimer::singleShot(2000, &loop, SLOT(quit()));
    loop.exec();

	refresh();	// refresh all lists

}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::on_actionStop_MOM_triggered()
{
	// stopping MOM...(user clicked on treeWidget_Nodes list)

	resetErrorMsgDlg();  // clear all "don't show this message again" flags

	QList<QTreeWidgetItem*> selectedItems = ui->treeWidget_Nodes->selectedItems();
	foreach (QTreeWidgetItem* item, selectedItems)	// list is extended-select
	{
		// get index from item's data
		QVariant qv = item->data(0, Qt::UserRole);
		int index = qv.value<int>();
		PbsNode* node = m_pbsNodes.at(index);

		issueCmd_StopMOM( node, false );  // false=for a standard compute node
	}

	refresh();	// refresh all lists
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::on_actionStop_MOM_Head_node_triggered()
{
	// stopping MOM...(user clicked on treeWidget_Nodes list)

	resetErrorMsgDlg();  // clear all "don't show this message again" flags

	QList<QTreeWidgetItem*> selectedItems = ui->treeWidget_Nodes->selectedItems();
	foreach (QTreeWidgetItem* item, selectedItems)	// list is extended-select
	{
		// get index from item's data
		QVariant qv = item->data(0, Qt::UserRole);
		int index = qv.value<int>();
		PbsNode* node = m_pbsNodes.at(index);

		issueCmd_StopMOM( node, true ); //true=for a head node
	}

	refresh();	// refresh all lists
}

/*******************************************************************************
 *
*******************************************************************************/
bool PbsNodesTab::issueCmd_StartMOM( PbsNode* node, bool bIsHeadNode )  // issue "Start MOM" command
{
	m_mainWindow->setIsCurrentlyExecutingCmd(true);  // indicate "TORQUEView IS currently executing a TORQUE client command"

	m_startMOM_Stdout.clear();
	m_startMOM_Stderr.clear();


	QString startMOMCommand;

	// is this a head node or compute node?
//	bool bIsHeadNode = false;
//	QString remoteServer = m_mainWindow->m_Config_RemoteServer;
//	bool isRemote = m_mainWindow->m_Config_IsRemote;
//	if (isRemote && remoteServer.compare(node->m_nodeName) == 0)
//		bIsHeadNode = true;

	if (bIsHeadNode)
	{
		if (m_mainWindow->m_Config_UsingMultiMoms)
		{
			startMOMCommand = QString (m_mainWindow->m_Config_Cmd_StartMOM_HeadNode_Multimom) // yes, we re-use param %1 twice
					.arg(node->m_nodeName)
					.arg(node->m_momServicePort)
					.arg(node->m_momManagerPort);
		}
		else if (m_mainWindow->m_Config_UseServiceToStartStopMOMs)
		{
			startMOMCommand = QString (m_mainWindow->m_Config_Cmd_StartMOM_HeadNode_Service)
					.arg(node->m_nodeName);
		}
		else
		{
			startMOMCommand = QString (m_mainWindow->m_Config_Cmd_StartMOM_HeadNode_Standard)
					.arg(node->m_nodeName);
		}
	}
	else  // it's a compute node
	{
		if (m_mainWindow->m_Config_UsingMultiMoms)
		{
			startMOMCommand = QString (m_mainWindow->m_Config_Cmd_StartMOM_ComputeNode_Multimom) // yes, we re-use param %1 twice
					.arg(node->m_nodeName)
					.arg(node->m_momServicePort)
					.arg(node->m_momManagerPort);
		}
		else if (m_mainWindow->m_Config_UseServiceToStartStopMOMs)
		{
			startMOMCommand = QString (m_mainWindow->m_Config_Cmd_StartMOM_ComputeNode_Service)
					.arg(node->m_nodeName);
		}
		else
		{
			startMOMCommand = QString (m_mainWindow->m_Config_Cmd_StartMOM_ComputeNode_Standard)
					.arg(node->m_nodeName);
		}
	}

	m_mainWindow->statusBar()->showMessage(QString("%1").arg(startMOMCommand), 10000);
	// show "Wait cursor"
	QApplication::setOverrideCursor(Qt::WaitCursor);


	m_startMOMProcess = new QProcess(this);
	connect (m_startMOMProcess, SIGNAL(readyReadStandardOutput()),
			 this, SLOT(startMOM_getStdout())); // connect process signals with my code
	connect (m_startMOMProcess, SIGNAL(readyReadStandardError()),
			 this, SLOT(startMOM_getStderr())); // same here
	m_startMOMProcess->start(startMOMCommand);

	if (!m_startMOMProcess->waitForStarted())
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Couldn't start node", "'start node' process unable to start.  Torque is probably not running.\n");
		m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
		return false;
	}

	if (!m_startMOMProcess->waitForFinished(-1)) //If msecs is -1, this function will not time out
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Error trying to start node", "Process's waitForFinished() method returned error.\n");
		m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
		return false;
	}

	delete m_startMOMProcess;
	m_startMOMProcess = NULL;


	startMOM_processStdout(); // parse the stdout data collected (above)
	// restore the original cursor
	QApplication::restoreOverrideCursor();


	if (m_showStartMomSTDERROutput)
	{
		// see if there's any errors to display
		if (!m_startMOM_Stderr.isEmpty())
		{
//			QMessageBox::critical(0, "Error issuing start MOM command",
//								  QString("Error issuing 'start MOM' command.  Error message was: %1").arg(m_startMOM_Stderr));
			ErrorMsgDlg dlg("TORQUEView Error", "Error issuing Start MOM command. Error message:", m_startMOM_Stderr);
			dlg.exec();
			if (dlg.isDontShowThisAgainChecked())  // see if the "Don't show this dialog again" checkbox was checked
				m_showStartMomSTDERROutput = false;
			m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
			return false;
		}
	}

	m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
	return true; // success

}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_StartMOM() - this gets called whenever the startMOM process has something to say...
void PbsNodesTab::startMOM_getStdout()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	QByteArray s = m_startMOMProcess->readAllStandardOutput(); // read normal output
	m_startMOM_Stdout.append( s );  // if there's any stdout
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_StartMOM() - this gets called whenever the startMOM process has something to say...
void PbsNodesTab::startMOM_getStderr()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	QByteArray s = m_startMOMProcess->readAllStandardError(); // read error channel
	m_startMOM_Stderr.append( s );  // if there's any stderr
}


/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::startMOM_processStdout() // parse the stdout data collected (above)
{
//	QMessageBox::information(0, "Starting MOM", "MOM started successfully.");
}



/*******************************************************************************
 *
*******************************************************************************/
bool PbsNodesTab::issueCmd_StopMOM( PbsNode* node, bool bIsHeadNode )  // issue "Stop MOM" command
{
	m_mainWindow->setIsCurrentlyExecutingCmd(true);  // indicate "TORQUEView IS currently executing a TORQUE client command"

	m_stopMOM_Stdout.clear();
	m_stopMOM_Stderr.clear();

	QString stopMOMCommand;

	// is this a head node or compute node?
//	bool bIsHeadNode = false;
//	QString remoteServer = m_mainWindow->m_Config_RemoteServer;
//	bool isRemote = m_mainWindow->m_Config_IsRemote;
//	if (isRemote && remoteServer.compare(node->m_nodeName) == 0)
//		bIsHeadNode = true;

	if (bIsHeadNode)
	{
		if (m_mainWindow->m_Config_UseServiceToStartStopMOMs)
		{
			stopMOMCommand = QString (m_mainWindow->m_Config_Cmd_StopMOM_HeadNode_Service)
					.arg(node->m_nodeName);
		}
		else
		{
			stopMOMCommand = QString (m_mainWindow->m_Config_Cmd_StopMOM_HeadNode_Standard)
					.arg(node->m_nodeName)
					.arg(node->m_momManagerPort);
		}
	}
	else
	{
		if (m_mainWindow->m_Config_UseServiceToStartStopMOMs)
		{
			stopMOMCommand = QString (m_mainWindow->m_Config_Cmd_StopMOM_ComputeNode_Service)
					.arg(node->m_nodeName);
		}
		else
		{
			stopMOMCommand = QString (m_mainWindow->m_Config_Cmd_StopMOM_ComputeNode_Standard)
					.arg(node->m_nodeName)
					.arg(node->m_momManagerPort);
		}
	}



	m_mainWindow->statusBar()->showMessage(QString("%1").arg(stopMOMCommand), 10000);
	// show "Wait cursor"
	QApplication::setOverrideCursor(Qt::WaitCursor);


	m_stopMOMProcess = new QProcess(this);
	connect (m_stopMOMProcess, SIGNAL(readyReadStandardOutput()),
			 this, SLOT(stopMOM_getStdout())); // connect process signals with my code
	connect (m_stopMOMProcess, SIGNAL(readyReadStandardError()),
			 this, SLOT(stopMOM_getStderr())); // same here
	m_stopMOMProcess->start(stopMOMCommand);

	if (!m_stopMOMProcess->waitForStarted())
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Couldn't stop node", "'stop node' process unable to start.  Torque is probably not running.\n");
		m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
		return false;
	}

	if (!m_stopMOMProcess->waitForFinished(-1)) //If msecs is -1, this function will not time out
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Error trying to stop node", "Process's waitForFinished() method returned error.\n");
		m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
		return false;
	}

	delete m_stopMOMProcess;
	m_stopMOMProcess = NULL;

	stopMOM_processStdout(); // parse the stdout data collected (above)
	// restore the original cursor
	QApplication::restoreOverrideCursor();


	if (m_showStopMomSTDERROutput)
	{
		// see if there's any errors to display
		if (!m_stopMOM_Stderr.isEmpty())
		{
//			QMessageBox::critical(0, "Error issuing stop MOM command",
//								  QString("Error issuing stop MOM command.  Error message was: %1").arg(m_stopMOM_Stderr));
			ErrorMsgDlg dlg("TORQUEView Error", "Error issuing Stop MOM command. Error message:", m_startMOM_Stderr);
			dlg.exec();
			if (dlg.isDontShowThisAgainChecked())  // see if the "Don't show this dialog again" checkbox was checked
				m_showStopMomSTDERROutput = false;
			m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
			return false;
		}
	}

	m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
	return true; // success
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_StopMOM() - this gets called whenever the stopMOM process has something to say...
void PbsNodesTab::stopMOM_getStdout()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	QByteArray s = m_stopMOMProcess->readAllStandardOutput(); // read normal output
	m_stopMOM_Stdout.append( s );  // if there's any stdout
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_StopMOM() - this gets called whenever the stopMOM process has something to say...
void PbsNodesTab::stopMOM_getStderr()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	QByteArray s = m_stopMOMProcess->readAllStandardError(); // read error channel
	m_stopMOM_Stderr.append( s );  // if there's any stderr
}


/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::stopMOM_processStdout() // parse the stdout data collected (above)
{
//	QMessageBox::information(0, "Stopping MOM", m_stopMOM_Stdout);
}


/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::on_actionMark_node_as_OFFLINE_triggered()
{
	// Marking Node as OFFLINE...(for nodes treewidget)

	resetErrorMsgDlg();  // clear all "don't show this message again" flags

	QList<QTreeWidgetItem*> selectedItems = ui->treeWidget_Nodes->selectedItems();
	foreach (QTreeWidgetItem* item, selectedItems)	// list is extended-select
	{
		// get index from item's data
		QVariant qv = item->data(0, Qt::UserRole);
		int index = qv.value<int>();
		PbsNode* node = m_pbsNodes.at(index);

		issueCmd_MarkNodeAsOFFLINE( node );
	}

	refresh();	// refresh all lists
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::on_actionClear_OFFLINE_Node_triggered()
{
	//	Clearing OFFLINE Node...(from nodes treeWidget)

	resetErrorMsgDlg();  // clear all "don't show this message again" flags

	QList<QTreeWidgetItem*> selectedItems = ui->treeWidget_Nodes->selectedItems();
	foreach (QTreeWidgetItem* item, selectedItems)	// list is extended-select
	{
		// get index from item's data
		QVariant qv = item->data(0, Qt::UserRole);
		int index = qv.value<int>();
		PbsNode* node = m_pbsNodes.at(index);

		issueCmd_ClearOFFLINENode( node );
	}

	refresh();	// refresh all lists
}

/*******************************************************************************
 *
*******************************************************************************/
bool PbsNodesTab::issueCmd_MarkNodeAsOFFLINE( PbsNode* node )  // issue "Mark Node as OFFLINE" command
{
	m_mainWindow->setIsCurrentlyExecutingCmd(true);  // indicate "TORQUEView IS currently executing a TORQUE client command"

	m_markNodeAsOFFLINE_Stdout.clear();
	m_markNodeAsOFFLINE_Stderr.clear();

	// ----------
	// string format example (local): "pbsnodes -o dim-6"
	// ----------
	QString markNodeAsOFFLINECommand = QString(m_mainWindow->m_Config_Cmd_MarkNodeAsOFFLINE)
			.arg(node->m_nodeName);

	m_mainWindow->statusBar()->showMessage(QString("%1").arg(markNodeAsOFFLINECommand), 10000);
	// show "Wait cursor"
	QApplication::setOverrideCursor(Qt::WaitCursor);


	m_markNodeAsOFFLINEProcess = new QProcess(this);
	connect (m_markNodeAsOFFLINEProcess, SIGNAL(readyReadStandardOutput()),
			 this, SLOT(markNodeAsOFFLINE_getStdout())); // connect process signals with code
	connect (m_markNodeAsOFFLINEProcess, SIGNAL(readyReadStandardError()),
			 this, SLOT(markNodeAsOFFLINE_getStderr())); // same here
	m_markNodeAsOFFLINEProcess->start(markNodeAsOFFLINECommand);

	if (!m_markNodeAsOFFLINEProcess->waitForStarted())
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Couldn't Mark Node As OFFLINE", "'markNodeAsOFFLINE' process unable to start.  Torque is probably not running.\n");
		m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
		return false;
	}

	if (!m_markNodeAsOFFLINEProcess->waitForFinished(-1)) //If msecs is -1, this function will not time out
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Error trying to Mark Node As OFFLINE", "Process's waitForFinished() method returned error.\n");
		m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
		return false;
	}

	delete m_markNodeAsOFFLINEProcess;
	m_markNodeAsOFFLINEProcess = NULL;

	markNodeAsOFFLINE_processStdout(); // parse the stdout data collected (above)
	// restore the original cursor
	QApplication::restoreOverrideCursor();


	if (m_showMarkNodeAsOfflineSTDERROutput)
	{
		// see if there's any errors to display
		if (!m_markNodeAsOFFLINE_Stderr.isEmpty())
		{
//			QMessageBox::critical(0, "Error issuing markNodeAsOFFLINE command",
//								  QString("Error issuing 'markNodeAsOFFLINE' command.  Error message was: %1").arg(m_markNodeAsOFFLINE_Stderr));
			ErrorMsgDlg dlg("TORQUEView Error", "Error issuing 'Mark Node As OFFLINE'' command. Error message:", m_markNodeAsOFFLINE_Stderr);
			dlg.exec();
			if (dlg.isDontShowThisAgainChecked())  // see if the "Don't show this dialog again" checkbox was checked
				m_showMarkNodeAsOfflineSTDERROutput = false;
			m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
			return false;
		}
	}

	m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
	return true; // success
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_markNodeAsOFFLINE() - this gets called whenever the markNodeAsOFFLINE process has something to say...
void PbsNodesTab::markNodeAsOFFLINE_getStdout()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	QByteArray s = m_markNodeAsOFFLINEProcess->readAllStandardOutput(); // read normal output
	m_markNodeAsOFFLINE_Stdout.append( s );  // if there's any stdout
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_markNodeAsOFFLINE() - this gets called whenever the markNodeAsOFFLINE process has something to say...
void PbsNodesTab::markNodeAsOFFLINE_getStderr()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	QByteArray s = m_markNodeAsOFFLINEProcess->readAllStandardError(); // read error channel
	m_markNodeAsOFFLINE_Stderr.append( s );  // if there's any stderr
}


/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::markNodeAsOFFLINE_processStdout() // parse the stdout data collected (above)
{
//	QMessageBox::information(0, "Marking Node As OFFLINE", m_markNodeAsOFFLINE_Stdout);
}


/*******************************************************************************
 *
*******************************************************************************/
bool PbsNodesTab::issueCmd_ClearOFFLINENode( PbsNode* node )  // issue "Clear OFFLINE Node" command
{
	m_mainWindow->setIsCurrentlyExecutingCmd(true);  // indicate "TORQUEView IS currently executing a TORQUE client command"

	m_clearOFFLINENode_Stdout.clear();
	m_clearOFFLINENode_Stderr.clear();

	// ----------
	// string format example (local): "pbsnodes -c dim-6"
	// ----------
	QString clearOFFLINENodeCommand = QString(m_mainWindow->m_Config_Cmd_ClearOFFLINEFromNode)
			.arg(node->m_nodeName);

	m_mainWindow->statusBar()->showMessage(QString("%1").arg(clearOFFLINENodeCommand), 10000);
	// show "Wait cursor"
	QApplication::setOverrideCursor(Qt::WaitCursor);


	m_clearOFFLINENodeProcess = new QProcess(this);
	connect (m_clearOFFLINENodeProcess, SIGNAL(readyReadStandardOutput()),
			 this, SLOT(clearOFFLINENode_getStdout())); // connect process signals with my code
	connect (m_clearOFFLINENodeProcess, SIGNAL(readyReadStandardError()),
			 this, SLOT(clearOFFLINENode_getStderr())); // same here
	m_clearOFFLINENodeProcess->start(clearOFFLINENodeCommand);

	if (!m_clearOFFLINENodeProcess->waitForStarted())
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Couldn't clear OFFLINE Node", "'clearOFFLINENode' process unable to start.  Torque is probably not running.\n");
		m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
		return false;
	}

	if (!m_clearOFFLINENodeProcess->waitForFinished(-1)) //If msecs is -1, this function will not time out
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Error trying to clear OFFLINE node", "Process's waitForFinished() method returned error.\n");
		m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
		return false;
	}

	delete m_clearOFFLINENodeProcess;
	m_clearOFFLINENodeProcess = NULL;

	clearOFFLINENode_processStdout(); // parse the stdout data collected (above)
	// restore the original cursor
	QApplication::restoreOverrideCursor();


	if (m_showClearOfflineNodeSTDERROutput)
	{
		// see if there's any errors to display
		if (!m_clearOFFLINENode_Stderr.isEmpty())
		{
//			QMessageBox::critical(0, "Error issuing clearOFFLINENode command",
//								  QString("Error issuing 'clearOFFLINENode' command.  Error message was: %1").arg(m_clearOFFLINENode_Stderr));
			ErrorMsgDlg dlg("TORQUEView Error", "Error issuing 'Clear OFFLINE Node' command. Error message:", m_clearOFFLINENode_Stderr);
			dlg.exec();
			if (dlg.isDontShowThisAgainChecked())  // see if the "Don't show this dialog again" checkbox was checked
				m_showClearOfflineNodeSTDERROutput = false;
			m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
			return false;
		}
	}

	m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
	return true; // success
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_clearOFFLINENode() - this gets called whenever the clearOFFLINENode process has something to say...
void PbsNodesTab::clearOFFLINENode_getStdout()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	QByteArray s = m_clearOFFLINENodeProcess->readAllStandardOutput(); // read normal output
	m_clearOFFLINENode_Stdout.append( s );  // if there's any stdout
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_clearOFFLINENode() - this gets called whenever the clearOFFLINENode process has something to say...
void PbsNodesTab::clearOFFLINENode_getStderr()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	QByteArray s = m_clearOFFLINENodeProcess->readAllStandardError(); // read error channel
	m_clearOFFLINENode_Stderr.append( s );  // if there's any stderr
}


/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::clearOFFLINENode_processStdout() // parse the stdout data collected (above)
{
//	QMessageBox::information(0, "Clearing OFFLINE Node", m_clearOFFLINENode_Stdout);
}




/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::on_actionAdd_Note_triggered()
{
	//  add a note to the node.... (user clicked on treeWidget_Nodes list)

	resetErrorMsgDlg();  // clear all "don't show this message again" flags

	AddNoteDlg dlg;
//	dlg.setAnnotation("This is some sample text, etc.");
	if (!dlg.exec())
		return;
	QString sAnnotation = dlg.getAnnotation();

	QList<QTreeWidgetItem*> selectedItems = ui->treeWidget_Nodes->selectedItems();
	foreach (QTreeWidgetItem* item, selectedItems)	// list is extended-select
	{
		// get index from item's data
		QVariant qv = item->data(0, Qt::UserRole);
		int index = qv.value<int>();
		PbsNode* node = m_pbsNodes.at(index);

		issueCmd_AddNote( node, sAnnotation );
	}

	refresh();	// refresh all lists
}

/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::on_actionRemove_Note_triggered()
{
	// remove a note from the node ..(user clicked on treeWidget_Nodes list)

	resetErrorMsgDlg();  // clear all "don't show this message again" flags

	QList<QTreeWidgetItem*> selectedItems = ui->treeWidget_Nodes->selectedItems();
	foreach (QTreeWidgetItem* item, selectedItems)	// list is extended-select
	{
		// get index from item's data
		QVariant qv = item->data(0, Qt::UserRole);
		int index = qv.value<int>();
		PbsNode* node = m_pbsNodes.at(index);

		issueCmd_RemoveNote( node );
	}

	refresh();	// refresh all lists
}

/*******************************************************************************
 *
*******************************************************************************/
bool PbsNodesTab::issueCmd_AddNote( PbsNode* node, QString sAnnotation )
{
	m_mainWindow->setIsCurrentlyExecutingCmd(true);  // indicate "TORQUEView IS currently executing a TORQUE client command"

	m_addNote_Stdout.clear();
	m_addNote_Stderr.clear();

	// -----------------
	QString baseCommand = m_mainWindow->m_Config_Cmd_AddNote;
	if (baseCommand.endsWith("\"")) // if ends with " char
		baseCommand.chop(1); // remove last char (the " char)
	QString addNoteCommand;
	if ( m_mainWindow->m_Config_DataSource == 0)  // 0=local host
	{
		addNoteCommand	= QString ("%1 \"%2\" %3")
				.arg(baseCommand)
				.arg(sAnnotation)
				.arg(node->m_nodeName);
	}
	else if ( m_mainWindow->m_Config_DataSource == 1)  // 1=remote host
	{
		addNoteCommand	= QString ("%1 '%2' %3\"")
				.arg(baseCommand)
				.arg(sAnnotation)
				.arg(node->m_nodeName);
	}


	m_mainWindow->statusBar()->showMessage(QString("%1").arg(addNoteCommand), 10000);
	// show "Wait cursor"
	QApplication::setOverrideCursor(Qt::WaitCursor);


	m_addNoteProcess = new QProcess(this);
	connect (m_addNoteProcess, SIGNAL(readyReadStandardOutput()),
			 this, SLOT(addNote_getStdout())); // connect process signals with my code
	connect (m_addNoteProcess, SIGNAL(readyReadStandardError()),
			 this, SLOT(addNote_getStderr())); // same here
	m_addNoteProcess->start(addNoteCommand);

	if (!m_addNoteProcess->waitForStarted())
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Couldn't add note to node", "'add note' process unable to start.  Torque is probably not running.\n");
		m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
		return false;
	}

	if (!m_addNoteProcess->waitForFinished(-1)) //If msecs is -1, this function will not time out
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Error trying to add note to node", "Process's waitForFinished() method returned error.\n");
		m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
		return false;
	}

	delete m_addNoteProcess;
	m_addNoteProcess = NULL;


	addNote_processStdout(); // parse the stdout data collected (above)
	// restore the original cursor
	QApplication::restoreOverrideCursor();


	if (m_showAddNoteSTDERROutput)
	{
		// see if there's any errors to display
		if (!m_addNote_Stderr.isEmpty())
		{
//			QMessageBox::critical(0, "Error issuing Add Note command",
//								  QString("Error issuing 'add Note' command.  Error message was: %1").arg(m_addNote_Stderr));
			ErrorMsgDlg dlg("TORQUEView Error", "Error issuing 'Add Note' command. Error message:", m_addNote_Stderr);
			dlg.exec();
			if (dlg.isDontShowThisAgainChecked())  // see if the "Don't show this dialog again" checkbox was checked
				m_showAddNoteSTDERROutput = false;
			m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
			return false;
		}
	}

    m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
    return true; // success

}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_AddNote() - this gets called whenever the addNote process has something to say...
void PbsNodesTab::addNote_getStdout()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	QByteArray s = m_addNoteProcess->readAllStandardOutput(); // read normal output
	m_addNote_Stdout.append( s );  // if there's any stdout
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_AddNote() - this gets called whenever the addNote process has something to say...
void PbsNodesTab::addNote_getStderr()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	QByteArray s = m_addNoteProcess->readAllStandardError(); // read error channel
	m_addNote_Stderr.append( s );  // if there's any stderr
}


/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::addNote_processStdout() // parse the stdout data collected (above)
{
//	QMessageBox::information(0, "Starting MOM", "MOM started successfully.");
}



/*******************************************************************************
 *
*******************************************************************************/
bool PbsNodesTab::issueCmd_RemoveNote( PbsNode* node )
{
	m_mainWindow->setIsCurrentlyExecutingCmd(true);  // indicate "TORQUEView IS currently executing a TORQUE client command"

	m_removeNote_Stdout.clear();
	m_removeNote_Stderr.clear();


	QString removeNoteCommand = QString (m_mainWindow->m_Config_Cmd_RemoveNote)
			.arg(node->m_nodeName);

	m_mainWindow->statusBar()->showMessage(QString("%1").arg(removeNoteCommand), 10000);
	// show "Wait cursor"
	QApplication::setOverrideCursor(Qt::WaitCursor);


	m_removeNoteProcess = new QProcess(this);
	connect (m_removeNoteProcess, SIGNAL(readyReadStandardOutput()),
			 this, SLOT(removeNote_getStdout())); // connect process signals with my code
	connect (m_removeNoteProcess, SIGNAL(readyReadStandardError()),
			 this, SLOT(removeNote_getStderr())); // same here
	m_removeNoteProcess->start(removeNoteCommand);

	if (!m_removeNoteProcess->waitForStarted())
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Couldn't remove note from node", "'Remove Note' process unable to start.  Torque is probably not running.\n");
		m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
		return false;
	}

	if (!m_removeNoteProcess->waitForFinished(-1)) //If msecs is -1, this function will not time out
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Error trying to Remove Note from node", "Process's waitForFinished() method returned error.\n");
		m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
		return false;
	}

	delete m_removeNoteProcess;
	m_removeNoteProcess = NULL;

	removeNote_processStdout(); // parse the stdout data collected (above)
	// restore the original cursor
	QApplication::restoreOverrideCursor();


	if (m_showRemoveNoteSTDERROutput)
	{
		// see if there's any errors to display
		if (!m_removeNote_Stderr.isEmpty())
		{
//			QMessageBox::critical(0, "Error issuing Remove Note command",
//								  QString("Error issuing 'Remove Note' command.  Error message was: %1").arg(m_removeNote_Stderr));
			ErrorMsgDlg dlg("TORQUEView Error", "Error issuing 'Remove Note' command. Error message:", m_removeNote_Stderr);
			dlg.exec();
			if (dlg.isDontShowThisAgainChecked())  // see if the "Don't show this dialog again" checkbox was checked
				m_showRemoveNoteSTDERROutput = false;
			m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
			return false;
		}
	}

    m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
    return true; // success
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_removeNote() - this gets called whenever the removeNote process has something to say...
void PbsNodesTab::removeNote_getStdout()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	QByteArray s = m_removeNoteProcess->readAllStandardOutput(); // read normal output
	m_removeNote_Stdout.append( s );  // if there's any stdout
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_removeNote() - this gets called whenever the removeNote process has something to say...
void PbsNodesTab::removeNote_getStderr()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	QByteArray s = m_removeNoteProcess->readAllStandardError(); // read error channel
	m_removeNote_Stderr.append( s );  // if there's any stderr
}


/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::removeNote_processStdout() // parse the stdout data collected (above)
{
//	QMessageBox::information(0, "Stopping MOM", m_stopMOM_Stdout);
}


/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::on_actionLog_Viewer_triggered()
{
	// bring up the node's log viewer ..(user clicked on treeWidget_Nodes list)

	resetErrorMsgDlg();  // clear all "don't show this message again" flags

	QList<QTreeWidgetItem*> selectedItems = ui->treeWidget_Nodes->selectedItems();
//	foreach (QTreeWidgetItem* item, selectedItems)	// list is extended-select
	if (selectedItems.count() > 0)
	{
		QTreeWidgetItem* item = selectedItems[0];  // only get first one (for now)
		// get index from item's data
		QVariant qv = item->data(0, Qt::UserRole);
		int index = qv.value<int>();
		PbsNode* node = m_pbsNodes.at(index);

		// need to get the node's logfilename here and pass it into logViewerDlg call (below) --------

		QString cmd = QString("%1 -f").arg(m_mainWindow->m_Config_Cmd_Tail);
		// replace the server's node name with the node name of the specific MOM he's loooking at.
		// For instance, we would replace this:
		//    ssh -o BatchMode=yes root@torque-devtest-01 "tail -f
		// with this:
		//    ssh -o BatchMode=yes root@torque-devtest-02 "tail -f
		QStringList slParts = cmd.split("@");
		if (slParts.count() > 1)
		{
			QString s = slParts[0];
			s.append(QString("@%1 %2")
					 .arg(node->m_nodeName)
					 .arg("\"tail -f\""));
			cmd = s;
		}

		QString logfilename = m_lastHomeDirectory;
		QDateTime now = QDateTime::currentDateTime();
		QString sDate = now.toString("yyyyMMdd");
//		QString sDate = now.toString("yyyy-MM-dd hh:mm:ss a");
//		QString sTime = now.toString("hh:mm:ss a");

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

/*******************************************************************************
 *
*******************************************************************************/
bool PbsNodesTab::issueCmd_GetServerHome()  // get pbs_server's "serverhome" dir  (issue "pbs_server --about" command and parse it)
{
	m_mainWindow->setIsCurrentlyExecutingCmd(true);  // indicate "TORQUEView IS currently executing a TORQUE client command"

	m_getServerHome_Stdout.clear();
	m_getServerHome_Stderr.clear();


	QString getPbsServerHomeCommand = m_mainWindow->m_Config_Cmd_GetServerHome;

//	m_mainWindow->statusBar()->showMessage(QString("%1").arg(pbsGetServerHomeCommand), 10000);
	// show "Wait cursor"
	QApplication::setOverrideCursor(Qt::WaitCursor);


	m_getServerHomeProcess = new QProcess(this);
	connect (m_getServerHomeProcess, SIGNAL(readyReadStandardOutput()),
			 this, SLOT(getServerHome_getStdout())); // connect process signals with my code
	connect (m_getServerHomeProcess, SIGNAL(readyReadStandardError()),
			 this, SLOT(getServerHome_getStderr())); // same here
	m_getServerHomeProcess->start(getPbsServerHomeCommand);

	if (!m_getServerHomeProcess->waitForStarted())
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Couldn't get server home dir", "'pbs_server --about' process unable to start.  Torque is probably not running.\n");
		m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
		return false;
	}

	if (!m_getServerHomeProcess->waitForFinished(-1)) //If msecs is -1, this function will not time out
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Error trying to get server home dir", "Process's waitForFinished() method returned error.\n");
		m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
		return false;
	}

	delete m_getServerHomeProcess;
	m_getServerHomeProcess = NULL;

	getServerHome_processStdout(); // parse the stdout data collected (above)
	// restore the original cursor
	QApplication::restoreOverrideCursor();


	if (m_showGetServerHomeSTDERROutput)
	{
		// see if there's any errors to display
		if (!m_getServerHome_Stderr.isEmpty())
		{
			ErrorMsgDlg dlg("TORQUEView Error", "Error issuing 'Get Server Home' command. Error message:", m_getServerHome_Stderr);
			dlg.exec();
			if (dlg.isDontShowThisAgainChecked())  // see if the "Don't show this dialog again" checkbox was checked
				m_showGetServerHomeSTDERROutput = false;
			m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
			return false;
		}
	}

	m_mainWindow->setIsCurrentlyExecutingCmd(false);  // indicate "TORQUEView is NOT currently executing a TORQUE client command"
	return true; // success
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_GetServerHome() - this gets called whenever the getServerHome process has something to say...
void PbsNodesTab::getServerHome_getStdout()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	QByteArray s = m_getServerHomeProcess->readAllStandardOutput(); // read normal output
	m_getServerHome_Stdout.append( s );  // if there's any stdout
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_GetServerHome() - this gets called whenever the getServerHome process has something to say...
void PbsNodesTab::getServerHome_getStderr()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	QByteArray s = m_getServerHomeProcess->readAllStandardError(); // read error channel
	m_getServerHome_Stderr.append( s );  // if there's any stderr
}


/*******************************************************************************
 *
*******************************************************************************/
void PbsNodesTab::getServerHome_processStdout() // parse the stdout data collected (above)
{
//  QMessageBox::information(0, "got 'pbs_server --about'' info", m_getServerHome_Stdout);
	QStringList lines = m_getServerHome_Stdout.split("\n");
	foreach (QString line, lines)
	{
		if (line.startsWith("Serverhome:", Qt::CaseInsensitive))
		{
			QStringList sl = line.split(" ", QString::SkipEmptyParts );
			if (sl.count() > 1)
			{
				QString s = sl[1];
				m_mainWindow->m_Config_PbsServerHomeDir = s;
				break;
			}
		}
	}
}
