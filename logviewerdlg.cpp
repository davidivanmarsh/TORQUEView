/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/

 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.

 * The Original Code is: logviewerdlg.cpp.

 * The Initial Developer of the Original Code is David I. Marsh.
 * All Rights Reserved.

 * Contributor(s): Ken Nielson.
*/

#include "logviewerdlg.h"
#include "ui_logviewerdlg.h"
#include "mainwindow.h"
#include "errormsgdlg.h"
#include <QFileDialog>
#include <QStatusBar>
#include <QMessageBox>
#include <QSettings>
#include <QTextStream>



/*******************************************************************************
 *
*******************************************************************************/
LogViewerDlg::LogViewerDlg(MainWindow* mainWindow, const QString& logfilename, const QString& command, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::LogViewerDlg)
{
	ui->setupUi(this);

	m_mainWindow = mainWindow;
	m_logViewerCommand = command;

	m_tailLogfileProcess = NULL;

	if (! logfilename.isEmpty())
	{
		ui->lineEdit_LogFilename->setText( logfilename );

		ui->checkBox_RunTail_f->blockSignals(true);	// prevent currentIndexChanged signals from being received
		ui->checkBox_RunTail_f->setChecked(true);   // (we don't want the checkBox_RunTail_f_toggled() method to be called here!)
		ui->checkBox_RunTail_f->blockSignals(false);	// allow currentIndexChanged signals to be received

		issueCmd_tailLogfile(ui->lineEdit_LogFilename->text(), true);  // also start up the "tail -f" process
	}

	// set wrapLines checkBox
	QSettings settings( m_mainWindow->SETTINGS_ADAPTIVE_COMPUTING, m_mainWindow->SETTINGS_TORQUEVIEW );
	bool bWrapLines = settings.value( "WrapLines" ).toBool();
	if (bWrapLines)
	{
		ui->checkBox_WrapLines->setChecked(true);
		// wrap words at the right edge of the text edit
		ui->plainTextEdit_LogView->setLineWrapMode ( QPlainTextEdit::WidgetWidth );
	}
	else
	{
		ui->checkBox_WrapLines->setChecked(false);
		ui->plainTextEdit_LogView->setLineWrapMode( QPlainTextEdit::NoWrap );
	}

	int iSetMaximumLineCount = settings.value("MaximumLineCount", 3000).toInt();
	ui->spinBox_MaxLineCount->setValue( iSetMaximumLineCount );


	// start out by hiding all the grep controls
	ui->label_GrepOptions->hide();
	ui->comboBox_GrepOptions->hide();
	ui->label_GrepPattern->hide();
	ui->btnGrep->hide();

    m_showGrepSTDERROutput = true;
}

/*******************************************************************************
 *
*******************************************************************************/
LogViewerDlg::~LogViewerDlg()
{
	stopTailLogfile();
	delete ui;
}

/*******************************************************************************
 *
*******************************************************************************/
void LogViewerDlg::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event); // prevents warning

    // user clicked the Close icon from the system menu
    stopTailLogfile();
}

/*******************************************************************************
 *
*******************************************************************************/
void LogViewerDlg::on_btnClose_clicked()
{
    // user clicked the Close button
    stopTailLogfile();
    close();
}


/*******************************************************************************
 *
*******************************************************************************/
void LogViewerDlg::on_btnBrowse_clicked()
{
	QString filename = ui->lineEdit_LogFilename->text();
	filename = QFileDialog::getOpenFileName(
		this,
		tr("Select log file to display"),
		filename);

	if (!filename.isEmpty())	// if Cancel wasn't pressed
	{
		ui->lineEdit_LogFilename->setText(filename);
	}

//	// open file for input
//	QFile file(filename);
//	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
//	{
//		QMessageBox::critical(0, "Error reading file", QString("Error, unable to read file '%1'.")
//			.arg(filename));
//		return;
//	}

//	QTextStream in(&file);
//	ui->plainTextEdit_LogView->setPlainText( in.readAll() );
//	file.close();
}

