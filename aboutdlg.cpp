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


#include "aboutdlg.h"
#include "ui_aboutdlg.h"

AboutDlg::AboutDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDlg)
{
    ui->setupUi(this);

	QString s = QString("TORQUEView Version:  %1.%2.%3").arg(TORQUEVIEW_MAJOR).arg(TORQUEVIEW_MINOR).arg(TORQUEVIEW_BUILD);
	ui->label_VersionBuildnumber->setText(s);

	s = QString("Built:  %1,  %2 ").arg(__DATE__).arg(__TIME__);
	ui->label_BuildDateTime->setText(s);

	s = QString("Built on Qt Version:  %1").arg(QT_VERSION_STR);
	ui->label_QtVersion->setText(s);
}

AboutDlg::~AboutDlg()
{
    delete ui;
}
