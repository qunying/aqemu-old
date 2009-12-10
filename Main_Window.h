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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QList>
#include <QCloseEvent>

#include "VM.h"
#include "Utils.h"
#include "ui_Main_Window.h"
#include "System_Info.h"
#include "HDD_Image_Info.h"
#include "Network_Widget.h"
#include "Old_Network_Widget.h"

class Ports_Tab_Widget;
class Device_Manager_Widget;
class Network_Card_Widget;

class Main_Window: public QMainWindow
{
	Q_OBJECT
	
	public:
		Main_Window( QWidget *parent = 0 );
		
	private slots:
		void on_Machines_List_currentItemChanged( QListWidgetItem *current, QListWidgetItem *previous );
		void on_Machines_List_customContextMenuRequested( const QPoint &pos );
		void on_Machines_List_itemDoubleClicked( QListWidgetItem *item );
		void VM_State_Changet( Virtual_Machine *vm, VM::VM_State s );
		void Show_State( Virtual_Machine *vm, VM::VM_State s );
		void VM_Changet();
		void Update_Emulator_Control();
		
		// Actions
		void on_actionChange_Icon_triggered();
		void on_actionAbout_AQEMU_triggered();
		void on_actionAbout_Qt_triggered();
		void on_actionDelete_VM_triggered();
		void on_actionExit_triggered();
		void on_actionShow_New_VM_Wizard_triggered();
		void on_actionAdd_New_VM_triggered();
		void on_actionCreate_HDD_Image_triggered();
		void on_actionConvert_HDD_Image_triggered();
		void on_actionShow_Settings_Window_triggered();
		void on_actionShow_Advanced_Settings_Window_triggered();
		void on_actionShow_First_Run_Wizard_triggered();
		void on_actionPower_On_triggered();
		void on_actionSave_triggered();
		void on_actionPower_Off_triggered();
		void on_actionPause_triggered();
		void on_actionReset_triggered();
		void on_actionLoad_VM_From_File_triggered();
		void on_actionSave_As_Template_triggered();
		void on_actionCopy_triggered();
		void on_actionManage_Snapshots_triggered();
		void on_actionShow_Emulator_Control_triggered();
		void on_actionShow_QEMU_Arguments_triggered();
		void on_actionCreate_Shell_Script_triggered();
		void on_actionShow_QEMU_Error_Log_Window_triggered();
		
		void on_Tabs_currentChanged( int index );
		
		// Apply and Cancel Buttons
		void on_Button_Apply_clicked();
		void on_Button_Cancel_clicked();
		
		// General Tab
		void on_CB_Computer_Type_currentIndexChanged( int index );
		void on_CB_Emulator_Type_currentIndexChanged( int index );
		void on_CB_Emulator_Version_currentIndexChanged( int index );
		void Apply_Emulator( int mode );
		
		void on_CH_Local_Time_toggled( bool on );
		
		// Memory
		void on_Memory_Size_valueChanged( int value );
		void on_CB_RAM_Size_editTextChanged( const QString &text );
		void on_Check_Host_Mem_stateChanged ( int state );
		
		// HDD's
		void on_TB_HDA_SetPath_clicked();
		void on_TB_HDA_Create_HDD_clicked();
		void on_TB_HDA_Format_HDD_clicked();
		
		void on_TB_HDB_SetPath_clicked();
		void on_TB_HDB_Create_HDD_clicked();
		void on_TB_HDB_Format_HDD_clicked();
		
		void on_TB_HDC_SetPath_clicked();
		void on_TB_HDC_Create_HDD_clicked();
		void on_TB_HDC_Format_HDD_clicked();
		
		void on_TB_HDD_SetPath_clicked();
		void on_TB_HDD_Create_HDD_clicked();
		void on_TB_HDD_Format_HDD_clicked();
		
