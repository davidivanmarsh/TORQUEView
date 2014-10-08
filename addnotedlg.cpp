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

