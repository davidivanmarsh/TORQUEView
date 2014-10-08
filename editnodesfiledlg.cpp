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