/*******************************************************************************
 *
*******************************************************************************/
void LogViewerDlg::startTailLogfile()  // start "tail f" process
{
    issueCmd_tailLogfile(ui->lineEdit_LogFilename->text(), true);
}

/*******************************************************************************
 *
*******************************************************************************/
void LogViewerDlg::stopTailLogfile() // stop "tail f" process
{
//  issueCmd_tailLogfile(ui->lineEdit_LogFilename->text(), false);

	// user clicked the Close icon from the system menu
	if (m_tailLogfileProcess != NULL)
	{
		delete m_tailLogfileProcess;
		m_tailLogfileProcess = NULL;
	}

}

/*******************************************************************************
 *
*******************************************************************************/
void LogViewerDlg::on_checkBox_RunTail_f_toggled(bool checked)
{
	if (checked)
		startTailLogfile();
	else
		stopTailLogfile();
}


/*******************************************************************************
 *
*******************************************************************************/
void LogViewerDlg::on_btnClear_clicked()
{
	ui->plainTextEdit_LogView->clear();
}

/*******************************************************************************
 *
*******************************************************************************/
void LogViewerDlg::on_btnSaveAs_clicked()
{
	stopTailLogfile();  // make sure "tail -f" command has been stopped

	QString filename = QFileDialog::getSaveFileName(this);
	if (filename.isEmpty())
		return;

	QFile file(filename);
	if (!file.open(QFile::WriteOnly | QFile::Text))
	{
		QMessageBox::warning(this, tr("Log Viewer"),
			tr("Cannot write file %1:\n%2.")
			.arg(filename)
			.arg(file.errorString()));
		return;
	}

	// open file for output
	QFile outputFile(filename);
	outputFile.open(QIODevice::WriteOnly);
	// opened OK?
	if(!outputFile.isOpen())
	{
	//	qDebug() << argv[0] << "- Error, unable to open" << outputFilename << "for output";
		QMessageBox::critical(0, "Error writing file", QString("Error, unable to open file '%1' for writing.")
			.arg(filename));
		return;
	}

	QTextStream outStream(&outputFile);
	outStream << ui->plainTextEdit_LogView->toPlainText();
	outputFile.close();
}

/*******************************************************************************
 *
*******************************************************************************/
bool LogViewerDlg::issueCmd_tailLogfile(QString logfilename, bool bStart) // either Start or Stop
{
	if (m_tailLogfileProcess != NULL)  // see if he's already opened and started another "tail -f" process (in this modeless dlg)
	{
		delete m_tailLogfileProcess;
		m_tailLogfileProcess = NULL;
	}
	m_tailLogfile_Stdout.clear();
	m_tailLogfile_Stderr.clear();

	QString	tailLogfileCommand;

	if (m_logViewerCommand.endsWith("\"")) // if ends with " char
	{
		m_logViewerCommand.chop(1); // remove last char (the " char)
	//	QString	tailLogfileCommand	= QString ("%1 \"%2\"  | grep -i PBS")
		if (bStart)
			tailLogfileCommand	= QString ("%1 '%2' -f\"")
					.arg( m_logViewerCommand )
					.arg(logfilename);
		else
			tailLogfileCommand	= QString ("%1 '%2'\"")
					.arg( m_logViewerCommand )
					.arg(logfilename);
	}
	else
	{
		if (bStart)
			tailLogfileCommand	= QString ("%1 %2 -f")
					.arg( m_logViewerCommand )
					.arg(logfilename);
		else
			tailLogfileCommand	= QString ("%1 %2")
					.arg( m_logViewerCommand )
					.arg(logfilename);

	}

//	m_mainWindow->statusBar()->showMessage(QString("%1").arg(tailLogfileCommand), 10000);


	m_tailLogfileProcess = new QProcess(this);
	connect (m_tailLogfileProcess, SIGNAL(readyReadStandardOutput()),
			 this, SLOT(tailLogfile_getStdout())); // connect process signals with my code
	connect (m_tailLogfileProcess, SIGNAL(readyReadStandardError()),
			 this, SLOT(tailLogfile_getStderr())); // same here
	m_tailLogfileProcess->start(tailLogfileCommand);

	if (!m_tailLogfileProcess->waitForStarted())
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Couldn't tail logfile", "'tailLogfile' process unable to start.  Torque is probably not running.\n");
		return false;
	}

