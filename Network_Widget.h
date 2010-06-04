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

#ifndef NETWORK_WIDGET_H
#define NETWORK_WIDGET_H

#include "ui_Network_Widget.h"
#include "VM_Devices.h"

class Network_Widget: public QWidget
{
	Q_OBJECT
	
	public:
		Network_Widget( QWidget *parent = 0 );
		
		bool Get_Network_Cards( QList<VM_Net_Card_Nativ> &cards );
		void Set_Network_Cards( const QList<VM_Net_Card_Nativ> &cards );
		
		void Set_Network_Card_Models( const QList<Device_Map> &models );
		
		void Set_Enabled( bool on );
		
	private slots:
		void Connect_Slots();
		void Disconnect_Slots();
		
		void on_Button_Add_clicked();
		void on_Button_Delete_clicked();
		void on_Items_List_currentItemChanged( QListWidgetItem *current, QListWidgetItem *previous );
		void on_TB_Help_clicked();
		void on_TB_Browse_script_clicked();
		void on_TB_Browse_downscript_clicked();
		void on_CB_Network_Type_currentIndexChanged( int index );
		void on_TB_Generate_New_MAC_clicked();
		void Update_Card_Item();
		
	private:
		VM_Net_Card_Nativ Get_Net_Card_From_Ui() const;
		void Set_Net_Card_To_Ui( const VM_Net_Card_Nativ &card );
		
		bool Net_Card_is_Valid();
		bool No_File_Found( const QString &name, const QString &path );
		
		QString Get_Items_Count( VM::Network_Mode_Nativ type );
		
		void Enable_Buttons( bool add, bool del );
		
	signals:
		void Changet();
		
	private:
		Ui::Network_Widget ui;
		QList<VM_Net_Card_Nativ> Network_Cards;
		QStringList Card_Models_QEMU_Name;
		int nic, user, chanel, tap, socket, multi, vde, dump;
};

#endif
