/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: choosestylesheetdialog.cpp.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
*/


#include "choosestylesheetdialog.h"
#include "ui_choosestylesheetdialog.h"

#include <QSettings>

/*******************************************************************************
 *
*******************************************************************************/
ChooseStylesheetDialog::ChooseStylesheetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseStylesheetDialog)
{
    ui->setupUi(this);
	readSettings();
}

/*******************************************************************************
 *
*******************************************************************************/
ChooseStylesheetDialog::~ChooseStylesheetDialog()
{
    delete ui;
}

/*******************************************************************************
// the done() function is reimplemented from QDialog. It is called when the user
// clicks OK or Cancel
*******************************************************************************/
void ChooseStylesheetDialog::done( int result )
{
    if ( result == QDialog::Accepted )  // if OK button pressed
    {
        iv_CurrentText = ui->styleCombo->currentText();
		writeSettings();
    }

    QDialog::done( result );
}

/*******************************************************************************
 *
*******************************************************************************/
void ChooseStylesheetDialog::readSettings()
{
	QSettings settings( "AdaptiveComputing", "NBrowseLog" );
	//settings.beginGroup(metaObject()->className());
	QString style = settings.value("StyleSheetName", "Default").toString();
	for (int i = 0; i < ui->styleCombo->count(); ++i) {
		if (ui->styleCombo->itemText(i) == style) {
			ui->styleCombo->setCurrentIndex(i);
			break;
		}
	}
}

/*******************************************************************************
 *
*******************************************************************************/
void ChooseStylesheetDialog::writeSettings()
{
	QSettings settings( "AdaptiveComputing", "NBrowseLog" );
	//settings.beginGroup(metaObject()->className());
	settings.setValue("StyleSheetName", ui->styleCombo->currentText());
}

