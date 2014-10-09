/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: replaysnapshotsdlg.cpp.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
*/

#include "replaysnapshotsdlg.h"
#include "ui_replaysnapshotsdlg.h"
#include <QDir>
#include <QDateTime>
#include <QFileDialog>


/*******************************************************************************
 *
*******************************************************************************/
ReplaySnapshotsDlg::ReplaySnapshotsDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReplaySnapshotsDlg)
{
    ui->setupUi(this);
}

/*******************************************************************************
 *
*******************************************************************************/
ReplaySnapshotsDlg::~ReplaySnapshotsDlg()
{
    delete ui;
}

/*******************************************************************************
 *
*******************************************************************************/
int ReplaySnapshotsDlg::TimeIntervalBetweenReplays()
{
    return ui->spinBox_TimeIntervalBetweenReplays->value();
}
/*******************************************************************************
 *
*******************************************************************************/
QStringList ReplaySnapshotsDlg::SnapshotFilelist()
{
    QStringList sl;

    // iterate through the tree and write out each item
    // (there are only top-level items so far, no child items, so this iterator should work OK)
    QTreeWidgetItemIterator it( ui->treeWidget_Files );
    while (*it)
    {
        sl.append( (*it)->text(0) );
        ++it;
    }

    return sl;
}

/*******************************************************************************
 *
*******************************************************************************/
void ReplaySnapshotsDlg::on_btnRefreshList_clicked()
{
    listFilesToBeReplayed();
}

/*******************************************************************************
 *
*******************************************************************************/
void ReplaySnapshotsDlg::on_btnBrowse_clicked()
{
    QString baseFilename;
    QString filename = ui->lineEdit_SnapshotFilename->text();
    filename = QFileDialog::getOpenFileName(
        this,
        tr("Select filename to use as base for snapshot series playback"),
        filename);

    if (filename.isEmpty())	// if Cancel pressed
        return;
    ui->lineEdit_SnapshotFilename->setText(filename);

    listFilesToBeReplayed();
}
/*******************************************************************************
 *
*******************************************************************************/
void ReplaySnapshotsDlg::listFilesToBeReplayed()
{
    QString sPath = ui->lineEdit_SnapshotFilename->text();
    QFileInfo fi(sPath);
    QString sFilename = fi.fileName();
    QString sPathname = fi.absolutePath();
    QString sBaseFilename;

    QStringList sl = sFilename.split("__");
    if (sl.count() > 1)
    {
       sBaseFilename = sl[0];
       sBaseFilename.append("__*.txt");
    }

    QDir dir(sPathname);

    QStringList nameFilter;
//  nameFilter << "*.png" << "*.jpg" << "*.gif" << "snap_*.txt";
    nameFilter << sBaseFilename;
    dir.setNameFilters(nameFilter);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Time | QDir::Reversed);

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);

        QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget_Files);
//      item->setText(0, fileInfo.fileName());
        item->setText(0, fileInfo.absoluteFilePath());
        item->setText(1, QString("%1").arg(fileInfo.size()));
        item->setTextAlignment(1, Qt::AlignRight);

        QDateTime dateTime = fileInfo.lastModified();
        QString sDate = dateTime.toString("MM/dd/yyyy  hh:mm:ss");
        item->setText(2, sDate);
    }

    // resize columns to contents
    int columnCount = ui->treeWidget_Files->header()->count();
    for(int i = 0; i < columnCount; i++)
    {
        ui->treeWidget_Files->resizeColumnToContents(i);
    }
}
