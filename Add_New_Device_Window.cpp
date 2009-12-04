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

#include "Utils.h"
#include "Add_New_Device_Window.h"

Add_New_Device_Window::Add_New_Device_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
	
	// Minimum Size
	resize( width(), minimumSizeHint().height() );
}

VM_Storage_Device Add_New_Device_Window::Get_Device() const
{
	return Device;
}

void Add_New_Device_Window::Set_Device( const VM_Storage_Device &dev )
{
	Device = dev;
	
	// Update View...
	
	ui.CH_Interface->setChecked( Device.Use_Interface() );
	
	// Interface
	switch( Device.Get_Interface() )
	{
		case VM::DI_IDE:
			ui.CB_Interface->setCurrentIndex( 0 );
			break;
			
		case VM::DI_SCSI:
			ui.CB_Interface->setCurrentIndex( 1 );
			break;
			
		case VM::DI_SD:
			ui.CB_Interface->setCurrentIndex( 2 );
			break;
			
		case VM::DI_MTD:
			ui.CB_Interface->setCurrentIndex( 3 );
			break;
			
		case VM::DI_Floppy:
			ui.CB_Interface->setCurrentIndex( 4 );
			break;
			
		case VM::DI_PFlash:
			ui.CB_Interface->setCurrentIndex( 5 );
			break;
			
		default:
			AQError( "void Add_New_Device_Window::Set_Device( const VM_Storage_Device &dev )",
					 "Interface Default Section! Use IDE!" );
			break;
	}
	
	// Media
	ui.CH_Media->setChecked( Device.Use_Media() );
	
	switch( Device.Get_Media() )
	{
		case VM::DM_Disk:
			ui.CB_Media->setCurrentIndex( 0 );
			break;
			
		case VM::DM_CD_ROM:
			ui.CB_Media->setCurrentIndex( 1 );
			break;
			
		default:
			AQError( "void Add_New_Device_Window::Set_Device( const VM_Storage_Device &dev )",
					 "Media Default Section! Use Disk!" );
			break;
	}
	
	// File Path
	ui.CH_File->setChecked( Device.Use_File_Path() );
	ui.Edit_File_Path->setText( Device.Get_File_Path() );
	
	// Index
	ui.CH_Index->setChecked( Device.Use_Index() );
	ui.SB_Index->setValue( Device.Get_Index() );
	
	// Bus, Unit
	ui.CH_Bus_Unit->setChecked( Device.Use_Bus_Unit() );
	ui.SB_Bus->setValue( Device.Get_Bus() );
	ui.SB_Unit->setValue( Device.Get_Unit() );
	
	// Snapshot
	ui.CH_Snapshot->setChecked( Device.Get_Snapshot() );
	
	// Cache
	ui.CH_Cache->setChecked( Device.Get_Cache() );
	
	// cyls, heads, secs, trans
	ui.GB_hdachs_Settings->setChecked( Device.Use_hdachs() );
	ui.Edit_Cyls->setText( QString::number(Device.Get_Cyls()) );
	ui.Edit_Heads->setText( QString::number(Device.Get_Heads()) );
	ui.Edit_Secs->setText( QString::number(Device.Get_Secs()) );
	ui.Edit_Trans->setText( QString::number(Device.Get_Trans()) );
}

void Add_New_Device_Window::on_CB_Interface_currentIndexChanged( const QString &text )
{
	if( text == "ide" || text == "floppy" )
	{
		ui.CH_Index->setEnabled( true );
		ui.SB_Index->setEnabled( true );
		
		ui.CH_Bus_Unit->setEnabled( false );
		ui.SB_Bus->setEnabled( false );
		ui.SB_Unit->setEnabled( false );
	}
	else if( text == "scsi" )
	{
		ui.CH_Index->setEnabled( false );
		ui.SB_Index->setEnabled( false );
		
		ui.CH_Bus_Unit->setEnabled( true );
		ui.SB_Bus->setEnabled( true );
		ui.SB_Unit->setEnabled( true );
	}
	else
	{
		ui.CH_Index->setEnabled( false );
		ui.SB_Index->setEnabled( false );
		
		ui.CH_Bus_Unit->setEnabled( false );
		ui.SB_Bus->setEnabled( false );
		ui.SB_Unit->setEnabled( false );
	}
}