//	if (!m_tailLogfileProcess->waitForFinished(-1)) //If msecs is -1, this function will not time out
//	{
//		QApplication::restoreOverrideCursor();
//		QMessageBox::critical(0, "Error trying to tail Logfile", "Process's waitForFinished() method returned error.\n");
//		return false;
//	}

//	delete m_tailLogfileProcess;
//	m_tailLogfileProcess = NULL;


//  tailLogfile_processStdout(); // parse the stdout data collected (above)


//	// see if there's any errors to display
//	if (!m_addNote_Stderr.isEmpty())
//	{
//		QMessageBox::critical(0, "Error issuing Add Note command",
//			  QString("Error issuing 'add Note' command.  Error message was: %1").arg(m_addNote_Stderr));
//		return false;
//	}


    return true; // success

}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_tailLogfile() - this gets called whenever the tailLogfile process has something to say...
void LogViewerDlg::tailLogfile_getStdout()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	QByteArray s = m_tailLogfileProcess->readAllStandardOutput(); // read normal output
//	m_tailLogfile_Stdout.append( stdout );  // if there's any stdout
	ui->plainTextEdit_LogView->appendPlainText ( s );

}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_tailLogfile() - this gets called whenever the tail logfile process has something to say...
void LogViewerDlg::tailLogfile_getStderr()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	QByteArray s = m_tailLogfileProcess->readAllStandardError(); // read error channel
	m_tailLogfile_Stderr.append( s );  // if there's any stderr
}


/*******************************************************************************
 *
*******************************************************************************/
void LogViewerDlg::tailLogfile_processStdout() // parse the stdout data collected (above)
{
//	QMessageBox::information(0, "Starting MOM", "MOM started successfully.");
    ui->plainTextEdit_LogView->appendPlainText ( m_tailLogfile_Stdout );
}

/*******************************************************************************
 *
*******************************************************************************/
void LogViewerDlg::on_btnGrep_clicked()
{
    QString logfilename  = ui->lineEdit_LogFilename->text();
	issueCmd_grep( logfilename );
}

/*******************************************************************************
 *
*******************************************************************************/
bool LogViewerDlg::issueCmd_grep(QString logfilename)
{
	stopTailLogfile();  // make sure "tail -f" command has been stopped

	m_grep_Stdout.clear();
	m_grep_Stderr.clear();

//	bool bMatchCase = ui->checkBox_MatchCase->isChecked();
//	bool bWholeWord = ui->checkBox_WholeWord->isChecked();
//	QString options;
//	if (! bMatchCase)
//		options += "-i ";
//	if (bWholeWord)
//		options += "-w ";
//	options += "-n "; // show line number

	QString sOptions = ui->comboBox_GrepOptions->currentText();

	QString s = m_mainWindow->m_Config_Cmd_Grep;
	QString grepPattern = ui->comboBox_Find->currentText();
	QString grepCommand = QString ("%1 %2 \"%3\" %4")
		.arg(m_mainWindow->m_Config_Cmd_Grep)
		.arg(sOptions)
		.arg(grepPattern)
		.arg(logfilename);

	m_mainWindow->statusBar()->showMessage(QString("%1").arg(grepCommand), 10000);

	// show "Wait cursor"
	QApplication::setOverrideCursor(Qt::WaitCursor);


	m_grepProcess = new QProcess(this);
	connect (m_grepProcess, SIGNAL(readyReadStandardOutput()),
			 this, SLOT(grep_getStdout())); // connect process signals with my code
	connect (m_grepProcess, SIGNAL(readyReadStandardError()),
			 this, SLOT(grep_getStderr())); // same here
	m_grepProcess->start(grepCommand);

	if (!m_grepProcess->waitForStarted())
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Couldn't run grep process", "'grep' process unable to start.");
		return false;
	}

	if (!m_grepProcess->waitForFinished(-1)) //If msecs is -1, this function will not time out
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, "Error trying to run grep process", "Process's waitForFinished() method returned error.\n");
		return false;
	}

	delete m_grepProcess;
	m_grepProcess = NULL;

	grep_processStdout(); // parse the stdout data collected (above)

	// restore the original cursor
	QApplication::restoreOverrideCursor();


    // see if there's any errors to display
    if (!m_grep_Stderr.isEmpty())
    {
//		QMessageBox::critical(0, "Error issuing grep command",
//							  QString("Error issuing 'grep' command.  Error message was: %1").arg(m_grep_Stderr));
        ErrorMsgDlg dlg("TORQUEView Error", "Error issuing \"grep\" command. Error message:", m_grep_Stderr, false);
        dlg.exec();
        return false;
	}

	// now highlight all occurances of matched string
	on_btnFindAll_clicked();

	return true; // success
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_grep() - this gets called whenever the grep process has something to say...
void LogViewerDlg::grep_getStdout()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	QByteArray s = m_grepProcess->readAllStandardOutput(); // read normal output
	m_grep_Stdout.append( s );  // if there's any stdout
}

