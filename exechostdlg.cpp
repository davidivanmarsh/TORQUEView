/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: exechostdlg.cpp.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
*/


#include "exechostdlg.h"
#include "ui_exechostdlg.h"

ExecHostDlg::ExecHostDlg(QString exec_host, QString exec_port, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ExecHostDlg),
	m_exec_host(exec_host),
	m_exec_port(exec_port)
{
	ui->setupUi(this);

	processExecHost();

}

ExecHostDlg::~ExecHostDlg()
{
	delete ui;
}

void ExecHostDlg::processExecHost()
{
	// look through the "exec_host" and "exec_port" strings and parse them,
	// generating one list containing both

	QMap<QString, PortProcs*> hostsMap;

	// first, split on "+" boundaries
	QStringList hostList = m_exec_host.split("+", QString::SkipEmptyParts);
	QStringList portList = m_exec_port.split("+", QString::SkipEmptyParts);

	// make sure there's same number of items in each list
	if (hostList.count() != portList.count())
		return;

	for (int j=0; j<hostList.count(); j++)
	{
		QString sHost = hostList[j];
		QString sPort = portList[j];

		QStringList parts = sHost.split("/");
		if (parts.count() > 1)
		{
			QString hostName = parts[0];
			if (hostsMap.contains(hostName))
			{
				PortProcs* portProcs = hostsMap.value(hostName);
				portProcs->m_procs.append(QString(",%1").arg(parts[1]));
				portProcs->m_port = sPort;
			}
			else
			{
				PortProcs* portProcs = new PortProcs();
				portProcs->m_procs = parts[1];
				portProcs->m_port = sPort;
				hostsMap[hostName] = portProcs;
			}
		}
	}

	// iterate through the m_momctlMap and write out each item
	QMapIterator<QString, PortProcs*> i(hostsMap);
	while (i.hasNext())
	{
		i.next();
		QString sHostName = i.key();
		PortProcs* portProcs = i.value();

		QTreeWidgetItem* newitem = new QTreeWidgetItem(ui->treeWidget);
		newitem->setText(0, sHostName);
		newitem->setText(1, portProcs->m_port);
		newitem->setText(2, portProcs->m_procs);

	}

}
