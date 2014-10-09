/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: exechostdlg.h.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
*/

#ifndef EXECHOSTDLG_H
#define EXECHOSTDLG_H

#include <QDialog>

namespace Ui {
class ExecHostDlg;
}

struct PortProcs
{
	QString m_port;
	QString m_procs;
};


class ExecHostDlg : public QDialog
{
	Q_OBJECT

public:
	explicit ExecHostDlg(QString exec_host, QString exec_port, QWidget *parent = 0);
	~ExecHostDlg();

private:
	Ui::ExecHostDlg *ui;
	QString m_exec_host;
	QString m_exec_port;
//	PortProcs m_portProcs;

	void processExecHost();

};

#endif // EXECHOSTDLG_H
