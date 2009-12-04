/****************************************************************************
**
** Copyright (C) 2009 Andrey Rijov <ANDron142@yandex.ru>
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

#include <QSettings>
#include "Edit_Emulator_Version_Window.h"
#include "Utils.h"

Edit_Emulator_Version_Window::Edit_Emulator_Version_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
	
	Emulators = Get_Emulators_List();
	
	Load_Emulators();
}

void Edit_Emulator_Version_Window::Load_Emulators()
{
	ui.List_Emulators->clear();
	
	for( int ix = 0; ix < Emulators.count(); ++ix )
	{
		if( Emulators[ix].Get_Type() == "QEMU" )
		{
			new QListWidgetItem( QString("QEMU Version: %1 (Located in %2)").arg(Emulators[ix].Get_QEMU_Version()).arg(Emulators[ix].Get_Path()),
								 ui.List_Emulators );
		}
		else if( Emulators[ix].Get_Type() == "KVM" )
		{
			new QListWidgetItem( QString("KVM Version: %1 (Located in %2)").arg(Emulators[ix].Get_KVM_Version()).arg(Emulators[ix].Get_Path()),
								 ui.List_Emulators );
		}
	}
}

void Edit_Emulator_Version_Window::on_Button_OK_clicked()
{
	QSettings settings;
	
	// Delete Old Emulators
	int emul_count = settings.value( "Emulators_Count", "0" ).toString().toInt();
	for( int dx = 0; dx < emul_count; ++dx ) settings.remove( "Emulator_" + QString::number(dx) );
	
	// Save New Emulators
	for( int ix = 0; ix < Emulators.count(); ++ix )
	{
		QString prefix = "Emulator_" + QString::number( ix ) + "/";
		
		// Emulator Type
		settings.setValue( prefix + "Type", Emulators[ix].Get_Type() );
		
		// Name
		settings.setValue( prefix + "Name", Emulators[ix].Get_Name() );
		
		// Default
		settings.setValue( prefix + "Default", Emulators[ix].Get_Default() );
		
		// Path
		settings.setValue( prefix + "Path", Emulators[ix].Get_Path() );
		
		// Check Version on Start AQEMU
		settings.setValue( prefix + "Check_QEMU_Version", Emulators[ix].Get_Check_QEMU_Version() );
		
		// QEMU Version
		settings.setValue( prefix + "QEMU_Version", Emulators[ix].Get_QEMU_Version() );
		
		// Check Version KVM on Start AQEMU
		settings.setValue( prefix + "Check_KVM_Version", Emulators[ix].Get_Check_KVM_Version() );
		
		// KVM Version
		settings.setValue( prefix + "KVM_Version", Emulators[ix].Get_KVM_Version() );
		
		// Check Available Audio Cards
		settings.setValue( prefix + "Check_Available_Audio_Cards", Emulators[ix].Get_Check_Available_Audio_Cards() );
		
		// Binary Files
		QMap<QString, QString> bin_files = Emulators[ix].Get_Binary_Files();
		
		// Save Emulators Binary Files
		for( QMap<QString, QString>::const_iterator iter = bin_files.constBegin(); iter != bin_files.constEnd(); iter++ )
		{
			settings.setValue( prefix + iter.key(), iter.value() );
		}
	}
	
	accept();
}

void Edit_Emulator_Version_Window::on_List_Emulators_currentRowChanged( int currentRow )
{
	disconnect( ui.CB_Versions, SIGNAL(currentIndexChanged(const QString &)),
				this, SLOT(CB_Versions_currentIndexChanged(const QString &)) );
	
	if( currentRow >= 0 && currentRow < ui.List_Emulators->count() )
	{
		int item_index = -1;
		
		// Add Items
		if( Emulators[currentRow].Get_Type() == "QEMU" )
		{
			ui.CB_Versions->clear();
			
			ui.CB_Versions->addItem( "0.9.0" );
			ui.CB_Versions->addItem( "0.9.1" );
			ui.CB_Versions->addItem( "0.10" );
			ui.CB_Versions->addItem( "New" );
			
			item_index = ui.CB_Versions->findText( Emulators[currentRow].Get_QEMU_Version() );
		}
		else if( Emulators[currentRow].Get_Type() == "KVM" )
		{
			ui.CB_Versions->clear();
			
			ui.CB_Versions->addItem( "7X" );
			ui.CB_Versions->addItem( "8X" );
			ui.CB_Versions->addItem( "New" );
			
			item_index = ui.CB_Versions->findText( Emulators[currentRow].Get_KVM_Version() );
		}
		
		// Select Version
		if( item_index < 0 )
		{
			AQError( "void Edit_Emulator_Version_Window::on_List_Emulators_currentRowChanged( int currentRow )",
					 "Cannot Find Version!" );
		}
		else
		{
			ui.CB_Versions->setCurrentIndex( item_index );
		}
	}
	
	connect( ui.CB_Versions, SIGNAL(currentIndexChanged(const QString &)),
			 this, SLOT(CB_Versions_currentIndexChanged(const QString &)) );
}

void Edit_Emulator_Version_Window::CB_Versions_currentIndexChanged( const QString &text )
{
	if( ui.List_Emulators->currentRow() < 0 ) return;
	
	if( Emulators[ui.List_Emulators->currentRow()].Get_Type() == "QEMU" )
	{
		Emulators[ui.List_Emulators->currentRow()].Set_Name( "QEMU " + text );
		Emulators[ui.List_Emulators->currentRow()].Set_QEMU_Version( text );
	}
	else if( Emulators[ui.List_Emulators->currentRow()].Get_Type() == "KVM" )
	{
		Emulators[ui.List_Emulators->currentRow()].Set_Name( "KVM " + text );
		Emulators[ui.List_Emulators->currentRow()].Set_KVM_Version( text );
	}
	
	Load_Emulators();
}