/*******************************************************************************
 *
*******************************************************************************/
// called from issueCmd_grep() - this gets called whenever the grep process has something to say...
void LogViewerDlg::grep_getStderr()
{
	// this method is called asynchronously whenever more data arrives and needs to be processed
	QByteArray s = m_grepProcess->readAllStandardError(); // read error channel
	m_grep_Stderr.append( s );  // if there's any stderr
}


/*******************************************************************************
 *
*******************************************************************************/
void LogViewerDlg::grep_processStdout() // parse the stdout data collected (above)
{
//  QMessageBox::information(0, "Grep Find results:", m_grep_Stdout);
	ui->plainTextEdit_LogView->setPlainText( m_grep_Stdout );
}


/*******************************************************************************
 *
*******************************************************************************/
void LogViewerDlg::on_spinBox_MaxLineCount_valueChanged(int val)
{
    ui->plainTextEdit_LogView->setMaximumBlockCount( val );
}

/*******************************************************************************
 *
*******************************************************************************/
void LogViewerDlg::on_checkBox_WrapLines_toggled(bool checked)
{
	if (checked)
	{
		// wrap words at the right edge of the text edit
		ui->plainTextEdit_LogView->setLineWrapMode ( QPlainTextEdit::WidgetWidth );
	}
	else
	{
		ui->plainTextEdit_LogView->setLineWrapMode( QPlainTextEdit::NoWrap );
	}

	// save off settings
	QSettings mysettings( m_mainWindow->SETTINGS_ADAPTIVE_COMPUTING, m_mainWindow->SETTINGS_TORQUEVIEW );
	// save off the wrapLines option settings -------------
	bool bWrapLines = false;
	if (checked)
		bWrapLines = true;
	else
		bWrapLines = false;
	mysettings.setValue( "WrapLines", bWrapLines );

}


/*******************************************************************************
 *
*******************************************************************************/
void LogViewerDlg::on_checkBox_UseGrep_toggled(bool checked)
{
	if (checked)
	{
		// hide all the find controls
		ui->label_Find->hide();
		ui->checkBox_MatchCase->hide();
		ui->checkBox_WholeWord->hide();
		ui->btnFindNext->hide();
		ui->btnFindAll->hide();

		// show all the grep controls
		ui->label_GrepOptions->show();
		ui->comboBox_GrepOptions->show();
		ui->label_GrepPattern->show();
		ui->btnGrep->show();

		ui->btnGrep->setFocus();
	}
	else
	{
		// show all the find controls
		ui->label_Find->show();
		ui->checkBox_MatchCase->show();
		ui->checkBox_WholeWord->show();
		ui->btnFindNext->show();
		ui->btnFindAll->show();

		// hide all the grep controls
		ui->label_GrepOptions->hide();
		ui->comboBox_GrepOptions->hide();
		ui->label_GrepPattern->hide();
		ui->btnGrep->hide();

		ui->btnFindNext->setFocus();
	}

//	// save off settings
//	QSettings mysettings( "AdaptiveComputing", "TORQUEView" );
//	// save off the wrapLines option settings -------------
//	bool bWrapLines = false;
//	if (checked)
//		bWrapLines = true;
//	else
//		bWrapLines = false;
//	mysettings.setValue( "WrapLines", bWrapLines );

}

