/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: remoteservercomboboxeditdlg.cpp.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
*/

#include "remoteservercomboboxeditdlg.h"
#include "ui_remoteservercomboboxeditdlg.h"

RemoteServerComboboxEditDlg::RemoteServerComboboxEditDlg(	QStringList remoteServerList,
															QWidget *parent) :
	QDialog(parent),
	ui(new Ui::RemoteServerComboboxEditDlg),
	m_remoteServerList(remoteServerList)
{
	ui->setupUi(this);

	ui->listWidget->addItems( m_remoteServerList );
	m_listIndex = 0;  // init
}

RemoteServerComboboxEditDlg::~RemoteServerComboboxEditDlg()
{
	delete ui;
}

void RemoteServerComboboxEditDlg::on_btnAdd_clicked()
{
	QString itemText = ui->lineEdit_ServerName->text();
	if (itemText.isEmpty())
	{
		return;
	}

	QList<QListWidgetItem *> list = ui->listWidget->findItems ( itemText, Qt::MatchFixedString );
	if (!list.isEmpty())
	{
		return;
	}


	QListWidgetItem *newItem = new QListWidgetItem;
	newItem->setText(itemText);
	ui->listWidget->addItem(newItem);

	// select the item just inserted into the list (the last item)
	int nRowCount = ui->listWidget->count();
	ui->listWidget->setCurrentRow(nRowCount-1);
}

void RemoteServerComboboxEditDlg::on_btnRemove_clicked()
{
	int row = ui->listWidget->row(ui->listWidget->currentItem());
	ui->listWidget->takeItem(row);
}

void RemoteServerComboboxEditDlg::on_listWidget_itemSelectionChanged()
{
	QListWidgetItem * currentitem = ui->listWidget->currentItem ();
	if (currentitem != NULL)
	{
		ui->lineEdit_ServerName->setText(currentitem->text());
	}

}

/*******************************************************************************
 *
*******************************************************************************/
// the done() function is reimplemented from QDialog. It is called when the user
// clicks OK or Cancel
void RemoteServerComboboxEditDlg::done( int result )
{
	if ( result == QDialog::Accepted )
	{
		// (not sure about doing this next line, commenting it out for now....)
	//	on_btnAdd_clicked();	// make sure current ui->lineEdit_ServerName contents has been added -- it's easy to forget to press the Add button!

		int count = ui->listWidget->count();
		for(int i=0; i<count; i++)
		{
			QString text = ui->listWidget->item(i)->text();
			m_List.append(text);
		}

		// save off the index (will be -1 if invalid)
		m_listIndex = ui->listWidget->currentRow();
	}

	QDialog::done( result );
}
