/****************************************************************************
**
** Copyright (C) 2008-2010 Andrey Rijov <ANDron142@yandex.ru>
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

#include <QDir>
#include <QFileDialog>
#include <QHeaderView>

#include "Utils.h"
#include "System_Info.h"
#include "QEMU_Binary_Window.h"

QEMU_Binary_Window::QEMU_Binary_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
	
	ui.Table_QEMU_Systems->resizeColumnsToContents();
	ui.Table_QEMU_Systems->resizeRowsToContents();
	
	QHeaderView *hv = new QHeaderView( Qt::Vertical, ui.Table_QEMU_Systems );
	hv->setResizeMode( QHeaderView::Fixed );
	ui.Table_QEMU_Systems->setVerticalHeader( hv );
	
	hv = new QHeaderView( Qt::Horizontal, ui.Table_QEMU_Systems );
	hv->setResizeMode( QHeaderView::Stretch );
	ui.Table_QEMU_Systems->setHorizontalHeader( hv );
}

void QEMU_Binary_Window::on_Button_Find_clicked()
{
	QStringList list;
	
	if( ui.Edit_Path_to_Dir->text().isEmpty() )
	{
		AQGraphic_Warning( tr("Error!"), tr("Path is Empty!") );
		return;
	}
	
	QDir dir = QDir( ui.Edit_Path_to_Dir->text() );
	
	if( ! dir.exists() )
	{
		AQGraphic_Warning( tr("Warning"), tr("Path is Not Exists!") );
		return;
	}
	
	if( ui.RB_QEMU->isChecked() ) list = System_Info::Find_QEMU_Binary_Files( ui.Edit_Path_to_Dir->text() );
	else list = System_Info::Find_KVM_Binary_Files( ui.Edit_Path_to_Dir->text() );
	
	// Clear
	ui.Table_QEMU_Systems->clearContents();
	while( ui.Table_QEMU_Systems->rowCount() > 0 ) ui.Table_QEMU_Systems->removeRow( 0 );
	
	QStringList bin_names_list;
	Emulator tmp_emul;
	
	if( ui.RB_QEMU->isChecked() ) tmp_emul.Set_Type( "QEMU" );
	else tmp_emul.Set_Type( "KVM" );
	
	bin_names_list = tmp_emul.Get_Binary_Files().keys();
	
	for( int ix = 0; ix < list.count(); ix++ )
	{
		ui.Table_QEMU_Systems->insertRow( ui.Table_QEMU_Systems->rowCount() );
		
		QTableWidgetItem *newItem;
		
		newItem = new QTableWidgetItem( bin_names_list[ix] );
		
		ui.Table_QEMU_Systems->setItem( ui.Table_QEMU_Systems->rowCount()-1, 0, newItem );
		
		if( ! list[ix].isEmpty() )
		{
			if( ui.Edit_Path_to_Dir->text().endsWith("/") || ui.Edit_Path_to_Dir->text().endsWith("\\") )
			{
				newItem = new QTableWidgetItem( ui.Edit_Path_to_Dir->text() + list[ix] );
			}
			else
			{
				#ifdef Q_OS_WIN32
				newItem = new QTableWidgetItem( ui.Edit_Path_to_Dir->text() + "\\" + list[ix] );
				#else
				newItem = new QTableWidgetItem( ui.Edit_Path_to_Dir->text() + "/" + list[ix] );
				#endif
			}
		}
		else
		{
			newItem = new QTableWidgetItem( "" );
		}
		
		ui.Table_QEMU_Systems->setItem( ui.Table_QEMU_Systems->rowCount()-1, 1, newItem );
	}
}

void QEMU_Binary_Window::on_TB_Browse_clicked()
{
	QString dir = QFileDialog::getExistingDirectory( this, tr("Select Directory"),
													 Get_Last_Dir_Path(ui.Edit_Path_to_Dir->text()),
													 QFileDialog::ShowDirsOnly );
	
	if( ! dir.isEmpty() )
	{
		ui.Edit_Path_to_Dir->setText( dir );
	}
}

void QEMU_Binary_Window::on_Button_OK_clicked()
{
	// Find all Emulators Names
	QStringList all_emulators_names;
	int emul_count = Settings.value( "Emulators_Count", "0" ).toString().toInt();
	
	for( int ix = 0; ix < emul_count; ++ix )
	{
		all_emulators_names << Settings.value( "Emulator_" + QString::number(ix) + "Name", "").toString();
	}
	
	if( ui.Edit_Name->text().isEmpty() )
	{
		AQGraphic_Warning( tr("Error!"), tr("This Enulator Name is Empty!") );
		return;
	}
	
	for( int ix = 0; ix < all_emulators_names.count(); ix++ )
	{
		if( ui.Edit_Name->text() == all_emulators_names[ix] )
		{
			AQGraphic_Warning( tr("Error!"), tr("This Enulator Name is Already Used!") );
			return;
		}
	}
	
	// Name
	Current_Emulator.Set_Name( ui.Edit_Name->text() );
	
	// Emulator Type
	if( ui.RB_QEMU->isChecked() )
	{
		Current_Emulator.Set_Type( "QEMU" );
	}
	else
	{
		Current_Emulator.Set_Type( "KVM" );
	}
	
	// Path
	Current_Emulator.Set_Path( ui.Edit_Path_to_Dir->text() );
	
	// Save Binary List
	QMap<QString, QString> bin_files;
	for( int ix = 0; ix < ui.Table_QEMU_Systems->rowCount(); ix++ )
	{
		bin_files[ ui.Table_QEMU_Systems->item(ix, 0)->text() ] = ui.Table_QEMU_Systems->item(ix, 1)->text();
	}
	
	Current_Emulator.Set_Binary_Files( bin_files );
	
	// Check Version Always Start AQEMU
	if( ui.RB_Check_QEMU_Version->isChecked() )
	{
		Current_Emulator.Set_Check_QEMU_Version( "yes" );
		
		// Get New QEMU Version
		if( ui.RB_QEMU->isChecked() )
		{
			Current_Emulator.Set_QEMU_Version( System_Info::Get_QEMU_Version(ui.Table_QEMU_Systems->item(0, 1)->text()) );
		}
	}
	else
	{
		Current_Emulator.Set_Check_QEMU_Version( "no" );
		
		//  Set QEMU Version
		switch( ui.CB_QEMU_Version->currentIndex() )
		{
			case 0:
				Current_Emulator.Set_QEMU_Version( "0.9.0" );
				break;
			
			case 1:
				Current_Emulator.Set_QEMU_Version( "0.9.1" );
				break;
			
			case 2:
				Current_Emulator.Set_QEMU_Version( "0.10" );
				break;
				
			case 3:
				Current_Emulator.Set_QEMU_Version( "New" );
				break;
			
			default:
				Current_Emulator.Set_QEMU_Version( "0.9.0" );
				break;
		}
	}
	
	// Check Version KVM Always Start AQEMU
	if( ui.RB_Check_KVM_Version->isChecked() )
	{
		Current_Emulator.Set_Check_KVM_Version( "yes" );
		
		// Get New KVM Version
		if( ui.RB_KVM->isChecked() )
		{
			Current_Emulator.Set_KVM_Version( System_Info::Get_KVM_Version(ui.Table_QEMU_Systems->item(0, 1)->text()) );
		}
	}
	else
	{
		Current_Emulator.Set_Check_KVM_Version( "no" );
		
		//  Set KVM Version Manual
		if( ui.RB_Force_KVM_Version->isChecked() )
		{
			switch( ui.CB_KVM_Version->currentIndex() )
			{
				case 0:
					Current_Emulator.Set_KVM_Version( "7X" );
					break;
				
				case 1:
					Current_Emulator.Set_KVM_Version( "8X" );
					break;
					
				case 2:
					Current_Emulator.Set_KVM_Version( "New" );
					break;
					
				default:
					Current_Emulator.Set_KVM_Version( "7X" );
					break;
			}
		}
	}
	
	// Check Available Audio Cards
	if( ui.CH_Check_Available_Audio_Cards->isChecked() )
	{
		Current_Emulator.Set_Check_Available_Audio_Cards( "yes" );
	}
	else
	{
		Current_Emulator.Set_Check_Available_Audio_Cards( "no" );
	}
	
	accept();
}

Emulator QEMU_Binary_Window::Get_Emulator() const
{
	return Current_Emulator;
}

void QEMU_Binary_Window::Set_Emulator( Emulator emul )
{
	Current_Emulator = emul;
	
	// Name
	ui.Edit_Name->setText( Current_Emulator.Get_Name() );
	
	// Emulator Type
	if( Current_Emulator.Get_Type() == "QEMU" )
	{
		ui.RB_QEMU->setChecked( true );
		ui.Widget_QEMU->setEnabled( true );
		ui.RB_KVM->setChecked( false );
		ui.Widget_KVM->setEnabled( false );
	}
	else if( Current_Emulator.Get_Type() == "KVM" )
	{
		ui.RB_QEMU->setChecked( false );
		ui.Widget_QEMU->setEnabled( false );
		ui.RB_KVM->setChecked( true );
		ui.Widget_KVM->setEnabled( true );
	}
	else
	{
		ui.RB_QEMU->setChecked( true );
		ui.Widget_QEMU->setEnabled( true );
		ui.RB_KVM->setChecked( false );
		ui.Widget_KVM->setEnabled( false );
	}
	
	// Path
	ui.Edit_Path_to_Dir->setText( Current_Emulator.Get_Path() );
	
	// Check Version on Start AQEMU
	if( Current_Emulator.Get_Check_QEMU_Version() == "yes" ) ui.RB_Force_QEMU_Version->setChecked( false );
	else if( Current_Emulator.Get_Check_QEMU_Version() == "no" ) ui.RB_Force_QEMU_Version->setChecked( true );
	else ui.RB_Force_QEMU_Version->setChecked( false );
	
	// Force QEMU Version
	QString qemu_verion = Current_Emulator.Get_QEMU_Version();
	
	if( qemu_verion == "0.9.0" )
	{
		ui.CB_QEMU_Version->setCurrentIndex( 0 );
	}
	else if( qemu_verion == "0.9.1" )
	{
		ui.CB_QEMU_Version->setCurrentIndex( 1 );
	}
	else if( qemu_verion == "0.10" )
	{
		ui.CB_QEMU_Version->setCurrentIndex( 2 );
	}
	else if( qemu_verion == "New" )
	{
		ui.CB_QEMU_Version->setCurrentIndex( 3 );
	}
	else
	{
		AQWarning( "void QEMU_Binary_Window::Set_Emulator( Emulator emul )",
				   "QEMU Version NOT Valid! Use Default: 0.9.0" );
		ui.CB_QEMU_Version->setCurrentIndex( 0 );
	}
	
	// Check Availability KVM Always Start AQEMU
	if( Current_Emulator.Get_Check_KVM_Version() == "yes" ) ui.RB_Force_KVM_Version->setChecked( false );
	else if( Current_Emulator.Get_Check_KVM_Version() == "no" ) ui.RB_Force_KVM_Version->setChecked( true );
	else ui.RB_Force_KVM_Version->setChecked( false );
	
	QString kvm_verion = Current_Emulator.Get_KVM_Version();
	
	if( kvm_verion == "7X" )
	{
		ui.CB_KVM_Version->setCurrentIndex( 0 );
	}
	else if( kvm_verion == "8X" || kvm_verion == "New" )
	{
		ui.CB_KVM_Version->setCurrentIndex( 1 );
	}
	else
	{
		AQWarning( "void QEMU_Binary_Window::Set_Emulator( Emulator emul )",
				   "KVM Version NOT Valid! Use Default: 7X" );
		ui.CB_KVM_Version->setCurrentIndex( 0 );
	}
	
	// Check Available Audio Cards
	ui.CH_Check_Available_Audio_Cards->setChecked( Current_Emulator.Get_Check_Available_Audio_Cards() == "yes" );
	
	// Emulator Binary Files
	QMap<QString, QString> bin_files = Current_Emulator.Get_Binary_Files();
	
	// Clear
	if( Current_Emulator.Get_Name().isEmpty() && Current_Emulator.Get_Type().isEmpty() ) return;
	
	ui.Table_QEMU_Systems->clearContents();
	while( ui.Table_QEMU_Systems->rowCount() > 0 ) ui.Table_QEMU_Systems->removeRow( 0 );
	
	for( QMap<QString, QString>::const_iterator iter = bin_files.constBegin(); iter != bin_files.constEnd(); iter++ )
	{
		ui.Table_QEMU_Systems->insertRow( ui.Table_QEMU_Systems->rowCount() );
		
		QTableWidgetItem *newItem;
		
		if( ui.RB_QEMU->isChecked() )
		{
			newItem = new QTableWidgetItem( iter.key() );
			ui.Table_QEMU_Systems->setItem( ui.Table_QEMU_Systems->rowCount()-1, 0, newItem );
		}
		else
		{
			newItem = new QTableWidgetItem( iter.key() );
			ui.Table_QEMU_Systems->setItem( ui.Table_QEMU_Systems->rowCount()-1, 0, newItem );
		}
		
		newItem = new QTableWidgetItem( iter.value() );
		ui.Table_QEMU_Systems->setItem( ui.Table_QEMU_Systems->rowCount()-1, 1, newItem );
	}
}

void QEMU_Binary_Window::on_Table_QEMU_Systems_itemDoubleClicked( QTableWidgetItem *item )
{
	QFileDialog::Options options;
	QString selectedFilter;
	
	QString binName = QFileDialog::getOpenFileName( this, tr("Select QEMU Binary File"), "/usr/",
													tr("All Files (*)"), &selectedFilter, options );
	
	if( ! (binName.isNull() || binName.isEmpty()) )
	{
		if( ui.Table_QEMU_Systems->currentRow() < 0 ) return;
		
		ui.Table_QEMU_Systems->item( ui.Table_QEMU_Systems->currentRow(), 1 )->setText( binName );
	}
}

void QEMU_Binary_Window::on_RB_QEMU_toggled( bool checked )
{
	// Clear
	ui.Table_QEMU_Systems->clearContents();
	while( ui.Table_QEMU_Systems->rowCount() > 0 ) ui.Table_QEMU_Systems->removeRow( 0 );
	
	Emulator tmp_emul;
	tmp_emul.Set_Type( "QEMU" );
	
	QMap<QString, QString> bin_files = tmp_emul.Get_Binary_Files();
	
	for( QMap<QString, QString>::const_iterator iter = bin_files.constBegin(); iter != bin_files.constEnd(); iter++ )
	{
		ui.Table_QEMU_Systems->insertRow( ui.Table_QEMU_Systems->rowCount() );
		QTableWidgetItem *newItem = new QTableWidgetItem( iter.key() );
		ui.Table_QEMU_Systems->setItem( ui.Table_QEMU_Systems->rowCount()-1, 0, newItem );
	}
}

void QEMU_Binary_Window::on_RB_KVM_toggled( bool checked )
{
	// Clear
	ui.Table_QEMU_Systems->clearContents();
	while( ui.Table_QEMU_Systems->rowCount() > 0 ) ui.Table_QEMU_Systems->removeRow( 0 );
	
	Emulator tmp_emul;
	tmp_emul.Set_Type( "KVM" );
	
	QMap<QString, QString> bin_files = tmp_emul.Get_Binary_Files();
	
	for( QMap<QString, QString>::const_iterator iter = bin_files.constBegin(); iter != bin_files.constEnd(); iter++ )
	{
		ui.Table_QEMU_Systems->insertRow( ui.Table_QEMU_Systems->rowCount() );
		QTableWidgetItem *newItem = new QTableWidgetItem( iter.key() );
		ui.Table_QEMU_Systems->setItem( ui.Table_QEMU_Systems->rowCount()-1, 0, newItem );
	}
}
