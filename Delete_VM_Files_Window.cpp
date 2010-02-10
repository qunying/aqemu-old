/****************************************************************************
**
** Copyright (C) 2010 Andrey Rijov <ANDron142@yandex.ru>
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

#include "Delete_VM_Files_Window.h"
#include "Utils.h"
#include <QHeaderView>
#include <QFile>

Delete_VM_Files_Window::Delete_VM_Files_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
	
	QHeaderView *hv = new QHeaderView( Qt::Vertical, ui.Files_List );
	hv->setResizeMode( QHeaderView::Fixed );
	ui.Files_List->setVerticalHeader( hv );
	
	hv = new QHeaderView( Qt::Horizontal, ui.Files_List );
	hv->setStretchLastSection( true );
	hv->setResizeMode( QHeaderView::ResizeToContents );
	ui.Files_List->setHorizontalHeader( hv );
}

Delete_VM_Files_Window::Delete_VM_Files_Window( Virtual_Machine *vm, QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
	
	QHeaderView *hv = new QHeaderView( Qt::Vertical, ui.Files_List );
	hv->setResizeMode( QHeaderView::Fixed );
	ui.Files_List->setVerticalHeader( hv );
	
	hv = new QHeaderView( Qt::Horizontal, ui.Files_List );
	hv->setStretchLastSection( true );
	hv->setResizeMode( QHeaderView::ResizeToContents );
	ui.Files_List->setHorizontalHeader( hv );
	
	Set_VM( vm );
}

void Delete_VM_Files_Window::Set_VM( Virtual_Machine *vm )
{
	if( vm == NULL )
	{
		AQError( "void Delete_VM_Files_Window::Set_VM( Virtual_Machine *vm )",
				 "vm == NULL" );
		return;
	}
	
	Clear_List();
	
	File_List_Item tmp;
	
	// VM File
	if( Path_Valid(vm->Get_VM_XML_File_Path()) )
	{
		tmp.Hard_Drive = false;
		tmp.Name = tr( "AQEMU VM File" );
		tmp.Path = vm->Get_VM_XML_File_Path();
		
		File_List_Items << tmp;
		Add_To_Files_List( tmp );
	}
	
	// Screenshot
	if( Path_Valid(vm->Get_Screenshot_Path()) )
	{
		tmp.Hard_Drive = false;
		tmp.Name = tr( "Screenshot" );
		tmp.Path = vm->Get_Screenshot_Path();
		
		File_List_Items << tmp;
		Add_To_Files_List( tmp );
	}
	
	// FD0
	if( Path_Valid(vm->Get_FD0().Get_File_Name()) )
	{
		tmp.Hard_Drive = false;
		tmp.Name = tr( "Floppy A" );
		tmp.Path = vm->Get_FD0().Get_File_Name();
		
		File_List_Items << tmp;
		Add_To_Files_List( tmp );
	}
	
	// FD1
	if( Path_Valid(vm->Get_FD1().Get_File_Name()) )
	{
		tmp.Hard_Drive = false;
		tmp.Name = tr( "Floppy B" );
		tmp.Path = vm->Get_FD1().Get_File_Name();
		
		File_List_Items << tmp;
		Add_To_Files_List( tmp );
	}
	
	// CD-ROM
	if( Path_Valid(vm->Get_CD_ROM().Get_File_Name()) )
	{
		tmp.Hard_Drive = false;
		tmp.Name = tr( "CD-ROM" );
		tmp.Path = vm->Get_CD_ROM().Get_File_Name();
		
		File_List_Items << tmp;
		Add_To_Files_List( tmp );
	}
	
	// HDA
	if( Path_Valid(vm->Get_HDA().Get_File_Name()) )
	{
		tmp.Hard_Drive = true;
		tmp.Name = tr( "HDA (Primary Master)" );
		tmp.Path = vm->Get_HDA().Get_File_Name();
		
		File_List_Items << tmp;
		Add_To_Files_List( tmp );
	}
	
	// HDB
	if( Path_Valid(vm->Get_HDB().Get_File_Name()) )
	{
		tmp.Hard_Drive = true;
		tmp.Name = tr( "HDB (Primary Slave)" );
		tmp.Path = vm->Get_HDB().Get_File_Name();
		
		File_List_Items << tmp;
		Add_To_Files_List( tmp );
	}
	
	// HDC
	if( Path_Valid(vm->Get_HDC().Get_File_Name()) )
	{
		tmp.Hard_Drive = true;
		tmp.Name = tr( "HDC (Secondary Master)" );
		tmp.Path = vm->Get_HDC().Get_File_Name();
		
		File_List_Items << tmp;
		Add_To_Files_List( tmp );
	}
	
	// HDD
	if( Path_Valid(vm->Get_HDD().Get_File_Name()) )
	{
		tmp.Hard_Drive = true;
		tmp.Name = tr( "HDD (Secondary Slave)" );
		tmp.Path = vm->Get_HDD().Get_File_Name();
		
		File_List_Items << tmp;
		Add_To_Files_List( tmp );
	}
	
	// ROM File
	if( Path_Valid(vm->Get_ROM_File()) )
	{
		tmp.Hard_Drive = false;
		tmp.Name = tr( "ROM File" );
		tmp.Path = vm->Get_ROM_File();
		
		File_List_Items << tmp;
		Add_To_Files_List( tmp );
	}
	
	// MTDBlock File
	if( Path_Valid(vm->Get_MTDBlock_File()) )
	{
		tmp.Hard_Drive = false;
		tmp.Name = tr( "MTDBlock File" );
		tmp.Path = vm->Get_MTDBlock_File();
		
		File_List_Items << tmp;
		Add_To_Files_List( tmp );
	}
	
	// SD Card File
	if( Path_Valid(vm->Get_SecureDigital_File()) )
	{
		tmp.Hard_Drive = false;
		tmp.Name = tr( "SD Card File" );
		tmp.Path = vm->Get_SecureDigital_File();
		
		File_List_Items << tmp;
		Add_To_Files_List( tmp );
	}
	
	// PFlash File
	if( Path_Valid(vm->Get_PFlash_File()) )
	{
		tmp.Hard_Drive = false;
		tmp.Name = tr( "PFlash File" );
		tmp.Path = vm->Get_PFlash_File();
		
		File_List_Items << tmp;
		Add_To_Files_List( tmp );
	}
	
	// bzImage
	if( Path_Valid(vm->Get_bzImage_Path()) )
	{
		tmp.Hard_Drive = false;
		tmp.Name = tr( "Kernel bzImage" );
		tmp.Path = vm->Get_bzImage_Path();
		
		File_List_Items << tmp;
		Add_To_Files_List( tmp );
	}
	
	// Initrd
	if( Path_Valid(vm->Get_Initrd_Path()) )
	{
		tmp.Hard_Drive = false;
		tmp.Name = tr( "Kernel Initrd" );
		tmp.Path = vm->Get_Initrd_Path();
		
		File_List_Items << tmp;
		Add_To_Files_List( tmp );
	}
	
	// Serial Ports
	if( vm->Get_Serial_Ports().count() > 0 )
	{
		QList<VM_Port> tmp_port = vm->Get_Serial_Ports();
		
		for( int ix = 0; ix < tmp_port.count(); ix++ )
		{
			if( tmp_port[ix].Get_Port_Redirection() == VM::PR_file )
			{
				if( Path_Valid(tmp_port[ix].Get_Parametrs_Line()) )
				{
					tmp.Hard_Drive = false;
					tmp.Name = tr( "Serial Port Redirection" );
					tmp.Path = tmp_port[ix].Get_Parametrs_Line();
					
					File_List_Items << tmp;
					Add_To_Files_List( tmp );
				}
			}
		}
	}
	
	// Parallel Ports
	if( vm->Get_Parallel_Ports().count() > 0 )
	{
		QList<VM_Port> tmp_port = vm->Get_Parallel_Ports();
		
		for( int ix = 0; ix < tmp_port.count(); ix++ )
		{
			if( tmp_port[ix].Get_Port_Redirection() == VM::PR_file )
			{
				if( Path_Valid(tmp_port[ix].Get_Parametrs_Line()) )
				{
					tmp.Hard_Drive = false;
					tmp.Name = tr( "Parallel Port Redirection" );
					tmp.Path = tmp_port[ix].Get_Parametrs_Line();
					
					File_List_Items << tmp;
					Add_To_Files_List( tmp );
				}
			}
		}
	}
	
	/*
	QList<VM_Nativ_Storage_Device> Storage_Devices;

	QList<VM_Net_Card> Network_Cards;
	QList<VM_Net_Card_Nativ> Network_Cards_Nativ;
	
	// 
	if( Path_Valid(vm->) )
	{
		tmp.Hard_Drive = false;
		tmp.Name = tr( "" );
		tmp.Path = vm->;
		
		File_List_Items << tmp;
		Add_To_Files_List( tmp );
	}
	
	// 
	if( Path_Valid(vm->) )
	{
		tmp.Hard_Drive = false;
		tmp.Name = tr( "" );
		tmp.Path = vm->;
		
		File_List_Items << tmp;
		Add_To_Files_List( tmp );
	}*/
}

