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
#include <QInputDialog>
#include <QTextFrame>
#include <QTextTableCell>
#include <QUrl>
#include <QHeaderView>

#include "Main_Window.h"
#include "Device_Manager_Widget.h"
#include "Select_Icon_Window.h"
#include "Settings_Window.h"
#include "About_Window.h"
#include "Create_HDD_Image_Window.h"
#include "Convert_HDD_Image_Window.h"
#include "VM_Wizard_Window.h"
#include "Ports_Tab_Widget.h"
#include "Create_Template_Window.h"
#include "Snapshots_Window.h"
#include "VNC_Password_Window.h"
#include "Copy_VM_Window.h"
#include "Advanced_Settings_Window.h"
#include "First_Start_Wizard.h"
#include "Emulator_Control_Window.h"

QList<Averable_Devices> System_Info::Emulator_QEMU_0_9_0;
QList<Averable_Devices> System_Info::Emulator_QEMU_0_9_1;
QList<Averable_Devices> System_Info::Emulator_QEMU_0_10;

QList<Averable_Devices> System_Info::Emulator_KVM_Old;
QList<Averable_Devices> System_Info::Emulator_KVM_7X;
QList<Averable_Devices> System_Info::Emulator_KVM_8X;

QList<VM_USB> System_Info::All_Host_USB;
QList<VM_USB> System_Info::Used_Host_USB;

Main_Window::Main_Window( QWidget *parent )
	: QMainWindow( parent )
{
	ui.setupUi( this );
	
	// This for Tab Info Backgroud Color
	Update_Info_Text( 1 );
	
	// Disk Object
	HDA_Info = new HDD_Image_Info();
	HDB_Info = new HDD_Image_Info();
	HDC_Info = new HDD_Image_Info();
	HDD_Info = new HDD_Image_Info();
	
	connect( HDA_Info, SIGNAL(Completed()),
			 this, SLOT(Update_HDA_Info()) );
	
	connect( HDB_Info, SIGNAL(Completed()),
			 this, SLOT(Update_HDB_Info()) );
	
	connect( HDC_Info, SIGNAL(Completed()),
			 this, SLOT(Update_HDC_Info()) );
	
	connect( HDD_Info, SIGNAL(Completed()),
			 this, SLOT(Update_HDD_Info()) );
	
	// Network Settigns
	New_Network_Settings_Widget = new Network_Widget();
	Old_Network_Settings_Widget = new Old_Network_Widget();
	
	// Update Emulators Information
	System_Info::Update_VM_Computers_List();
	
	All_Emulators_List = Get_Emulators_List();
	
	GUI_User_Mode = true;
	Apply_Emulator( 0 );
	
	// Create Icon_Menu
	Icon_Menu = new QMenu( ui.Machines_List );
	
	Icon_Menu->addAction( ui.actionPower_On );
	Icon_Menu->addAction( ui.actionSave );
	Icon_Menu->addAction( ui.actionPause );
	Icon_Menu->addAction( ui.actionPower_Off );
	Icon_Menu->addAction( ui.actionReset );
	Icon_Menu->addSeparator();
	Icon_Menu->addAction( ui.actionDelete_VM );
	Icon_Menu->addAction( ui.actionSave_As_Template );
	Icon_Menu->addAction( ui.actionCopy );
	Icon_Menu->addSeparator();
	Icon_Menu->addAction( ui.actionManage_Snapshots );
	Icon_Menu->addAction( ui.actionShow_Emulator_Control );
	Icon_Menu->addAction( ui.actionShow_QEMU_Arguments );
	Icon_Menu->addAction( ui.actionCreate_Shell_Script );
	Icon_Menu->addAction( ui.actionShow_QEMU_Error_Log_Window );
	Icon_Menu->addAction( ui.actionChange_Icon );
	
	// Create VM List Menu
	VM_List_Menu = new QMenu( ui.Machines_List );
	
	VM_List_Menu->addAction( ui.actionAdd_New_VM );
	VM_List_Menu->addAction( ui.actionShow_New_VM_Wizard );
	VM_List_Menu->addAction( ui.actionLoad_VM_From_File );
	VM_List_Menu->addAction( ui.actionCreate_HDD_Image );
	
	Ports_Tab = new Ports_Tab_Widget();
	
	// Ports NOT Working in Windows!
	#ifndef Q_OS_WIN32
	ui.Tabs->insertTab( 5, Ports_Tab, tr("Ports") );
	#endif
	
	Dev_Manager = new Device_Manager_Widget();
	
	// This For Network Redirections Table
	QHeaderView *hv = new QHeaderView( Qt::Vertical, ui.Redirections_List );
	hv->setResizeMode( QHeaderView::Fixed );
	ui.Redirections_List->setVerticalHeader( hv );
	
	hv = new QHeaderView( Qt::Horizontal, ui.Redirections_List );
	hv->setResizeMode( QHeaderView::Stretch );
	ui.Redirections_List->setHorizontalHeader( hv );
	
	hv = new QHeaderView( Qt::Vertical, ui.Redirections_List );
	hv->setResizeMode( QHeaderView::Fixed );
	ui.Redirections_List->setVerticalHeader( hv );
	
	hv = new QHeaderView( Qt::Horizontal, ui.Redirections_List );
	hv->setResizeMode( QHeaderView::Stretch );
	ui.Redirections_List->setHorizontalHeader( hv );
	
	// Loading AQEMU Settings
	if( ! Load_Settings() )
	{
		// no Settings
		AQWarning( "Main_Window::Main_Window( QWidget *parent )", "Cannot Load Settings!" );
	}
	else
	{
		if( ! Load_Virtual_Machines() ) // Loading XML VM files
		{
			// no vm's
			AQWarning( "Main_Window::Main_Window( QWidget *parent )", "No VM Loaded!" );
			
			ui.actionPower_On->setEnabled( false );
			ui.actionSave->setEnabled( false );
			ui.actionPause->setEnabled( false );
			ui.actionPower_Off->setEnabled( false );
			ui.actionReset->setEnabled( false );
			
			ui.Tab_General->setEnabled( false );
			ui.Tab_HDD->setEnabled( false );
			ui.Tab_Removable_Disks->setEnabled( false );
			Dev_Manager->setEnabled( false );
			ui.Tab_Network->setEnabled( false );
			Ports_Tab->setEnabled( false );
			ui.Tab_Other->setEnabled( false );
			ui.Tab_Advanced->setEnabled( false );
			
			ui.Button_Apply->setEnabled( false );
			ui.Button_Cancel->setEnabled( false );
		}
		else
		{
			// ok, vm's loaded. show it...
			AQDebug( "Main_Window::Main_Window( QWidget *parent )", "All OK Loading Complete!" );
		}
	}
	
	// Singals for watch VM Changes
	Connect_Signals();
}

void Main_Window::closeEvent( QCloseEvent *event )
{
	if( ! Save_Settings() )
	{
		AQGraphic_Error( "void Main_Window::closeEvent( QCloseEvent *event )",
						 tr("Cannot Save Settings!"), false );
	}
	else
	{
		// Find Run VM
		for( int vx = 0; vx < VM_List.count(); ++vx )
		{
			if( VM_List[vx].Get_State() == VM::VMS_Running ||
				VM_List[vx].Get_State() == VM::VMS_Pause )
			{
				int mes_res = QMessageBox::question( this, tr("Close AQEMU?"),
													 tr("One or More VM is Running!\nTerminate All Running VM and Close AQEMU?"),
													 QMessageBox::Yes | QMessageBox::No, QMessageBox::No );
				
				if( mes_res != QMessageBox::Yes )
				{
					event->ignore();
					return;
				}
				
				break;
			}
		}
		
		// Close All Emu_Ctl and QEMU_Error_Log Windows
		for( int ex = 0; ex < VM_List.count(); ++ex )
		{
			VM_List[ ex ].Hide_Emu_Ctl_Win();
			VM_List[ ex ].Hide_QEMU_Error_Log();
		}
		
		event->accept();
	}
}

