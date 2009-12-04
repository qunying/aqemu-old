/****************************************************************************
**
** Copyright (C) 2008-2009 Andrey Rijov <ANDron142@yandex.ru>
**
** This file is part of AQEMU.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor,
** Boston, MA  02110-1301, USA.
**
****************************************************************************/

#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QRegExp>
#include <QSettings>

#include "Utils.h"
#include "VM_Devices.h"
#include "Convert_HDD_Image_Window.h"

// Convert_HDD_Thread -------------------------------------------------------

Convert_HDD_Thread::Convert_HDD_Thread()
{
}

void Convert_HDD_Thread::Set_Args( const QStringList &args )
{
	Arguments = args;
}

void Convert_HDD_Thread::run()
{
	QProcess *proc = new QProcess();
	
	QSettings settings;
	
	proc->start( Get_Default_QEMU_IMG_Path(), Arguments );
	
	if( ! proc->waitForStarted(5000) )
	{
		AQError( "void Convert_HDD_Thread::run()", "Cannot Start qemu-img!" );
	}
	
	if( ! proc->waitForFinished(-1) )
	{
		AQError( "void Convert_HDD_Thread::run()", "Cannot Finish qemu-img!" );
	}
	
	QString err_str = proc->readAllStandardError();
	
	delete proc;
	
	if( err_str.count() > 0 )
	{
		AQError( "void Convert_HDD_Thread::run()", "qemu-img Send Error String!\nDetalis: " + err_str );
		
		emit Conversion_Complete( false );
	}
	else
	{
		AQDebug( "void Convert_HDD_Thread::run()", "Conversion Complete!" );
		
		emit Conversion_Complete( true );
	}
}

//===========================================================================

Convert_HDD_Image_Window::Convert_HDD_Image_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
	
	resize( width(), minimumSizeHint().height() );
}

void Convert_HDD_Image_Window::on_Button_Browse_Base_clicked()
{
	QFileDialog::Options options;
	QString selectedFilter;
	
	QString fileName = QFileDialog::getOpenFileName( this, tr("Select Base HDD Image File"), QDir::homePath(),
													 tr("All Files (*);;Images Files (*.img *.qcow *.qcow2 *.wmdk)"),
													 &selectedFilter, options );
	
	if( ! (fileName.isNull() || fileName.isEmpty()) )
	{
		ui.Edit_Base_File_Name->setText( fileName );
	}
}

void Convert_HDD_Image_Window::on_Button_Browse_Output_clicked()
{
	QFileDialog::Options options;
	QString selectedFilter;
	
	QString fileName = QFileDialog::getSaveFileName( this, tr("Save Out HDD Image File"), QDir::homePath(),
													 tr("All Files (*);;Images Files (*.img *.qcow *.qcow2 *.wmdk)"),
													 &selectedFilter, options );
	
	if( ! (fileName.isNull() || fileName.isEmpty()) )
	{
		ui.Edit_Output_File_Name->setText( fileName );
	}
}

void Convert_HDD_Image_Window::on_CB_Output_Format_currentIndexChanged( const QString &text )
{
	if( text == "qcow2" || text == "qcow" )
	{
		ui.GB_QCOW_Options->setEnabled( true );
	}
	else
	{
		ui.GB_QCOW_Options->setEnabled( false );
	}
}

void Convert_HDD_Image_Window::on_Button_Convert_clicked()
{
	if( ui.Edit_Output_File_Name->text().isEmpty() )
	{
		AQGraphic_Warning( tr("File Name is Empty!"), tr("Please Enter Correct File Name for You HDD Image!") );
		return;
	}
	
	QStringList args;
	args << "convert";
	
	if( ui.GB_QCOW_Options->isEnabled() )
	{
		if( ui.CH_Compressed->isChecked() ) args << "-c"; // compressed
		if( ui.CH_Encrypted->isChecked() ) args << "-e"; // encrypted
	}
	
	args << "-f" << ui.CB_Input_Format->currentText(); // Input format
	
	args << ui.Edit_Base_File_Name->text(); // In file name
	
	args << "-O" << ui.CB_Output_Format->currentText(); // Output format
	
	args << ui.Edit_Output_File_Name->text(); // Output File name
	
	QObject::connect( &Conv_Thread, SIGNAL(Conversion_Complete(bool)),
					  this, SLOT(Conversion_Done(bool)) );
	
	Conv_Thread.Set_Args( args );
	Conv_Thread.start();
	
	ProgDial = new QProgressDialog( tr("Please Wait. Converting HDD Image..."), tr("Cancel"), 0, 0, this );
	
	QObject::connect( ProgDial, SIGNAL(canceled()),
					  this, SLOT(Cancel_Convertion()) );
	
	ProgDial->setWindowModality( Qt::WindowModal );
	ProgDial->setValue( 0 );
	
	ProgDial->exec();
}

void Convert_HDD_Image_Window::on_Button_Identify_clicked()
{
	if( ! QFile::exists(ui.Edit_Base_File_Name->text()) )
	{
		AQGraphic_Warning( tr("Error!"), tr("Cannot Locate Input File!") );
		return;
	}
	
	VM_HDD *tmp_hdd = new VM_HDD( true, ui.Edit_Base_File_Name->text() );
	
	int format_ix = ui.CB_Input_Format->findText(
			tmp_hdd->Get_Image_Format() );
	
	if( format_ix != -1 )
	{
		ui.CB_Input_Format->setCurrentIndex( format_ix );
	}
	else
	{
		AQError( "void Convert_HDD_Image_Window::on_Button_Identify_clicked()",
				 "Cannot Find Format" );
	}
	
	delete tmp_hdd;
}

void Convert_HDD_Image_Window::Conversion_Done( bool ok )
{
	if( ok )
	{
		ProgDial->accept();
		QMessageBox::information( this, tr("Information:"), tr("Conversion Complete!") );
		accept();
	}
	else
	{
		ProgDial->accept();
		AQGraphic_Warning( tr("Error!"), tr("Cannot Convert Image!") );
	}
}

void Convert_HDD_Image_Window::Cancel_Convertion()
{
	Conv_Thread.terminate();
}
