/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: configitempropertiesdlg.cpp.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
*/

#include "configitempropertiesdlg.h"
#include "ui_configitempropertiesdlg.h"

ConfigItemPropertiesDlg::ConfigItemPropertiesDlg(QString sName, QString sVal, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigItemPropertiesDlg)
{
    ui->setupUi(this);
    ui->label_ItemName->setText(sName);
    ui->lineEdit_ItemValue->setText(sVal);
}

ConfigItemPropertiesDlg::~ConfigItemPropertiesDlg()
{
    delete ui;
}
