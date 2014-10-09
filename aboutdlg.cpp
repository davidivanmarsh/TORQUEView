/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: aboutdlg.cpp.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
*/


#include "aboutdlg.h"
#include "ui_aboutdlg.h"

AboutDlg::AboutDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDlg)
{
    ui->setupUi(this);

	QString s = QString("TORQUEView Version:  %1.%2.%3").arg(TORQUEVIEW_MAJOR).arg(TORQUEVIEW_MINOR).arg(TORQUEVIEW_BUILD);
	ui->label_VersionBuildnumber->setText(s);

	s = QString("Built:  %1,  %2 ").arg(__DATE__).arg(__TIME__);
	ui->label_BuildDateTime->setText(s);

	s = QString("Built on Qt Version:  %1").arg(QT_VERSION_STR);
	ui->label_QtVersion->setText(s);
}

AboutDlg::~AboutDlg()
{
    delete ui;
}