void Main_Window::Connect_Signals()
{
	// General Tab
	connect( ui.Edit_Machine_Name, SIGNAL(textChanged(const QString &)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CB_Computer_Type, SIGNAL(currentIndexChanged(int)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CB_CPU_Type, SIGNAL(currentIndexChanged(int)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CB_CPU_Count, SIGNAL(currentIndexChanged(int)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CB_Machine_Type, SIGNAL(currentIndexChanged(int)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CB_Boot_Prioritet, SIGNAL(currentIndexChanged(int)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CB_Video_Card, SIGNAL(currentIndexChanged(int)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CB_Keyboard_Layout, SIGNAL(currentIndexChanged(int)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.Memory_Size, SIGNAL(valueChanged(int)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CB_RAM_Size, SIGNAL(currentIndexChanged(int)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.Check_Host_Mem, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_sb16, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_es1370, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_Adlib, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_AC97, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_GUS, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_PCSPK, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_Fullscreen, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_Local_Time, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_Snapshot, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_ACPI, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_No_Reboot, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_No_Shutdown, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	// Devices Widgets
	if( Settings.value("Use_Device_Manager", "no").toString() == "yes" )
	{
		if( Dev_Manager ) connect( Dev_Manager, SIGNAL(Device_Changet()), this, SLOT(VM_Changet()) );
		else AQError( "void Main_Window::Connect_Signals()", "Dev_Manager == NULL" );
	}
	else
	{
		// Hard Drives
		connect( ui.GB_HDA, SIGNAL(toggled(bool)),
				 this, SLOT(VM_Changet()) );
		
		connect( ui.Edit_HDA_Image_Path, SIGNAL(textChanged(const QString &)),
				 this, SLOT(VM_Changet()) );
		
		connect( ui.GB_HDB, SIGNAL(toggled(bool)),
				 this, SLOT(VM_Changet()) );
		
		connect( ui.Edit_HDB_Image_Path, SIGNAL(textChanged(const QString &)),
				 this, SLOT(VM_Changet()) );
		
		connect( ui.GB_HDC, SIGNAL(toggled(bool)),
				 this, SLOT(VM_Changet()) );
		
		connect( ui.Edit_HDC_Image_Path, SIGNAL(textChanged(const QString &)),
				 this, SLOT(VM_Changet()) );
		
		connect( ui.GB_HDD, SIGNAL(toggled(bool)),
				 this, SLOT(VM_Changet()) );
		
		connect( ui.Edit_HDD_Image_Path, SIGNAL(textChanged(const QString &)),
				 this, SLOT(VM_Changet()) );
		
		// CD-ROM
		connect( ui.GB_CDROM, SIGNAL(toggled(bool)),
				 this, SLOT(VM_Changet()) );
		
		connect( ui.RB_CDROM_Use_Host_Device, SIGNAL(toggled(bool)),
				 this, SLOT(VM_Changet()) );
		
		connect( ui.CB_CDROM_Host_Devices, SIGNAL(currentIndexChanged(int)),
				 this, SLOT(VM_Changet()) );
		
		connect( ui.RB_CDROM_Use_Image_File, SIGNAL(toggled(bool)),
				 this, SLOT(VM_Changet()) );
		
		connect( ui.Edit_CDROM_Image_File_Path, SIGNAL(textChanged(const QString &)),
				 this, SLOT(VM_Changet()) );
		
		// FDD 1
		connect( ui.GB_Floppy0, SIGNAL(toggled(bool)),
				 this, SLOT(VM_Changet()) );
		
		connect( ui.RB_FD0_Use_Host_Device, SIGNAL(toggled(bool)),
				 this, SLOT(VM_Changet()) );
		
		connect( ui.CB_FD0_Host_Devices, SIGNAL(currentIndexChanged(int)),
				 this, SLOT(VM_Changet()) );
		
		connect( ui.RB_FD0_Use_Host_Device, SIGNAL(toggled(bool)),
				 this, SLOT(VM_Changet()) );
		
		connect( ui.Edit_FD0_Image_File_Path, SIGNAL(textChanged(const QString &)),
				 this, SLOT(VM_Changet()) );
		
		// FDD 2
		connect( ui.GB_Floppy1, SIGNAL(toggled(bool)),
				 this, SLOT(VM_Changet()) );
		
		connect( ui.RB_FD1_Use_Host_Device, SIGNAL(toggled(bool)),
				 this, SLOT(VM_Changet()) );
		
		connect( ui.CB_FD1_Host_Devices, SIGNAL(currentIndexChanged(int)),
				 this, SLOT(VM_Changet()) );
		
		connect( ui.RB_FD1_Use_Host_Device, SIGNAL(toggled(bool)),
				 this, SLOT(VM_Changet()) );
		
		connect( ui.Edit_FD1_Image_File_Path, SIGNAL(textChanged(const QString &)),
				 this, SLOT(VM_Changet()) );
	}
	
	// Network Tab
	connect( ui.CH_Use_Network, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.RB_Network_Mode_Old, SIGNAL(toggled(bool)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.RB_Network_Mode_New, SIGNAL(toggled(bool)),
			 this, SLOT(VM_Changet()) );
	
	connect( New_Network_Settings_Widget, SIGNAL(Changet()),
			 this, SLOT(VM_Changet()) );

	connect( Old_Network_Settings_Widget, SIGNAL(Changet()),
			 this, SLOT(VM_Changet()) );
	
	// Ports
	connect( Ports_Tab, SIGNAL(Settings_Changet()),
			 this, SLOT(VM_Changet()) );
	
	// Additional Network Settings
	connect( ui.CH_Redirections, SIGNAL(toggled(bool)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.Redirections_List, SIGNAL(itemChanged(QTableWidgetItem*)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.RB_TCP, SIGNAL(toggled(bool)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.RB_UDP, SIGNAL(toggled(bool)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.SB_Redir_Port, SIGNAL(valueChanged(int)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.Edit_Guest_IP, SIGNAL(textChanged(const QString &)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.SB_Guest_Port, SIGNAL(valueChanged(int)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.Edit_TFTP_Prefix, SIGNAL(textChanged(const QString &)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.Edit_SMB_Folder, SIGNAL(textChanged(const QString &)),
			 this, SLOT(VM_Changet()) );
	
	// Advanced Tab
	connect( ui.CH_No_Frame, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_Alt_Grab, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_No_Quit, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_Portrait, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_Curses, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_Show_Cursor, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_Start_CPU, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_FDD_Boot, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_QEMU_Log, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_Win2K_Hack, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_RTC_TD_Hack, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_Start_Date, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.DTE_Start_Date, SIGNAL(dateTimeChanged(const QDateTime &)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_Init_Graphic_Mode, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.SB_InitGM_Width, SIGNAL(valueChanged(int)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.SB_InitGM_Height, SIGNAL(valueChanged(int)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CB_InitGM_Depth, SIGNAL(currentIndexChanged(int)),
			 this, SLOT(VM_Changet()) );
	
	// Other Tab
	connect( ui.CH_Use_GDB, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.SB_GDB_Port, SIGNAL(valueChanged(int)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.Edit_Additional_Args, SIGNAL(textChanged()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_Only_User_Args, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_Use_User_Binary, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	// Hardware Virtualization Tab
	connect( ui.CH_No_KVM_IRQChip, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_No_KVM_Pit, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_KVM_Shadow_Memory, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.SB_KVM_Shadow_Memory_Size, SIGNAL(valueChanged(int)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.RB_KQEMU_Disabled, SIGNAL(toggled(bool)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.RB_KQEMU_Use_if_Possible, SIGNAL(toggled(bool)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.RB_KQEMU_Enabled, SIGNAL(toggled(bool)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.RB_KQEMU_Full, SIGNAL(toggled(bool)),
			 this, SLOT(VM_Changet()) );
	
	// VNC Tab
	connect( ui.CH_Activate_VNC, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.RB_VNC_Display_Number, SIGNAL(toggled(bool)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.SB_VNC_Display, SIGNAL(valueChanged(int)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.RB_VNC_Unix_Socket, SIGNAL(toggled(bool)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_VNC_Password, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_Use_VNC_TLS, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_x509_Folder, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.Edit_x509_Folder, SIGNAL(textChanged(const QString &)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_x509verify_Folder, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.Edit_x509verify_Folder, SIGNAL(textChanged(const QString &)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_No_Use_Embedded_Display, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	// Optional Images
	connect( ui.CH_ROM_File, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.Edit_ROM_File, SIGNAL(textChanged(const QString &)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_MTDBlock, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.Edit_MTDBlock_File, SIGNAL(textChanged(const QString &)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_SD_Image, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.Edit_SD_Image_File, SIGNAL(textChanged(const QString &)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.CH_PFlash, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.Edit_PFlash_File, SIGNAL(textChanged(const QString &)),
			 this, SLOT(VM_Changet()) );
	
	// Boot Linux Kernel
	connect( ui.CH_Use_Linux_Boot, SIGNAL(clicked()),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.Edit_Linux_bzImage_Path, SIGNAL(textChanged(const QString &)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.Edit_Linux_Initrd_Path, SIGNAL(textChanged(const QString &)),
			 this, SLOT(VM_Changet()) );
	
	connect( ui.Edit_Linux_Command_Line, SIGNAL(textChanged(const QString &)),
			 this, SLOT(VM_Changet()) );
}

QList<Averable_Devices> &Main_Window::Get_Devices_Info()
{
	Emulator cur_emul;
	
	if( ui.CB_Emulator_Version->currentIndex() > 0 )
	{
		cur_emul = Get_Emulator_By_Name( ui.CB_Emulator_Version->currentText() );
	}
	else
	{
		if( ui.CB_Emulator_Type->currentText() == "QEMU" )
		{
			cur_emul = Get_Default_Emulator( "QEMU" );
		}
		else if( ui.CB_Emulator_Type->currentText() == "KVM" )
		{
			cur_emul = Get_Default_Emulator( "KVM" );
		}
		else
		{
			cur_emul = Get_Default_Emulator( "QEMU" );
			
			AQError( "QList<Averable_Devices> &Main_Window::Get_Devices_Info()",
					 "Incorrect Emulator Type. Use Default: QEMU" );
		}
	}
	
	if( cur_emul.Get_Type() == "QEMU" )
	{
		switch( String_To_QEMU_Version(cur_emul.Get_QEMU_Version()) )
		{
			case VM::QEMU_Old:
				return System_Info::Emulator_QEMU_0_9_0;
				break;
				
			case VM::QEMU_0_9_0:
				return System_Info::Emulator_QEMU_0_9_0;
				break;
				
			case VM::QEMU_0_9_1:
				return System_Info::Emulator_QEMU_0_9_1;
				break;
				
			case VM::QEMU_0_10:
				return System_Info::Emulator_QEMU_0_10;
				break;
				
			case VM::QEMU_New:
				return System_Info::Emulator_QEMU_0_10;
				break;
				
			default:
				AQError( "QList<Averable_Devices> &Main_Window::Get_Devices_Info()",
						 "QEMU Version Incorrect! Use Default: 0.9.0" );
				
				return System_Info::Emulator_QEMU_0_9_0;
				break;
		}
	}
	else if( cur_emul.Get_Type() == "KVM" )
	{
		switch( String_To_KVM_Version(cur_emul.Get_KVM_Version()) )
		{
			case VM::KVM_Old:
				return System_Info::Emulator_KVM_Old;
				break;
				
			case VM::KVM_7X:
				return System_Info::Emulator_KVM_7X;
				break;
				
			case VM::KVM_8X:
				return System_Info::Emulator_KVM_8X;
				break;
				
			case VM::KVM_New:
				return System_Info::Emulator_KVM_8X;
				break;
				
			default:
				AQError( "QList<Averable_Devices> &Main_Window::Get_Devices_Info()",
						 "KVM Version Incorrect! Use Default: 7X" );
				
				return System_Info::Emulator_KVM_7X;
				break;
		}
	}
	
	AQError( "QList<Averable_Devices> &Main_Window::Get_Devices_Info()",
			 "Incorrect Emulator Type! Cannot Load Version!" );
	
	return System_Info::Emulator_QEMU_0_9_0;
}

bool Main_Window::Current_Emulator_Version_Good( VM::QEMU_Version qver, VM::KVM_Version kver )
{
	const Emulator *cur_emul;
	
	if( ui.CB_Emulator_Version->currentIndex() <= 0 )
	{
		cur_emul = &Get_Default_Emulator( ui.CB_Emulator_Type->currentText() );
	}
	else
	{
		cur_emul = &Get_Emulator_By_Name( ui.CB_Emulator_Version->currentText() );
	}
	
	if( ! cur_emul )
	{
		AQError( "bool Main_Window::Current_Emulator_Version_Good( VM::QEMU_Version qver, VM::KVM_Version kver )",
				 "Emulator is NULL!" );
		return false;
	}
	
	if( cur_emul->Get_Type() == "QEMU" )
	{
		if( String_To_QEMU_Version(cur_emul->Get_QEMU_Version()) >= qver ) return true;
		else return false;
	}
	else if( cur_emul->Get_Type() == "KVM" )
	{
		if( String_To_KVM_Version(cur_emul->Get_KVM_Version()) >= kver ) return true;
		else return false;
	}
	else
	{
		AQError( "bool Main_Window::Current_Emulator_Version_Good( VM::QEMU_Version qver, VM::KVM_Version kver )",
				 "Incorrect Emulator Type: \"" + cur_emul->Get_Type() + "\"");
		return false;
	}
}

bool Main_Window::Create_VM_From_Ui( Virtual_Machine &tmp_vm, int index )
{
	tmp_vm.Set_VM_XML_File_Path( VM_List[index].Get_VM_XML_File_Path() );
	
	// Machine Name
	if( ui.Edit_Machine_Name->text().isEmpty() )
	{
		AQGraphic_Warning( tr("Error!"), tr("VM Name is Empty!") );
		return false;
	}
	else
	{
		tmp_vm.Set_Machine_Name( ui.Edit_Machine_Name->text() );
	}
	
	// Icon Path
	if( ui.Machines_List->item(index)->data(128).toString() ==
		Settings.value("VM_Directory", "~").toString() + Get_FS_Compatible_VM_Name(ui.Edit_Machine_Name->text()) )
	{
		tmp_vm.Set_Icon_Path( VM_List[index].Get_Icon_Path() );
		tmp_vm.Set_Screenshot_Path( ui.Machines_List->item(index)->data(128).toString() );
	}
	else
	{
		tmp_vm.Set_Icon_Path( ui.Machines_List->item(index)->data(128).toString() );
	}
	
	Averable_Devices* cur_comp = &Get_Devices_Info()[ ui.CB_Computer_Type->currentIndex() ];
	
	// Computer Type
	tmp_vm.Set_Computer_Type( cur_comp->System.QEMU_Name );
	
	// Machine Type
	tmp_vm.Set_Machine_Type( cur_comp->Machine_List[ui.CB_Machine_Type->currentIndex()].QEMU_Name );
	
	// CPU Type
	tmp_vm.Set_CPU_Type( cur_comp->CPU_List[ui.CB_CPU_Type->currentIndex()].QEMU_Name );
	
	// Create Emulator Info
	if( ui.CB_Emulator_Version->currentIndex() <= 0 )
	{
		Emulator tmp_emul = Get_Default_Emulator(ui.CB_Emulator_Type->currentText());
		tmp_emul.Set_Name( "" );
		tmp_vm.Set_Emulator( tmp_emul );
	}
	else
	{
		tmp_vm.Set_Emulator( Get_Emulator_By_Name(ui.CB_Emulator_Version->currentText()) );
	}
	
	// Video
	tmp_vm.Set_Video_Card( cur_comp->Video_Card_List[ui.CB_Video_Card->currentIndex()].QEMU_Name );
	
	// CPU Count
	switch( ui.CB_CPU_Count->currentIndex() )
	{
		case 0:
			tmp_vm.Set_SMP_CPU_Count( 1 );
			break;
			
		case 1:
			tmp_vm.Set_SMP_CPU_Count( 2 );
			break;
			
		case 2:
			tmp_vm.Set_SMP_CPU_Count( 4 );
			break;
			
		case 3:
			tmp_vm.Set_SMP_CPU_Count( 8 );
			break;
			
		case 4:
			tmp_vm.Set_SMP_CPU_Count( 16 );
			break;
			
		case 5:
			tmp_vm.Set_SMP_CPU_Count( 32 );
			break;
			
		case 6:
			tmp_vm.Set_SMP_CPU_Count( 64 );
			break;
			
		case 7:
			tmp_vm.Set_SMP_CPU_Count( 128 );
			break;
			
		case 8:
			tmp_vm.Set_SMP_CPU_Count( 255 );
			break;
			
		default:
			AQWarning( "bool Main_Window::Create_VM_From_Ui( Virtual_Machine &tmp_vm, int index )",
					   "Set Default CPU Count: 1" );
			tmp_vm.Set_SMP_CPU_Count( 1 );
			break;
	}
	
	// Keyboard Layout
	if( ui.CB_Keyboard_Layout->currentIndex() == 0 ) // Default
	{
		tmp_vm.Set_Keyboard_Layout( "Default" );
	}
	else
	{
		tmp_vm.Set_Keyboard_Layout( ui.CB_Keyboard_Layout->currentText() );
	}
	
	// Boot Prioritet
	switch( ui.CB_Boot_Prioritet->currentIndex() )
	{
		case 0:
			tmp_vm.Set_Boot_Device( VM::Boot_From_FDD );
			break;
			
		case 1:
			tmp_vm.Set_Boot_Device( VM::Boot_From_HDD );
			break;
			
		case 2:
			tmp_vm.Set_Boot_Device( VM::Boot_From_CDROM );
			break;
			
		case 3:
			tmp_vm.Set_Boot_Device( VM::Boot_From_Network );
			break;
			
		case 4:
			tmp_vm.Set_Boot_Device( VM::Boot_None );
			break;
			
		default:
			AQWarning( "bool Main_Window::Create_VM_From_Ui( Virtual_Machine &tmp_vm, int index )",
					   "Use Default Boot Device: CD-ROM" );
			tmp_vm.Set_Boot_Device( VM::Boot_From_CDROM );
			break;
	}
	
	// Acseleration
	if( ui.RB_KQEMU_Use_if_Possible->isChecked() )
	{
		tmp_vm.Set_KQEMU_Mode( VM::KQEMU_Default );
	}
	else if( ui.RB_KQEMU_Disabled->isChecked() )
	{
		tmp_vm.Set_KQEMU_Mode( VM::KQEMU_Disabled );
	}
	else if( ui.RB_KQEMU_Enabled->isChecked() )
	{
		tmp_vm.Set_KQEMU_Mode( VM::KQEMU_Enabled );
	}
	else if( ui.RB_KQEMU_Full->isChecked() )
	{
		tmp_vm.Set_KQEMU_Mode( VM::KQEMU_Full );
	}
	else
	{
		AQWarning( "bool Main_Window::Create_VM_From_Ui( Virtual_Machine &tmp_vm, int index )",
				   "Use Default KQEMU Mode: Default" );
		tmp_vm.Set_KQEMU_Mode( VM::KQEMU_Default );
	}
	
	// Audio
	VM::Sound_Cards snd_card;
	
	if( ui.CH_sb16->isChecked() ) snd_card.Audio_sb16 = true;
	else snd_card.Audio_sb16 = false;
	
	if( ui.CH_es1370->isChecked() ) snd_card.Audio_es1370 = true;
	else snd_card.Audio_es1370 = false;
	
	if( ui.CH_Adlib->isChecked() ) snd_card.Audio_Adlib = true;
	else snd_card.Audio_Adlib = false;
	
	if( ui.CH_PCSPK->isChecked() ) snd_card.Audio_PC_Speaker = true;
	else snd_card.Audio_PC_Speaker = false;
	
	if( ui.CH_GUS->isChecked() ) snd_card.Audio_GUS = true;
	else snd_card.Audio_GUS = false;
	
	if( ui.CH_AC97->isChecked() ) snd_card.Audio_AC97 = true;
	else snd_card.Audio_AC97 = false;
	
	tmp_vm.Set_Audio_Cards( snd_card );
	
	// Memory
	tmp_vm.Set_Memory_Size( ui.Memory_Size->value() );
	
	// Check free ram
	tmp_vm.Set_Check_Free_RAM( ui.Check_Host_Mem->isChecked() );
	
	// Options
	tmp_vm.Use_Fullscreen_Mode( ui.CH_Fullscreen->isChecked() );
	tmp_vm.Use_Win2K_Hack( ui.CH_Win2K_Hack->isChecked() );
	tmp_vm.Use_Local_Time( ui.CH_Local_Time->isChecked() );
	tmp_vm.Use_Check_FDD_Boot_Sector( ui.CH_FDD_Boot->isChecked() );
	tmp_vm.Use_ACPI( ui.CH_ACPI->isChecked() );
	tmp_vm.Use_Snapshot_Mode( ui.CH_Snapshot->isChecked() );
	tmp_vm.Use_Start_CPU( ui.CH_Start_CPU->isChecked() );
	tmp_vm.Use_QEMU_Log( ui.CH_QEMU_Log->isChecked() );
	tmp_vm.Use_No_Reboot( ui.CH_No_Reboot->isChecked() );
	tmp_vm.Use_No_Shutdown( ui.CH_No_Shutdown->isChecked() );
	
	// Use Device Manager Mode
	if( Settings.value("Use_Device_Manager", "no").toString() == "yes" )
	{
		tmp_vm.Set_FD0( Dev_Manager->Floppy1 );
		tmp_vm.Set_FD1( Dev_Manager->Floppy2 );
		tmp_vm.Set_CD_ROM( Dev_Manager->CD_ROM );
		tmp_vm.Set_HDA( Dev_Manager->HDA );
		tmp_vm.Set_HDB( Dev_Manager->HDB );
		tmp_vm.Set_HDC( Dev_Manager->HDC );
		tmp_vm.Set_HDD( Dev_Manager->HDD );
		tmp_vm.Set_Storage_Devices_List( Dev_Manager->Storage_Devices );
	}
	else
	{
		// Floppy 1
		VM_Floppy tmp_fd;
		
		tmp_fd.Set_Enabled( ui.GB_Floppy0->isChecked() );
		tmp_fd.Set_Host_Device( ui.RB_FD0_Use_Host_Device->isChecked() );
		tmp_fd.Set_Host_File_Name( ui.CB_FD0_Host_Devices->currentText() );
		tmp_fd.Set_Image_File_Name( ui.Edit_FD0_Image_File_Path->text() );
		
		tmp_vm.Set_FD0( tmp_fd );
		
		// Floppy 2
		tmp_fd.Set_Enabled( ui.GB_Floppy1->isChecked() );
		tmp_fd.Set_Host_Device( ui.RB_FD1_Use_Host_Device->isChecked() );
		tmp_fd.Set_Host_File_Name( ui.CB_FD1_Host_Devices->currentText() );
		tmp_fd.Set_Image_File_Name( ui.Edit_FD1_Image_File_Path->text() );
		
		tmp_vm.Set_FD1( tmp_fd );
		
		// CD-ROM
		VM_CDROM tmp_cd;
		
		tmp_cd.Set_Enabled( ui.GB_CDROM->isChecked() );
		tmp_cd.Set_Host_Device( ui.RB_CDROM_Use_Host_Device->isChecked() );
		tmp_cd.Set_Host_File_Name( ui.CB_CDROM_Host_Devices->currentText() );
		tmp_cd.Set_Image_File_Name( ui.Edit_CDROM_Image_File_Path->text() );
		
		tmp_vm.Set_CD_ROM( tmp_cd );
		
		// Hard Disks
		VM_HDD tmp_hd;
		
		tmp_hd.Set_Enabled( ui.GB_HDA->isChecked() );
		tmp_hd.Set_Image_File_Name( ui.Edit_HDA_Image_Path->text() );
		tmp_vm.Set_HDA( tmp_hd );
		
		tmp_hd.Set_Enabled( ui.GB_HDB->isChecked() );
		tmp_hd.Set_Image_File_Name( ui.Edit_HDB_Image_Path->text() );
		tmp_vm.Set_HDB( tmp_hd );
		
		tmp_hd.Set_Enabled( ui.GB_HDC->isChecked() );
		tmp_hd.Set_Image_File_Name( ui.Edit_HDC_Image_Path->text() );
		tmp_vm.Set_HDC( tmp_hd );
		
		tmp_hd.Set_Enabled( ui.GB_HDD->isChecked() );
		tmp_hd.Set_Image_File_Name( ui.Edit_HDD_Image_Path->text() );
		tmp_vm.Set_HDD( tmp_hd );
	}
	
	// Network Tab
	tmp_vm.Set_Use_Network( ui.CH_Use_Network->isChecked() );
	
	// Use Nativ Network
	tmp_vm.Use_Nativ_Network( ui.RB_Network_Mode_New->isChecked() );
	
	// Redirections List
	if( ui.CH_Redirections->isChecked() && ui.Redirections_List->rowCount() < 1 )
	{
		AQGraphic_Warning( tr("Error!"), tr("Redirection List is Empty! Please Disable Redirections!") );
		return false;
	}
	
	// Redirections
	tmp_vm.Set_Use_Redirections( ui.CH_Redirections->isChecked() );
	
	// Redirections List
	for( int rx = 0; rx < ui.Redirections_List->rowCount(); rx++ )
	{
		VM_Redirection tmp_redir;
		
		if( ui.Redirections_List->item(rx, 0)->text() == "TCP" ) tmp_redir.Set_Protocol( "TCP" );
		else tmp_redir.Set_Protocol( "UDP" );
		
		tmp_redir.Set_Host_Port( ui.Redirections_List->item(rx, 1)->text().toInt() );
		tmp_redir.Set_Guest_IP( ui.Redirections_List->item(rx, 2)->text() );
		tmp_redir.Set_Guest_Port( ui.Redirections_List->item(rx, 3)->text().toInt() );
		
		tmp_vm.Add_Network_Redirection( tmp_redir );
	}
	
	// TFTP
	tmp_vm.Set_TFTP_Prefix( ui.Edit_TFTP_Prefix->text() );
	
	// SMB Dir
	tmp_vm.Set_SMB_Directory( ui.Edit_SMB_Folder->text() );
	
	// Network Cards
	QList<VM_Net_Card> tmp_net_cards;
	if( Old_Network_Settings_Widget->Get_Network_Cards(tmp_net_cards) )
	{
		tmp_vm.Set_Network_Cards( tmp_net_cards );
	}
	else return false;
	
	// Nativ
	QList<VM_Net_Card_Nativ> tmp_net_cards_nativ;
	if( New_Network_Settings_Widget->Get_Network_Cards(tmp_net_cards_nativ) )
	{
		tmp_vm.Set_Network_Cards_Nativ( tmp_net_cards_nativ );
	}
	else return false;
	
	// Port page
	tmp_vm.Set_Serial_Ports( Ports_Tab->Get_Serial_Ports() );
	tmp_vm.Set_Parallel_Ports( Ports_Tab->Get_Parallel_Ports() );
	tmp_vm.Set_USB_Ports( Ports_Tab->Get_USB_Ports() );
	
	// Other Page
	tmp_vm.Set_Use_Linux_Boot( ui.CH_Use_Linux_Boot->isChecked() );
	tmp_vm.Set_bzImage_Path( ui.Edit_Linux_bzImage_Path->text() );
	tmp_vm.Set_Initrd_Path( ui.Edit_Linux_Initrd_Path->text() );
	tmp_vm.Set_Kernel_ComLine( ui.Edit_Linux_Command_Line->text() );
	
	// Optional Images
	// ROM File
	tmp_vm.Set_Use_ROM_File( ui.CH_ROM_File->isChecked() );
	tmp_vm.Set_ROM_File( ui.Edit_ROM_File->text() );
	
	// On-Board Flash Image
	tmp_vm.Use_MTDBlock_File( ui.CH_MTDBlock->isChecked() );
	tmp_vm.Set_MTDBlock_File( ui.Edit_MTDBlock_File->text() );
	
	// SecureDigital Card Image
	tmp_vm.Use_SecureDigital_File( ui.CH_SD_Image->isChecked() );
	tmp_vm.Set_SecureDigital_File( ui.Edit_SD_Image_File->text() );
	
	// Parallel Flash Image
	tmp_vm.Use_PFlash_File( ui.CH_PFlash->isChecked() );
	tmp_vm.Set_PFlash_File( ui.Edit_PFlash_File->text() );
	
	// GDB
	tmp_vm.Use_GDB( ui.CH_Use_GDB->isChecked() );
	tmp_vm.Set_GDB_Port( (uint)ui.SB_GDB_Port->value() );
	
	// Additional QEMU Arguments
	tmp_vm.Set_Additional_Args( ui.Edit_Additional_Args->toPlainText() );
	
	// Only_User_Args
	tmp_vm.Set_Only_User_Args( ui.CH_Only_User_Args->isChecked() );
	
	// Use_User_Emulator_Binary
	tmp_vm.Set_Use_User_Emulator_Binary( ui.CH_Use_User_Binary->isChecked() );
	
	// Disable KVM kernel mode PIC/IOAPIC/LAPIC
	tmp_vm.Use_KVM_IRQChip( ui.CH_No_KVM_IRQChip->isChecked() );
	
	// Disable KVM kernel mode PIT
	tmp_vm.Use_No_KVM_Pit( ui.CH_No_KVM_Pit->isChecked() );
	
	// KVM_No_Pit_Reinjection
	tmp_vm.Use_KVM_No_Pit_Reinjection( ui.CH_KVM_No_Pit_Reinjection->isChecked() );
	
	// KVM_Nesting
	tmp_vm.Use_KVM_Nesting( ui.CH_KVM_Nesting->isChecked() );
	
	// KVM Shadow Memory
	tmp_vm.Use_KVM_Shadow_Memory( ui.CH_KVM_Shadow_Memory->isChecked() );
	tmp_vm.Set_KVM_Shadow_Memory_Size( ui.SB_KVM_Shadow_Memory_Size->value() );
	
	// Initial Graphical Mode
	VM_Init_Graphic_Mode tmp_mode;
	
	tmp_mode.Set_Enabled( ui.CH_Init_Graphic_Mode->isChecked() );
	tmp_mode.Set_Width( ui.SB_InitGM_Width->value() );
	tmp_mode.Set_Height( ui.SB_InitGM_Height->value() );
	
	switch( ui.CB_InitGM_Depth->currentIndex() )
	{
		case 0:
			tmp_mode.Set_Depth( 8 );
			break;
			
		case 1:
			tmp_mode.Set_Depth( 16 );
			break;
			
		case 2:
			tmp_mode.Set_Depth( 24 );
			break;
			
		case 3:
			tmp_mode.Set_Depth( 32 );
			break;
			
		default:
			AQError( "bool Main_Window::Create_VM_From_Ui( Virtual_Machine &tmp_vm, int index )",
					 "Initial Graphical Mode: Default Section!" );
			tmp_mode.Set_Depth( 24 );
			break;
	}
	
	tmp_vm.Set_Init_Graphic_Mode( tmp_mode );
	
	// Show QEMU Window Without a Frame and Window Decorations
	tmp_vm.Use_No_Frame( ui.CH_No_Frame->isChecked() );
	
	// Use Ctrl-Alt-Shift to Grab Mouse (Instead of Ctrl-Alt)
	tmp_vm.Use_Alt_Grab( ui.CH_Alt_Grab->isChecked() );
	
	// Disable SDL Window Close Capability
	tmp_vm.Use_No_Quit( ui.CH_No_Quit->isChecked() );
	
	// Rotate Graphical Output 90 Deg Left (Only PXA LCD)
	tmp_vm.Use_Portrait( ui.CH_Portrait->isChecked() );
	
	// Show_Cursor
	tmp_vm.Use_Show_Cursor( ui.CH_Show_Cursor->isChecked() );
	
	// Curses
	tmp_vm.Use_Curses( ui.CH_Curses->isChecked() );
	
	// RTC_TD_Hack
	tmp_vm.Use_RTC_TD_Hack( ui.CH_RTC_TD_Hack->isChecked() );
	
	// Start Date
	tmp_vm.Use_Start_Date( ui.CH_Start_Date->isChecked() );
	tmp_vm.Set_Start_Date( ui.DTE_Start_Date->dateTime() );
	
	// VNC
	tmp_vm.Use_VNC( ui.CH_Activate_VNC->isChecked() );
	
	// Use Unix Socket Mode for VNC
	tmp_vm.Set_VNC_Socket_Mode( ui.RB_VNC_Unix_Socket->isChecked() );
	
	// UNIX Domain Socket Path
	tmp_vm.Set_VNC_Unix_Socket_Path( ui.Edit_VNC_Unix_Socket->text() );
	
	// VNC Display Number
	tmp_vm.Set_VNC_Display_Number( ui.SB_VNC_Display->value() );
	
	// Use Password for VNC
	tmp_vm.Use_VNC_Password( ui.CH_VNC_Password->isChecked() );
	
	// Use TLS
	tmp_vm.Use_VNC_TLS( ui.CH_Use_VNC_TLS->isChecked() );
	
	// Use x509
	tmp_vm.Use_VNC_x509( ui.CH_x509_Folder->isChecked() );
	
	// x509 Folder
	tmp_vm.Set_VNC_x509_Folder_Path( ui.Edit_x509_Folder->text() );
	
	// Use x509verify
	tmp_vm.Use_VNC_x509verify( ui.CH_x509verify_Folder->isChecked() );
	
	// x509 Folder
	tmp_vm.Set_VNC_x509verify_Folder_Path( ui.Edit_x509verify_Folder->text() );
	
	// No_Use_Embedded_Display
	tmp_vm.Use_No_Use_Embedded_Display( ui.CH_No_Use_Embedded_Display->isChecked() );
	
	return true;
}

bool Main_Window::Load_Settings()
{
	AQDebug( "bool Main_Window::Load_Settings()", "Begin" );
	
	// Main Window Size
	resize( Settings.value("General_Window_Width", "670").toInt(),
			Settings.value("General_Window_Height", "625").toInt() );
	
	// Main Window Position
	move( Settings.value("General_Window_Position", QPoint(300, 300)).toPoint() );
	
	// Splitter
	ui.splitter->restoreState( Settings.value("General_Splitter",
							   QByteArray("\0\0\0\xff\0\0\0\0\0\0\0\x2\0\0\0\xbc\0\0\x2$\0\0\0\0\x4\x1\0\0\0\x1")).toByteArray() );
	
	// VM Icons Size
	ui.Machines_List->setIconSize( QSize( Settings.value("VM_Icons_Size", "64").toInt(),
								   Settings.value("VM_Icons_Size", "64").toInt()) );
	
	// Load CD Exists Images List
	VM_Folder = Settings.value("VM_Directory", "~").toString();
	Load_Recent_Images_List();
	
	if( Settings.status() == QSettings::NoError )
	{
		// Apply Settings...
		if( Settings.value( "Use_Device_Manager", "yes" ).toString() == "yes" )
		{
			if( ui.Tabs->indexOf(ui.Tab_HDD) != -1 ) // delete
			{
				ui.Tabs->removeTab( ui.Tabs->indexOf(ui.Tab_HDD) );
			}
			
			if( ui.Tabs->indexOf(ui.Tab_Removable_Disks) != -1 ) // delete
			{
				ui.Tabs->removeTab( ui.Tabs->indexOf(ui.Tab_Removable_Disks) );
			}
			
			if( ui.Tabs->indexOf(Dev_Manager) == -1 ) // add
			{
				ui.Tabs->insertTab( 2, Dev_Manager, tr("Device Manager") );
			}
			
			if( ui.Machines_List->count() > 0 ) Update_VM_Ui();
		}
		else
		{
			if( ui.Tabs->indexOf(Dev_Manager) != -1 ) // delete
			{
				ui.Tabs->removeTab( ui.Tabs->indexOf(Dev_Manager) );
			}
			
			if( ui.Tabs->indexOf(ui.Tab_HDD) == -1 ) // add
			{
				ui.Tabs->insertTab( 2, ui.Tab_HDD, tr("HDD") );
			}
			
			if( ui.Tabs->indexOf(ui.Tab_Removable_Disks) == -1 ) // add
			{
				ui.Tabs->insertTab( 3, ui.Tab_Removable_Disks, tr("CD/DVD/Floppy") );
			}
			
			// update devices list for floppy and cdrom
			QStringList fd_list = System_Info::Get_Host_FDD_List();
			
			ui.CB_FD0_Host_Devices->clear();
			ui.CB_FD1_Host_Devices->clear();
			
			if( fd_list.count() < 1 )
			{
				AQDebug( "bool Main_Window::Load_Settings()",
						 "Cannot Find Host Floppy Devices!" );
				
				ui.RB_FD0_Use_Host_Device->setEnabled( false );
				ui.CB_FD0_Host_Devices->setEnabled( false );
				
				ui.RB_FD1_Use_Host_Device->setEnabled( false );
				ui.CB_FD1_Host_Devices->setEnabled( false );
			}
			else
			{
				for( int d = 0; d < fd_list.count(); ++d )
				{
					ui.CB_FD0_Host_Devices->addItem( fd_list[d] );
					ui.CB_FD1_Host_Devices->addItem( fd_list[d] );
				}
			}
			
			// CD-ROM
			QStringList cd_list = System_Info::Get_Host_CDROM_List();
			
			ui.CB_CDROM_Host_Devices->clear();
			
			if( cd_list.count() < 1 )
			{
				AQDebug( "bool Main_Window::Load_Settings()",  "Cannot Find Host CD-ROM Devices!" );
				
				ui.RB_CDROM_Use_Host_Device->setEnabled( false );
				ui.CB_CDROM_Host_Devices->setEnabled( false );
			}
			else
			{
				for( int d = 0; d < cd_list.count(); ++d ) ui.CB_CDROM_Host_Devices->addItem( cd_list[d] );
			}
			
			Connect_Signals();
			
			if( ui.Machines_List->count() > 0 ) Update_VM_Ui();
		}
		
		return true;
	}
	else
	{
		AQError( "bool Main_Window::Load_Settings()", "Settings.status() != QSettings::NoError" );
		return false;
	}
}

bool Main_Window::Save_Settings()
{
	// Current VM Index
	Settings.setValue( "Current_VM_Index", ui.Machines_List->currentRow() );
	
	// Save Windows Size
	Settings.setValue( "General_Window_Width", QString::number(this->width()) );
	Settings.setValue( "General_Window_Height", QString::number(this->height()) );
	
	// Save Main Window Position
	Settings.setValue( "General_Window_Position", pos() );
	
	// Splitter
	Settings.setValue( "General_Splitter", ui.splitter->saveState() );
	
	// Save
	Settings.sync();
	
	if( Settings.status() == QSettings::NoError ) return true;
	else return false;
}

bool Main_Window::Load_Virtual_Machines()
{
	AQDebug( "bool Main_Window::Load_Virtual_Machines()", "Begin" );
	
	QDir vm_dir( Settings.value("VM_Directory", "~").toString() );
	QFileInfoList fil = vm_dir.entryInfoList( QStringList("*.aqemu"), QDir::Files, QDir::Name );
	
	if( fil.count() <= 0 ) return false;
	
	int real_index = 0;
	for( int ix = 0; ix < fil.count(); ix++ )
	{
		// Check Permissions
		if( ! fil[ix].isWritable() )
		{
			AQGraphic_Error( "bool Main_Window::Load_Virtual_Machines()", tr("Error!"),
							 tr("VM File \"") + fil[ix].filePath() + tr("\" is Read Only!\nCheck Permissions!"), true );
		}
		
		Virtual_Machine new_vm;
		
		if( ! new_vm.Load_VM(fil[ix].filePath()) )
		{
			--real_index;
		}
		else
		{
			VM_List << new_vm;
			
			QObject::connect( &VM_List[real_index], SIGNAL(State_Changet(Virtual_Machine*, VM::VM_State)),
							  this, SLOT(VM_State_Changet(Virtual_Machine*, VM::VM_State)) );
			
			new QListWidgetItem( new_vm.Get_Machine_Name(), ui.Machines_List );
			
			// Load OS Logo or OS Screenshot Icon
			if( new_vm.Get_State() == VM::VMS_Saved &&
				Settings.value("Use_Screenshot_for_OS_Logo", "yes").toString() == "yes" )
			{
				if( QFile::exists(new_vm.Get_Screenshot_Path()) )
				{
					ui.Machines_List->item(real_index)->setIcon( QIcon(new_vm.Get_Screenshot_Path()) );
				}
				else
				{
					// Screenshot File Not Found? Use OS Icon.
					ui.Machines_List->item(real_index)->setIcon( QIcon(new_vm.Get_Icon_Path()) );
				}
			}
			else
			{
				ui.Machines_List->item(real_index)->setIcon( QIcon(new_vm.Get_Icon_Path()) );
			}
		}
		
		++real_index;
	}
	
	// Set last used vm
	int cur_row = Settings.value( "Current_VM_Index", 0 ).toInt();
	
	if( cur_row >= 0 )
	{
		if( cur_row < ui.Machines_List->count() )
		{
			ui.Machines_List->setCurrentRow( cur_row );
		}
		else
		{
			AQWarning( "bool Main_Window::Load_Virtual_Machines()", "cur_row > ui.Machines_List->count()" );
			ui.Machines_List->setCurrentRow( 0 );
		}
	}
	else
	{
		AQWarning( "bool Main_Window::Load_Virtual_Machines()", "cur_row < 0" );
		ui.Machines_List->setCurrentRow( 0 );
	}
	
	Update_VM_Ui();
	
	AQDebug( "bool Main_Window::Load_Virtual_Machines()", "End" );
	return true;
}

bool Main_Window::Save_Virtual_Machines()
{
	return true;
}

void Main_Window::Update_VM_Ui()
{
	AQDebug( "void Main_Window::Update_VM_Ui()", "Begin" );
	
	Update_VM_Port_Number();
	
	if( ui.Machines_List->currentRow() < 0 )
	{
		AQGraphic_Error( "void Main_Window::Update_VM_Ui()", tr("Critical Error!"),
						 tr("VM Index Out of Range! Close AQEMU?"), true );
	}
	
	Virtual_Machine &tmp_vm = VM_List[ ui.Machines_List->currentRow() ];
	
	// Machine Name
	ui.Edit_Machine_Name->setText( tmp_vm.Get_Machine_Name() );
	
	Show_State( &VM_List[ui.Machines_List->currentRow()], VM_List[ui.Machines_List->currentRow()].Get_State() );
	if( VM_List[ui.Machines_List->currentRow()].Get_State() == VM::VMS_In_Error ) return;
	
	// Emulator Type
	if( ui.CB_Emulator_Type->count() <= 0 )
	{
		AQError( "void Main_Window::Update_VM_Ui()", "ui.CB_Emulator_Type->count() <= 0" );
		return;
	}
	
	ui.CB_Emulator_Type->setCurrentIndex( 0 );
	
	for( int ix = 0; ix < ui.CB_Emulator_Type->count(); ix++ )
	{
		if( ui.CB_Emulator_Type->itemText(ix) == tmp_vm.Get_Emulator_Type() )
		{
			ui.CB_Emulator_Type->setCurrentIndex( ix );
			break;
		}
	}
	
	// Emulator Version
	if( ui.CB_Emulator_Type->count() <= 0 )
	{
		AQError( "void Main_Window::Update_VM_Ui()", "ui.CB_Emulator_Type->count() <= 0" );
		return;
	}
	
	ui.CB_Emulator_Version->setCurrentIndex( 0 );
	
	for( int ix = 0; ix < ui.CB_Emulator_Version->count(); ix++ )
	{
		if( ui.CB_Emulator_Version->itemText(ix) == tmp_vm.Get_Emulator().Get_Name() )
		{
			ui.CB_Emulator_Version->setCurrentIndex( ix );
			break;
		}
	}
	
	// Computer Type
	Averable_Devices* cur_comp;
	QString tmp_str = tmp_vm.Get_Computer_Type();
	int dev_count = Get_Devices_Info().count();
	
	for( int cx = 0; cx < dev_count; ++cx )
	{
		Averable_Devices* tmp_comp = &Get_Devices_Info()[cx];
		
		if( tmp_str == tmp_comp->System.QEMU_Name )
		{
			ui.CB_Computer_Type->setCurrentIndex( cx );
			cur_comp = tmp_comp;
			break;
		}
	}
	
	// Machine Type
	tmp_str = tmp_vm.Get_Machine_Type();
	for( int mx = 0; mx < cur_comp->Machine_List.count(); ++mx )
	{
		if( tmp_str == cur_comp->Machine_List[mx].QEMU_Name )
		{
			ui.CB_Machine_Type->setCurrentIndex( mx );
			break;
		}
	}
	
	// CPU Type
	tmp_str = tmp_vm.Get_CPU_Type();
	for( int cx = 0; cx < cur_comp->CPU_List.count(); ++cx )
	{
		if( tmp_str == cur_comp->CPU_List[cx].QEMU_Name )
		{
			ui.CB_CPU_Type->setCurrentIndex( cx );
			break;
		}
	}
	
	// Video Card
	tmp_str = tmp_vm.Get_Video_Card();
	for( int vx = 0; vx < cur_comp->Video_Card_List.count(); ++vx )
	{
		if( tmp_str == cur_comp->Video_Card_List[vx].QEMU_Name )
		{
			ui.CB_Video_Card->setCurrentIndex( vx );
			break;
		}
	}
	
	// Count CPU's
	switch( tmp_vm.Get_SMP_CPU_Count() )
	{
		case 1:
			ui.CB_CPU_Count->setCurrentIndex( 0 );
			break;
			
		case 2:
			ui.CB_CPU_Count->setCurrentIndex( 1 );
			break;
			
		case 4:
			ui.CB_CPU_Count->setCurrentIndex( 2 );
			break;
			
		case 8:
			ui.CB_CPU_Count->setCurrentIndex( 3 );
			break;
			
		case 16:
			ui.CB_CPU_Count->setCurrentIndex( 4 );
			break;
			
		case 32:
			ui.CB_CPU_Count->setCurrentIndex( 5 );
			break;
			
		case 64:
			ui.CB_CPU_Count->setCurrentIndex( 6 );
			break;
			
		case 128:
			ui.CB_CPU_Count->setCurrentIndex( 7 );
			break;
			
		case 255:
			ui.CB_CPU_Count->setCurrentIndex( 8 );
			break;
			
		default:
			ui.CB_CPU_Count->setCurrentIndex( 0 );
			AQError( "void Main_Window::Update_VM_Ui()",
					 "CPU_Count Default Section!" );
			break;
	}
	
	// Keyboard Layout
	int lang_index = ui.CB_Keyboard_Layout->findText( tmp_vm.Get_Keyboard_Layout() );
	
	if( lang_index >= 0 && lang_index < ui.CB_Keyboard_Layout->count() )
	{
		ui.CB_Keyboard_Layout->setCurrentIndex( lang_index );
	}
	else
	{
		ui.CB_Keyboard_Layout->setCurrentIndex( 0 ); // default lang
	}
	
	// Boot
	switch( tmp_vm.Get_Boot_Device() )
	{
		case VM::Boot_From_FDD:
			ui.CB_Boot_Prioritet->setCurrentIndex( 0 );
			break;
			
			
		case VM::Boot_From_HDD:
			ui.CB_Boot_Prioritet->setCurrentIndex( 1 );
			break;
			
		case VM::Boot_From_CDROM:
			ui.CB_Boot_Prioritet->setCurrentIndex( 2 );
			break;
			
		case VM::Boot_From_Network:
			ui.CB_Boot_Prioritet->setCurrentIndex( 3 );
			break;
			
		case VM::Boot_None:
			ui.CB_Boot_Prioritet->setCurrentIndex( 4 );
			break;
			
		default:
			ui.CB_Boot_Prioritet->setCurrentIndex( 1 );
			AQError( "void Main_Window::Update_VM_Ui()",
					 "Boot_Prioritet Default Section!" );
			break;
	}
	
	// Aceleration
	switch( tmp_vm.Get_KQEMU_Mode() )
	{
		case VM::KQEMU_Default:
			ui.RB_KQEMU_Use_if_Possible->setChecked( true );
			break;
		
		case VM::KQEMU_Disabled:
			ui.RB_KQEMU_Disabled->setChecked( true );
			break;
			
		case VM::KQEMU_Enabled:
			ui.RB_KQEMU_Enabled->setChecked( true );
			break;
			
		case VM::KQEMU_Full:
			ui.RB_KQEMU_Full->setChecked( true );
			break;
			
		default:
			ui.RB_KQEMU_Use_if_Possible->setChecked( true );
			AQError( "void Main_Window::Update_VM_Ui()",
					 "Acseleration Default Section!" );
			break;
	}
	
	// Audio Cards
	if( tmp_vm.Get_Audio_Cards().Audio_sb16 ) ui.CH_sb16->setChecked( true );
	else ui.CH_sb16->setChecked( false );
	
	if( tmp_vm.Get_Audio_Cards().Audio_es1370 ) ui.CH_es1370->setChecked( true );
	else ui.CH_es1370->setChecked( false );
	
	if( tmp_vm.Get_Audio_Cards().Audio_Adlib ) ui.CH_Adlib->setChecked( true );
	else ui.CH_Adlib->setChecked( false );
	
	if( tmp_vm.Get_Audio_Cards().Audio_PC_Speaker ) ui.CH_PCSPK->setChecked( true );
	else ui.CH_PCSPK->setChecked( false );
	
	if( tmp_vm.Get_Audio_Cards().Audio_GUS ) ui.CH_GUS->setChecked( true );
	else ui.CH_GUS->setChecked( false );
	
	if( tmp_vm.Get_Audio_Cards().Audio_AC97 ) ui.CH_AC97->setChecked( true );
	else ui.CH_AC97->setChecked( false );
	
	// RAM
	ui.Memory_Size->setValue( tmp_vm.Get_Memory_Size() );
	ui.Check_Host_Mem->setChecked( tmp_vm.Get_Check_Free_RAM() );
	on_Check_Host_Mem_stateChanged( ui.Check_Host_Mem->checkState() );
	
	// General Tab. Options
	ui.CH_Fullscreen->setChecked( tmp_vm.Use_Fullscreen_Mode() );
	ui.CH_ACPI->setChecked( tmp_vm.Use_ACPI() );
	ui.CH_Snapshot->setChecked( tmp_vm.Use_Snapshot_Mode() );
	ui.CH_FDD_Boot->setChecked( tmp_vm.Use_Check_FDD_Boot_Sector() );
	ui.CH_Local_Time->setChecked( tmp_vm.Use_Local_Time() );
	ui.CH_Win2K_Hack->setChecked( tmp_vm.Use_Win2K_Hack() );
	
	// Use Device Manager Mode
	if( Settings.value("Use_Device_Manager", "").toString() == "yes" )
	{
		Dev_Manager->Set_VM( VM_List[ui.Machines_List->currentRow()] );
	}
	else
	{
		// Floppy 0
		if( ui.CB_FD0_Host_Devices->count() < 1 )
		{
			ui.RB_FD0_Use_Host_Device->setChecked( false );
			ui.RB_FD0_Use_Image_File->setChecked( true );
		}
		else
		{
			ui.RB_FD0_Use_Host_Device->setChecked( tmp_vm.Get_FD0().Get_Host_Device() );
			ui.RB_FD0_Use_Image_File->setChecked( ! tmp_vm.Get_FD0().Get_Host_Device() );
			
			int fd0_ix = ui.CB_FD0_Host_Devices->findText( tmp_vm.Get_FD0().Get_Host_File_Name() );
			
			if( fd0_ix != -1 )
			{
				ui.CB_FD0_Host_Devices->setCurrentIndex( fd0_ix );
			}
		}
		
		ui.Edit_FD0_Image_File_Path->setText( tmp_vm.Get_FD0().Get_Image_File_Name() );
		
		ui.GB_Floppy0->setChecked( tmp_vm.Get_FD0().Get_Enabled() );
		
		// Floppy 1
		if( ui.CB_FD1_Host_Devices->count() < 1 )
		{
			ui.RB_FD1_Use_Host_Device->setChecked( false );
			ui.RB_FD1_Use_Image_File->setChecked( true );
		}
		else
		{
			ui.RB_FD1_Use_Host_Device->setChecked( tmp_vm.Get_FD0().Get_Host_Device() );
			ui.RB_FD1_Use_Image_File->setChecked( ! tmp_vm.Get_FD1().Get_Host_Device() );
			
			int fd1_ix = ui.CB_FD1_Host_Devices->findText( tmp_vm.Get_FD1().Get_Host_File_Name() );
			
			if( fd1_ix != -1 )
			{
				ui.CB_FD1_Host_Devices->setCurrentIndex( fd1_ix );
			}
		}
		
		ui.Edit_FD1_Image_File_Path->setText( tmp_vm.Get_FD1().Get_Image_File_Name() );
		
		ui.GB_Floppy1->setChecked( tmp_vm.Get_FD1().Get_Enabled() );
		
		// CD-ROM
		if( ui.CB_CDROM_Host_Devices->count() < 1 )
		{
			ui.RB_CDROM_Use_Host_Device->setChecked( false );
			ui.RB_CDROM_Use_Image_File->setChecked( true );
		}
		else
		{
			ui.RB_CDROM_Use_Host_Device->setChecked( tmp_vm.Get_CD_ROM().Get_Host_Device() );
			ui.RB_CDROM_Use_Image_File->setChecked( ! tmp_vm.Get_CD_ROM().Get_Host_Device() );
			
			int cd_ix = ui.CB_CDROM_Host_Devices->findText( tmp_vm.Get_CD_ROM().Get_Host_File_Name() );
			
			if( cd_ix != -1 )
			{
				ui.CB_CDROM_Host_Devices->setCurrentIndex( cd_ix );
			}
		}
		
		ui.Edit_CDROM_Image_File_Path->setText( tmp_vm.Get_CD_ROM().Get_Image_File_Name() );
		
		ui.GB_CDROM->setChecked( tmp_vm.Get_CD_ROM().Get_Enabled() );
		
		ui.GB_HDA->setChecked( tmp_vm.Get_HDA().Get_Enabled() );
		ui.Edit_HDA_Image_Path->setText( tmp_vm.Get_HDA().Get_Image_File_Name() );
		
		ui.GB_HDB->setChecked( tmp_vm.Get_HDB().Get_Enabled() );
		ui.Edit_HDB_Image_Path->setText( tmp_vm.Get_HDB().Get_Image_File_Name() );
		
		ui.GB_HDC->setChecked( tmp_vm.Get_HDC().Get_Enabled() );
		ui.Edit_HDC_Image_Path->setText( tmp_vm.Get_HDC().Get_Image_File_Name() );
		
		ui.GB_HDD->setChecked( tmp_vm.Get_HDD().Get_Enabled() );
		ui.Edit_HDD_Image_Path->setText( tmp_vm.Get_HDD().Get_Image_File_Name() );
		
		HDA_Info->Update_Disk_Info( tmp_vm.Get_HDA().Get_Image_File_Name() );
		HDB_Info->Update_Disk_Info( tmp_vm.Get_HDB().Get_Image_File_Name() );
		HDC_Info->Update_Disk_Info( tmp_vm.Get_HDC().Get_Image_File_Name() );
		HDD_Info->Update_Disk_Info( tmp_vm.Get_HDD().Get_Image_File_Name() );
	}
	
	// Network tab. Redirections
	
	// Remove all rows...
	while( ui.Redirections_List->rowCount() > 0 ) ui.Redirections_List->removeRow( 0 );
	
	// Add values
	for( int rx = 0; rx < tmp_vm.Get_Network_Redirections_Count(); rx++ )
	{
		ui.Redirections_List->insertRow( ui.Redirections_List->rowCount() );
		
		// protocol
		QTableWidgetItem *newItem = new QTableWidgetItem( tmp_vm.Get_Network_Redirection(rx).Get_Protocol() );
		ui.Redirections_List->setItem( ui.Redirections_List->rowCount()-1, 0, newItem );
		
		// host port
		newItem = new QTableWidgetItem( QString::number(tmp_vm.Get_Network_Redirection(rx).Get_Host_Port()) );
		ui.Redirections_List->setItem( ui.Redirections_List->rowCount()-1, 1, newItem );
		
		// ip
		newItem = new QTableWidgetItem( tmp_vm.Get_Network_Redirection(rx).Get_Guest_IP() );
		ui.Redirections_List->setItem( ui.Redirections_List->rowCount()-1, 2, newItem );
		
		// guest port
		newItem = new QTableWidgetItem( QString::number(tmp_vm.Get_Network_Redirection(rx).Get_Guest_Port()) );
		ui.Redirections_List->setItem( ui.Redirections_List->rowCount()-1, 3, newItem );
		
		// set focus to new row
		ui.Redirections_List->setCurrentCell( ui.Redirections_List->rowCount()-1 , 0 );
	}
	
	Old_Network_Settings_Widget->Set_Network_Card_Models( cur_comp->Network_Card_List );
	Old_Network_Settings_Widget->Set_Network_Cards( tmp_vm.Get_Network_Cards() );
	
	New_Network_Settings_Widget->Set_Network_Card_Models( cur_comp->Network_Card_List );
	New_Network_Settings_Widget->Set_Network_Cards( tmp_vm.Get_Network_Cards_Nativ() );
	
	// Use Nativ Network Cards
	ui.RB_Network_Mode_New->setChecked( tmp_vm.Use_Nativ_Network() );
	ui.RB_Network_Mode_Old->setChecked( ! tmp_vm.Use_Nativ_Network() );
	on_RB_Network_Mode_New_toggled( ui.RB_Network_Mode_New->isChecked() );
	
	ui.Edit_TFTP_Prefix->setText( tmp_vm.Get_TFTP_Prefix() );
	ui.Edit_SMB_Folder->setText( tmp_vm.Get_SMB_Directory() );
	
	ui.CH_Redirections->setChecked( ! tmp_vm.Get_Use_Redirections() );
	ui.CH_Redirections->setChecked( tmp_vm.Get_Use_Redirections() );
	ui.CH_Use_Network->setChecked( ! tmp_vm.Get_Use_Network() );
	ui.CH_Use_Network->setChecked( tmp_vm.Get_Use_Network() );
	
	// Ports Tab
	Ports_Tab->Clear_Old_Ports();
	Ports_Tab->Set_Serial_Ports( tmp_vm.Get_Serial_Ports() );
	Ports_Tab->Set_Parallel_Ports( tmp_vm.Get_Parallel_Ports() );
	Ports_Tab->Set_USB_Ports( tmp_vm.Get_USB_Ports() );
	
	// Advanced Tab
	// Additional Options
	ui.CH_QEMU_Log->setChecked( tmp_vm.Use_QEMU_Log() );
	ui.CH_RTC_TD_Hack->setChecked( tmp_vm.Use_RTC_TD_Hack() );
	ui.CH_No_Shutdown->setChecked( tmp_vm.Use_No_Shutdown() );
	ui.CH_No_Reboot->setChecked( tmp_vm.Use_No_Reboot() );
	ui.CH_Start_CPU->setChecked( tmp_vm.Use_Start_CPU() );
	
	// Start Date
	ui.CH_Start_Date->setChecked( tmp_vm.Use_Start_Date() );
	ui.DTE_Start_Date->setDateTime( tmp_vm.Get_Start_Date() );
	
	// GDB
	ui.CH_Use_GDB->setChecked( tmp_vm.Use_GDB());
	ui.SB_GDB_Port->setValue( (int)tmp_vm.Get_GDB_Port() );
	
	// Additional Arguments
	ui.Edit_Additional_Args->setPlainText( tmp_vm.Get_Additional_Args() );
	
	// Only_User_Args
	ui.CH_Only_User_Args->setChecked( tmp_vm.Get_Only_User_Args() );
	
	// Use_User_Emulator_Binary
	ui.CH_Use_User_Binary->setChecked( tmp_vm.Get_Use_User_Emulator_Binary() );
	
	// QEMU/KVM Window Option
	
	// Show QEMU Window Without a Frame and Window Decorations
	ui.CH_No_Frame->setChecked( tmp_vm.Use_No_Frame() );
	
	// Use Ctrl-Alt-Shift to Grab Mouse (Instead of Ctrl-Alt)
	ui.CH_Alt_Grab->setChecked( tmp_vm.Use_Alt_Grab() );
	
	// Disable SDL Window Close Capability
	ui.CH_No_Quit->setChecked( tmp_vm.Use_No_Quit() );
	
	// Rotate Graphical Output 90 Deg Left (Only PXA LCD)
	ui.CH_Portrait->setChecked( tmp_vm.Use_Portrait() );
	
	// Curses
	ui.CH_Curses->setChecked( tmp_vm.Use_Curses() );
	
	// Show_Cursor
	ui.CH_Show_Cursor->setChecked( tmp_vm.Use_Show_Cursor() );
	
	// Initial Graphical Mode
	ui.CH_Init_Graphic_Mode->setChecked( tmp_vm.Get_Init_Graphic_Mode().Get_Enabled() );
	ui.SB_InitGM_Width->setValue( tmp_vm.Get_Init_Graphic_Mode().Get_Width() );
	ui.SB_InitGM_Height->setValue( tmp_vm.Get_Init_Graphic_Mode().Get_Height() );
	
	switch( tmp_vm.Get_Init_Graphic_Mode().Get_Depth() )
	{
		case 8:
			ui.CB_InitGM_Depth->setCurrentIndex( 0 );
			break;
			
		case 16:
			ui.CB_InitGM_Depth->setCurrentIndex( 1 );
			break;
			
		case 24:
			ui.CB_InitGM_Depth->setCurrentIndex( 2 );
			break;
			
		case 32:
			ui.CB_InitGM_Depth->setCurrentIndex( 3 );
			break;
			
		default:
			AQError( "void Main_Window::Update_VM_Ui()",
					 "Initial Graphical Mode: Default Section!" );
			ui.CB_InitGM_Depth->setCurrentIndex( 2 );
			break;
	}
	
	// Other tab
	ui.CH_Use_Linux_Boot->setChecked( tmp_vm.Get_Use_Linux_Boot() );
	ui.Edit_Linux_bzImage_Path->setText( tmp_vm.Get_bzImage_Path() );
	ui.Edit_Linux_Initrd_Path->setText( tmp_vm.Get_Initrd_Path() );
	ui.Edit_Linux_Command_Line->setText( tmp_vm.Get_Kernel_ComLine() );
	
	// ROM File
	ui.CH_ROM_File->setChecked( tmp_vm.Get_Use_ROM_File() );
	ui.Edit_ROM_File->setText( tmp_vm.Get_ROM_File() );
	
	// On-Board Flash Image
	ui.CH_MTDBlock->setChecked( tmp_vm.Use_MTDBlock_File() );
	ui.Edit_MTDBlock_File->setText( tmp_vm.Get_MTDBlock_File() );
	
	// SecureDigital Card Image
	ui.CH_SD_Image->setChecked( tmp_vm.Use_SecureDigital_File() );
	ui.Edit_SD_Image_File->setText( tmp_vm.Get_SecureDigital_File() );
	
	// Parallel Flash Image
	ui.CH_PFlash->setChecked( tmp_vm.Use_PFlash_File() );
	ui.Edit_PFlash_File->setText( tmp_vm.Get_PFlash_File() );
	
	// Disable KVM kernel mode PIC/IOAPIC/LAPIC
	ui.CH_No_KVM_IRQChip->setChecked( tmp_vm.Use_KVM_IRQChip() );
	
	// Disable KVM kernel mode PIT
	ui.CH_No_KVM_Pit->setChecked( tmp_vm.Use_No_KVM_Pit() );
	
	// KVM_No_Pit_Reinjection
	ui.CH_KVM_No_Pit_Reinjection->setChecked( tmp_vm.Use_KVM_No_Pit_Reinjection() );
	
	// KVM_Nesting
	ui.CH_KVM_Nesting->setChecked( tmp_vm.Use_KVM_Nesting() );
	
	// KVM Shadow Memory
	ui.CH_KVM_Shadow_Memory->setChecked( tmp_vm.Use_KVM_Shadow_Memory() );
	ui.SB_KVM_Shadow_Memory_Size->setValue( tmp_vm.Get_KVM_Shadow_Memory_Size() );
	
	// VNC
	ui.CH_Activate_VNC->setChecked( tmp_vm.Use_VNC() );
	
	// Use Unix Socket Mode for VNC
	ui.RB_VNC_Unix_Socket->setChecked( tmp_vm.Get_VNC_Socket_Mode() );
	
	// UNIX Domain Socket Path
	ui.Edit_VNC_Unix_Socket->setText( tmp_vm.Get_VNC_Unix_Socket_Path() );
	
	// VNC Display Number
	ui.SB_VNC_Display->setValue( tmp_vm.Get_VNC_Display_Number() );
	
	// Use Password for VNC
	ui.CH_VNC_Password->setChecked( tmp_vm.Use_VNC_Password() );
	
	// Use TLS
	ui.CH_Use_VNC_TLS->setChecked( tmp_vm.Use_VNC_TLS() );
	
	// Use x509
	ui.CH_x509_Folder->setChecked( tmp_vm.Use_VNC_x509() );
	
	// x509 Folder
	ui.Edit_x509_Folder->setText( tmp_vm.Get_VNC_x509_Folder_Path() );
	
	// Use x509verify
	ui.CH_x509verify_Folder->setChecked( tmp_vm.Use_VNC_x509verify() );
	
	// x509 Folder
	ui.Edit_x509verify_Folder->setText( tmp_vm.Get_VNC_x509verify_Folder_Path() );
	
	// No_Use_Embedded_Display
	if( Settings.value("Use_VNC_Display", "yes").toString() == "yes" )
	{
		ui.Label_VNC_Warning->setVisible( true );
		ui.CH_No_Use_Embedded_Display->setVisible( true );
	}
	else
	{
		ui.Label_VNC_Warning->setVisible( false );
		ui.CH_No_Use_Embedded_Display->setVisible( false );
	}
	
	ui.CH_No_Use_Embedded_Display->setChecked( tmp_vm.Use_No_Use_Embedded_Display() );
	
	Update_Info_Text();
	//Update_Disabled_Controls();
	
	// For VM Changes Signals
	ui.Button_Apply->setEnabled( false );
	ui.Button_Cancel->setEnabled( false );
	
	AQDebug( "void Main_Window::Update_VM_Ui()", "End" );
}

void Main_Window::Update_VM_Port_Number()
{
	for( int ix = 0; ix < VM_List.count(); ++ix )
	{
		VM_List[ ix ].Set_Embedded_Display_Port( ix );
	}
}

void Main_Window::Update_Info_Text( int info_mode )
{
	if( info_mode != 0 )
	{
		// This for Tab Info Backgroud Color
		ui.VM_Information_Text->clear();
		QPalette qpal;
		qpal.color(QPalette::Window).name();
		ui.VM_Information_Text->setHtml( "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">p, li { white-space: pre-wrap; }</style></head><body bgcolor=\"" + qpal.color(QPalette::Window).name() + "\" style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>" );
		
		QTextCursor cursor = ui.VM_Information_Text->textCursor();
		QTextCharFormat format = QTextCharFormat();
		
		QTextCharFormat bold_format = format;
		bold_format.setFontWeight( QFont::Bold );
		bold_format.setForeground( QColor("#3C4FA1") );
		
		QTextTableFormat table_format;
		
		table_format.setAlignment( Qt::AlignLeft );
		table_format.setCellPadding( 0 );
		table_format.setCellSpacing( 0 );
		
		QVector<QTextLength> constraints;
		constraints << QTextLength( QTextLength::FixedLength, 10 ) << QTextLength( QTextLength::FixedLength, 220 );
		table_format.setColumnWidthConstraints(constraints);
		
		if( info_mode == 1 ) cursor.insertText( tr("You must create a new virtual machine"), bold_format );
		else if( info_mode == 2 ) cursor.insertText( tr("This VM uses an emulator \"%1\" which is not installed in the system.\n"
					"The work of the VM is not possible!").arg( VM_List[ui.Machines_List->currentRow()].Get_Emulator_Type() ), bold_format );
		
		cursor.insertBlock();
		
		return;
	}
	
	ui.VM_Information_Text->clear();
	
	QPalette qpal;
	qpal.color(QPalette::Window).name();
	ui.VM_Information_Text->setHtml( "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">p, li { white-space: pre-wrap; }</style></head><body bgcolor=\"" + qpal.color(QPalette::Window).name() + "\" style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>" );
	
	QTextCursor cursor = ui.VM_Information_Text->textCursor();
	//QTextCharFormat format = cursor.charFormat();
	QTextCharFormat format = QTextCharFormat();
	QTextFrame *topFrame = cursor.currentFrame();
	
	// Use Scrennshot in Save Mode
	if( Settings.value("Info/Show_Screenshot_in_Save_Mode", "no").toString() == "yes" )
	{
		// Find Full Size Screenshot
		QString img_path = Settings.value( "VM_Directory", "~" ).toString() +
				Get_FS_Compatible_VM_Name( VM_List[ui.Machines_List->currentRow()].Get_Machine_Name() ) + "_screenshot";
		
		if( ! QFile::exists(img_path) )
		{
			AQDebug( "void Main_Window::Update_Info_Text()",
					 "Screenshot Path is Empty!" );
		}
		else
		{
			cursor.insertHtml( "<img width=\"" + QString::number(width() - ui.Machines_List->width() -50) + "\" src=\"" + img_path + "\">" );
			
			return;
		}
	}
	
	QTextCharFormat bold_format = format;
	bold_format.setFontWeight( QFont::Bold );
	bold_format.setForeground( QColor("#3C4FA1") );
	
	QTextTableFormat table_format;
	
	table_format.setAlignment( Qt::AlignLeft );
	table_format.setCellPadding( 0 );
	table_format.setCellSpacing( 0 );
	
	QVector<QTextLength> constraints;
	constraints << QTextLength( QTextLength::FixedLength, 10 )
				<< QTextLength( QTextLength::FixedLength, 220 )
				<< QTextLength( QTextLength::VariableLength, 250 );
	table_format.setColumnWidthConstraints(constraints);
	
	// Machine State
	cursor.insertText( tr("Machine Details"), bold_format );
	cursor.insertBlock();
	
	QTextTable *table = cursor.insertTable( 1, 3, table_format );
	
	QTextFrame *frame = cursor.currentFrame();
	
	QTextFrameFormat frame_format = frame->frameFormat();
	frame_format.setBorder( 0 );
	frame->setFrameFormat( frame_format );
	
	QTextTableCell cell;
	QTextCursor cell_cursor;
	
	cell = table->cellAt( table->rows()-1, 1 );
	cell_cursor = cell.firstCursorPosition();
	cell_cursor.insertText( tr("Machine State:"), format );
	
	QString state_text = "";
	
	switch( VM_List[ui.Machines_List->currentRow()].Get_State() )
	{
		case VM::VMS_Running:
			state_text = tr("Running");
			break;
			
		case VM::VMS_Power_Off:
			state_text = tr("Power Off");
			break;
			
		case VM::VMS_Pause:
			state_text = tr("Pause");
			break;
			
		case VM::VMS_Saved:
			state_text = tr("Saved");
			break;
			
		default:
			state_text = tr("Error!");
			break;
	}
	
	cell = table->cellAt( table->rows()-1, 2 );
	cell_cursor = cell.firstCursorPosition();
	cell_cursor.insertText( state_text, format );
	table->insertRows( table->rows(), 1 );
	
	// General Tab
	cursor.setPosition( topFrame->lastPosition() );
	cursor.insertText( tr("General"), bold_format );
	cursor.insertBlock();
	
	table = cursor.insertTable( 1, 3, table_format );
	
	frame = cursor.currentFrame();
	frame->setFrameFormat( frame_format );
	
	if( Settings.value("Info/Emulator_Type", "no").toString() == "yes" )
	{
		cell = table->cellAt( table->rows()-1, 1 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( tr("Emulator Type:"), format );
		
		cell = table->cellAt( table->rows()-1, 2 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( ui.CB_Emulator_Type->currentText(), format );
		table->insertRows( table->rows(), 1 );
	}
	
	if( Settings.value("Info/Emulator_Version", "no").toString() == "yes" )
	{
		cell = table->cellAt( table->rows()-1, 1 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( tr("Emulator Version:"), format );
		
		cell = table->cellAt( table->rows()-1, 2 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( ui.CB_Emulator_Version->currentText(), format );
		table->insertRows( table->rows(), 1 );
	}
	
	if( Settings.value("Info/Computer_Type", "no").toString() == "yes" )
	{
		cell = table->cellAt( table->rows()-1, 1 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( tr("Computer Type:"), format );
		
		cell = table->cellAt( table->rows()-1, 2 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( ui.CB_Computer_Type->currentText(), format );
		table->insertRows( table->rows(), 1 );
	}
	
	if( Settings.value("Info/Machine_Type", "no").toString() == "yes" )
	{
		cell = table->cellAt( table->rows()-1, 1 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( tr("Machine Type:"), format );
		
		cell = table->cellAt( table->rows()-1, 2 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( ui.CB_Machine_Type->currentText(), format );
		table->insertRows( table->rows(), 1 );
	}
	
	if( Settings.value("Info/CPU_Type", "no").toString() == "yes" )
	{
		cell = table->cellAt( table->rows()-1, 1 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( tr("CPU Type:"), format );
		
		cell = table->cellAt( table->rows()-1, 2 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( ui.CB_CPU_Type->currentText(), format );
		table->insertRows( table->rows(), 1 );
	}
	
	if( Settings.value("Info/Memory_Size", "no").toString() == "yes" )
	{
		cell = table->cellAt( table->rows()-1, 1 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( tr("Memory Size:"), format );
		
		cell = table->cellAt( table->rows()-1, 2 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( QString::number(ui.Memory_Size->value()), format );
		table->insertRows( table->rows(), 1 );
	}
	
	if( Settings.value("Info/Number_of_CPU", "no").toString() == "yes" )
	{
		cell = table->cellAt( table->rows()-1, 1 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( tr("Number of CPU:"), format );
		
		cell = table->cellAt( table->rows()-1, 2 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( ui.CB_CPU_Count->currentText(), format );
		table->insertRows( table->rows(), 1 );
	}
	
	if( Settings.value("Info/Acceleration", "no").toString() == "yes" )
	{
		cell = table->cellAt( table->rows()-1, 1 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( tr("Acceleration:"), format );
		
		cell = table->cellAt( table->rows()-1, 2 );
		cell_cursor = cell.firstCursorPosition();
		
		if( ui.RB_KQEMU_Use_if_Possible->isChecked() )
		{
			cell_cursor.insertText( tr("Use If Possible"), format );
		}
		else if( ui.RB_KQEMU_Disabled->isChecked() )
		{
			cell_cursor.insertText( tr("Disabled"), format );
		}
		else if( ui.RB_KQEMU_Enabled->isChecked() )
		{
			cell_cursor.insertText( tr("Enabled"), format );
		}
		else if( ui.RB_KQEMU_Full->isChecked() )
		{
			cell_cursor.insertText( tr("Full"), format );
		}
		else
		{
			AQWarning( "void Main_Window::Update_Info_Text()",
					   "Use Default KQEMU Mode: Default" );
			cell_cursor.insertText( tr("Use If Possible"), format );
		}
		
		table->insertRows( table->rows(), 1 );
	}
	
	if( Settings.value("Info/Boot_Priority", "no").toString() == "yes" )
	{
		cell = table->cellAt( table->rows()-1, 1 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( tr("Boot Priority:"), format );
		
		cell = table->cellAt( table->rows()-1, 2 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( ui.CB_Boot_Prioritet->currentText(), format );
		table->insertRows( table->rows(), 1 );
	}
	
	if( Settings.value("Info/Video_Card", "no").toString() == "yes" )
	{
		cell = table->cellAt( table->rows()-1, 1 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( tr("Video Card:"), format );
		
		cell = table->cellAt( table->rows()-1, 2 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( ui.CB_Video_Card->currentText(), format );
		table->insertRows( table->rows(), 1 );
	}
	
	if( Settings.value("Info/Keyboard_Layout", "no").toString() == "yes" )
	{
		cell = table->cellAt( table->rows()-1, 1 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( tr("Keyboard Layout:"), format );
		
		cell = table->cellAt( table->rows()-1, 2 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( ui.CB_Keyboard_Layout->currentText(), format );
		table->insertRows( table->rows(), 1 );
	}
	
	if( Settings.value("Info/Use_Sound", "no").toString() == "yes" )
	{
		cell = table->cellAt( table->rows()-1, 1 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( tr("Use Sound:"), format );
		
		cell = table->cellAt( table->rows()-1, 2 );
		cell_cursor = cell.firstCursorPosition();
		
		if( ui.CH_sb16->isChecked() || ui.CH_es1370->isChecked() || ui.CH_Adlib->isChecked() ||
			ui.CH_AC97->isChecked() || ui.CH_GUS->isChecked()	 || ui.CH_PCSPK->isChecked() )
		{
			cell_cursor.insertText( tr("Yes"), format );
		}
		else
		{
			cell_cursor.insertText( tr("No"), format );
		}
		
		table->insertRows( table->rows(), 1 );
	}
	
	if( Settings.value("Info/Fullscreen", "no").toString() == "yes" )
	{
		cell = table->cellAt( table->rows()-1, 1 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( tr("Fullscreen Mode:"), format );
		
		cell = table->cellAt( table->rows()-1, 2 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( ui.CH_Fullscreen->isChecked() ? tr("Yes") : tr("No"), format );
		table->insertRows( table->rows(), 1 );
	}
	
	if( Settings.value("Info/ACPI", "no").toString() == "yes" )
	{
		cell = table->cellAt( table->rows()-1, 1 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( tr("Enable ACPI:"), format );
		
		cell = table->cellAt( table->rows()-1, 2 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( ui.CH_ACPI->isChecked() ? tr("Yes") : tr("No"), format );
		table->insertRows( table->rows(), 1 );
	}
	
	if( Settings.value("Info/Localtime", "no").toString() == "yes" )
	{
		cell = table->cellAt( table->rows()-1, 1 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( tr("Use Local Time:"), format );
		
		cell = table->cellAt( table->rows()-1, 2 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( ui.CH_Local_Time->isChecked() ? tr("Yes") : tr("No"), format );
		table->insertRows( table->rows(), 1 );
	}
	
	if( Settings.value("Info/Start_CPU", "no").toString() == "yes" )
	{
		cell = table->cellAt( table->rows()-1, 1 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( tr("Start CPU at Startup:"), format );
		
		cell = table->cellAt( table->rows()-1, 2 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( ui.CH_Start_CPU->isChecked() ? tr("Yes") : tr("No"), format );
		table->insertRows( table->rows(), 1 );
	}
	
	if( Settings.value("Info/Snapshot", "no").toString() == "yes" )
	{
		cell = table->cellAt( table->rows()-1, 1 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( tr("Use Snapshot Mode:"), format );
		
		cell = table->cellAt( table->rows()-1, 2 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( ui.CH_Snapshot->isChecked() ? tr("Yes") : tr("No"), format );
		table->insertRows( table->rows(), 1 );
	}
	
	if( Settings.value("Info/No_Reboot", "no").toString() == "yes" )
	{
		cell = table->cellAt( table->rows()-1, 1 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( tr("No Reboot:"), format );
		
		cell = table->cellAt( table->rows()-1, 2 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( ui.CH_No_Reboot->isChecked() ? tr("Yes") : tr("No"), format );
		table->insertRows( table->rows(), 1 );
	}
	
	// FDD/CD/HDD
	if( Settings.value("Info/Show_FDD", "no").toString() == "yes" ||
		Settings.value("Info/Show_CD", "no").toString() == "yes" ||
		Settings.value("Info/Show_HDD", "no").toString() == "yes" )
	{
		
		cursor.setPosition( topFrame->lastPosition() );
		cursor.insertText( tr("Storage Devices"), bold_format );
		cursor.insertBlock();
		
		table = cursor.insertTable( 1, 3, table_format );
		
		frame = cursor.currentFrame();
		frame->setFrameFormat( frame_format );
		
		if( VM_List[ui.Machines_List->currentRow()].Get_FD0().Get_Enabled() ||
			VM_List[ui.Machines_List->currentRow()].Get_FD1().Get_Enabled() ||
			VM_List[ui.Machines_List->currentRow()].Get_CD_ROM().Get_Enabled() ||
			VM_List[ui.Machines_List->currentRow()].Get_HDA().Get_Enabled() ||
			VM_List[ui.Machines_List->currentRow()].Get_HDB().Get_Enabled() ||
			VM_List[ui.Machines_List->currentRow()].Get_HDC().Get_Enabled() ||
			VM_List[ui.Machines_List->currentRow()].Get_HDD().Get_Enabled() )
		{
			QFileInfo fi;
			
			if( Settings.value("Info/Show_FDD", "no").toString() == "yes" )
			{
				if( VM_List[ui.Machines_List->currentRow()].Get_FD0().Get_Enabled() )
				{
					cell = table->cellAt( table->rows()-1, 1 );
					cell_cursor = cell.firstCursorPosition();
					cell_cursor.insertText( tr("Floppy 1:"), format );
					
					cell = table->cellAt( table->rows()-1, 2 );
					cell_cursor = cell.firstCursorPosition();
					
					if( VM_List[ui.Machines_List->currentRow()].Get_FD0().Get_Host_Device() )
					{
						fi = QFileInfo( VM_List[ui.Machines_List->currentRow()].Get_FD0().Get_Host_File_Name() );
					}
					else
					{
						fi = QFileInfo( VM_List[ui.Machines_List->currentRow()].Get_FD0().Get_Image_File_Name() );
					}
					
					cell_cursor.insertText( fi.fileName(), format );
					table->insertRows( table->rows(), 1 );
				}
				
				if( VM_List[ui.Machines_List->currentRow()].Get_FD1().Get_Enabled() )
				{
					cell = table->cellAt( table->rows()-1, 1 );
					cell_cursor = cell.firstCursorPosition();
					cell_cursor.insertText( tr("Floppy 2:"), format );
					
					cell = table->cellAt( table->rows()-1, 2 );
					cell_cursor = cell.firstCursorPosition();
					
					if( VM_List[ui.Machines_List->currentRow()].Get_FD1().Get_Host_Device() )
					{
						fi = QFileInfo( VM_List[ui.Machines_List->currentRow()].Get_FD1().Get_Host_File_Name() );
					}
					else
					{
						fi = QFileInfo( VM_List[ui.Machines_List->currentRow()].Get_FD1().Get_Image_File_Name() );
					}
					
					cell_cursor.insertText( fi.fileName(), format );
					table->insertRows( table->rows(), 1 );
				}
			}
			
			if( Settings.value("Info/Show_CD", "no").toString() == "yes" )
			{
				if( VM_List[ui.Machines_List->currentRow()].Get_CD_ROM().Get_Enabled() )
				{
					cell = table->cellAt( table->rows()-1, 1 );
					cell_cursor = cell.firstCursorPosition();
					cell_cursor.insertText( tr("CD/DVD-ROM:"), format );
					
					cell = table->cellAt( table->rows()-1, 2 );
					cell_cursor = cell.firstCursorPosition();
					
					if( VM_List[ui.Machines_List->currentRow()].Get_CD_ROM().Get_Host_Device() )
					{
						fi = QFileInfo( VM_List[ui.Machines_List->currentRow()].Get_CD_ROM().Get_Host_File_Name() );
					}
					else
					{
						fi = QFileInfo( VM_List[ui.Machines_List->currentRow()].Get_CD_ROM().Get_Image_File_Name() );
					}
					
					cell_cursor.insertText( fi.fileName(), format );
					table->insertRows( table->rows(), 1 );
				}
			}
			
			if( Settings.value("Info/Show_HDD", "no").toString() == "yes" )
			{
				if( VM_List[ui.Machines_List->currentRow()].Get_HDA().Get_Enabled() )
				{
					cell = table->cellAt( table->rows()-1, 1 );
					cell_cursor = cell.firstCursorPosition();
					cell_cursor.insertText( tr("Primary Master (HDA):"), format );
					
					cell = table->cellAt( table->rows()-1, 2 );
					cell_cursor = cell.firstCursorPosition();
					fi = QFileInfo( VM_List[ui.Machines_List->currentRow()].Get_HDA().Get_Image_File_Name() );
					cell_cursor.insertText( fi.fileName(), format );
					table->insertRows( table->rows(), 1 );
				}
				
				if( VM_List[ui.Machines_List->currentRow()].Get_HDB().Get_Enabled() )
				{
					cell = table->cellAt( table->rows()-1, 1 );
					cell_cursor = cell.firstCursorPosition();
					cell_cursor.insertText( tr("Primary Slave (HDB):"), format );
					
					cell = table->cellAt( table->rows()-1, 2 );
					cell_cursor = cell.firstCursorPosition();
					fi = QFileInfo( VM_List[ui.Machines_List->currentRow()].Get_HDB().Get_Image_File_Name() );
					cell_cursor.insertText( fi.fileName(), format );
					table->insertRows( table->rows(), 1 );
				}
				
				if( VM_List[ui.Machines_List->currentRow()].Get_HDC().Get_Enabled() )
				{
					cell = table->cellAt( table->rows()-1, 1 );
					cell_cursor = cell.firstCursorPosition();
					cell_cursor.insertText( tr("Secondary Master (HDC):"), format );
					
					cell = table->cellAt( table->rows()-1, 2 );
					cell_cursor = cell.firstCursorPosition();
					fi = QFileInfo( VM_List[ui.Machines_List->currentRow()].Get_HDC().Get_Image_File_Name() );
					cell_cursor.insertText( fi.fileName(), format );
					table->insertRows( table->rows(), 1 );
				}
				
				if( VM_List[ui.Machines_List->currentRow()].Get_HDD().Get_Enabled() )
				{
					cell = table->cellAt( table->rows()-1, 1 );
					cell_cursor = cell.firstCursorPosition();
					cell_cursor.insertText( tr("Secondary Slave (HDD):"), format );
					
					cell = table->cellAt( table->rows()-1, 2 );
					cell_cursor = cell.firstCursorPosition();
					fi = QFileInfo( VM_List[ui.Machines_List->currentRow()].Get_HDD().Get_Image_File_Name() );
					cell_cursor.insertText( fi.fileName(), format );
					table->insertRows( table->rows(), 1 );
				}
			}
		}
		else
		{
			// No Devices
			cell = table->cellAt( table->rows()-1, 1 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( tr("No Devices Found"), format );
			cell = table->cellAt( table->rows()-1, 2 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( "", format );
			table->insertRows( table->rows(), 1 );
		}
	}
	
	// Network
	if( Settings.value("Info/Network_Cards", "no").toString() == "yes" &&
		VM_List[ui.Machines_List->currentRow()].Get_Network_Cards_Count() > 0 )
	{
		cursor.setPosition( topFrame->lastPosition() );
		cursor.insertText( tr("Network"), bold_format );
		cursor.insertBlock();
		
		table = cursor.insertTable( 1, 3, table_format );
		
		frame = cursor.currentFrame();
		frame->setFrameFormat( frame_format );
		
		if( ui.CH_Use_Network->isChecked() == false )
		{
			cell = table->cellAt( table->rows()-1, 1 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( tr("Use Network:"), format );
			
			cell = table->cellAt( table->rows()-1, 2 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( tr("No"), format );
			table->insertRows( table->rows(), 1 );
		}
		else
		{
			if( VM_List[ui.Machines_List->currentRow()].Use_Nativ_Network() == false )
			{
				for( int nx = 0; nx < VM_List[ui.Machines_List->currentRow()].Get_Network_Cards_Count(); ++nx )
				{
					cell = table->cellAt( table->rows()-1, 1 );
					cell_cursor = cell.firstCursorPosition();
					cell_cursor.insertText( tr("Card: ") + QString::number(nx+1), format );
					
					QString con_mode = "";
					
					switch( VM_List[ui.Machines_List->currentRow()].Get_Network_Card(nx).Get_Net_Mode() )
					{
						case VM::Net_Mode_Usermode:
							con_mode = tr("User mode network stack");
							break;
						
						case VM::Net_Mode_Tuntap:
							con_mode = tr("TUN/TAP Interface");
							break;
						
						case VM::Net_Mode_Tuntapfd:
							con_mode = tr("TUN/TAP Interface");
							break;
						
						case VM::Net_Mode_Tcplisten:
							con_mode = tr("TCP Socket");
							break;
						
						case VM::Net_Mode_Tcpfd:
							con_mode = tr("TCP Socket");
							break;
						
						case VM::Net_Mode_Tcpconnect:
							con_mode = tr("VLAN");
							break;
						
						case VM::Net_Mode_Multicast:
							con_mode = tr("UDP multicast socket");
							break;
						
						case VM::Net_Mode_Multicastfd:
							con_mode = tr("UDP multicast socket");
							break;
						
						default:
							con_mode = tr("No Connection");
							break;
					}
					
					cell = table->cellAt( table->rows()-1, 2 );
					cell_cursor = cell.firstCursorPosition();
					cell_cursor.insertText( con_mode, format );
					table->insertRows( table->rows(), 1 );
				}
			}
			else
			{
				QList<VM_Net_Card_Nativ> nativ_Cards = VM_List[ ui.Machines_List->currentRow() ].Get_Network_Cards_Nativ();
				
				for( int ix = 0; ix < nativ_Cards.count(); ix++ )
				{
					cell = table->cellAt( table->rows()-1, 1 );
					cell_cursor = cell.firstCursorPosition();
					cell_cursor.insertText( tr("Item %1").arg(ix+1), format );
					
					QString con_mode = "";
					
					switch( nativ_Cards[ix].Get_Network_Type() )
					{
						case VM::Net_Mode_Nativ_NIC:
							con_mode = tr( "NIC" );
							break;
							
						case VM::Net_Mode_Nativ_User:
							con_mode = tr( "User" );
							break;
							
						case VM::Net_Mode_Nativ_Chanel:
							con_mode = tr( "Channel" );
							break;
							
						case VM::Net_Mode_Nativ_TAP:
							con_mode = tr( "TAP" );
							break;
							
						case VM::Net_Mode_Nativ_Socket:
							con_mode = tr( "Socket" );
							break;
							
						case VM::Net_Mode_Nativ_MulticastSocket:
							con_mode = tr( "Multicast Socket" );
							break;
							
						case VM::Net_Mode_Nativ_VDE:
							con_mode = tr( "VDE" );
							break;
							
						case VM::Net_Mode_Nativ_Dump:
							con_mode = tr( "Dump" );
							break;
							
						default:
							con_mode = tr( "Unknown Type Item" );
							break;
					}
					
					cell = table->cellAt( table->rows()-1, 2 );
					cell_cursor = cell.firstCursorPosition();
					cell_cursor.insertText( con_mode, format );
					table->insertRows( table->rows(), 1 );
				}
			}
		}
	}
	
	// Network Redirections
	if( Settings.value("Info/Redirections", "no").toString() == "yes" )
	{
		if( VM_List[ui.Machines_List->currentRow()].Get_Use_Redirections() ||
			VM_List[ui.Machines_List->currentRow()].Get_Network_Redirections_Count() < 1 )
		{
			for( int rx = 0; rx < VM_List[ui.Machines_List->currentRow()].Get_Network_Redirections_Count(); ++rx )
			{
				cell = table->cellAt( table->rows()-1, 1 );
				cell_cursor = cell.firstCursorPosition();
				cell_cursor.insertText( tr("Redirection ") + QString::number(rx+1) + ":", format );
				
				cell = table->cellAt( table->rows()-1, 2 );
				cell_cursor = cell.firstCursorPosition();
				cell_cursor.insertText(
						VM_List[ui.Machines_List->currentRow()].Get_Network_Redirection(rx).Get_Protocol() + " " +
						QString::number(VM_List[ui.Machines_List->currentRow()].Get_Network_Redirection(rx).Get_Host_Port()) + ":" +
						VM_List[ui.Machines_List->currentRow()].Get_Network_Redirection(rx).Get_Guest_IP() + ":" +
						QString::number(VM_List[ui.Machines_List->currentRow()].Get_Network_Redirection(rx).Get_Guest_Port()), format );
				table->insertRows( table->rows(), 1 );
			}
		}
		else
		{
			cell = table->cellAt( table->rows()-1, 1 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( tr("Use Redirections:"), format );
			
			cell = table->cellAt( table->rows()-1, 2 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( tr("No"), format );
			table->insertRows( table->rows(), 1 );
		}
	}
	
	// TFTP Server Prefix
	if( Settings.value("Info/TFTP_Server_Prefix", "no").toString() == "yes" )
	{
		cell = table->cellAt( table->rows()-1, 1 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( tr("TFTP Server Prefix:"), format );
		
		cell = table->cellAt( table->rows()-1, 2 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( ui.Edit_TFTP_Prefix->text() , format );
		table->insertRows( table->rows(), 1 );
	}
	
	// SMB Folder
	if( Settings.value("Info/SMB_Dir", "no").toString() == "yes" )
	{
		cell = table->cellAt( table->rows()-1, 1 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( tr("SMB Folder:"), format );
		
		cell = table->cellAt( table->rows()-1, 2 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( ui.Edit_SMB_Folder->text(), format );
		table->insertRows( table->rows(), 1 );
	}
	
	// Ports Tab
	if( Settings.value("Info/Serial_Port", "no").toString() == "yes" ||
		Settings.value("Info/Parallel_Port", "no").toString() == "yes" ||
		Settings.value("Info/USB_Port", "no").toString() == "yes" )
	{
		cursor.setPosition( topFrame->lastPosition() );
		cursor.insertText( tr("Ports"), bold_format );
		cursor.insertBlock();
		
		table = cursor.insertTable( 1, 3, table_format );
		
		frame = cursor.currentFrame();
		frame->setFrameFormat( frame_format );
		
		if( Settings.value("Info/Serial_Port", "no").toString() == "yes" )
		{
			for( int ix = 0; ix < VM_List[ui.Machines_List->currentRow()].Get_Serial_Ports().count(); ix++ )
			{
				cell = table->cellAt( table->rows()-1, 1 );
				cell_cursor = cell.firstCursorPosition();
				cell_cursor.insertText( tr("Serial Port %1:").arg(ix), format );
				
				cell = table->cellAt( table->rows()-1, 2 );
				cell_cursor = cell.firstCursorPosition();
				
				switch( VM_List[ui.Machines_List->currentRow()].Get_Serial_Ports()[ix].Get_Port_Redirection() )
				{
					case VM::PR_Default:
						cell_cursor.insertText( tr("Default"), format );
						break;
						
					case VM::PR_vc:
						cell_cursor.insertText( tr("vc"), format );
						break;
						
					case VM::PR_pty:
						cell_cursor.insertText( tr("pty"), format );
						break;
						
					case VM::PR_none:
						cell_cursor.insertText( tr("none"), format );
						break;
						
					case VM::PR_null:
						cell_cursor.insertText( tr("null"), format );
						break;
						
					case VM::PR_dev:
						cell_cursor.insertText( tr("dev"), format );
						break;
						
					case VM::PR_host_port:
						cell_cursor.insertText( tr("host_port"), format );
						break;
						
					case VM::PR_file:
						cell_cursor.insertText( tr("file"), format );
						break;
						
					case VM::PR_stdio:
						cell_cursor.insertText( tr("stdio"), format );
						break;
						
					case VM::PR_pipe:
						cell_cursor.insertText( tr("pipe"), format );
						break;
						
					case VM::PR_udp:
						cell_cursor.insertText( tr("udp"), format );
						break;
						
					case VM::PR_tcp:
						cell_cursor.insertText( tr("tcp"), format );
						break;
						
					case VM::PR_telnet:
						cell_cursor.insertText( tr("telnet"), format );
						break;
						
					case VM::PR_unix:
						cell_cursor.insertText( tr("unix"), format );
						break;
						
					default:
						cell_cursor.insertText( tr("Default"), format );
						break;
				}
				
				table->insertRows( table->rows(), 1 );
			}
		}
		
		for( int ix = 0; ix < VM_List[ui.Machines_List->currentRow()].Get_Parallel_Ports().count(); ix++ )
		{
			if( Settings.value("Info/Parallel_Port", "no").toString() == "yes" )
			{
				cell = table->cellAt( table->rows()-1, 1 );
				cell_cursor = cell.firstCursorPosition();
				cell_cursor.insertText( tr("Parallel Port %1:").arg(ix), format );
				
				cell = table->cellAt( table->rows()-1, 2 );
				cell_cursor = cell.firstCursorPosition();
				
				switch( VM_List[ui.Machines_List->currentRow()].Get_Parallel_Ports()[ix].Get_Port_Redirection() )
				{
					case VM::PR_Default:
						cell_cursor.insertText( tr("Default"), format );
						break;
						
					case VM::PR_vc:
						cell_cursor.insertText( tr("vc"), format );
						break;
						
					case VM::PR_pty:
						cell_cursor.insertText( tr("pty"), format );
						break;
						
					case VM::PR_none:
						cell_cursor.insertText( tr("none"), format );
						break;
						
					case VM::PR_null:
						cell_cursor.insertText( tr("null"), format );
						break;
						
					case VM::PR_dev:
						cell_cursor.insertText( tr("dev"), format );
						break;
						
					case VM::PR_host_port:
						cell_cursor.insertText( tr("host_port"), format );
						break;
						
					case VM::PR_file:
						cell_cursor.insertText( tr("file"), format );
						break;
						
					case VM::PR_stdio:
						cell_cursor.insertText( tr("stdio"), format );
						break;
						
					case VM::PR_pipe:
						cell_cursor.insertText( tr("pipe"), format );
						break;
						
					case VM::PR_udp:
						cell_cursor.insertText( tr("udp"), format );
						break;
						
					case VM::PR_tcp:
						cell_cursor.insertText( tr("tcp"), format );
						break;
						
					case VM::PR_telnet:
						cell_cursor.insertText( tr("telnet"), format );
						break;
						
					case VM::PR_unix:
						cell_cursor.insertText( tr("unix"), format );
						break;
						
					default:
						cell_cursor.insertText( tr("Default"), format );
						break;
				}
				
				table->insertRows( table->rows(), 1 );
			}
		}
		
		for( int ix = 0; ix < VM_List[ui.Machines_List->currentRow()].Get_USB_Ports().count(); ix++ )
		{
			if( Settings.value("Info/USB_Port", "no").toString() == "yes" )
			{
				cell = table->cellAt( table->rows()-1, 1 );
				cell_cursor = cell.firstCursorPosition();
				cell_cursor.insertText( tr("USB Port %1:").arg(ix), format );
				
				cell = table->cellAt( table->rows()-1, 2 );
				cell_cursor = cell.firstCursorPosition();
				cell_cursor.insertText( VM_List[ui.Machines_List->currentRow()].Get_USB_Ports()[ix].Get_Manufacturer_Name() + " " +
										VM_List[ui.Machines_List->currentRow()].Get_USB_Ports()[ix].Get_Product_Name(), format );
				table->insertRows( table->rows(), 1 );
			}
		}
	}
	
	// Other Tab
	if( Settings.value("Info/Linux_Boot", "no").toString() == "yes" ||
		Settings.value("Info/ROM_File", "no").toString() == "yes" ||
		Settings.value("Info/MTDBlock", "no").toString() == "yes" ||
		Settings.value("Info/SD_Image", "no").toString() == "yes" ||
		Settings.value("Info/PFlash", "no").toString() == "yes" ||
		Settings.value("Info/GDB_Port", "no").toString() == "yes" )
	{
		cursor.setPosition( topFrame->lastPosition() );
		cursor.insertText( tr("Other"), bold_format );
		cursor.insertBlock();
		
		table = cursor.insertTable( 1, 3, table_format );
		
		frame = cursor.currentFrame();
		frame->setFrameFormat( frame_format );
		
		// Linux Boot
		if( Settings.value("Info/Linux_Boot", "no").toString() == "yes" )
		{
			cell = table->cellAt( table->rows()-1, 1 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( tr("Use Linux Boot:"), format );
			
			cell = table->cellAt( table->rows()-1, 2 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( ui.CH_Use_Linux_Boot->isChecked() ? tr("Yes") : tr("No"), format );
			table->insertRows( table->rows(), 1 );
			
			if( ui.CH_Use_Linux_Boot->isChecked() )
			{
				cell = table->cellAt( table->rows()-1, 1 );
				cell_cursor = cell.firstCursorPosition();
				cell_cursor.insertText( tr("Kernel bzImage:"), format );
				
				cell = table->cellAt( table->rows()-1, 2 );
				cell_cursor = cell.firstCursorPosition();
				QFileInfo lb_tmp = QFileInfo( ui.Edit_Linux_bzImage_Path->text() );
				cell_cursor.insertText( lb_tmp.fileName(), format );
				table->insertRows( table->rows(), 1 );
				
				cell = table->cellAt( table->rows()-1, 1 );
				cell_cursor = cell.firstCursorPosition();
				cell_cursor.insertText( tr("Initrd File:"), format );
				
				cell = table->cellAt( table->rows()-1, 2 );
				cell_cursor = cell.firstCursorPosition();
				lb_tmp = QFileInfo( ui.Edit_Linux_Initrd_Path->text() );
				cell_cursor.insertText( lb_tmp.fileName(), format );
				table->insertRows( table->rows(), 1 );
				
				cell = table->cellAt( table->rows()-1, 1 );
				cell_cursor = cell.firstCursorPosition();
				cell_cursor.insertText( tr("Kernel ComLine:"), format );
				
				cell = table->cellAt( table->rows()-1, 2 );
				cell_cursor = cell.firstCursorPosition();
				cell_cursor.insertText( ui.Edit_Linux_Command_Line->text(), format );
				table->insertRows( table->rows(), 1 );
			}
		}
		
		QFileInfo im_info;
		
		// ROM File
		if( Settings.value("Info/ROM_File", "no").toString() == "yes" )
		{
			cell = table->cellAt( table->rows()-1, 1 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( tr("ROM File:"), format );
			
			cell = table->cellAt( table->rows()-1, 2 );
			cell_cursor = cell.firstCursorPosition();
			
			if( ui.CH_ROM_File->isChecked() )
			{
				im_info = QFileInfo( ui.Edit_ROM_File->text() );
				cell_cursor.insertText( im_info.fileName(), format );
			}
			else
			{
				cell_cursor.insertText( tr("No"), format );
			}
			
			table->insertRows( table->rows(), 1 );
		}
		
		// On-Board Flash Image
		if( Settings.value("Info/MTDBlock", "no").toString() == "yes" )
		{
			cell = table->cellAt( table->rows()-1, 1 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( tr("On-Board Flash Image:"), format );
			
			cell = table->cellAt( table->rows()-1, 2 );
			cell_cursor = cell.firstCursorPosition();
			
			if( ui.CH_MTDBlock->isChecked() )
			{
				im_info = QFileInfo( ui.Edit_MTDBlock_File->text() );
				cell_cursor.insertText( im_info.fileName(), format );
			}
			else
			{
				cell_cursor.insertText( tr("No"), format );
			}
			
			table->insertRows( table->rows(), 1 );
		}
		
		// SD Card Image
		if( Settings.value("Info/SD_Image", "no").toString() == "yes" )
		{
			cell = table->cellAt( table->rows()-1, 1 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( tr("SD Card Image:"), format );
			
			cell = table->cellAt( table->rows()-1, 2 );
			cell_cursor = cell.firstCursorPosition();
			
			if( ui.CH_SD_Image->isChecked() )
			{
				im_info = QFileInfo( ui.Edit_SD_Image_File->text() );
				cell_cursor.insertText( im_info.fileName(), format );
			}
			else
			{
				cell_cursor.insertText( tr("No"), format );
			}
			
			table->insertRows( table->rows(), 1 );
		}
		
		// Parallel Flash Image
		if( Settings.value("Info/PFlash", "no").toString() == "yes" )
		{
			cell = table->cellAt( table->rows()-1, 1 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( tr("Parallel Flash Image:"), format );
			
			cell = table->cellAt( table->rows()-1, 2 );
			cell_cursor = cell.firstCursorPosition();
			
			if( ui.CH_PFlash->isChecked() )
			{
				im_info = QFileInfo( ui.Edit_PFlash_File->text() );
				cell_cursor.insertText( im_info.fileName(), format );
			}
			else
			{
				cell_cursor.insertText( tr("No"), format );
			}
			
			table->insertRows( table->rows(), 1 );
		}
		
		// GDB
		if( Settings.value("Info/GDB_Port", "no").toString() == "yes" )
		{
			cell = table->cellAt( table->rows()-1, 1 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( tr("Use GDB:"), format );
			
			cell = table->cellAt( table->rows()-1, 2 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( ui.CH_Use_GDB->isChecked() ? tr("Yes") : tr("No"), format );
			table->insertRows( table->rows(), 1 );
		}
	}
	
	// Advanced Tab
	if( Settings.value("Info/No_Frame", "no").toString() == "yes" ||
		Settings.value("Info/Alt_Grab", "no").toString() == "yes" ||
		Settings.value("Info/No_Quit", "no").toString() == "yes" ||
		Settings.value("Info/Portrait", "no").toString() == "yes" ||
		Settings.value("Info/Check_Boot_on_FDD", "no").toString() == "yes" ||
		Settings.value("Info/Win2K_Hack", "no").toString() == "yes" ||
		Settings.value("Info/QEMU_Log", "no").toString() == "yes" )
	{
		cursor.setPosition( topFrame->lastPosition() );
		cursor.insertText( tr("Advanced"), bold_format );
		cursor.insertBlock();
		
		table = cursor.insertTable( 1, 3, table_format );
		
		frame = cursor.currentFrame();
		frame->setFrameFormat( frame_format );
		
		if( Settings.value("Info/No_Frame", "no").toString() == "yes" )
		{
			cell = table->cellAt( table->rows()-1, 1 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( tr("No Frame:"), format );
			
			cell = table->cellAt( table->rows()-1, 2 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( ui.CH_No_Frame->isChecked() ? tr("Yes") : tr("No"), format );
			table->insertRows( table->rows(), 1 );
		}
		
		if( Settings.value("Info/Alt_Grab", "no").toString() == "yes" )
		{
			cell = table->cellAt( table->rows()-1, 1 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( tr("Alt Grab:"), format );
			
			cell = table->cellAt( table->rows()-1, 2 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( ui.CH_Alt_Grab->isChecked() ? tr("Yes") : tr("No"), format );
			table->insertRows( table->rows(), 1 );
		}
		
		if( Settings.value("Info/No_Quit", "no").toString() == "yes" )
		{
			cell = table->cellAt( table->rows()-1, 1 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( tr("No Quit:"), format );
			
			cell = table->cellAt( table->rows()-1, 2 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( ui.CH_No_Quit->isChecked() ? tr("Yes") : tr("No"), format );
			table->insertRows( table->rows(), 1 );
		}
		
		if( Settings.value("Info/Portrait", "no").toString() == "yes" )
		{
			cell = table->cellAt( table->rows()-1, 1 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( tr("Portrait Mode:"), format );
			
			cell = table->cellAt( table->rows()-1, 2 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( ui.CH_Portrait->isChecked() ? tr("Yes") : tr("No"), format );
			table->insertRows( table->rows(), 1 );
		}
		
		if( Settings.value("Info/Check_Boot_on_FDD", "no").toString() == "yes" )
		{
			cell = table->cellAt( table->rows()-1, 1 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( tr("Check Boot Sector on FDD:"), format );
			
			cell = table->cellAt( table->rows()-1, 2 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( ui.CH_FDD_Boot->isChecked() ? tr("Yes") : tr("No"), format );
			table->insertRows( table->rows(), 1 );
		}
		
		if( Settings.value("Info/Win2K_Hack", "no").toString() == "yes" )
		{
			cell = table->cellAt( table->rows()-1, 1 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( tr("Windows 2000 Hack:"), format );
			
			cell = table->cellAt( table->rows()-1, 2 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( ui.CH_Win2K_Hack->isChecked() ? tr("Yes") : tr("No"), format );
			table->insertRows( table->rows(), 1 );
		}
		
		if( Settings.value("Info/QEMU_Log", "no").toString() == "yes" )
		{
			cell = table->cellAt( table->rows()-1, 1 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( tr("Use QEMU Log:"), format );
			
			cell = table->cellAt( table->rows()-1, 2 );
			cell_cursor = cell.firstCursorPosition();
			cell_cursor.insertText( ui.CH_QEMU_Log->isChecked() ? tr("Yes") : tr("No"), format );
			table->insertRows( table->rows(), 1 );
		}
	}
	
	// Show_QEMU_Arguments
	if( Settings.value("Info/Show_QEMU_Args", "no").toString() == "yes" )
	{
		cursor.setPosition( topFrame->lastPosition() );
		cursor.insertText( tr("QEMU/KVM Arguments"), bold_format );
		cursor.insertBlock();
		
		QTextTableFormat table_format2;
		
		table_format2.setAlignment( Qt::AlignLeft );
		table_format2.setCellPadding( 0 );
		table_format2.setCellSpacing( 0 );
		
		QVector<QTextLength> constraints2;
		constraints2 << QTextLength( QTextLength::FixedLength, 10 )
					 << QTextLength( QTextLength::VariableLength, 500 )
					 << QTextLength( QTextLength::FixedLength, 10 );
		table_format2.setColumnWidthConstraints( constraints2 );
		
		QTextTable *table2 = cursor.insertTable( 1, 3, table_format2 );
		
		frame = cursor.currentFrame();
		QTextFrameFormat frame_format2 = frame->frameFormat();
		frame_format2.setBorder( 0 );
		frame->setFrameFormat( frame_format2 );
		
		cell = table2->cellAt( table2->rows()-1, 1 );
		cell_cursor = cell.firstCursorPosition();
		cell_cursor.insertText( Get_QEMU_Args(), format );
	}
}

void Main_Window::Update_Disabled_Controls()
{
	Averable_Devices* cur_comp = &Get_Devices_Info()[ ui.CB_Computer_Type->currentIndex() ];
	
	ui.CB_CPU_Count->clear();
	
	for( int cx = 1; cx -1 <= cur_comp->PSO_SMP_Count; cx *= 2 )
	{
		if( cx == 256 ) ui.CB_CPU_Count->addItem( QString::number(255) );
		else ui.CB_CPU_Count->addItem( QString::number(cx) );
	}
	
	// Initial Graphic Mode
	if( cur_comp->PSO_Initial_Graphic_Mode ) ui.CH_Init_Graphic_Mode->setEnabled( true );
	else ui.CH_Init_Graphic_Mode->setEnabled( false );
	
	// Check Boot Sector on FDD
	if( cur_comp->PSO_No_FB_Boot_Check ) ui.CH_FDD_Boot->setEnabled( true );
	else ui.CH_FDD_Boot->setEnabled( false );
	
	// Win2K_Hack
	if( cur_comp->PSO_Win2K_Hack ) ui.CH_Win2K_Hack->setEnabled( true );
	else ui.CH_Win2K_Hack->setEnabled( false );
	
	// KQEMU
	if( cur_comp->PSO_Kernel_KQEMU )
	{
		ui.RB_KQEMU_Full->setEnabled( true );
		ui.RB_KQEMU_Enabled->setEnabled( true );
		ui.RB_KQEMU_Use_if_Possible->setEnabled( true );
		ui.RB_KQEMU_Disabled->setEnabled( true );
		
		ui.CH_No_KVM_IRQChip->setEnabled( false );
		ui.CH_No_KVM_Pit->setEnabled( false );
		ui.CH_KVM_Shadow_Memory->setEnabled( false );
		ui.SB_KVM_Shadow_Memory_Size->setEnabled( false );
		ui.Label_KVM_Shadow_Memory_Mb->setEnabled( false );
	}
	else
	{
		ui.RB_KQEMU_Full->setEnabled( false );
		ui.RB_KQEMU_Enabled->setEnabled( false );
		ui.RB_KQEMU_Use_if_Possible->setEnabled( false );
		ui.RB_KQEMU_Disabled->setEnabled( false );
		
		ui.CH_No_KVM_IRQChip->setEnabled( true );
		ui.CH_No_KVM_Pit->setEnabled( true );
		ui.CH_KVM_Shadow_Memory->setEnabled( true );
		ui.SB_KVM_Shadow_Memory_Size->setEnabled( true );
		ui.Label_KVM_Shadow_Memory_Mb->setEnabled( true );
	}
	
	// ACPI
	if( cur_comp->PSO_No_ACPI ) ui.CH_ACPI->setEnabled( true );
	else ui.CH_ACPI->setEnabled( false );
	
	// PROM_ENV
	if( cur_comp->PSO_PROM_ENV ) ui.CH_Init_Graphic_Mode->setEnabled( true );
	else ui.CH_Init_Graphic_Mode->setEnabled( false );
	
	// RTC_TD_Hack
	if( cur_comp->PSO_RTC_TD_Hack ) ui.CH_RTC_TD_Hack->setEnabled( true );
	else ui.CH_RTC_TD_Hack->setEnabled( false );
	
	// KVM
	if( cur_comp->PSO_KVM )
	{
		ui.RB_KQEMU_Full->setEnabled( false );
		ui.RB_KQEMU_Enabled->setEnabled( false );
		ui.RB_KQEMU_Use_if_Possible->setEnabled( false );
		ui.RB_KQEMU_Disabled->setEnabled( false );
		
		ui.CH_No_KVM_IRQChip->setEnabled( true );
		ui.CH_No_KVM_Pit->setEnabled( true );
		ui.CH_KVM_Shadow_Memory->setEnabled( true );
		ui.SB_KVM_Shadow_Memory_Size->setEnabled( true );
		ui.Label_KVM_Shadow_Memory_Mb->setEnabled( true );
	}
	else
	{
		ui.RB_KQEMU_Full->setEnabled( true );
		ui.RB_KQEMU_Enabled->setEnabled( true );
		ui.RB_KQEMU_Use_if_Possible->setEnabled( true );
		ui.RB_KQEMU_Disabled->setEnabled( true );
		
		ui.CH_No_KVM_IRQChip->setEnabled( false );
		ui.CH_No_KVM_Pit->setEnabled( false );
		ui.CH_KVM_Shadow_Memory->setEnabled( false );
		ui.SB_KVM_Shadow_Memory_Size->setEnabled( false );
		ui.Label_KVM_Shadow_Memory_Mb->setEnabled( false );
		
		ui.CH_KVM_No_Pit_Reinjection->setEnabled( false );
		ui.CH_KVM_Nesting->setEnabled( false );
	}
	
	// Verions
	ui.CH_MTDBlock->setEnabled( false );
	ui.CH_SD_Image->setEnabled( false );
	ui.CH_PFlash->setEnabled( false );
	ui.CH_No_Frame->setEnabled( false );
	ui.CH_Alt_Grab->setEnabled( false );
	ui.CH_Portrait->setEnabled( false );
	ui.RB_VNC_Unix_Socket->setEnabled( false );
	ui.CH_VNC_Password->setEnabled( false );
	ui.CH_Use_VNC_TLS->setEnabled( false );
	ui.CH_x509_Folder->setEnabled( false );
	ui.CH_x509verify_Folder->setEnabled( false );
	ui.CH_Show_Cursor->setEnabled( false );
	ui.CH_Curses->setEnabled( false );
	ui.CH_No_Shutdown->setEnabled( false );
	ui.CH_KVM_No_Pit_Reinjection->setEnabled( false );
	ui.CH_KVM_Nesting->setEnabled( false );
	
	if( Current_Emulator_Version_Good(VM::QEMU_0_9_1, VM::KVM_7X) )
	{
		ui.CH_MTDBlock->setEnabled( true );
		ui.CH_SD_Image->setEnabled( true );
		ui.CH_PFlash->setEnabled( true );
		ui.CH_No_Frame->setEnabled( true );
		ui.CH_Alt_Grab->setEnabled( true );
		ui.CH_Portrait->setEnabled( true );
		ui.RB_VNC_Unix_Socket->setEnabled( true );
		ui.CH_VNC_Password->setEnabled( true );
		ui.CH_Use_VNC_TLS->setEnabled( true );
		ui.CH_x509_Folder->setEnabled( true );
		ui.CH_x509verify_Folder->setEnabled( true );
	}
	
	if( Current_Emulator_Version_Good(VM::QEMU_0_10, VM::KVM_8X) )
	{
		ui.CH_Show_Cursor->setEnabled( true );
		ui.CH_Curses->setEnabled( true );
		ui.CH_No_Shutdown->setEnabled( true );
		
		if( cur_comp->PSO_KVM )
		{
			ui.CH_KVM_No_Pit_Reinjection->setEnabled( true );
			ui.CH_KVM_Nesting->setEnabled( true );
		}
	}
}

void Main_Window::VM_State_Changet( Virtual_Machine *vm, VM::VM_State s )
{
	int vm_index = VM_List.indexOf( *vm, 0 );
	
	if( vm_index < 0 ) // not found
	{
		AQError( "void Main_Window::VM_State_Changet( Virtual_Machine *vm, VM::VM_State s )",
				 "Error: vm_index < 0" );
	}
	else
	{
		if( vm_index == ui.Machines_List->currentRow() ) // State Changet in current item
		{
			Update_VM_Ui();
		}
		
		vm->Save_VM(); // Save New State
	}
}

void Main_Window::Show_State( Virtual_Machine *vm, VM::VM_State s )
{
	if( s == VM::VMS_Saved && Settings.value("Use_Screenshot_for_OS_Logo", "yes").toString() == "yes" )
	{
		ui.Machines_List->currentItem()->setIcon( QIcon(VM_List[ui.Machines_List->currentRow()].Get_Screenshot_Path()) );
		ui.Machines_List->currentItem()->setData( 128, VM_List[ui.Machines_List->currentRow()].Get_Screenshot_Path() );
	}
	else
	{
		ui.Machines_List->currentItem()->setIcon( QIcon(VM_List[ui.Machines_List->currentRow()].Get_Icon_Path()) );
		ui.Machines_List->currentItem()->setData( 128, VM_List[ui.Machines_List->currentRow()].Get_Icon_Path() );
	}
	
	switch( s )
	{
		case VM::VMS_Running:
			ui.actionPower_On->setEnabled( false );
			ui.actionSave->setEnabled( true );
			ui.actionPause->setEnabled( true );
			ui.actionPower_Off->setEnabled( true );
			ui.actionReset->setEnabled( true );
			
			ui.Tab_General->setEnabled( false );
			ui.Tab_HDD->setEnabled( false );
			ui.Tab_Removable_Disks->setEnabled( false );
			Dev_Manager->setEnabled( false );
			ui.Tab_Network->setEnabled( false );
			Ports_Tab->setEnabled( false );
			ui.Tab_Other->setEnabled( false );
			ui.Tab_Advanced->setEnabled( false );
			
			ui.Button_Apply->setEnabled( false );
			ui.Button_Cancel->setEnabled( false );
			
			ui.actionPause->setChecked( false );
			break;
			
		case VM::VMS_Power_Off:
			ui.actionPower_On->setEnabled( true );
			ui.actionSave->setEnabled( false );
			ui.actionPause->setEnabled( false );
			ui.actionPower_Off->setEnabled( false );
			ui.actionReset->setEnabled( false );
			
			ui.Tab_General->setEnabled( true );
			ui.Tab_HDD->setEnabled( true );
			ui.Tab_Removable_Disks->setEnabled( true );
			Dev_Manager->setEnabled( true );
			ui.Tab_Network->setEnabled( true );
			Ports_Tab->setEnabled( true );
			ui.Tab_Other->setEnabled( true );
			ui.Tab_Advanced->setEnabled( true );
			
			ui.Button_Apply->setEnabled( true );
			ui.Button_Cancel->setEnabled( true );
			
			ui.actionPause->setChecked( false );
			
			vm->Hide_Emu_Ctl_Win();
			break;
			
		case VM::VMS_Pause:
			ui.actionPower_On->setEnabled( false );
			ui.actionSave->setEnabled( true );
			ui.actionPause->setEnabled( true );
			ui.actionPower_Off->setEnabled( true );
			ui.actionReset->setEnabled( true );
			
			ui.Tab_General->setEnabled( false );
			ui.Tab_HDD->setEnabled( false );
			ui.Tab_Removable_Disks->setEnabled( false );
			Dev_Manager->setEnabled( false );
			ui.Tab_Network->setEnabled( false );
			Ports_Tab->setEnabled( false );
			ui.Tab_Other->setEnabled( false );
			ui.Tab_Advanced->setEnabled( false );
			
			ui.Button_Apply->setEnabled( false );
			ui.Button_Cancel->setEnabled( false );
			
			ui.actionPause->setChecked( true );
			break;
			
		case VM::VMS_Saved:
			ui.actionPower_On->setEnabled( true );
			ui.actionSave->setEnabled( false );
			ui.actionPause->setEnabled( false );
			ui.actionPower_Off->setEnabled( true );
			ui.actionReset->setEnabled( true );
			
			ui.Tab_General->setEnabled( false );
			ui.Tab_HDD->setEnabled( false );
			ui.Tab_Removable_Disks->setEnabled( false );
			Dev_Manager->setEnabled( false );
			ui.Tab_Network->setEnabled( false );
			Ports_Tab->setEnabled( false );
			ui.Tab_Other->setEnabled( false );
			
			ui.Button_Apply->setEnabled( false );
			ui.Button_Cancel->setEnabled( false );
			ui.Tab_Advanced->setEnabled( false );
			
			ui.actionPause->setChecked( false );
			
			vm->Hide_Emu_Ctl_Win();
			break;
			
		case VM::VMS_In_Error:
			ui.actionPower_On->setEnabled( false );
			ui.actionSave->setEnabled( false );
			ui.actionPause->setEnabled( false );
			ui.actionPower_Off->setEnabled( false );
			ui.actionReset->setEnabled( false );
			
			ui.Tab_General->setEnabled( false );
			ui.Tab_HDD->setEnabled( false );
			ui.Tab_Removable_Disks->setEnabled( false );
			Dev_Manager->setEnabled( false );
			ui.Tab_Network->setEnabled( false );
			Ports_Tab->setEnabled( false );
			ui.Tab_Other->setEnabled( false );
			
			ui.Button_Apply->setEnabled( false );
			ui.Button_Cancel->setEnabled( false );
			ui.Tab_Advanced->setEnabled( false );
			
			ui.actionPause->setChecked( false );
			
			vm->Hide_Emu_Ctl_Win();
			
			Update_Info_Text( 2 );
			break;
			
		default:
			break;
	}
	
	Update_Emulator_Control();
}

void Main_Window::VM_Changet()
{
	ui.Button_Apply->setEnabled( true );
	ui.Button_Cancel->setEnabled( true );
}

void Main_Window::Update_Emulator_Control()
{
	// Check and Delete All Emulator Controls
	if( ui.Tabs->tabText(0) == tr("Display") )
	{
		ui.Tabs->removeTab( 0 );
		ui.Tabs->setCurrentIndex( 0 );
	}
	
	// Add new Emulator Control
	VM::VM_State tmp_state = VM_List[ ui.Machines_List->currentRow() ].Get_State();
	
	if( tmp_state == VM::VMS_Running || tmp_state == VM::VMS_Pause )
	{
		if( Settings.value("Show_Emulator_Control_Window", "no").toString() == "yes" )
		{
			if( Settings.value("Include_Emulator_Control", "no").toString() == "yes" )
			{
				if( Settings.value("Use_VNC_Display", "no").toString() == "yes" )
				{
					// Display Tab
					VM_List[ui.Machines_List->currentRow()].Emu_Ctl->Use_Minimal_Size( false );
					ui.Tabs->insertTab( 0, VM_List[ui.Machines_List->currentRow()].Emu_Ctl, tr("Display") );
					ui.Tabs->setCurrentIndex( 0 );
				}
				else
				{
					delete ui.Tab_Info->layout();
					
					QVBoxLayout *layout = new QVBoxLayout;
					VM_List[ui.Machines_List->currentRow()].Emu_Ctl->setMaximumSize( 4096, 30 );
					layout->addWidget( VM_List[ui.Machines_List->currentRow()].Emu_Ctl );
					layout->addWidget( ui.VM_Information_Text );
					ui.Tab_Info->setLayout( layout );
				}
			}
			else VM_List[ ui.Machines_List->currentRow() ].Show_Emu_Ctl_Win();
		}
	}
}

void Main_Window::on_Machines_List_currentItemChanged( QListWidgetItem *current, QListWidgetItem *previous )
{
	if( VM_List.count() < 1 )
	{
		AQDebug( "void Main_Window::on_Machines_List_currentItemChanged( QListWidgetItem* current, QListWidgetItem* previous )",
				 "VM_List.count() < 1" );
		return;
	}
	
	if( ui.Machines_List->row(previous) < 0 ) return;
	
	Virtual_Machine tmp_vm;
	
	if( Create_VM_From_Ui(tmp_vm, ui.Machines_List->row(previous)) == false &&
		VM_List[ui.Machines_List->row(previous)].Get_State() != VM::VMS_In_Error )
	{
		AQError( "void Main_Window::on_Machines_List_currentItemChanged( QListWidgetItem* current, QListWidgetItem* previous )",
				 "Cannot Create VM!" );
		
		int mes_res = QMessageBox::question( this, tr("Warning!"), tr("VM be Changed. Discard Changes?"),
											 QMessageBox::Yes | QMessageBox::No, QMessageBox::No );
		
		if( mes_res == QMessageBox::No )
		{
			ui.Machines_List->setCurrentItem( previous );
			return;
		}
		else
		{
			// discart changes
			if( ui.Machines_List->row(current) >= 0 && ui.Machines_List->row(current) < ui.Machines_List->count() )
			{
				if( VM_List[ui.Machines_List->row(previous)].Get_State() == VM::VMS_Saved )
				{
					previous->setIcon( QIcon(VM_List[ui.Machines_List->row(previous)].Get_Screenshot_Path()) );
					previous->setData( 128, VM_List[ui.Machines_List->row(previous)].Get_Screenshot_Path() );
				}
				else
				{
					previous->setIcon( QIcon(VM_List[ui.Machines_List->row(previous)].Get_Icon_Path()) );
					previous->setData( 128, VM_List[ui.Machines_List->row(previous)].Get_Icon_Path() );
				}
				
				Update_VM_Ui();
			}
			else
			{
				AQError( "void Main_Window::on_Machines_List_currentItemChanged( QListWidgetItem* current, QListWidgetItem* previous )",
						 "Index Invalid!" );
			}
		}
	}
	
	// on priv machine Settings be changed
	if( VM_List[ui.Machines_List->row(previous)] != tmp_vm &&
		VM_List[ui.Machines_List->row(previous)].Get_State() != VM::VMS_In_Error )
	{
		int mes_res = QMessageBox::question( this, tr("Warning!"), tr("VM be Changed. Save Changes?"),
											 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
		
		if( mes_res == QMessageBox::Yes )
		{
			VM_List[ ui.Machines_List->row(previous) ] = tmp_vm;
			VM_List[ ui.Machines_List->row(previous) ].Save_VM();
			Update_VM_Ui();
			return;
		}
		else
		{
			// discart changes
			if( VM_List[ui.Machines_List->row(previous)].Get_State() == VM::VMS_Saved )
			{
				previous->setIcon( QIcon(VM_List[ui.Machines_List->row(previous)].Get_Screenshot_Path()) );
				previous->setData( 128, VM_List[ui.Machines_List->row(previous)].Get_Screenshot_Path() );
			}
			else
			{
				previous->setIcon( QIcon(VM_List[ui.Machines_List->row(previous)].Get_Icon_Path()) );
				previous->setData( 128, VM_List[ui.Machines_List->row(previous)].Get_Icon_Path() );
			}
			
			Update_VM_Ui();
		}
		
		AQWarning( "void Main_Window::on_Machines_List_currentItemChanged( QListWidgetItem* current, QListWidgetItem* previous )",
				   "Changet VM Not Saved!" );
	}
	else
	{
		if( ui.Machines_List->row(current) >= 0 && ui.Machines_List->row(current) < ui.Machines_List->count() )
		{
			Update_VM_Ui();
			
			Boot_Is_Correct( VM_List[ui.Machines_List->row(current)] );
		}
		else
		{
			AQError( "void Main_Window::on_Machines_List_currentItemChanged( QListWidgetItem* current, QListWidgetItem* previous )",
					 "Index Invalid!" );
		}
	}
}

void Main_Window::on_Machines_List_customContextMenuRequested( const QPoint &pos )
{
	QListWidgetItem *it = ui.Machines_List->itemAt( pos );
	
	if( it != NULL )
	{
		Icon_Menu->exec( ui.Machines_List->mapToGlobal(pos) );
	}
	else
	{
		VM_List_Menu->exec( ui.Machines_List->mapToGlobal(pos) );
	}
}

void Main_Window::on_Machines_List_itemDoubleClicked( QListWidgetItem *item )
{
	if( VM_List[ui.Machines_List->currentRow()].Get_State() == VM::VMS_Saved )
	{
		AQGraphic_Warning( tr("Warning"), tr("Cannot Change Icon When VM in Save State.") );
		return;
	}
	
	on_actionChange_Icon_triggered();
}

QString Main_Window::Get_QEMU_Args()
{
	if( ui.Machines_List->currentRow() < 0 || ui.CB_Computer_Type->currentIndex() < 0 )
	{
		AQWarning( "QString Main_Window::Get_QEMU_Args()", "Index < 0" );
		return "";
	}
	
	QString line = "";
	
	if( VM_List[ui.Machines_List->currentRow()].Get_Use_User_Emulator_Binary() &&
		VM_List[ui.Machines_List->currentRow()].Get_Only_User_Args() )
	{
		QStringList all_args = VM_List[ ui.Machines_List->currentRow() ].Build_QEMU_Args_For_Tab_Info();
		line = all_args.takeAt( 0 );
		
		for( int i = 0; i < all_args.count(); ++i ) line += " " + all_args[i];
	}
	else
	{
		line = Get_Current_Binary_Name();
		
		QStringList all_args = VM_List[ ui.Machines_List->currentRow() ].Build_QEMU_Args_For_Tab_Info();
		
		for( int i = 0; i < all_args.count(); ++i ) line += " " + all_args[i];
	}
	
	return line;
}

QString Main_Window::Get_Current_Binary_Name()
{
	QString line = "";
	Emulator cur_emul;
	
	if( ui.CB_Emulator_Version->currentIndex() > 0 ) cur_emul = Get_Emulator_By_Name( ui.CB_Emulator_Version->currentText() );
	else
	{
		if( ui.CB_Emulator_Type->currentText() == "QEMU" ) cur_emul = Get_Default_Emulator( "QEMU" );
		else if( ui.CB_Emulator_Type->currentText() == "KVM" ) cur_emul = Get_Default_Emulator( "KVM" );
		else
		{
			cur_emul = Get_Default_Emulator( "QEMU" );
			
			AQError( "QString Main_Window::Get_Current_Binary_Name()",
					 "Incorrect Emulator Type. Use Default: QEMU" );
		}
	}
	
	QMap<QString, QString> bin_list = cur_emul.Get_Binary_Files();
	QString find_name = Get_Devices_Info()[ ui.CB_Computer_Type->currentIndex() ].System.QEMU_Name;
	
	for( QMap<QString, QString>::const_iterator iter = bin_list.constBegin(); iter != bin_list.constEnd(); iter++ )
	{
		if( iter.key() == find_name ||
			(find_name == "qemu-system-x86" && iter.key() == "qemu") ||
			(find_name == "qemu-kvm" && iter.key() == "kvm") )
		{
			line = iter.value();
			break;
		}
	}
	
	return line;
}

bool Main_Window::Boot_Is_Correct( Virtual_Machine &tmp_vm )
{
	bool update_UI = false;
	
	// Floppy A
	if( tmp_vm.Get_FD0().Get_Enabled() )
	{
		if( tmp_vm.Get_FD0().Get_Host_Device() )
		{
			if( ! QFile::exists(tmp_vm.Get_FD0().Get_Host_File_Name()) )
			{
				if( ! No_Device_Found("Floppy A", tmp_vm.Get_FD0().Get_Host_File_Name(), VM::Boot_From_FDD) )
				{
					return false;
				}
				else
				{
					VM_Floppy tmp_fd = tmp_vm.Get_FD0();
					tmp_fd.Set_Enabled( false );
					tmp_vm.Set_FD0( tmp_fd );
					
					if( Settings.value("Use_Device_Manager", "").toString() == "no" ) ui.GB_Floppy0->setChecked( false );
				}
			}
		}
		else
		{
			if( ! QFile::exists(tmp_vm.Get_FD0().Get_Image_File_Name()) )
			{
				if( ! No_Device_Found("Floppy A", tmp_vm.Get_FD0().Get_Image_File_Name(), VM::Boot_From_FDD) )
				{
					return false;
				}
				else
				{
					VM_Floppy tmp_fd = tmp_vm.Get_FD0();
					tmp_fd.Set_Enabled( false );
					tmp_vm.Set_FD0( tmp_fd );
					
					if( Settings.value("Use_Device_Manager", "").toString() == "no" ) ui.GB_Floppy0->setChecked( false );
				}
			}
		}
	}
	
	// Floppy B
	if( tmp_vm.Get_FD1().Get_Enabled() )
	{
		if( tmp_vm.Get_FD1().Get_Host_Device() )
		{
			if( ! QFile::exists(tmp_vm.Get_FD1().Get_Host_File_Name()) )
			{
				if( ! No_Device_Found("Floppy B", tmp_vm.Get_FD1().Get_Host_File_Name(), VM::Boot_From_FDD) )
				{
					return false;
				}
				else
				{
					VM_Floppy tmp_fd = tmp_vm.Get_FD1();
					tmp_fd.Set_Enabled( false );
					tmp_vm.Set_FD1( tmp_fd );
					
					if( Settings.value("Use_Device_Manager", "").toString() == "no" ) ui.GB_Floppy1->setChecked( false );
				}
			}
		}
		else
		{
			if( ! QFile::exists(tmp_vm.Get_FD1().Get_Image_File_Name()) )
			{
				if( ! No_Device_Found("Floppy B", tmp_vm.Get_FD1().Get_Image_File_Name(), VM::Boot_From_FDD) )
				{
					return false;
				}
				else
				{
					VM_Floppy tmp_fd = tmp_vm.Get_FD1();
					tmp_fd.Set_Enabled( false );
					tmp_vm.Set_FD1( tmp_fd );
					
					if( Settings.value("Use_Device_Manager", "").toString() == "no" ) ui.GB_Floppy1->setChecked( false );
				}
			}
		}
	}
	
	// CD-ROM
	if( tmp_vm.Get_CD_ROM().Get_Enabled() )
	{
		if( tmp_vm.Get_CD_ROM().Get_Host_Device() )
		{
			if( ! QFile::exists(tmp_vm.Get_CD_ROM().Get_Host_File_Name()) )
			{
				if( ! No_Device_Found("CD-ROM", tmp_vm.Get_CD_ROM().Get_Host_File_Name(), VM::Boot_From_CDROM) )
				{
					return false;
				}
				else
				{
					VM_CDROM tmp_cd = tmp_vm.Get_CD_ROM();
					tmp_cd.Set_Enabled( false );
					tmp_vm.Set_CD_ROM( tmp_cd );
					
					if( Settings.value("Use_Device_Manager", "").toString() == "no" ) ui.GB_CDROM->setChecked( false );
				}
			}
		}
		else
		{
			if( ! QFile::exists(tmp_vm.Get_CD_ROM().Get_Image_File_Name()) )
			{
				if( ! No_Device_Found("CD-ROM", tmp_vm.Get_CD_ROM().Get_Image_File_Name(), VM::Boot_From_CDROM) )
				{
					return false;
				}
				else
				{
					VM_CDROM tmp_cd = tmp_vm.Get_CD_ROM();
					tmp_cd.Set_Enabled( false );
					tmp_vm.Set_CD_ROM( tmp_cd );
					
					if( Settings.value("Use_Device_Manager", "").toString() == "no" ) ui.GB_CDROM->setChecked( false );
				}
			}
		}
	}
	
	// HDA
	if( tmp_vm.Get_HDA().Get_Enabled() )
	{
		if( ! QFile::exists(tmp_vm.Get_HDA().Get_Image_File_Name()) )
		{
			if( ! No_Device_Found("HDA", tmp_vm.Get_HDA().Get_Image_File_Name(), VM::Boot_From_HDD) )
			{
				return false;
			}
			else
			{
				VM_HDD tmp_hd = tmp_vm.Get_HDA();
				tmp_hd.Set_Enabled( false );
				tmp_vm.Set_HDA( tmp_hd );
				
				if( Settings.value("Use_Device_Manager", "").toString() == "no" ) ui.GB_HDA->setChecked( false );
			}
		}
	}
	
	// HDB
	if( tmp_vm.Get_HDB().Get_Enabled() )
	{
		if( ! QFile::exists(tmp_vm.Get_HDB().Get_Image_File_Name()) )
		{
			if( ! No_Device_Found("HDB", tmp_vm.Get_HDB().Get_Image_File_Name(), VM::Boot_From_HDD) )
			{
				return false;
			}
			else
			{
				VM_HDD tmp_hd = tmp_vm.Get_HDB();
				tmp_hd.Set_Enabled( false );
				tmp_vm.Set_HDB( tmp_hd );
				
				if( Settings.value("Use_Device_Manager", "").toString() == "no" ) ui.GB_HDB->setChecked( false );
			}
		}
	}
	
	// HDC
	if( tmp_vm.Get_HDC().Get_Enabled() )
	{
		if( ! QFile::exists(tmp_vm.Get_HDC().Get_Image_File_Name()) )
		{
			if( ! No_Device_Found("HDC", tmp_vm.Get_HDC().Get_Image_File_Name(), VM::Boot_From_HDD) )
			{
				return false;
			}
			else
			{
				VM_HDD tmp_hd = tmp_vm.Get_HDC();
				tmp_hd.Set_Enabled( false );
				tmp_vm.Set_HDC( tmp_hd );
				
				if( Settings.value("Use_Device_Manager", "").toString() == "no" ) ui.GB_HDC->setChecked( false );
			}
		}
	}
	
	// HDD
	if( tmp_vm.Get_HDD().Get_Enabled() )
	{
		if( ! QFile::exists(tmp_vm.Get_HDD().Get_Image_File_Name()) )
		{
			if( ! No_Device_Found("HDD", tmp_vm.Get_HDD().Get_Image_File_Name(), VM::Boot_From_HDD) )
			{
				return false;
			}
			else
			{
				VM_HDD tmp_hd = tmp_vm.Get_HDD();
				tmp_hd.Set_Enabled( false );
				tmp_vm.Set_HDD( tmp_hd );
				
				if( Settings.value("Use_Device_Manager", "").toString() == "no" ) ui.GB_HDD->setChecked( false );
			}
		}
	}
	
	if( Settings.value("Use_Device_Manager", "").toString() == "yes" )
	{
		if( ui.Machines_List->currentRow() >= 0 &&
			ui.Machines_List->currentRow() < VM_List.count() )
		{
			Dev_Manager->Set_VM( VM_List[ui.Machines_List->currentRow()] );
		}
	}
	
	// Linux Kernel Files
	if( tmp_vm.Get_Use_Linux_Boot() )
	{
		if( ! QFile::exists(tmp_vm.Get_bzImage_Path()) )
		{
			if( ! No_Device_Found(tr("bzImage"), tmp_vm.Get_bzImage_Path(), VM::Boot_None) )
			{
				return false;
			}
			else
			{
				ui.CH_Use_Linux_Boot->setChecked( false );
				tmp_vm.Set_Use_Linux_Boot( false );
			}
		}
		
		if( ! QFile::exists(tmp_vm.Get_Initrd_Path()) )
		{
			if( ! No_Device_Found(tr("Initrd"), tmp_vm.Get_Initrd_Path(), VM::Boot_None) )
			{
				return false;
			}
			else
			{
				ui.CH_Use_Linux_Boot->setChecked( false );
				tmp_vm.Set_Use_Linux_Boot( false );
			}
		}
	}
	
	// ROM File
	if( tmp_vm.Get_Use_ROM_File() )
	{
		if( ! QFile::exists(tmp_vm.Get_ROM_File()) )
		{
			if( ! No_Device_Found(tr("ROM File"), tmp_vm.Get_ROM_File(), VM::Boot_None) )
			{
				return false;
			}
			else
			{
				ui.CH_ROM_File->setChecked( false );
				tmp_vm.Set_Use_ROM_File( false );
			}
		}
	}
	
	// On-Board Flash Image
	if( tmp_vm.Use_MTDBlock_File() )
	{
		if( ! QFile::exists(tmp_vm.Get_MTDBlock_File()) )
		{
			if( ! No_Device_Found(tr("On-Board Flash"), tmp_vm.Get_MTDBlock_File(), VM::Boot_None) )
			{
				return false;
			}
			else
			{
				ui.CH_MTDBlock->setChecked( false );
				tmp_vm.Use_MTDBlock_File( false );
			}
		}
	}
	
	// SecureDigital Card Image
	if( tmp_vm.Use_SecureDigital_File() )
	{
		if( ! QFile::exists(tmp_vm.Get_SecureDigital_File()) )
		{
			if( ! No_Device_Found(tr("SecureDigital Card"), tmp_vm.Get_SecureDigital_File(), VM::Boot_None) )
			{
				return false;
			}
			else
			{
				ui.CH_SD_Image->setChecked( false );
				tmp_vm.Use_SecureDigital_File( false );
			}
		}
	}
	
	// Parallel Flash Image
	if( tmp_vm.Use_PFlash_File() )
	{
		if( ! QFile::exists(tmp_vm.Get_PFlash_File()) )
		{
			if( ! No_Device_Found(tr("Parallel Flash"), tmp_vm.Get_PFlash_File(), VM::Boot_None) )
			{
				return false;
			}
			else
			{
				ui.CH_PFlash->setChecked( false );
				tmp_vm.Use_PFlash_File( false );
			}
		}
	}
	
	// VNC Sertificates
	if( tmp_vm.Use_VNC() && tmp_vm.Use_VNC_TLS() )
	{
		if( tmp_vm.Use_VNC_x509() )
		{
			if( ! QFile::exists(tmp_vm.Get_VNC_x509_Folder_Path()) )
			{
				if( ! No_Device_Found(tr("VNC x509 Folder"), tmp_vm.Get_VNC_x509_Folder_Path(), VM::Boot_None) )
				{
					return false;
				}
				else
				{
					ui.CH_Use_VNC_TLS->setChecked( false );
					tmp_vm.Use_VNC_x509( false );
				}
			}
		}
		
		if( tmp_vm.Use_VNC_x509verify() )
		{
			if( ! QFile::exists(tmp_vm.Get_VNC_x509verify_Folder_Path()) )
			{
				if( ! No_Device_Found(tr("VNC x509verify Folder"), tmp_vm.Get_VNC_x509verify_Folder_Path(), VM::Boot_None) )
				{
					return false;
				}
				else
				{
					ui.CH_Use_VNC_TLS->setChecked( false );
					tmp_vm.Use_VNC_x509verify( false );
				}
			}
		}
	}
	
	// Boot is correct?
	switch( tmp_vm.Get_Boot_Device() )
	{
		case VM::Boot_From_FDD:
			if( tmp_vm.Get_FD0().Get_Enabled() )
			{
				return true;
			}
			else
			{
				AQGraphic_Warning( tr("Error!"),
								   tr("To boot from the Floppy you must activate the device \"Floppy 1\".") );
				return false;
			}
			break;
			
		case VM::Boot_From_CDROM:
			if( tmp_vm.Get_CD_ROM().Get_Enabled() )
			{
				return true;
			}
			else
			{
				AQGraphic_Warning( tr("Error!"),
								   tr("To boot from a CD-ROM, you must activate the device \"CD/DVD-ROM\".") );
				return false;
			}
			break;
			
		case VM::Boot_From_HDD:
			if( tmp_vm.Get_HDA().Get_Enabled() )
			{
				return true;
			}
			else
			{
				AQGraphic_Warning( tr("Error!"),
								   tr("To boot from the hard drive, you must activate the device \"HDA\".") );
				return false;
			}
			break;
			
		case VM::Boot_From_Network:
			if( tmp_vm.Get_Use_Network() )
			{
				return true;
			}
			else
			{
				AQGraphic_Warning( tr("Error!"),
								   tr("To boot from the network, you must activate the network support!") );
				return false;
			}
			break;
			
		case VM::Boot_None:
			return true;
			break;
			
		default:
			AQError( "bool Main_Window::Boot_Is_Correct( Virtual_Machine &tmp_vm )",
					 "Default Section!" );
			return false;
			break;
	}
}

bool Main_Window::No_Device_Found( const QString &name, const QString &path, VM::Boot_Device type )
{
	int retVal = QMessageBox::critical( this, tr("Error!"),
										tr("%1 Image \"%2\" Not Exist! Continue Without It Image?").arg(name).arg(path),
										QMessageBox::Yes | QMessageBox::No, QMessageBox::No );
	
	if( retVal == QMessageBox::No ) return false;
	else
	{
		if( ui.Machines_List->currentRow() >= 0 &&
			ui.Machines_List->currentRow() < VM_List.count() )
		{
			VM_List[ ui.Machines_List->currentRow() ].Save_VM();
		}
		
		return true;
	}
}

void Main_Window::on_actionChange_Icon_triggered()
{
	if( VM_List.count() <= 0 ) return;
	
	Select_Icon_Window *icon_win = new Select_Icon_Window( this );
	icon_win->Set_Previous_Icon_Path( VM_List[ui.Machines_List->currentRow()].Get_Icon_Path() );
	
	if( QDialog::Accepted == icon_win->exec() )
	{
		if( ! icon_win->Get_New_Icon_Path().isEmpty() )
		{
			ui.Machines_List->currentItem()->setIcon( QIcon(icon_win->Get_New_Icon_Path()) );
			ui.Machines_List->currentItem()->setData( 128, icon_win->Get_New_Icon_Path() );
		}
		
		VM_List[ ui.Machines_List->currentRow() ].Set_Icon_Path( icon_win->Get_New_Icon_Path() );
		VM_List[ ui.Machines_List->currentRow() ].Save_VM();
	}
	
	delete icon_win;
}

void Main_Window::on_actionAbout_AQEMU_triggered()
{
	About_Window *about_win = new About_Window( this );
	about_win->exec();
	delete about_win;
}

void Main_Window::on_actionAbout_Qt_triggered()
{
	QApplication::aboutQt();
}

void Main_Window::on_actionDelete_VM_triggered()
{
	if( VM_List.count() <= 0 ) return;
	if( ui.Machines_List->currentRow() < 0 ) return;
	
	int mes_ret = QMessageBox::question( this, tr("Delete?"),
										 tr("Delete \"") + VM_List[ ui.Machines_List->currentRow() ].Get_Machine_Name() + tr("\" VM?"),
										 QMessageBox::Yes | QMessageBox::No, QMessageBox::No );
	
	if( mes_ret == QMessageBox::Yes )
	{
		//Update_VM_Ui(); // FIXME this be?
		if( QFile::remove(VM_List[ ui.Machines_List->currentRow() ].Get_VM_XML_File_Path()) )
		{
			int cur = ui.Machines_List->currentRow();
			ui.Machines_List->takeItem( cur );
			VM_List.removeAt( cur );
		}
		else
		{
			AQGraphic_Error( "void Main_Window::on_actionDelete_Virtual_Machine_triggered()",
							 tr("Error!"), tr("Cannot Delete VM XML File!"), false );
		}
	}
	
	if( VM_List.count() <= 0 )
	{
		ui.actionPower_On->setEnabled( false );
		ui.actionSave->setEnabled( false );
		ui.actionPause->setEnabled( false );
		ui.actionPower_Off->setEnabled( false );
		ui.actionReset->setEnabled( false );
		
		ui.Tab_General->setEnabled( false );
		ui.Tab_HDD->setEnabled( false );
		ui.Tab_Removable_Disks->setEnabled( false );
		Dev_Manager->setEnabled( false );
		ui.Tab_Network->setEnabled( false );
		Ports_Tab->setEnabled( false );
		ui.Tab_Other->setEnabled( false );
		ui.Tab_Advanced->setEnabled( false );
		
		ui.Button_Apply->setEnabled( false );
		ui.Button_Cancel->setEnabled( false );
		
		Update_Info_Text( 1 );
	}
}

void Main_Window::on_actionExit_triggered()
{
	close();
}

void Main_Window::on_actionShow_New_VM_Wizard_triggered()
{
	VM_Wizard_Window *Wizard_Win = new VM_Wizard_Window();
	
	Wizard_Win->Set_VM_List( &VM_List );
	
	if( Wizard_Win->exec() == QDialog::Accepted )
	{
		VM_List << *Wizard_Win->New_VM;
		
		QObject::connect( &VM_List[ VM_List.count()-1 ],
						  SIGNAL(State_Changet(Virtual_Machine*, VM::VM_State)),
						  this, SLOT(VM_State_Changet(Virtual_Machine*, VM::VM_State)) );
		
		new QListWidgetItem( VM_List[VM_List.count()-1].Get_Machine_Name(),
							 ui.Machines_List );
		
		ui.Machines_List->item( VM_List.count()-1 )->setIcon(
								QIcon(VM_List[VM_List.count()-1].Get_Icon_Path()) );
		
		ui.Machines_List->setCurrentRow( ui.Machines_List->count()-1 );
		
		Update_VM_Ui();
		on_Button_Apply_clicked();
	}
	
	delete Wizard_Win;
}

void Main_Window::on_actionAdd_New_VM_triggered()
{
	// Add New VM
	QString new_vm_path = "";
	
	bool ok;
	QString new_vm_name = QInputDialog::getText( this, tr("VM Name"), tr("New Virtual Machine Name:"), QLineEdit::Normal, "", &ok );
	
	if( ! ok ) return;
	
	if( ! new_vm_name.isEmpty() )
	{
		// Unique Name?
		for( int nx = 0; nx < VM_List.count(); ++nx )
		{
			if( VM_List[nx].Get_Machine_Name() == new_vm_name )
			{
				AQGraphic_Warning( "Warning", "This Name is Not Unique! Please Enter Unique VM Name!" );
				return;
			}
		}
		
		// Create new vm file name
		new_vm_path = Get_Complete_VM_File_Path( new_vm_name );
	}
	else
	{
		AQGraphic_Error( "void Main_Window::on_actionCreate_triggered()", tr("Error!"),
						 tr("You New VM Name is Empty! VM Not Created!"), false );
		return;
	}
	
	Virtual_Machine new_vm;
	
	// load default template
	if( QFile::exists( Settings.value("VM_Directory", "").toString() +
		"os_templates/" + Settings.value("Default_VM_Template", "Linux_2_6").toString() + ".aqvmt") )
	{
		new_vm.Load_VM( Settings.value("VM_Directory", "").toString() +
			"os_templates/" + Settings.value("Default_VM_Template", "Linux_2_6").toString() + ".aqvmt" );
	}
	else if( QFile::exists( Settings.value("AQEMU_Data_Folder", "").toString() +
			 "os_templates/" + Settings.value("Default_VM_Template", "Linux_2_6").toString() + ".aqvmt") )
	{
		new_vm.Load_VM( Settings.value("AQEMU_Data_Folder", "").toString() +
			"os_templates/" + Settings.value("Default_VM_Template", "Linux_2_6").toString() + ".aqvmt" );
	}
	else
	{
		AQError( "void Main_Window::on_actionCreate_triggered()", "Cannot Locate VM Templates!" );
	}
	
	new_vm.Set_Machine_Name( new_vm_name );
	new_vm.Set_VM_XML_File_Path( new_vm_path );
	
	// In this mode do not create hard drives
	new_vm.Set_HDA( VM_HDD(false, "") );
	
	new_vm.Save_VM();
	
	VM_List << new_vm;
	
	connect( &VM_List[ VM_List.count()-1 ], SIGNAL(State_Changet(Virtual_Machine*, VM::VM_State)),
			 this, SLOT(VM_State_Changet(Virtual_Machine*, VM::VM_State)) );
	
	new QListWidgetItem( new_vm.Get_Machine_Name(), ui.Machines_List );
	ui.Machines_List->item(VM_List.count()-1)->setIcon( QIcon(new_vm.Get_Icon_Path()) );
	
	ui.Machines_List->setCurrentRow( ui.Machines_List->count() -1 );
	
	Update_VM_Ui();
	on_Button_Apply_clicked();
}

void Main_Window::on_actionCreate_HDD_Image_triggered()
{
	Create_HDD_Image_Window *Create_HDD_Win = new Create_HDD_Image_Window( this );
	
	Create_HDD_Win->exec();
	delete Create_HDD_Win;
}

void Main_Window::on_actionConvert_HDD_Image_triggered()
{
	Convert_HDD_Image_Window *Convert_HDD_Win = new Convert_HDD_Image_Window( this );
	
	Convert_HDD_Win->exec();
	delete Convert_HDD_Win;
}

void Main_Window::on_actionShow_Settings_Window_triggered()
{
	Settings_Window *Settings_Win = new Settings_Window( this );
	
	if( Settings_Win->exec() == QDialog::Accepted )
	{
		Save_Settings();
		
		if( Settings.value("VM_Directory", "~").toString() != VM_Folder )
		{
			// Apply Settings
			Load_Settings();
			
			// Clear old vm's
			VM_List.clear();
			ui.Machines_List->clear();
			
			// Load new vm's
			Load_Virtual_Machines();
			
			delete Settings_Win;
			return;
		}
		else
		{
			// Apply Settings
			Load_Settings();
		}
		
		// Update Icons
		for( int ix = 0; ix < VM_List.count(); ++ix )
		{
			if( VM_List[ix].Get_State() == VM::VMS_Saved &&
				Settings.value("Use_Screenshot_for_OS_Logo", "yes").toString() == "yes" )
			{
				ui.Machines_List->item(ix)->setIcon( QIcon(VM_List[ix].Get_Screenshot_Path()) );
				ui.Machines_List->item(ix)->setData( 128, VM_List[ix].Get_Screenshot_Path() );
			}
			else
			{
				ui.Machines_List->item(ix)->setIcon( QIcon(VM_List[ix].Get_Icon_Path()) );
				ui.Machines_List->item(ix)->setData( 128, VM_List[ix].Get_Icon_Path() );
			}
		}
		
		ui.Button_Apply->setEnabled( false );
		ui.Button_Cancel->setEnabled( false );
	}
	
	delete Settings_Win;
}

void Main_Window::on_actionShow_Advanced_Settings_Window_triggered()
{
	Advanced_Settings_Window *ad_set = new Advanced_Settings_Window();
	
	if( ad_set->exec() == QDialog::Accepted )
	{
		Save_Settings();
		
		// Use Log
		if( Settings.value( "Log/Save_In_File", "yes" ).toString() == "yes" )
		{
			AQUse_Log( true );
		}
		else
		{
			AQUse_Log( false );
		}
		
		// Log File Name
		AQLog_Path( Settings.value("Log/Log_Path", "").toString() );
		
		// Log Filter
		AQUse_Debug_Output( Settings.value("Log/Print_In_STDOUT", "yes").toString() == "yes",
							Settings.value("Log/Save_Debug","yes").toString() == "yes",
							Settings.value("Log/Save_Warning","yes").toString() == "yes",
							Settings.value("Log/Save_Error","yes").toString() == "yes" );
		
		// Update Emulators Information
		All_Emulators_List = Get_Emulators_List();
		
		GUI_User_Mode = true;
		Apply_Emulator( 0 );
		
		bool q = false, k = false;
		
		for( int ix = 0; ix < ui.CB_Emulator_Type->count(); ix++ )
		{
			if( ui.CB_Emulator_Type->itemText(ix) == "QEMU" ) q = true;
			else if( ui.CB_Emulator_Type->itemText(ix) == "KVM" ) k = true;
		}
		
		for( int ix = 0; ix < VM_List.count(); ix++ )
		{
			QString type = VM_List[ ix ].Get_Emulator_Type();
			
			if( type == "QEMU" && q == false ) VM_List[ix].Set_State( VM::VMS_In_Error );
			else if( type == "KVM" && k == false ) VM_List[ix].Set_State( VM::VMS_In_Error );
			else
			{
				if( VM_List[ix].Get_State() == VM::VMS_In_Error )
				{
					if( (type == "QEMU" && q == true) || (type == "KVM" && k == true) )
					{
						VM_List[ ix ].Update_Current_Emulator_Devices();
						VM_List[ ix ].Set_State( VM::VMS_Power_Off );
						VM_List[ ix ].Save_VM();
						VM_List[ ix ].Load_VM( VM_List[ix].Get_VM_XML_File_Path() );
					}
				}
			}
		}
		
		Load_Settings();
		Update_VM_Ui();
	}
	
	delete ad_set;
}

void Main_Window::on_actionShow_First_Run_Wizard_triggered()
{
	First_Start_Wizard *first_start_win = new First_Start_Wizard( NULL );
	
	if( first_start_win->exec() == QDialog::Accepted )
	{
		// Update Emulator List
		All_Emulators_List = Get_Emulators_List();
		
		GUI_User_Mode = true;
		Apply_Emulator( 0 );
		
		// Apply Settings
		Load_Settings();
		
		// Clear old vm's
		VM_List.clear();
		ui.Machines_List->clear();
		
		// Load new vm's
		Load_Virtual_Machines();
	}
	
	delete first_start_win;
}

void Main_Window::on_actionPower_On_triggered()
{
	Virtual_Machine tmp_vm;
	
	if( Create_VM_From_Ui(tmp_vm,ui.Machines_List->currentRow()) == false )
	{
		AQError( "void Main_Window::on_action_Power_On_triggered()",
				 "Cannot Create VM From Ui!" );
		
		return;
	}
	else
	{
		if( tmp_vm != VM_List[ui.Machines_List->currentRow()] )
		{
			int mes_res = QMessageBox::question( this, tr("Warning!"), tr("VM be Changed. Save Changes?"),
												 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
		
			if( mes_res == QMessageBox::Yes )
			{
				VM_List[ ui.Machines_List->currentRow() ] = tmp_vm;
				VM_List[ ui.Machines_List->currentRow() ].Save_VM();
				Update_VM_Ui();
			}
			else
			{
				// discart changes
				Update_VM_Ui();
			}
		}
	}
	
	if( ! Boot_Is_Correct(VM_List[ui.Machines_List->currentRow()]) ) return;
	
	if( VM_List[ui.Machines_List->currentRow()].Start() )
	{
		// VNC Password
		if( VM_List[ui.Machines_List->currentRow()].Use_VNC_Password() )
		{
			VNC_Password_Window *vnc_pas_win = new VNC_Password_Window();
			
			if( vnc_pas_win->exec() == QDialog::Accepted )
			{
				VM_List[ ui.Machines_List->currentRow() ].Set_VNC_Password( vnc_pas_win->Get_Password() );
			}
			
			delete vnc_pas_win;
		}
	}
	else
	{
		AQError( "void Main_Window::on_action_Power_On_triggered()", "Cannot Start VM!" );
	}
}

void Main_Window::on_actionSave_triggered()
{
	if( VM_List.count() <= 0 ) return;
	
	if( VM_List[ui.Machines_List->currentRow()].Use_Snapshot_Mode() )
	{
		AQGraphic_Warning( tr("Warning!"), tr("QEMU running in snapshot mode. VM can not be saved in this mode.") );
		return;
	}
	
	if( Settings.value("Info/Show_Screenshot_in_Save_Mode", "no").toString() == "yes" )
	{
		QString img_path = Settings.value( "VM_Directory", "~" ).toString() +
				Get_FS_Compatible_VM_Name( VM_List[ui.Machines_List->currentRow()].Get_Machine_Name() ) + "_screenshot";
		
		VM_List[ ui.Machines_List->currentRow() ].Take_Screenshot( img_path );
	}
	
	VM_List[ ui.Machines_List->currentRow() ].Save_VM_State();
}

void Main_Window::on_actionPower_Off_triggered()
{
	if( VM_List.count() <= 0 ) return;
	
	if( QMessageBox::question(this, tr("You Sure?"),
		tr("Shutdown VM \"%1\"?").arg(VM_List[ui.Machines_List->currentRow()].Get_Machine_Name()),
		QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No )
	{
		return;
	}
	
	VM_List[ ui.Machines_List->currentRow() ].Stop();
}

void Main_Window::on_actionPause_triggered()
{
	if( VM_List.count() <= 0 ) return;
	VM_List[ ui.Machines_List->currentRow() ].Pause();
}

void Main_Window::on_actionReset_triggered()
{
	if( VM_List.count() <= 0 ) return;
	
	if( QMessageBox::question(this, tr("You Sure?"),
		tr("Reboot VM \"%1\"?").arg(VM_List[ui.Machines_List->currentRow()].Get_Machine_Name()),
		QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No )
	{
		return;
	}
	
	VM_List[ ui.Machines_List->currentRow() ].Reset();
}

void Main_Window::on_actionLoad_VM_From_File_triggered()
{
	QFileDialog::Options options;
	QString selectedFilter;
	
	QString load_path = QFileDialog::getOpenFileName( this, tr("Open AQEMU VM File"), QDir::homePath(),
													  tr("AQEMU VM (*.aqemu)"), &selectedFilter, options );
	
	if( ! QFile::exists(load_path) ) return;
	
	// ok file name valid
	QFileInfo vm_file( load_path );
	
	QString new_file_path = Settings.value("VM_Directory", "~").toString() + vm_file.fileName();
	
	if( QFile::exists(new_file_path) )
	{
		AQGraphic_Warning( tr("Warning"), tr("VM With This Name Already Exists!") );
		return;
	}
	
	QFile::copy( load_path, new_file_path );
	
	Virtual_Machine new_vm;
	
	new_vm.Load_VM( Settings.value("VM_Directory", "~").toString() + vm_file.fileName() );
	VM_List << new_vm;
	
	connect( &VM_List[VM_List.count()-1], SIGNAL(State_Changet(Virtual_Machine*, VM::VM_State)),
			 this, SLOT(VM_State_Changet(Virtual_Machine*, VM::VM_State)) );
	
	new QListWidgetItem( new_vm.Get_Machine_Name(), ui.Machines_List );
	ui.Machines_List->item(VM_List.count()-1)->setIcon( QIcon(new_vm.Get_Icon_Path()) );
	
	ui.Machines_List->setCurrentRow( ui.Machines_List->count()-1 );
	
	Update_VM_Ui();
}

void Main_Window::on_actionCopy_triggered()
{
	Copy_VM_Window *copy_win = new Copy_VM_Window();
	
	// Create Machine Name List
	for( int ix = 0; ix < VM_List.count(); ++ix )
	{
		copy_win->Add_VM_Machine_Name( VM_List[ix].Get_Machine_Name() );
	}
	
	if( copy_win->exec() == QDialog::Accepted )
	{
		// Copy VM Object
		Virtual_Machine new_vm = VM_List[ ui.Machines_List->currentRow() ];
		
		new_vm.Set_Machine_Name( copy_win->Get_New_VM_Name() );
		new_vm.Set_VM_XML_File_Path( Get_Complete_VM_File_Path(copy_win->Get_New_VM_Name()) );
		
		// Copy Disk Images
		if( copy_win->Get_Copy_Disk_Images() )
		{
			// Copy Floppy Images
			if( copy_win->Get_Copy_Floppy() )
			{
				if( new_vm.Get_FD0().Get_Enabled() && (new_vm.Get_FD0().Get_Host_Device() == false) )
				{
					new_vm.Set_FD0( VM_Floppy( true, "", Copy_VM_Floppy(new_vm.Get_Machine_Name(), "FD0", new_vm.Get_FD0()), false) );
				}
				
				if( new_vm.Get_FD1().Get_Enabled() && (new_vm.Get_FD1().Get_Host_Device() == false) )
				{
					new_vm.Set_FD1( VM_Floppy( true, "", Copy_VM_Floppy(new_vm.Get_Machine_Name(), "FD1", new_vm.Get_FD1()), false ) );
				}
			}
			
			// Copy Hard Drive Images
			if( copy_win->Get_Copy_Hard_Drive() )
			{
				if( new_vm.Get_HDA().Get_Enabled() )
				{
					new_vm.Set_HDA( VM_HDD( true, Copy_VM_Hard_Drive(new_vm.Get_Machine_Name(), "HDA", new_vm.Get_HDA()) ) );
				}
				
				if( new_vm.Get_HDB().Get_Enabled() )
				{
					new_vm.Set_HDB( VM_HDD( true, Copy_VM_Hard_Drive(new_vm.Get_Machine_Name(), "HDB", new_vm.Get_HDB()) ) );
				}
				
				if( new_vm.Get_HDC().Get_Enabled() )
				{
					new_vm.Set_HDC( VM_HDD( true, Copy_VM_Hard_Drive(new_vm.Get_Machine_Name(), "HDC", new_vm.Get_HDC()) ) );
				}
				
				if( new_vm.Get_HDD().Get_Enabled() )
				{
					new_vm.Set_HDD( VM_HDD( true, Copy_VM_Hard_Drive(new_vm.Get_Machine_Name(), "HDD", new_vm.Get_HDD()) ) );
				}
			}
		}
		
		// Add New VM
		VM_List << new_vm;
		
		connect( &VM_List[ VM_List.count()-1 ], SIGNAL(State_Changet(Virtual_Machine*, VM::VM_State)),
				 this, SLOT(VM_State_Changet(Virtual_Machine*, VM::VM_State)) );
		
		new QListWidgetItem( new_vm.Get_Machine_Name(), ui.Machines_List );
		ui.Machines_List->item(VM_List.count()-1)->setIcon( QIcon(new_vm.Get_Icon_Path()) );
		
		ui.Machines_List->setCurrentRow( ui.Machines_List->count()-1 );
		
		Update_VM_Ui();
		
		on_Button_Apply_clicked();
	}
}

void Main_Window::on_actionSave_As_Template_triggered()
{
	Create_Template_Window *templ_win = new Create_Template_Window();
	
	if( VM_List.count() > 0 ) templ_win->Set_VM_Path( VM_List[ui.Machines_List->currentRow()].Get_VM_XML_File_Path() );
	
	if( templ_win->exec() == QDialog::Accepted ) QMessageBox::information( this, tr("Information"), tr("New Template Created!") );
	
	delete templ_win;
}

void Main_Window::on_actionShow_Emulator_Control_triggered()
{
	if( VM_List.count() < 0 || ui.Machines_List->currentRow() < 0 ) return;
	
	if( VM_List[ui.Machines_List->currentRow()].Get_State() == VM::VMS_Running ||
		VM_List[ui.Machines_List->currentRow()].Get_State() == VM::VMS_Pause )
	{
		// Emulator Control is Visible?
		if( (Settings.value("Use_VNC_Display", "no").toString() == "yes" && ui.Tabs->tabText(0) == tr("Display")) )
		{
			AQGraphic_Warning( tr("Warning"), tr("Emulator Control Already Show") );
		}
		else
		{
			VM_List[ ui.Machines_List->currentRow() ].Show_Emu_Ctl_Win();
		}
	}
	else
	{
		AQGraphic_Warning( tr("Warning"), tr("This Function Work Only On Running VM!") );
	}
}

void Main_Window::on_actionManage_Snapshots_triggered()
{
	if( VM_List.count() < 0 ||
		ui.Machines_List->currentRow() < 0 )
	{
		return;
	}
	
	Snapshots_Window *snapshot_win = new Snapshots_Window( this );
	snapshot_win->Set_VM( &VM_List[ui.Machines_List->currentRow()] );
	snapshot_win->exec();
	
	delete snapshot_win;
}

void Main_Window::on_actionShow_QEMU_Arguments_triggered()
{
	if( VM_List.count() > 0 ) QMessageBox::information( this, tr("QEMU/KVM Arguments:"), Get_QEMU_Args() );
	else QMessageBox::information( this, tr("QEMU/KVM Arguments:"), tr("No VM Found!") );
}

void Main_Window::on_actionCreate_Shell_Script_triggered()
{
	if( VM_List.count() <= 0 ) return;
	
	QString script_code = "#!/bin/sh\n# This script created by AQEMU\n" + Get_Current_Binary_Name();
	QStringList all_args = VM_List[ui.Machines_List->currentRow()].Build_QEMU_Args_For_Script();
	
	for( int ix = 0; ix < all_args.count(); ix++ ) script_code += " " + all_args[ ix ];
	
	script_code = script_code.remove( "-monitor stdio" );
	
	// Turn Off Embedded VNC Display
	if( Settings.value("Use_VNC_Display", "no").toString() == "yes" &&
		ui.CH_No_Use_Embedded_Display->isChecked() == false )
	{
		script_code = script_code.remove( "-vnc :" + QString::number(100 + ui.Machines_List->currentRow()) );
	}
	
	// Save Script
	QString selectedFilter;
	QString fileName;
	
	fileName = QFileDialog::getSaveFileName( this, tr("Save VM to Script"),
			"VM_" + Get_FS_Compatible_VM_Name(VM_List[ui.Machines_List->currentRow()].Get_Machine_Name()),
			tr("Shell Script Files (*.sh);;All Files (*)"), &selectedFilter );
	
	if( ! fileName.isEmpty() )
	{
		// Save to File
		if( selectedFilter.indexOf("(*.sh)") >= 0 &&
			fileName.endsWith(".sh") == false )
		{
			fileName += ".sh";
		}
		
		QFile scriptFile( fileName );
		
		if( ! scriptFile.open(QIODevice::WriteOnly | QIODevice::Text) )
		{
			AQGraphic_Error( "void Main_Window::on_actionCreate_Shell_Script_triggered()",
							 tr("Error!"), tr("Cannot Open File!") );
			return;
		}
		
		QTextStream out( &scriptFile );
		out << script_code << " $*\n";
		
		// Set File Permissions
		scriptFile.setPermissions( scriptFile.permissions() | QFile::ExeOwner | QFile::ExeUser );
	}
}

void Main_Window::on_actionShow_QEMU_Error_Log_Window_triggered()
{
	if( VM_List.count() < 0 ||
		ui.Machines_List->currentRow() < 0 ) return;
	
	VM_List[ ui.Machines_List->currentRow() ].Show_Error_Log_Window();
}

void Main_Window::on_Memory_Size_valueChanged( int value )
{
	if( value >= 1 && value <= 2048 ) ui.CB_RAM_Size->setEditText( QString::number(value) );
}

void Main_Window::on_CB_RAM_Size_editTextChanged( const QString &text )
{
	bool ok = false;
	int value = text.toInt( &ok, 10 );
	
	if( ! ok )
	{
		AQWarning( "void Main_Window::on_CB_RAM_Size_editTextChanged( const QString &text )",
				   "Cannot Convert to Int!" );
		return;
	}
	
	if( value >= 1 && value <= 2048 )
	{
		ui.Memory_Size->setValue( value );
	}
}

void Main_Window::on_Check_Host_Mem_stateChanged( int state )
{
	switch( state )
	{
		case Qt::Checked:
			ui.Label_Max_Mem->setText( QString::number(System_Info::Get_Free_Memory_Size(), 10) + tr(" MB") );
			break;
			
		case Qt::Unchecked:
			ui.Label_Max_Mem->setText( tr("2048 MB") );
			break;
			
		default:
			ui.Label_Max_Mem->setText( tr("2048 MB") );
			break;
	}
}

void Main_Window::on_TB_FD0_SetPath_clicked()
{
	QFileDialog::Options options;
	QString selectedFilter;
	
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open Floppy Image File"),
													 Get_Last_Dir_Path(ui.Edit_FD0_Image_File_Path->text()),
													 tr("All Files (*);;Images Files (*.img *.ima)"), &selectedFilter, options );
	
	if( ! (fileName.isNull() || fileName.isEmpty()) )
	{
		ui.Edit_FD0_Image_File_Path->setText( fileName );
	}
}

void Main_Window::on_TB_FD1_SetPath_clicked()
{
	QFileDialog::Options options;
	QString selectedFilter;
	
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open Floppy Image File"),
													 Get_Last_Dir_Path(ui.Edit_FD1_Image_File_Path->text()),
													tr("All Files (*);;Images Files (*.img *.ima)"), &selectedFilter, options );
	
	if( ! (fileName.isNull() || fileName.isEmpty()) )
	{
		ui.Edit_FD1_Image_File_Path->setText( fileName );
	}
}

void Main_Window::on_TB_CDROM_SetPath_clicked()
{
	QFileDialog::Options options;
	QString selectedFilter;
	
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open CD\\DVD-ROM Image File"),
													 Get_Last_Dir_Path(ui.Edit_CDROM_Image_File_Path->text()),
													 tr("All Files (*);;Images Files (*.iso)"), &selectedFilter, options );
	
	if( ! (fileName.isNull() || fileName.isEmpty()) )
	{
		ui.Edit_CDROM_Image_File_Path->setText( fileName );
		
		Add_To_Recent_Files( fileName );
	}
}

void Main_Window::on_TB_HDA_SetPath_clicked()
{
	QFileDialog::Options options;
	QString selectedFilter;
	
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open HDD Image File"),
													 Get_Last_Dir_Path(ui.Edit_HDA_Image_Path->text()),
													 tr("All Files (*);;Images Files (*.img *.qcow *.qcow2 *.wmdk)"),
													 &selectedFilter, options );
	
	if( ! (fileName.isNull() || fileName.isEmpty()) )
	{
		ui.Edit_HDA_Image_Path->setText( fileName );
		HDA_Info->Update_Disk_Info( fileName );
	}
}

void Main_Window::on_TB_HDA_Create_HDD_clicked()
{
	Create_HDD_Image_Window *Create_HDD_Win = new Create_HDD_Image_Window( this );
	
	if( Create_HDD_Win->exec() == QDialog::Accepted )
	{
		ui.Edit_HDA_Image_Path->setText( Create_HDD_Win->Get_Image_File_Name() );
		HDA_Info->Update_Disk_Info( ui.Edit_HDA_Image_Path->text() );
	}
	
	delete Create_HDD_Win;
}

void Main_Window::on_TB_HDA_Format_HDD_clicked()
{
	Create_HDD_Image_Window *hdd_win = new Create_HDD_Image_Window( this );
	
	hdd_win->Set_Image_File_Name( ui.Edit_HDA_Image_Path->text() );
	hdd_win->Set_Image_Info( HDA_Info->Get_Disk_Info() );
	
	if( hdd_win->exec() == QDialog::Accepted )
	{
		HDA_Info->Update_Disk_Info( ui.Edit_HDA_Image_Path->text() );
	}
	
	delete hdd_win;
}

void Main_Window::on_TB_HDB_SetPath_clicked()
{
	QFileDialog::Options options;
	QString selectedFilter;
	
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open HDD Image File"),
													 Get_Last_Dir_Path(ui.Edit_HDB_Image_Path->text()),
													 tr("All Files (*);;Images Files (*.img *.qcow *.qcow2 *.wmdk)"),
													 &selectedFilter, options );
	
	if( ! (fileName.isNull() || fileName.isEmpty()) )
	{
		ui.Edit_HDB_Image_Path->setText( fileName );
		HDB_Info->Update_Disk_Info( fileName );
	}
}

void Main_Window::on_TB_HDB_Create_HDD_clicked()
{
	Create_HDD_Image_Window *Create_HDD_Win = new Create_HDD_Image_Window( this );
	
	if( Create_HDD_Win->exec() == QDialog::Accepted )
	{
		ui.Edit_HDB_Image_Path->setText( Create_HDD_Win->Get_Image_File_Name() );
		HDB_Info->Update_Disk_Info( ui.Edit_HDB_Image_Path->text() );
	}
	
	delete Create_HDD_Win;
}

void Main_Window::on_TB_HDB_Format_HDD_clicked()
{
	Create_HDD_Image_Window *hdd_win = new Create_HDD_Image_Window( this );
	
	hdd_win->Set_Image_File_Name( ui.Edit_HDB_Image_Path->text() );
	hdd_win->Set_Image_Info( HDB_Info->Get_Disk_Info() );
	hdd_win->exec();
	
	if( hdd_win->exec() == QDialog::Accepted )
	{
		HDB_Info->Update_Disk_Info( ui.Edit_HDB_Image_Path->text() );
	}
	
	delete hdd_win;
}

void Main_Window::on_TB_HDC_SetPath_clicked()
{
	QFileDialog::Options options;
	QString selectedFilter;
	
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open HDD Image File"),
													 Get_Last_Dir_Path(ui.Edit_HDC_Image_Path->text()),
													 tr("All Files (*);;Images Files (*.img *.qcow *.qcow2 *.wmdk)"),
													 &selectedFilter, options );
	
	if( ! (fileName.isNull() || fileName.isEmpty()) )
	{
		ui.Edit_HDC_Image_Path->setText( fileName );
		HDC_Info->Update_Disk_Info( fileName );
	}
}

void Main_Window::on_TB_HDC_Create_HDD_clicked()
{
	Create_HDD_Image_Window *Create_HDD_Win = new Create_HDD_Image_Window( this );
	
	if( Create_HDD_Win->exec() == QDialog::Accepted )
	{
		ui.Edit_HDC_Image_Path->setText( Create_HDD_Win->Get_Image_File_Name() );
		HDC_Info->Update_Disk_Info( ui.Edit_HDC_Image_Path->text() );
	}
	
	delete Create_HDD_Win;
}

void Main_Window::on_TB_HDC_Format_HDD_clicked()
{
	Create_HDD_Image_Window *hdd_win = new Create_HDD_Image_Window( this );
	
	hdd_win->Set_Image_File_Name( ui.Edit_HDC_Image_Path->text() );
	hdd_win->Set_Image_Info( HDC_Info->Get_Disk_Info() );
	hdd_win->exec();
	
	if( hdd_win->exec() == QDialog::Accepted )
	{
		HDC_Info->Update_Disk_Info( ui.Edit_HDC_Image_Path->text() );
	}
	
	delete hdd_win;
}

void Main_Window::on_TB_HDD_SetPath_clicked()
{
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open HDD Image File"),
													 Get_Last_Dir_Path(ui.Edit_HDD_Image_Path->text()),
													 tr("All Files (*);;Images Files (*.img *.qcow *.qcow2 *.wmdk)") );
	
	if( ! (fileName.isNull() || fileName.isEmpty()) )
	{
		ui.Edit_HDD_Image_Path->setText( fileName );
		HDD_Info->Update_Disk_Info( fileName );
	}
}

void Main_Window::on_TB_HDD_Create_HDD_clicked()
{
	Create_HDD_Image_Window *Create_HDD_Win = new Create_HDD_Image_Window( this );
	
	if( Create_HDD_Win->exec() == QDialog::Accepted )
	{
		ui.Edit_HDD_Image_Path->setText( Create_HDD_Win->Get_Image_File_Name() );
		HDD_Info->Update_Disk_Info( ui.Edit_HDD_Image_Path->text() );
	}
	
	delete Create_HDD_Win;
}

void Main_Window::on_TB_HDD_Format_HDD_clicked()
{
	Create_HDD_Image_Window *hdd_win = new Create_HDD_Image_Window( this );
	
	hdd_win->Set_Image_File_Name( ui.Edit_HDD_Image_Path->text() );
	hdd_win->Set_Image_Info( HDD_Info->Get_Disk_Info() );
	hdd_win->exec();
	
	if( hdd_win->exec() == QDialog::Accepted )
	{
		HDD_Info->Update_Disk_Info( ui.Edit_HDD_Image_Path->text() );
	}
	
	delete hdd_win;
}

void Main_Window::on_GB_HDC_toggled( bool on ) // CD-ROM or HDC
{
	if( on ) ui.GB_CDROM->setChecked( false );
}

QStringList Main_Window::Create_Info_HDD_String( const QString &disk_format, const VM::Device_Size &virtual_size,
												 const VM::Device_Size &disk_size, int cluster_size )
{
	QString suf_v = Get_TR_Size_Suffix( virtual_size );
	QString suf_d = Get_TR_Size_Suffix( disk_size );
	
	QStringList ret;
	ret << tr("Image Format: ") + disk_format + "\n" + tr("Allocated Disk Space: ") + QString::number(disk_size.Size) + " " + suf_d;
	ret << tr("Virtual Size: ") + QString::number(virtual_size.Size) + " " + suf_v + "\n" + tr("Cluster Size: ") + QString::number(cluster_size);
	
	return ret;
}

void Main_Window::Update_HDA_Info()
{
	VM_HDD tmp_hda = VM_HDD( true, ui.Edit_HDA_Image_Path->text() );
	tmp_hda.Set_Disk_Info( HDA_Info->Get_Disk_Info() );
	
	QStringList lines = Create_Info_HDD_String( tmp_hda.Get_Image_Format(), tmp_hda.Get_Virtual_Size(),
												tmp_hda.Get_Disk_Size(), tmp_hda.Get_Cluster_Size() );
	
	if( lines.count() == 2 )
	{
		ui.Label_HDA_Info_L->setText( lines[0] );
		ui.Label_HDA_Info_R->setText( lines[1] );
	}
}

void Main_Window::Update_HDB_Info()
{
	VM_HDD tmp_hdb = VM_HDD( true, ui.Edit_HDB_Image_Path->text() );
	tmp_hdb.Set_Disk_Info( HDB_Info->Get_Disk_Info() );
	
	QStringList lines = Create_Info_HDD_String( tmp_hdb.Get_Image_Format(), tmp_hdb.Get_Virtual_Size(),
												tmp_hdb.Get_Disk_Size(), tmp_hdb.Get_Cluster_Size() );
	
	if( lines.count() == 2 )
	{
		ui.Label_HDB_Info_L->setText( lines[0] );
		ui.Label_HDB_Info_R->setText( lines[1] );
	}
}

void Main_Window::Update_HDC_Info()
{
	VM_HDD tmp_hdc = VM_HDD( true, ui.Edit_HDC_Image_Path->text() );
	tmp_hdc.Set_Disk_Info( HDC_Info->Get_Disk_Info() );
	
	QStringList lines = Create_Info_HDD_String( tmp_hdc.Get_Image_Format(), tmp_hdc.Get_Virtual_Size(),
												tmp_hdc.Get_Disk_Size(), tmp_hdc.Get_Cluster_Size() );
	
	if( lines.count() == 2 )
	{
		ui.Label_HDC_Info_L->setText( lines[0] );
		ui.Label_HDC_Info_R->setText( lines[1] );
	}
}

void Main_Window::Update_HDD_Info()
{
	VM_HDD tmp_hdd = VM_HDD( true, ui.Edit_HDD_Image_Path->text() );
	tmp_hdd.Set_Disk_Info( HDD_Info->Get_Disk_Info() );
	
	QStringList lines = Create_Info_HDD_String( tmp_hdd.Get_Image_Format(), tmp_hdd.Get_Virtual_Size(),
												tmp_hdd.Get_Disk_Size(), tmp_hdd.Get_Cluster_Size() );
	
	if( lines.count() == 2 )
	{
		ui.Label_HDD_Info_L->setText( lines[0] );
		ui.Label_HDD_Info_R->setText( lines[1] );
	}
}

void Main_Window::on_RB_FD0_Use_Host_Device_toggled( bool on )
{
	ui.CB_FD0_Host_Devices->setEnabled( on );
	ui.RB_FD0_Use_Image_File->setChecked( ! on );
	ui.Edit_FD0_Image_File_Path->setEnabled( ! on );
	ui.TB_FD0_SetPath->setEnabled( ! on );
}

void Main_Window::on_RB_FD1_Use_Host_Device_toggled( bool on )
{
	ui.CB_FD1_Host_Devices->setEnabled( on );
	ui.RB_FD1_Use_Image_File->setChecked( ! on );
	ui.Edit_FD1_Image_File_Path->setEnabled( ! on );
	ui.TB_FD1_SetPath->setEnabled( ! on );
}

void Main_Window::on_RB_CDROM_Use_Host_Device_toggled( bool on )
{
	ui.CB_CDROM_Host_Devices->setEnabled( on );
	ui.RB_CDROM_Use_Image_File->setChecked( ! on );
	ui.Edit_CDROM_Image_File_Path->setEnabled( ! on );
	ui.TB_CDROM_SetPath->setEnabled( ! on );
}

void Main_Window::on_GB_CDROM_toggled( bool on ) // CD-ROM or HDC...
{
	if( on ) ui.GB_HDC->setChecked( false );
}

void Main_Window::on_CB_Computer_Type_currentIndexChanged( int index )
{
	Apply_Emulator( 3 );
}

void Main_Window::on_CB_Emulator_Version_currentIndexChanged( int index )
{
	Apply_Emulator( 2 );
}

void Main_Window::on_CB_Emulator_Type_currentIndexChanged( int index )
{
	Apply_Emulator( 1 );
}

void Main_Window::Apply_Emulator( int mode )
{
	static bool running = false;
	if( GUI_User_Mode == false || running == true ) return;
	running = true;
	
	// Vairables for switch
	int comp_index;
	QStringList cl;
	bool q = false, k = false;
	QList<Averable_Devices> *current_devices;
	int current_devices_count;
	Averable_Devices* cur_comp;
	
	switch( mode )
	{
		case 0:
			// Find Emulators
			for( int ix = 0; ix < All_Emulators_List.count(); ix++ )
			{
				if( All_Emulators_List[ix].Get_Type() == "QEMU" ) q = true;
				else if( All_Emulators_List[ix].Get_Type() == "KVM" ) k = true;
				else AQWarning( "void Main_Window::Apply_Emulator( int mode )", "Incorrect Emulator Type!" );
			}
			
			ui.CB_Emulator_Type->clear();
			if( q ) ui.CB_Emulator_Type->addItem( tr("QEMU") );
			if( k ) ui.CB_Emulator_Type->addItem( tr("KVM") );
			
		case 1:
			// Emulator Type
			ui.CB_Emulator_Version->clear();
			ui.CB_Emulator_Version->addItem( tr("Default") );
			
			for( int ix = 0; ix < All_Emulators_List.count(); ix++ )
			{
				if( All_Emulators_List[ix].Get_Type() == ui.CB_Emulator_Type->currentText() )
					ui.CB_Emulator_Version->addItem( All_Emulators_List[ix].Get_Name() );
			}
			
		case 2:
			// Emulator Version
			ui.CB_Computer_Type->clear();
			
			current_devices = &Get_Devices_Info();
			current_devices_count = current_devices->count();
			
			for( QList<Averable_Devices>::const_iterator i = current_devices->constBegin(); i != current_devices->constEnd(); i++ )
				ui.CB_Computer_Type->addItem( i->System.Caption );
			
		case 3:
			// Computer Type
			comp_index = ui.CB_Computer_Type->currentIndex();
			
			if( comp_index < 0 ) return;
			
			// CPU
			ui.CB_CPU_Type->clear();
			
			cl = QStringList();
			
			cur_comp = &Get_Devices_Info()[ comp_index ];
			
			for( int mx = 0; mx < cur_comp->CPU_List.count(); ++mx )
				cl << cur_comp->CPU_List[mx].Caption;
			
			ui.CB_CPU_Type->addItems( cl );
			
			// Machine
			ui.CB_Machine_Type->clear();
			
			cl = QStringList();
			
			for( int mx = 0; mx < cur_comp->Machine_List.count(); ++mx )
				cl << cur_comp->Machine_List[mx].Caption;
			
			ui.CB_Machine_Type->addItems( cl );
			
			// Video
			ui.CB_Video_Card->clear();
			
			cl = QStringList();
			
			for( int vx = 0; vx < cur_comp->Video_Card_List.count(); ++vx )
				cl << cur_comp->Video_Card_List[vx].Caption;
			
			ui.CB_Video_Card->addItems( cl );
			
			// Use Nativ Network Cards
			if( ui.RB_Network_Mode_New->isChecked() )
			{
				New_Network_Settings_Widget->Set_Network_Card_Models( cur_comp->Network_Card_List );
			}
			else
			{
				Old_Network_Settings_Widget->Set_Network_Card_Models( cur_comp->Network_Card_List );
			}
			
			// Audio
			if( cur_comp->Audio_Card_List.Audio_sb16 ) ui.CH_sb16->setEnabled( true );
			else ui.CH_sb16->setEnabled( false );
			
			if( cur_comp->Audio_Card_List.Audio_es1370 ) ui.CH_es1370->setEnabled( true );
			else ui.CH_es1370->setEnabled( false );
			
			if( cur_comp->Audio_Card_List.Audio_Adlib ) ui.CH_Adlib->setEnabled( true );
			else ui.CH_Adlib->setEnabled( false );
			
			if( cur_comp->Audio_Card_List.Audio_AC97 ) ui.CH_AC97->setEnabled( true );
			else ui.CH_AC97->setEnabled( false );
			
			if( cur_comp->Audio_Card_List.Audio_GUS ) ui.CH_GUS->setEnabled( true );
			else ui.CH_GUS->setEnabled( false );
			
			if( cur_comp->Audio_Card_List.Audio_PC_Speaker ) ui.CH_PCSPK->setEnabled( true );
			else ui.CH_PCSPK->setEnabled( false );
			
			// Other Options
			
			// SMP
			ui.CB_CPU_Count->setEnabled( cur_comp->PSO_SMP_Count != 1 );
			
			ui.CB_CPU_Count->clear();
			
			for( int cx = 1; cx -1 <= cur_comp->PSO_SMP_Count; cx *= 2 )
			{
				if( cx == 256 ) ui.CB_CPU_Count->addItem( QString::number(255) );
				else ui.CB_CPU_Count->addItem( QString::number(cx) );
			}
			
			// Initial Graphic Mode
			ui.CH_Init_Graphic_Mode->setEnabled( cur_comp->PSO_Initial_Graphic_Mode );
			
			// No FB Boot Check
			ui.CH_FDD_Boot->setEnabled( cur_comp->PSO_No_FB_Boot_Check );
			
			// Win2K Hack
			ui.CH_Win2K_Hack->setEnabled( cur_comp->PSO_Win2K_Hack );
			
			// Kernel KQEMU
			ui.RB_KQEMU_Full->setEnabled( cur_comp->PSO_Kernel_KQEMU );
			ui.RB_KQEMU_Enabled->setEnabled( cur_comp->PSO_Kernel_KQEMU );
			ui.RB_KQEMU_Use_if_Possible->setEnabled( cur_comp->PSO_Kernel_KQEMU );
			ui.RB_KQEMU_Disabled->setEnabled( cur_comp->PSO_Kernel_KQEMU );
			
			// PSO_No_ACPI
			ui.CH_ACPI->setEnabled( cur_comp->PSO_No_ACPI );
			
			// PSO_KVM
			ui.CH_No_KVM_IRQChip->setEnabled( cur_comp->PSO_KVM );
			ui.CH_No_KVM_Pit->setEnabled( cur_comp->PSO_KVM );
			ui.CH_KVM_Shadow_Memory->setEnabled( cur_comp->PSO_KVM );
			ui.SB_KVM_Shadow_Memory_Size->setEnabled( cur_comp->PSO_KVM );
			ui.Label_KVM_Shadow_Memory_Mb->setEnabled( cur_comp->PSO_KVM );
			
			// Verions
			ui.CH_MTDBlock->setEnabled( false );
			ui.CH_SD_Image->setEnabled( false );
			ui.CH_PFlash->setEnabled( false );
			ui.CH_No_Frame->setEnabled( false );
			ui.CH_Alt_Grab->setEnabled( false );
			ui.CH_Portrait->setEnabled( false );
			ui.RB_VNC_Unix_Socket->setEnabled( false );
			ui.CH_VNC_Password->setEnabled( false );
			ui.CH_Use_VNC_TLS->setEnabled( false );
			ui.CH_x509_Folder->setEnabled( false );
			ui.CH_x509verify_Folder->setEnabled( false );
			ui.CH_Show_Cursor->setEnabled( false );
			ui.CH_Curses->setEnabled( false );
			ui.CH_No_Shutdown->setEnabled( false );
			ui.CH_KVM_No_Pit_Reinjection->setEnabled( false );
			ui.CH_KVM_Nesting->setEnabled( false );
			
			if( Current_Emulator_Version_Good(VM::QEMU_0_9_1, VM::KVM_7X) )
			{
				ui.CH_MTDBlock->setEnabled( true );
				ui.CH_SD_Image->setEnabled( true );
				ui.CH_PFlash->setEnabled( true );
				ui.CH_No_Frame->setEnabled( true );
				ui.CH_Alt_Grab->setEnabled( true );
				ui.CH_Portrait->setEnabled( true );
				ui.RB_VNC_Unix_Socket->setEnabled( true );
				ui.CH_VNC_Password->setEnabled( true );
				ui.CH_Use_VNC_TLS->setEnabled( true );
				ui.CH_x509_Folder->setEnabled( true );
				ui.CH_x509verify_Folder->setEnabled( true );
			}
			
			if( Current_Emulator_Version_Good(VM::QEMU_0_10, VM::KVM_8X) )
			{
				ui.CH_Show_Cursor->setEnabled( true );
				ui.CH_Curses->setEnabled( true );
				ui.CH_No_Shutdown->setEnabled( true );
				
				if( cur_comp->PSO_KVM )
				{
					ui.CH_KVM_No_Pit_Reinjection->setEnabled( true );
					ui.CH_KVM_Nesting->setEnabled( true );
				}
			}
			break;
			
		default:
			AQWarning( "void Main_Window::Apply_Emulator( int mode )", "Default Section!" );
			break;
	}
	
	running = false;
}

void Main_Window::on_CH_Local_Time_toggled( bool on )
{
	if( on ) ui.CH_Start_Date->setChecked( false );
}

void Main_Window::on_Tabs_currentChanged( int index )
{
	// Use Device Manager Mode
	if( Settings.value("Use_Device_Manager", "").toString() == "yes" )
	{
		if( index == 2 )
		{
			Dev_Manager->Update_List_Mode();
		}
	}
}

void Main_Window::on_Button_Apply_clicked()
{
	AQDebug( "void Main_Window::on_Button_Apply_clicked()", "Begin" );
	
	Virtual_Machine tmp_vm;
	
	if( Create_VM_From_Ui(tmp_vm,ui.Machines_List->currentRow()) == false ) return;
	
	QString old_path = "";
	
	if( VM_List[ ui.Machines_List->currentRow() ].Get_Machine_Name() != tmp_vm.Get_Machine_Name() )
	{
		old_path = VM_List[ ui.Machines_List->currentRow() ].Get_VM_XML_File_Path();
	}
	
	// save all Settings
	disconnect( &VM_List[ui.Machines_List->currentRow()], SIGNAL(State_Changet(Virtual_Machine*, VM::VM_State)),
				this, SLOT(VM_State_Changet(Virtual_Machine*, VM::VM_State)) );
	
	VM_List[ ui.Machines_List->currentRow() ] = tmp_vm;
	
	connect( &VM_List[ui.Machines_List->currentRow()], SIGNAL(State_Changet(Virtual_Machine*, VM::VM_State)),
			 this, SLOT(VM_State_Changet(Virtual_Machine*, VM::VM_State)) );
	
	// save to file
	if( ! old_path.isEmpty() )
	{
		// Create new file name
		VM_List[ ui.Machines_List->currentRow() ].Set_VM_XML_File_Path(
				Get_Complete_VM_File_Path(VM_List[ui.Machines_List->currentRow()].Get_Machine_Name()) );
	}
	
	if( VM_List[ui.Machines_List->currentRow()].Save_VM() == false )
	{
		AQGraphic_Error( "void Main_Window::on_Button_Apply_clicked()",
						 tr("Error!"), tr("Cannot Save This VM to File!") );
		return;
	}
	else if( ! old_path.isEmpty() ) // OK New File Saved
	{
		if( ! QFile::remove( old_path ) )
		{
			AQWarning( "void Main_Window::on_Button_Apply_clicked()",
					   "Cannot Remove File: \"" + old_path + "\"" );
		}
	}
	
	// Set VM Name
	ui.Machines_List->currentItem()->setText( VM_List[ui.Machines_List->currentRow()].Get_Machine_Name() );
	
	Update_Info_Text();
	
	// For VM Changes Signals
	ui.Button_Apply->setEnabled( false );
	ui.Button_Cancel->setEnabled( false );
	
	AQDebug( "void Main_Window::on_Button_Apply_clicked()", "End" );
}

void Main_Window::on_Button_Cancel_clicked()
{
	// load Settings
	Update_VM_Ui();
}

void Main_Window::on_RB_Network_Mode_New_toggled( bool on )
{
	while( ui.Network_Cards_Tabs->count() > 1 )
	{
		ui.Network_Cards_Tabs->removeTab( 0 );
	}
	
	if( on )
	{
		ui.Network_Cards_Tabs->insertTab( 0, New_Network_Settings_Widget, tr("Network Settings") );
	}
	else
	{
		ui.Network_Cards_Tabs->insertTab( 0, Old_Network_Settings_Widget, tr("Network Cards") );
	}
	
	ui.Network_Cards_Tabs->setCurrentIndex( 0 );
}

void Main_Window::on_Redirections_List_cellClicked ( int row, int column )
{
	if( ui.Redirections_List->item( row, 0 )->text() == "TCP" ) ui.RB_TCP->setChecked( true );
	else ui.RB_UDP->setChecked( true );
	
	ui.SB_Redir_Port->setValue( ui.Redirections_List->item( row, 1 )->text().toInt() );
	ui.Edit_Guest_IP->setText( ui.Redirections_List->item( row, 2 )->text() );
	ui.SB_Guest_Port->setValue( ui.Redirections_List->item( row, 3 )->text().toInt() );
}

void Main_Window::on_Button_Add_Redirections_clicked()
{
	// Port < 1024
	if( ui.SB_Redir_Port->value() < 1024 )
	{
		int ret = QMessageBox::question( this, tr("Warning!"),
					tr("For Create Socket With Port Number < 1024, in Unix You Need to Run AQEMU in root Mode!\nAdd This Record?"),
					QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
		
		if( ret == QMessageBox::No ) return;
	}
	
	QTableWidgetItem *newItem;
	VM_Redirection *tmp_r = new VM_Redirection();
	
	if( ui.RB_TCP->isChecked() )
	{
		newItem = new QTableWidgetItem( "TCP" );
		tmp_r->Set_Protocol( "TCP" );
	}
	else
	{
		newItem = new QTableWidgetItem( "UDP" );
		tmp_r->Set_Protocol( "UDP" );
	}
	
	ui.Redirections_List->insertRow( ui.Redirections_List->rowCount() );
	
	ui.Redirections_List->setItem( ui.Redirections_List->rowCount()-1, 0, newItem ); // protocol
	
	newItem = new QTableWidgetItem( QString::number(ui.SB_Redir_Port->value()) );
	ui.Redirections_List->setItem( ui.Redirections_List->rowCount()-1, 1, newItem ); // port
	tmp_r->Set_Host_Port( ui.SB_Redir_Port->value() );
	
	newItem = new QTableWidgetItem( ui.Edit_Guest_IP->text() );
	ui.Redirections_List->setItem( ui.Redirections_List->rowCount()-1, 2, newItem ); // ip
	tmp_r->Set_Guest_IP( ui.Edit_Guest_IP->text() );
	
	newItem = new QTableWidgetItem( QString::number(ui.SB_Guest_Port->value()) );
	ui.Redirections_List->setItem( ui.Redirections_List->rowCount()-1, 3, newItem ); // guest port
	tmp_r->Set_Guest_Port( ui.SB_Guest_Port->value() );
	
	ui.Redirections_List->setCurrentCell( ui.Redirections_List->rowCount()-1 , 0 );
}

void Main_Window::on_Button_Delete_Redirections_clicked()
{
	if( ui.Redirections_List->currentRow() > -1 ) ui.Redirections_List->removeRow( ui.Redirections_List->currentRow() );
}

void Main_Window::on_Button_Apply_Redirections_clicked()
{
	QTableWidgetItem *newItem;
	VM_Redirection *tmp_r = new VM_Redirection();
	
	if( ui.RB_TCP->isChecked() )
	{
		newItem = new QTableWidgetItem( "TCP" );
		tmp_r->Set_Protocol( "TCP" );
	}
	else
	{
		newItem = new QTableWidgetItem( "UDP" );
		tmp_r->Set_Protocol( "UDP" );
	}
	
	ui.Redirections_List->setItem( ui.Redirections_List->currentRow(), 0, newItem ); // protocol
	
	newItem = new QTableWidgetItem( QString::number(ui.SB_Redir_Port->value()) );
	ui.Redirections_List->setItem( ui.Redirections_List->currentRow(), 1, newItem ); // host port
	tmp_r->Set_Host_Port( ui.SB_Redir_Port->value() );
	
	newItem = new QTableWidgetItem( ui.Edit_Guest_IP->text() );
	ui.Redirections_List->setItem( ui.Redirections_List->currentRow(), 2, newItem ); // to guets ip
	tmp_r->Set_Guest_IP( ui.Edit_Guest_IP->text() );
	
	newItem = new QTableWidgetItem( QString::number(ui.SB_Guest_Port->value()) );
	ui.Redirections_List->setItem( ui.Redirections_List->currentRow(), 3, newItem ); // guest port
	tmp_r->Set_Guest_Port( ui.SB_Guest_Port->value() );
}

void Main_Window::on_Button_Clear_Redirections_clicked()
{
	while( ui.Redirections_List->currentRow() > -1 ) ui.Redirections_List->removeRow( ui.Redirections_List->currentRow() );
}

void Main_Window::on_TB_Browse_SMB_clicked()
{
	QString SMB_Dir = QFileDialog::getExistingDirectory( this, tr("Select SMB Directory"), "/", QFileDialog::ShowDirsOnly );
	
	if( ! (SMB_Dir.isNull() || SMB_Dir.isEmpty()) )
	{
		ui.Edit_SMB_Folder->setText( SMB_Dir );
	}
}

void Main_Window::on_CH_Start_Date_toggled( bool on )
{
	if( on ) ui.CH_Local_Time->setChecked( false );
}

void Main_Window::on_TB_VNC_Unix_Socket_Browse_clicked()
{
	QFileDialog::Options options;
	QString selectedFilter;
	
	QString socket_path = QFileDialog::getOpenFileName( this, tr("UNIX Domain Socket Path"),
														Get_Last_Dir_Path(ui.Edit_Linux_bzImage_Path->text()),
														tr("All Files (*)"), &selectedFilter, options );
	
	if( ! (socket_path.isNull() || socket_path.isEmpty()) ) ui.Edit_VNC_Unix_Socket->setText( socket_path );
}

void Main_Window::on_TB_x509_Browse_clicked()
{
	QString x509dir = QFileDialog::getExistingDirectory( this, tr("Select x509 Certificate Folder"), "/", QFileDialog::ShowDirsOnly );
	
	if( ! (x509dir.isNull() || x509dir.isEmpty()) ) ui.Edit_x509_Folder->setText( x509dir );
}

void Main_Window::on_TB_x509verify_Browse_clicked()
{
	QString x509verify_dir = QFileDialog::getExistingDirectory( this, tr("Select x509 Verify Certificate Folder"), "/", QFileDialog::ShowDirsOnly );
	
	if( ! (x509verify_dir.isNull() || x509verify_dir.isEmpty()) ) ui.Edit_x509verify_Folder->setText( x509verify_dir );
}

void Main_Window::on_TB_Linux_bzImage_SetPath_clicked()
{
	QFileDialog::Options options;
	QString selectedFilter;
	
	QString kernel = QFileDialog::getOpenFileName( this, tr("Select Kernel Image File"),
												   Get_Last_Dir_Path(ui.Edit_Linux_bzImage_Path->text()),
												   tr("All Files (*)"), &selectedFilter, options );
	
	if( ! (kernel.isNull() || kernel.isEmpty()) ) ui.Edit_Linux_bzImage_Path->setText( kernel );
}

void Main_Window::on_TB_Linux_Initrd_SetPath_clicked()
{
	QFileDialog::Options options;
	QString selectedFilter;
	
	QString initrd = QFileDialog::getOpenFileName( this, tr("Select InitRD File"),
												   Get_Last_Dir_Path(ui.Edit_Linux_Initrd_Path->text()),
												   tr("All Files (*)"), &selectedFilter, options );
	
	if( ! (initrd.isNull() || initrd.isEmpty()) ) ui.Edit_Linux_Initrd_Path->setText( initrd );
}

void Main_Window::on_TB_ROM_File_Browse_clicked()
{
	QFileDialog::Options options;
	QString selectedFilter;
	
	QString rom_file = QFileDialog::getOpenFileName( this, tr("Select ROM File"),
													 Get_Last_Dir_Path(ui.Edit_ROM_File->text()),
													 tr("All Files (*)"), &selectedFilter, options );
	
	if( ! (rom_file.isNull() || rom_file.isEmpty()) ) ui.Edit_ROM_File->setText( rom_file );
}

void Main_Window::on_TB_MTDBlock_File_Browse_clicked()
{
	QFileDialog::Options options;
	QString selectedFilter;
	
	QString mtd_file = QFileDialog::getOpenFileName( this, tr("Select On-Board Flash Image"),
													 Get_Last_Dir_Path(ui.Edit_MTDBlock_File->text()),
													 tr("All Files (*)"), &selectedFilter, options );
	
	if( ! (mtd_file.isNull() || mtd_file.isEmpty()) ) ui.Edit_MTDBlock_File->setText( mtd_file );
}

void Main_Window::on_TB_SD_Image_File_Browse_clicked()
{
	QFileDialog::Options options;
	QString selectedFilter;
	
	QString sd_file = QFileDialog::getOpenFileName( this, tr("Select SecureDigital Card Image"),
													Get_Last_Dir_Path(ui.Edit_SD_Image_File->text()),
													tr("All Files (*)"), &selectedFilter, options );
	
	if( ! (sd_file.isNull() || sd_file.isEmpty()) ) ui.Edit_SD_Image_File->setText( sd_file );
}

void Main_Window::on_TB_PFlash_File_Browse_clicked()
{
	QFileDialog::Options options;
	QString selectedFilter;
	
	QString flash_file = QFileDialog::getOpenFileName( this, tr("Select Parallel Flash Image"),
													   Get_Last_Dir_Path(ui.Edit_PFlash_File->text()),
													   tr("All Files (*)"), &selectedFilter, options );
	
	if( ! (flash_file.isNull() || flash_file.isEmpty()) ) ui.Edit_PFlash_File->setText( flash_file );
}

QString Main_Window::Copy_VM_Hard_Drive( const QString &vm_name, const QString &hd_name, const VM_HDD &hd )
{
	if( vm_name.isEmpty() )
	{
		AQError( "QString Main_Window::Copy_VM_Hard_Drive( const QString &vm_name, const QString &hd_name, const VM_HDD &hd )",
				 "vm_name is Empty!" );
		return "";
	}
	else
	{
		QString new_name = Settings.value("VM_Directory", "~").toString() +
						   Get_FS_Compatible_VM_Name( vm_name ) + "_" + hd_name;
		
		if( QFile::exists(new_name) )
		{
			for( int ix = 0; ix < 1024; ++ix )
			{
				if( ! QFile::exists(new_name + "_" + QString::number(ix)) ) new_name += "_" + QString::number( ix );
			}
		}
		
		if( ! QFile::copy(hd.Get_Image_File_Name(), new_name) )
		{
			AQError( "QString Main_Window::Copy_VM_Hard_Drive( const QString &vm_name, const QString &hd_name, const VM_HDD &hd )",
					 "Copy Error!" );
		}
		
		return new_name;
	}
}

QString Main_Window::Copy_VM_Floppy( const QString &vm_name, const QString &fd_name, const VM_Floppy &fd )
{
	if( vm_name.isEmpty() )
	{
		AQError( "QString Main_Window::Copy_VM_Floppy( const QString &vm_name, const QString &fd_name, const VM_Floppy &fd )",
				 "vm_name is Empty!" );
		return "";
	}
	else
	{
		QString new_name = Settings.value("VM_Directory", "~").toString() +
				Get_FS_Compatible_VM_Name( vm_name ) + "_" + fd_name;
		
		if( QFile::exists(new_name) )
		{
			for( int ix = 0; ix < 1024; ++ix )
			{
				if( ! QFile::exists(new_name + "_" + QString::number(ix)) ) new_name += "_" + QString::number( ix );
			}
		}
		
		if( ! QFile::copy(fd.Get_Image_File_Name(), new_name) )
		{
			AQError( "QString Main_Window::Copy_VM_Floppy( const QString &vm_name, const QString &fd_name, const VM_Floppy &fd )",
					 "Copy Error!" );
		}
		
		return new_name;
	}
}
