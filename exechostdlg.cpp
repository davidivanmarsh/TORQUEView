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