		QStringList Create_Info_HDD_String( const QString &disk_format, const VM::Device_Size &virtual_size,
											const VM::Device_Size &disk_size, int cluster_size );
		
		void Update_HDA_Info();
		void Update_HDB_Info();
		void Update_HDC_Info();
		void Update_HDD_Info();
		
		void on_GB_HDC_toggled( bool on ); // CD-ROM or HDC...
		
		// FD0
		void on_TB_FD0_SetPath_clicked();
		void on_RB_FD0_Use_Host_Device_toggled( bool on );
		
		// FD1
		void on_TB_FD1_SetPath_clicked();
		void on_RB_FD1_Use_Host_Device_toggled( bool on );
		
		// CDROM
		void on_TB_CDROM_SetPath_clicked();
		void on_RB_CDROM_Use_Host_Device_toggled( bool on );
		void on_GB_CDROM_toggled( bool on ); // CD-ROM or HDC...
		
		// Network Tab
		void on_RB_Network_Mode_New_toggled( bool on );
		
		void on_Redirections_List_cellClicked( int row, int column );
		void on_Button_Add_Redirections_clicked();
		void on_Button_Delete_Redirections_clicked();
		void on_Button_Apply_Redirections_clicked();
		void on_Button_Clear_Redirections_clicked();
		
		void on_TB_Browse_SMB_clicked();
		
		// Advanced
		void on_CH_Start_Date_toggled( bool on );
		
		// Other Tab
		void on_TB_VNC_Unix_Socket_Browse_clicked();
		void on_TB_x509_Browse_clicked();
		void on_TB_x509verify_Browse_clicked();
		
		void on_TB_Linux_bzImage_SetPath_clicked();
		void on_TB_Linux_Initrd_SetPath_clicked();
		
		void on_TB_ROM_File_Browse_clicked();
		void on_TB_MTDBlock_File_Browse_clicked();
		void on_TB_SD_Image_File_Browse_clicked();
		void on_TB_PFlash_File_Browse_clicked();
		
	protected:
		void closeEvent( QCloseEvent *event );
		
	private:
		void Connect_Signals();
		
		QList<Averable_Devices> &Get_Devices_Info();
		bool Current_Emulator_Version_Good( VM::QEMU_Version qver, VM::KVM_Version kver );
		
		bool Create_VM_From_Ui( Virtual_Machine &tmp_vm, int index );
		
		bool Load_Settings();
		bool Save_Settings();
		
		void Update_VM_Ui();
		void Update_VM_Port_Number();
		void Update_Info_Text( int info_mode = 0 );
		void Update_Disabled_Controls();
		
		bool Load_Virtual_Machines();
		bool Save_Virtual_Machines();
		
		QString Get_QEMU_Args();
		QString Get_Current_Binary_Name();
		bool Boot_Is_Correct();
		bool No_Device_Found( const QString &name, const QString &path, VM::Boot_Device type );
		
		QString Copy_VM_Hard_Drive( const QString &vm_name, const QString &hd_name, const VM_HDD &hd );
		QString Copy_VM_Floppy( const QString &vm_name, const QString &fd_name, const VM_Floppy &fd );
		
		Ui::Main_Window ui;
		QMenu *Icon_Menu; // Context menu for vm icons
		QMenu *VM_List_Menu; // Context menu for vm list
		QSettings Settings;
		
		bool GUI_User_Mode;
		QString VM_Folder;
		
		QList<Virtual_Machine> VM_List;
		QList<Emulator> All_Emulators_List;
		
		HDD_Image_Info* HDA_Info;
		HDD_Image_Info* HDB_Info;
		HDD_Image_Info* HDC_Info;
		HDD_Image_Info* HDD_Info;
		
		Ports_Tab_Widget *Ports_Tab;
		Device_Manager_Widget *Dev_Manager;
		
		Network_Widget *New_Network_Settings_Widget;
		Old_Network_Widget *Old_Network_Settings_Widget;
};

#endif