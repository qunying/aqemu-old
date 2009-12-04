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

#ifndef QEMU_BINARY_WINDOW_H
#define QEMU_BINARY_WINDOW_H

#include <QSettings>
#include "VM_Devices.h"
#include "ui_QEMU_Binary_Window.h"

class QEMU_Binary_Window: public QDialog
{
	Q_OBJECT

	public:
		QEMU_Binary_Window( QWidget *parent = 0 );
		
	public slots:
		void on_Button_OK_clicked();
		
		Emulator Get_Emulator() const;
		void Set_Emulator( Emulator emul );
		
	private slots:
		void on_Button_Find_clicked();
		void on_TB_Browse_clicked();
		void on_Table_QEMU_Systems_itemDoubleClicked( QTableWidgetItem *item );
		void on_RB_QEMU_toggled( bool checked );
		void on_RB_KVM_toggled( bool checked );
		
	private:
		Ui::QEMU_Binary_Window ui;
		QSettings Settings;
		
		Emulator Current_Emulator;
		//QStringList QEMU_Binary_Names, KVM_Binary_Names;
};

#endif
