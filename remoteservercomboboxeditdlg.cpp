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
		on_btnAdd_clicked();	// make sure current ui->lineEdit_ServerName contents has been added -- it's easy to forget to press the Add button!

		int count = ui->listWidget->count();
		for(int i=0; i<count; i++)
		{
			QString text = ui->listWidget->item(i)->text();
			m_List.append(text);
		}
	}

	QDialog::done( result );
}
