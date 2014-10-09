/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: editnodesfiledlg.cpp.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
*/


#include "editnodesfiledlg.h"
#include "ui_editnodesfiledlg.h"
#include <QMessageBox>
#include <QTextStream>


/*******************************************************************************
 *
*******************************************************************************/
EditNodesFileDlg::EditNodesFileDlg(QString sContent, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditNodesFileDlg)
{
    ui->setupUi(this);
//  m_nodesFilename = nodesFilename;

//  ui->label_NodesFilePath->setText(nodesFilename);  // show path in title label

    ui->plainTextEdit_NodesFile->setPlainText( sContent );

}

/*******************************************************************************
 *
*******************************************************************************/
EditNodesFileDlg::~EditNodesFileDlg()
{
    delete ui;
}

/*******************************************************************************
 *
*******************************************************************************/
QString EditNodesFileDlg::getPlainTextEdit_NodesFile_contents()
{
    return ui->plainTextEdit_NodesFile->toPlainText();
}

/*******************************************************************************
 *
*******************************************************************************/
// the done() function is reimplemented from QDialog. It is called when the user
// clicks OK or Cancel
void EditNodesFileDlg::done( int result )
{
	if ( result == QDialog::Accepted ) // if user clicked OK
	{
//		QMessageBox::critical(0, "\"Save nodes file contents\" feature not implemented", "The Save Nodes File Contents functionality has not been implemented.");
//		return;
	}

	QDialog::done( result );  // exit dialog
}


