/****************************************************************************
**
** Copyright (C) 2009-2010 Andrey Rijov <ANDron142@yandex.ru>
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

#include <QCheckBox>
#include "Boot_Device_Window.h"

Boot_Device_Window::Boot_Device_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
	
	ui.Boot_Devices_List->clear();
	ui.Boot_Devices_List->resizeColumnToContents( 0 );
	
	QTreeWidgetItem *item = new QTreeWidgetItem( ui.Boot_Devices_List );
	item->setText( 0, tr("Floppy") );
	item->setData( 0, Qt::UserRole, "fdd" );
	item->setCheckState( 0, Qt::Checked );
	
	item = new QTreeWidgetItem( ui.Boot_Devices_List );
	item->setText( 0, tr("CD-ROM") );
	item->setData( 0, Qt::UserRole, "cd" );
	item->setCheckState( 0, Qt::Checked );
	
	item = new QTreeWidgetItem( ui.Boot_Devices_List );
	item->setText( 0, tr("Hard Drive") );
	item->setData( 0, Qt::UserRole, "hdd" );
	item->setCheckState( 0, Qt::Checked );
	
	item = new QTreeWidgetItem( ui.Boot_Devices_List );
	item->setText( 0, tr("Network") );
	item->setData( 0, Qt::UserRole, "net" );
	item->setCheckState( 0, Qt::Checked );
}

void Boot_Device_Window::on_Button_OK_clicked()
{
}
