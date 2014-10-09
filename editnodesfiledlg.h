/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: editnodesfiledlg.h.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
*/

#ifndef EDITNODESFILEDLG_H
#define EDITNODESFILEDLG_H

#include <QDialog>

namespace Ui {
class EditNodesFileDlg;
}

class EditNodesFileDlg : public QDialog
{
    Q_OBJECT

public:
    explicit EditNodesFileDlg(QString sContent, QWidget *parent = 0);
    ~EditNodesFileDlg();

    QString getPlainTextEdit_NodesFile_contents();

private:
    Ui::EditNodesFileDlg *ui;

 // QString m_nodesFilename;

    void done( int result );
};

#endif // EDITNODESFILEDLG_H
