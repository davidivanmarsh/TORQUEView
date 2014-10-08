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


#include "errormsgdlg.h"
#include "ui_errormsgdlg.h"

ErrorMsgDlg::ErrorMsgDlg(QString sDlgTitle, QString sMsgTitle, QString sMsg, bool bShowCheckbox, QWidget *parent) :
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
}

ErrorMsgDlg::~ErrorMsgDlg()
{
    delete ui;
}

bool ErrorMsgDlg::isDontShowThisAgainChecked()
{
    return ui->checkBox_DontShowThisAgain->isChecked();
}
