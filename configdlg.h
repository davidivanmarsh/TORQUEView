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


#ifndef CONFIGDLG_H
#define CONFIGDLG_H

#include <QDialog>
#include <QTreeWidgetItem>


class MainWindow;


namespace Ui {
class ConfigDlg;
}

class ConfigDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDlg(QWidget *parent = 0);
    ~ConfigDlg();

    void init();

private slots:
	void on_checkBox_UsingMultiMoms_stateChanged();

	void on_treeWidget_Commands_itemActivated(QTreeWidgetItem * item, int column);
	void on_actionShow_Properties_triggered();

    void on_rbtn_Remote_toggled(bool checked);
    void on_comboBox_Remote_Server_editTextChanged(const QString & text);

    void on_btnEditList_clicked();

private:
    Ui::ConfigDlg *ui;

    MainWindow* m_mainWindow;

    void CreateContextMenus();

    void updateCommandListWithLocal();
    void updateCommandListWithRemote( QString remoteServer );

	void done( int result );
};

#endif // CONFIGDLG_H
