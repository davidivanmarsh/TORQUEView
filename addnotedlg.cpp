/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: addnotedlg.cpp.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
*/


#include "addnotedlg.h"
#include "ui_addnotedlg.h"
#include <QMessageBox>


/*******************************************************************************
 *
*******************************************************************************/
AddNoteDlg::AddNoteDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNoteDlg)
{
    ui->setupUi(this);
}

/*******************************************************************************
 *
*******************************************************************************/
AddNoteDlg::~AddNoteDlg()
{
    delete ui;
}

/*******************************************************************************
 *
*******************************************************************************/
void AddNoteDlg::setAnnotation(QString text)
{
    ui->plainTextEdit_Note->setPlainText( text );
}

/*******************************************************************************
 *
*******************************************************************************/
QString AddNoteDlg::getAnnotation()
{
    return ui->plainTextEdit_Note->toPlainText();
}

/*******************************************************************************
 *
*******************************************************************************/
// the done() function is reimplemented from QDialog. It is called when the user
// clicks OK or Cancel
void AddNoteDlg::done( int result )
{
	if ( result == QDialog::Accepted ) // if user clicked OK
	{
		QString text = ui->plainTextEdit_Note->toPlainText();
		if (text.contains("'") || (text.contains("\"")))  // (text can't have single- or double-quotes in the text)
		{
			QMessageBox::information( this,
									  "Add Note",
									  "Text can't contain single or double-quotes.",
									  QMessageBox::Ok,
									  QMessageBox::Ok);
			return;  // stay in this dialog -- don't exit
		}
	}

	QDialog::done( result );  // exit dialog
}