void Add_New_Device_Window::on_TB_File_Path_Browse_clicked()
{
	QFileDialog::Options options;
	QString selectedFilter;
	
	QString file_name = QFileDialog::getOpenFileName( this, tr("Select You Device"), "/", tr("All Files (*)"),
													  &selectedFilter, options );
	
	if( ! (file_name.isNull() || file_name.isEmpty()) )
	{
		ui.Edit_File_Path->setText( file_name );
	}
}

void Add_New_Device_Window::on_Button_OK_clicked()
{
	// Interface
	switch( ui.CB_Interface->currentIndex() )
	{
		case 0:
			Device.Set_Interface( VM::DI_IDE );
			break;
			
		case 1:
			Device.Set_Interface( VM::DI_SCSI );
			break;
			
		case 2:
			Device.Set_Interface( VM::DI_SD );
			break;
			
		case 3:
			Device.Set_Interface( VM::DI_MTD );
			break;
			
		case 4:
			Device.Set_Interface( VM::DI_Floppy );
			break;
			
		case 5:
			Device.Set_Interface( VM::DI_PFlash );
			break;
			
		default:
			AQError( "void Add_New_Device_Window::on_Button_OK_clicked()",
					 "Invalid Interface Index! Use IDE" );
			Device.Set_Interface( VM::DI_IDE );
			break;
	}
	
	Device.Use_Interface( ui.CH_Interface->isChecked() );
	
	// Media
	switch( ui.CB_Media->currentIndex() )
	{
		case 0:
			Device.Set_Media( VM::DM_Disk );
			break;
			
		case 1:
			Device.Set_Media( VM::DM_CD_ROM );
			break;
			
		default:
			AQError( "void Add_New_Device_Window::on_Button_OK_clicked()",
					 "Invalid Media Index! Use Disk" );
			Device.Set_Media( VM::DM_Disk );
			break;
	}
	
	Device.Use_Media( ui.CH_Media->isChecked() );
	
	// File Path
	if( ui.CH_File->isChecked() )
	{
		if( ! QFile::exists(ui.Edit_File_Path->text()) )
		{
			AQGraphic_Warning( tr("Error!"),
							   tr("File Not Exists!") );
			return;
		}
	}
	
	Device.Use_File_Path( ui.CH_File->isChecked() );
	Device.Set_File_Path( ui.Edit_File_Path->text() );
	
	// Index
	Device.Use_Index( ui.CH_Index->isChecked() );
	Device.Set_Index( ui.SB_Index->value() );
	
	// Bus, Unit
	Device.Use_Bus_Unit( ui.CH_Bus_Unit->isChecked() );
	Device.Set_Bus( ui.SB_Bus->value() );
	Device.Set_Unit( ui.SB_Unit->value() );
	
	// Snapshot
	Device.Set_Snapshot( ui.CH_Snapshot->isChecked() );
	
	// Cache
	Device.Set_Cache( ui.CH_Cache->isChecked() );
	
	// hdachs
	if( ui.GB_hdachs_Settings->isChecked() )
	{
		bool ok;
		
		qulonglong cyls = ui.Edit_Cyls->text().toULongLong( &ok, 10 );
		
		if( ! ok )
		{
			AQGraphic_Warning( tr("Warning!"), tr("\"Cyls\" Value Incorrect!") );
			return;
		}
		
		qulonglong heads = ui.Edit_Heads->text().toULongLong( &ok, 10 );
		
		if( ! ok )
		{
			AQGraphic_Warning( tr("Warning!"), tr("\"Heads\" Value Incorrect!") );
			return;
		}
		
		qulonglong secs = ui.Edit_Secs->text().toULongLong( &ok, 10) ;
		
		if( ! ok )
		{
			AQGraphic_Warning( tr("Warning!"), tr("\"Secs\" Value Incorrect!") );
			return;
		}
		
		qulonglong trans = ui.Edit_Trans->text().toULongLong( &ok, 10 );
		
		if( ! ok )
		{
			AQGraphic_Warning( tr("Warning!"), tr("\"Trans\" Value Incorrect!") );
			return;
		}
		
		Device.Use_hdachs( ui.GB_hdachs_Settings->isChecked() );
		Device.Set_Cyls( cyls );
		Device.Set_Heads( heads );
		Device.Set_Secs( secs );
		Device.Set_Trans( trans );
	}
	
	accept();
}