void Delete_VM_Files_Window::on_Button_Delete_clicked()
{
}

void Delete_VM_Files_Window::on_RB_Show_HDD_toggled( bool checked )
{
	static bool old = false;
	
	if( old == checked ) return;
	else old = checked;
	
	Clear_List();
	
	for( int ix = 0; ix < File_List_Items.count(); ix++ )
	{
		if( checked )
		{
			if( File_List_Items[ix].Hard_Drive ) Add_To_Files_List( File_List_Items[ix] );
		}
		else
		{
			Add_To_Files_List( File_List_Items[ix] );
		}
	}
}

void Delete_VM_Files_Window::Add_To_Files_List( const File_List_Item &item )
{
	if( item.Name.isEmpty() || item.Path.isEmpty() ) return;
	
	QTableWidgetItem *it = new QTableWidgetItem();
	ui.Files_List->insertRow( ui.Files_List->rowCount() );
	
	// CheckBox
	it->setFlags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable );
	it->setCheckState( Qt::Checked );
	ui.Files_List->setItem( ui.Files_List->rowCount()-1, 0, it );
	
	// Name
	it = new QTableWidgetItem( item.Name );
	ui.Files_List->setItem( ui.Files_List->rowCount()-1, 1, it );
	
	// Path
	it = new QTableWidgetItem( item.Path );
	ui.Files_List->setItem( ui.Files_List->rowCount()-1, 2, it );
}

bool Delete_VM_Files_Window::Path_Valid( const QString &path )
{
	if( path.isEmpty() ) return false;
	if( ! QFile::exists(path) ) return false;
	if( path.startsWith("/dev/") ) return false; // FIXME Windows version
	
	return true; // All OK
}

void Delete_VM_Files_Window::Clear_List()
{
	ui.Files_List->clearContents();
	while( ui.Files_List->rowCount() > 0 ) ui.Files_List->removeRow( 0 );
}