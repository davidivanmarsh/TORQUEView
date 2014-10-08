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

