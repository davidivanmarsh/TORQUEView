/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: errormsgdlg.cpp.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
*/

#include "errormsgdlg.h"
#include "ui_errormsgdlg.h"
#include <QPushButton>


ErrorMsgDlg::ErrorMsgDlg(QString sDlgTitle, QString sMsgTitle, QString sMsg,
		 bool bShowCheckbox, bool bShowOKButtonOnly, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ErrorMsgDlg)
{
    ui->setupUi(this);

    setWindowTitle(sDlgTitle);
    ui->label_MsgTitle->setText(sMsgTitle);
    ui->plainTextEdit_Msg->setPlainText(sMsg);
    if (bShowCheckbox == false)
        ui->checkBox_DontShowThisAgain->hide();
    else
        ui->checkBox_DontShowThisAgain->show();

	if (bShowOKButtonOnly)  // if showing only the OK button (not the "OK | Cancel" buttons)
	{
		ui->buttonBox->button( QDialogButtonBox::Cancel ) ->setVisible( false );
	}
}

ErrorMsgDlg::~ErrorMsgDlg()
{
    delete ui;
}

bool ErrorMsgDlg::isDontShowThisAgainChecked()
{
    return ui->checkBox_DontShowThisAgain->isChecked();
}
