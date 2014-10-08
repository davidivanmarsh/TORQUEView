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


#ifndef CONFIGITEMPROPERTIESDLG_H
#define CONFIGITEMPROPERTIESDLG_H

#include <QDialog>

namespace Ui {
class ConfigItemPropertiesDlg;
}

class ConfigItemPropertiesDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigItemPropertiesDlg(QString sName, QString sVal, QWidget *parent = 0);
    ~ConfigItemPropertiesDlg();

private:
    Ui::ConfigItemPropertiesDlg *ui;
};

#endif // CONFIGITEMPROPERTIESDLG_H