/*******************************************************************************
 *
*******************************************************************************/
void LogViewerDlg::on_btnFindNext_clicked()
{
	QString srchText = ui->comboBox_Find->currentText();
	bool bMatchCase = ui->checkBox_MatchCase->isChecked();
	bool bWholeWord = ui->checkBox_WholeWord->isChecked();

	//Set Cursor position to the start of document
	//to let find() method make search in whole text,
	//otherwise it will make search from cursor position to the end.
	//ui->textEdit->moveCursor(QTextCursor::Start);
	QTextDocument::FindFlags flags = (QTextDocument::FindFlag)0x00000;
	if (bMatchCase)
		flags |= QTextDocument::FindCaseSensitively;
	if (bWholeWord)
		flags |= QTextDocument::FindWholeWords;

	if (ui->plainTextEdit_LogView->find(srchText, flags) == false)
	{
		// if couldn't find it, restart at beginning of document
		ui->plainTextEdit_LogView->moveCursor(QTextCursor::Start);
		if (ui->plainTextEdit_LogView->find(srchText, flags) == false)
		{
			QMessageBox::information(NULL, "String not found", QString("Couldn't find '%1'").arg(srchText));
			return;
		}
	}

	// highlight the text found in the search (red text, yellow background)
	QPalette p = ui->plainTextEdit_LogView->palette();
	QColor colorBackground = QColor(Qt::yellow).lighter(130);
	QColor colorForeground = QColor(Qt::red).lighter(130);
	p.setColor(QPalette::Highlight, colorBackground);
	p.setColor(QPalette::HighlightedText, colorForeground);
	ui->plainTextEdit_LogView->setPalette(p);
}

/*******************************************************************************
 *
*******************************************************************************/
void LogViewerDlg::on_btnFindAll_clicked(/*QString& str, bool matchCase */)
{
	QList<QTextEdit::ExtraSelection> extraSelections;
	QString srchText = ui->comboBox_Find->currentText();
	bool bMatchCase = ui->checkBox_MatchCase->isChecked();
	bool bWholeWord = ui->checkBox_WholeWord->isChecked();

	QTextDocument::FindFlags flags = (QTextDocument::FindFlag)0x00000;
	if (bMatchCase)
		flags |= QTextDocument::FindCaseSensitively;
	if (bWholeWord)
		flags |= QTextDocument::FindWholeWords;


	ui->plainTextEdit_LogView->moveCursor(QTextCursor::Start);
	QColor color = QColor(Qt::yellow).lighter(130);

	while(ui->plainTextEdit_LogView->find(srchText, flags))
	{
		QTextEdit::ExtraSelection extra;
		//extra.format.setUnderlineStyle(QTextCharFormat::SingleUnderline);
		//extra.format.setUnderlineColor(QColor(200,200,200));
		extra.format.setBackground(color);
		//extra.format.setBackground(QBrush(QColor(230,230,230)));

		extra.cursor = ui->plainTextEdit_LogView->textCursor();
		extraSelections.append(extra);
	}

	if (extraSelections.count() == 0)
	{
		QMessageBox::information(NULL, "String not found", QString("Couldn't find '%1'").arg(srchText));
		// clear all selections
		QList<QTextEdit::ExtraSelection> extraSelections;
		ui->plainTextEdit_LogView->setExtraSelections(extraSelections);	// clear extra selections
		return;
	}

	ui->plainTextEdit_LogView->setExtraSelections(extraSelections);

	// now go back to top of document and highlight the first one
	//ui->textEdit->moveCursor(QTextCursor::Start);
	//on_btnFindNext_clicked();

}



