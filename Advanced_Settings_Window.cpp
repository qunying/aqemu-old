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
#include <QInputDialog>

#include "Advanced_Settings_Window.h"
#include "System_Info.h"
#include "Utils.h"
#include "Emulator_Options_Window.h"
#include "First_Start_Wizard.h"

Advanced_Settings_Window::Advanced_Settings_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
	
	QHeaderView *hv = new QHeaderView( Qt::Vertical, ui.Emulators_Table );
	hv->setResizeMode( QHeaderView::Fixed );
	ui.Emulators_Table->setVerticalHeader( hv );
	
	hv = new QHeaderView( Qt::Horizontal, ui.Emulators_Table );
	hv->setResizeMode( QHeaderView::Stretch );
	ui.Emulators_Table->setHorizontalHeader( hv );
	
	// Minimum Size
	resize( minimumSizeHint().width(), minimumSizeHint().height() );
	
	// Execute Before Start QEMU
	ui.Edit_Before_Start_Command->setText( Settings.value("Run_Before_QEMU", "").toString() );
	
	// Execute After Stop QEMU
	ui.Edit_After_Stop_Command->setText( Settings.value("Run_After_QEMU", "").toString() );
	
	// Use Shared Folder For All Screenshots
	ui.CH_Screenshot_Folder->setChecked( Settings.value("Use_Screenshots_Folder", "no").toString() == "yes" );
	
	// Screenshots Shared Folder Path
	ui.Edit_Screenshot_Folder->setText( Settings.value("Screenshot_Folder_Path", "").toString() );
	
	// Screenshot save format
	QString fmt = Settings.value( "Screenshot_Save_Format", "PNG" ).toString();
	
	if( fmt == "PNG" ) ui.RB_Format_PNG->setChecked( true );
	else if( fmt == "JPEG" ) ui.RB_Format_Jpeg->setChecked( true );
	else ui.RB_Format_PPM->setChecked( true );
	
	// Jpeg Quality
	ui.HS_Jpeg_Quality->setValue( Settings.value("Jpeg_Quality", "75").toString().toInt() );
	
	// Additional CDROM
	int cdrom_count = Settings.value( "Additional_CDROM_Devices/Count", "0" ).toString().toInt();
	
	for( int ix = 0; ix < cdrom_count; ix++ )
	{
		ui.CDROM_List->addItem( Settings.value("Additional_CDROM_Devices/Device" + QString::number(ix), "").toString() );
	}
	
	// Information in Info Tab
	ui.CH_Show_Tab_Info->setChecked( Settings.value("Info/Show_Tab_Info", "yes").toString() == "yes" );
	ui.CH_Show_QEMU_Args->setChecked( Settings.value("Info/Show_QEMU_Args", "no").toString() == "yes" );
	ui.CH_Show_Screenshot_in_Save_Mode->setChecked( Settings.value("Info/Show_Screenshot_in_Save_Mode", "no").toString() == "yes" );
	ui.CH_Machine_Details->setChecked( Settings.value("Info/Machine_Details", "yes").toString() == "yes" );
	ui.CH_Machine_Name->setChecked( Settings.value("Info/Machine_Name", "yes").toString() == "yes" );
	ui.CH_Computer_Type->setChecked( Settings.value("Info/Computer_Type", "yes").toString() == "yes" );
	ui.CH_Machine_Type->setChecked( Settings.value("Info/Machine_Type", "yes").toString() == "yes" );
	ui.CH_CPU_Type->setChecked( Settings.value("Info/CPU_Type", "yes").toString() == "yes" );
	ui.CH_Number_of_CPU->setChecked( Settings.value("Info/Number_of_CPU", "no").toString() == "yes" );
	ui.CH_Acceleration->setChecked( Settings.value("Info/Acceleration", "no").toString() == "yes" );
	ui.CH_Boot_Priority->setChecked( Settings.value("Info/Boot_Priority", "yes").toString() == "yes" );
	ui.CH_Video_Card->setChecked( Settings.value("Info/Video_Card", "no").toString() == "yes" );
	ui.CH_Keyboard_Layout->setChecked( Settings.value("Info/Keyboard_Layout", "no").toString() == "yes" );
	ui.CH_Memory_Size->setChecked( Settings.value("Info/Memory_Size", "yes").toString() == "yes" );
	ui.CH_Use_Sound->setChecked( Settings.value("Info/Use_Sound", "yes").toString() == "yes" );
	ui.CH_Fullscreen->setChecked( Settings.value("Info/Fullscreen", "yes").toString() == "yes" );
	ui.CH_Win2K_Hack->setChecked( Settings.value("Info/Win2K_Hack", "no").toString() == "yes" );
	ui.CH_ACPI->setChecked( Settings.value("Info/ACPI", "yes").toString() == "yes" );
	ui.CH_Localtime->setChecked( Settings.value("Info/Localtime", "yes").toString() == "yes" );
	ui.CH_Check_Boot_on_FDD->setChecked( Settings.value("Info/Check_Boot_on_FDD", "no").toString() == "yes" );
	ui.CH_Start_CPU->setChecked( Settings.value("Info/Start_CPU", "no").toString() == "yes" );
	ui.CH_Snapshot->setChecked( Settings.value("Info/Snapshot", "yes").toString() == "yes" );
	ui.CH_QEMU_Log->setChecked( Settings.value("Info/QEMU_Log", "no").toString() == "yes" );
	ui.CH_No_Reboot->setChecked( Settings.value("Info/No_Reboot", "no").toString() == "yes" );
	ui.CH_Network_Cards->setChecked( Settings.value("Info/Network_Cards", "yes").toString() == "yes" );
	ui.CH_Redirections->setChecked( Settings.value("Info/Redirections", "no").toString() == "yes" );
	ui.CH_TFTP_Server_Prefix->setChecked( Settings.value("Info/TFTP_Server_Prefix", "no").toString() == "yes" );
	ui.CH_SMB_Dir->setChecked( Settings.value("Info/SMB_Dir", "no").toString() == "yes" );
	ui.CH_Serial_Port->setChecked( Settings.value("Info/Serial_Port", "no").toString() == "yes" );
	ui.CH_Parallel_Port->setChecked( Settings.value("Info/Parallel_Port", "no").toString() == "yes" );
	ui.CH_USB_Port->setChecked( Settings.value("Info/USB_Port", "no").toString() == "yes" );
	ui.CH_ROM_File->setChecked( Settings.value("Info/ROM_File", "no").toString() == "yes" );
	ui.CH_MTDBlock->setChecked( Settings.value("Info/MTDBlock", "no").toString() == "yes" );
	ui.CH_SD_Image->setChecked( Settings.value("Info/SD_Image", "no").toString() == "yes" );
	ui.CH_PFlash->setChecked( Settings.value("Info/PFlash", "no").toString() == "yes" );
	ui.CH_No_Frame->setChecked( Settings.value("Info/No_Frame", "no").toString() == "yes" );
	ui.CH_Alt_Grab->setChecked( Settings.value("Info/Alt_Grab", "no").toString() == "yes" );
	ui.CH_No_Quit->setChecked( Settings.value("Info/No_Quit", "no").toString() == "yes" );
	ui.CH_Portrait->setChecked( Settings.value("Info/Portrait", "no").toString() == "yes" );
	ui.CH_GDB_Port->setChecked( Settings.value("Info/GDB_Port", "no").toString() == "yes" );
	ui.CH_Linux_Boot->setChecked( Settings.value("Info/Linux_Boot", "no").toString() == "yes" );
	ui.CH_Show_FDD->setChecked( Settings.value("Info/Show_FDD", "yes").toString() == "yes" );
	ui.CH_Show_CD->setChecked( Settings.value("Info/Show_CD", "yes").toString() == "yes" );
	ui.CH_Show_HDD->setChecked( Settings.value("Info/Show_HDD", "yes").toString() == "yes" );
	
	// MAC Address Generation Mode
	ui.RB_MAC_Random->setChecked( Settings.value("MAC_Generation_Mode", "Model").toString() == "Random" );
	ui.RB_MAC_QEMU->setChecked( Settings.value("MAC_Generation_Mode", "Model").toString() == "QEMU_Segment" );
	ui.RB_MAC_Valid->setChecked( Settings.value("MAC_Generation_Mode", "Model").toString() == "Model" );
	
	// Save to Log File
	ui.CH_Log_Save_in_File->setChecked( Settings.value("Log/Save_In_File", "yes").toString() == "yes" );
	
	// Print In StdOut
	ui.CH_Log_Print_in_STDIO->setChecked( Settings.value("Log/Print_In_STDOUT", "yes").toString() == "yes" );
	
	// Log File Path
	ui.Edit_Log_Path->setText( Settings.value("Log/Log_Path", Settings.value("VM_Directory", "").toString() + "aqemu.log").toString() );
	
	// Save to AQEMU Log
	ui.CH_Log_Debug->setChecked( Settings.value("Log/Save_Debug", "yes").toString() == "yes" );
	ui.CH_Log_Warning->setChecked( Settings.value("Log/Save_Warning", "yes").toString() == "yes" );
	ui.CH_Log_Error->setChecked( Settings.value("Log/Save_Error", "yes").toString() == "yes" );
	
	// QEMU-IMG Path
	ui.Edit_QEMU_IMG_Path->setText( Settings.value("QEMU-IMG_Path", "qemu-img").toString() );
	
	// Recent CD Count
	ui.SB_Recent_CD_Count->setValue( Settings.value("CD_ROM_Exits_Images/Max", "5").toString().toInt() );
	
	// Recent FDD Count
	ui.SB_Recent_FDD_Count->setValue( Settings.value("Floppy_Exits_Images/Max", "5").toString().toInt() );
	
	// First VNC Port for Embedded Display
	ui.SB_First_VNC_Port->setValue( Settings.value("First_VNC_Port", "5910").toString().toInt() );
	
	// QEMU_AUDIO
	ui.CH_Audio_Default->setChecked( Settings.value("QEMU_AUDIO/Use_Default_Driver", "yes").toString() == "no" );
	
	// QEMU_AUDIO_DRV
	for( int ix = 0; ix < ui.CB_Host_Sound_System->count(); ++ix )
	{
		if( ui.CB_Host_Sound_System->itemText(ix) ==
			Settings.value("QEMU_AUDIO/QEMU_AUDIO_DRV", "alsa").toString() )
		{
			ui.CB_Host_Sound_System->setCurrentIndex( ix );
		}
	}
	
	if( Load_Emulators_Info() ) Update_Emulators_Info();
}

void Advanced_Settings_Window::on_Button_OK_clicked()
{
	// Execute Before Start QEMU
	Settings.setValue( "Run_Before_QEMU", ui.Edit_Before_Start_Command->text() );
	
	// Execute After Stop QEMU
	Settings.setValue( "Run_After_QEMU", ui.Edit_After_Stop_Command->text() );
	
	// Use Shared Folder For All Screenshots
	if( ui.CH_Screenshot_Folder->isChecked() )
	{
		Settings.setValue( "Use_Screenshots_Folder", "yes" );
		
		QDir dir; // For Check on valid
		
		// Screenshots Shared Folder Path
		if( dir.exists(ui.Edit_Screenshot_Folder->text()) )
		{
			Settings.setValue( "Screenshot_Folder_Path", ui.Edit_Screenshot_Folder->text() );
		}
		else
		{
			AQGraphic_Warning( tr("Invalid Value!"), tr("Shared Folder for Screenshots is Not Exists!") );
			return;
		}
	}
	else
	{
		Settings.setValue( "Use_Screenshots_Folder", "no" );
		
		// Screenshots Shared Folder Path
		Settings.setValue( "Screenshot_Folder_Path", ui.Edit_Screenshot_Folder->text() );
	}
	
	// Screenshot save format
	if( ui.RB_Format_PNG->isChecked() ) Settings.setValue( "Screenshot_Save_Format", "PNG" );
	else if( ui.RB_Format_Jpeg->isChecked() ) Settings.setValue( "Screenshot_Save_Format", "JPEG" );
	else Settings.setValue( "Screenshot_Save_Format", "PPM" );
	
	// Jpeg Quality
	Settings.setValue( "Jpeg_Quality", QString::number(ui.HS_Jpeg_Quality->value()) );
	
	// Additional CDROM
	int old_count = Settings.value( "Additional_CDROM_Devices/Count", "0" ).toString().toInt();
	
	if( old_count > ui.CDROM_List->count() )
	{
		// Delete Old Items
		for( int dx = ui.CDROM_List->count()-1; dx < old_count; dx++ )
		{
			Settings.remove( "Additional_CDROM_Devices/Device" + QString::number(dx) );
		}
	}
	
	Settings.setValue( "Additional_CDROM_Devices/Count", QString::number(ui.CDROM_List->count()) );
	
	for( int ix = 0; ix < ui.CDROM_List->count(); ix++ )
	{
		Settings.setValue( "Additional_CDROM_Devices/Device" + QString::number(ix), ui.CDROM_List->item(ix)->text() );
	}
	
	// Information in Info Tab
	if( ui.CH_Show_Tab_Info->isChecked() ) Settings.setValue( "Info/Show_Tab_Info", "yes" );
	else Settings.setValue( "Info/Show_Tab_Info", "no" );
	
	if( ui.CH_Show_QEMU_Args->isChecked() ) Settings.setValue( "Info/Show_QEMU_Args", "yes" );
	else Settings.setValue( "Info/Show_QEMU_Args", "no" );
	
	if( ui.CH_Show_Screenshot_in_Save_Mode->isChecked() ) Settings.setValue( "Info/Show_Screenshot_in_Save_Mode", "yes" );
	else Settings.setValue( "Info/Show_Screenshot_in_Save_Mode", "no" );
	
	if( ui.CH_Emulator_Type->isChecked() ) Settings.setValue( "Info/Emulator_Type", "yes" );
	else Settings.setValue( "Info/Emulator_Type", "no" );
	
	if( ui.CH_Emulator_Version->isChecked() ) Settings.setValue( "Info/Emulator_Version", "yes" );
	else Settings.setValue( "Info/Emulator_Version", "no" );
	
	if( ui.CH_Machine_Details->isChecked() ) Settings.setValue( "Info/Machine_Details", "yes" );
	else Settings.setValue( "Info/Machine_Details", "no" );
	
	if( ui.CH_Machine_Name->isChecked() ) Settings.setValue( "Info/Machine_Name", "yes" );
	else Settings.setValue( "Info/Machine_Name", "no" );
	
	if( ui.CH_Computer_Type->isChecked() ) Settings.setValue( "Info/Computer_Type", "yes" );
	else Settings.setValue( "Info/Computer_Type", "no" );
	
	if( ui.CH_Machine_Type->isChecked() ) Settings.setValue( "Info/Machine_Type", "yes" );
	else Settings.setValue( "Info/Machine_Type", "no" );
	
	if( ui.CH_CPU_Type->isChecked() ) Settings.setValue( "Info/CPU_Type", "yes" );
	else Settings.setValue( "Info/CPU_Type", "no" );
	
	if( ui.CH_Number_of_CPU->isChecked() ) Settings.setValue( "Info/Number_of_CPU", "yes" );
	else Settings.setValue( "Info/Number_of_CPU", "no" );
	
	if( ui.CH_Acceleration->isChecked() ) Settings.setValue( "Info/Acceleration", "yes" );
	else Settings.setValue( "Info/Acceleration", "no" );
	
	if( ui.CH_Boot_Priority->isChecked() ) Settings.setValue( "Info/Boot_Priority", "yes" );
	else Settings.setValue( "Info/Boot_Priority", "no" );
	
	if( ui.CH_Video_Card->isChecked() ) Settings.setValue( "Info/Video_Card", "yes" );
	else Settings.setValue( "Info/Video_Card", "no" );
	
	if( ui.CH_Keyboard_Layout->isChecked() ) Settings.setValue( "Info/Keyboard_Layout", "yes" );
	else Settings.setValue( "Info/Keyboard_Layout", "no" );
	
	if( ui.CH_Memory_Size->isChecked() ) Settings.setValue( "Info/Memory_Size", "yes" );
	else Settings.setValue( "Info/Memory_Size", "no" );
	
	if( ui.CH_Use_Sound->isChecked() ) Settings.setValue( "Info/Use_Sound", "yes" );
	else Settings.setValue( "Info/Use_Sound", "no" );
	
	if( ui.CH_Fullscreen->isChecked() ) Settings.setValue( "Info/Fullscreen", "yes" );
	else Settings.setValue( "Info/Fullscreen", "no" );
	
	if( ui.CH_Win2K_Hack->isChecked() ) Settings.setValue( "Info/Win2K_Hack", "yes" );
	else Settings.setValue( "Info/Win2K_Hack", "no" );
	
	if( ui.CH_ACPI->isChecked() ) Settings.setValue( "Info/ACPI", "yes" );
	else Settings.setValue( "Info/ACPI", "no" );
	
	if( ui.CH_Localtime->isChecked() ) Settings.setValue( "Info/Localtime", "yes" );
	else Settings.setValue( "Info/Localtime", "no" );
	
	if( ui.CH_Check_Boot_on_FDD->isChecked() ) Settings.setValue( "Info/Check_Boot_on_FDD", "yes" );
	else Settings.setValue( "Info/Check_Boot_on_FDD", "no" );
	
	if( ui.CH_Start_CPU->isChecked() ) Settings.setValue( "Info/Start_CPU", "yes" );
	else Settings.setValue( "Info/Start_CPU", "no" );
	
	if( ui.CH_Snapshot->isChecked() ) Settings.setValue( "Info/Snapshot", "yes" );
	else Settings.setValue( "Info/Snapshot", "no" );
	
	if( ui.CH_QEMU_Log->isChecked() ) Settings.setValue( "Info/QEMU_Log", "yes" );
	else Settings.setValue( "Info/QEMU_Log", "no" );
	
	if( ui.CH_No_Reboot->isChecked() ) Settings.setValue( "Info/No_Reboot", "yes" );
	else Settings.setValue( "Info/No_Reboot", "no" );
	
	if( ui.CH_Network_Cards->isChecked() ) Settings.setValue( "Info/Network_Cards", "yes" );
	else Settings.setValue( "Info/Network_Cards", "no" );
	
	if( ui.CH_Redirections->isChecked() ) Settings.setValue( "Info/Redirections", "yes" );
	else Settings.setValue( "Info/Redirections", "no" );
	
	if( ui.CH_TFTP_Server_Prefix->isChecked() ) Settings.setValue( "Info/TFTP_Server_Prefix", "yes" );
	else Settings.setValue( "Info/TFTP_Server_Prefix", "no" );
	
	if( ui.CH_SMB_Dir->isChecked() ) Settings.setValue( "Info/SMB_Dir", "yes" );
	else Settings.setValue( "Info/SMB_Dir", "no" );
	
	if( ui.CH_Serial_Port->isChecked() ) Settings.setValue( "Info/Serial_Port", "yes" );
	else Settings.setValue( "Info/Serial_Port", "no" );
	
	if( ui.CH_Parallel_Port->isChecked() ) Settings.setValue( "Info/Parallel_Port", "yes" );
	else Settings.setValue( "Info/Parallel_Port", "no" );
	
	if( ui.CH_USB_Port->isChecked() ) Settings.setValue( "Info/USB_Port", "yes" );
	else Settings.setValue( "Info/USB_Port", "no" );
	
	if( ui.CH_ROM_File->isChecked() ) Settings.setValue( "Info/ROM_File", "yes" );
	else Settings.setValue( "Info/ROM_File", "no" );
	
	if( ui.CH_MTDBlock->isChecked() ) Settings.setValue( "Info/MTDBlock", "yes" );
	else Settings.setValue( "Info/MTDBlock", "no" );
	
	if( ui.CH_SD_Image->isChecked() ) Settings.setValue( "Info/SD_Image", "yes" );
	else Settings.setValue( "Info/SD_Image", "no" );
	
	if( ui.CH_PFlash->isChecked() ) Settings.setValue( "Info/PFlash", "yes" );
	else Settings.setValue( "Info/PFlash", "no" );
	
	if( ui.CH_GDB_Port->isChecked() ) Settings.setValue( "Info/GDB_Port", "yes" );
	else Settings.setValue( "Info/GDB_Port", "no" );
	
	if( ui.CH_No_Frame->isChecked() ) Settings.setValue( "Info/No_Frame", "yes" );
	else Settings.setValue( "Info/No_Frame", "no" );
	
	if( ui.CH_Alt_Grab->isChecked() ) Settings.setValue( "Info/Alt_Grab", "yes" );
	else Settings.setValue( "Info/Alt_Grab", "no" );
	
	if( ui.CH_No_Quit->isChecked() ) Settings.setValue( "Info/No_Quit", "yes" );
	else Settings.setValue( "Info/No_Quit", "no" );
	
	if( ui.CH_Portrait->isChecked() ) Settings.setValue( "Info/Portrait", "yes" );
	else Settings.setValue( "Info/Portrait", "no" );
	
	if( ui.CH_Linux_Boot->isChecked() ) Settings.setValue( "Info/Linux_Boot", "yes" );
	else Settings.setValue( "Info/Linux_Boot", "no" );
	
	if( ui.CH_Show_FDD->isChecked() ) Settings.setValue( "Info/Show_FDD", "yes" );
	else Settings.setValue( "Info/Show_FDD", "no" );
	
	if( ui.CH_Show_CD->isChecked() ) Settings.setValue( "Info/Show_CD", "yes" );
	else Settings.setValue( "Info/Show_CD", "no" );
	
	if( ui.CH_Show_HDD->isChecked() ) Settings.setValue( "Info/Show_HDD", "yes" );
	else Settings.setValue( "Info/Show_HDD", "no" );
	
	// MAC Address Generation Mode
	if( ui.RB_MAC_Random->isChecked() ) Settings.setValue( "MAC_Generation_Mode", "Random" );
	else if( ui.RB_MAC_QEMU->isChecked() ) Settings.setValue( "MAC_Generation_Mode", "QEMU_Segment" );
	else if( ui.RB_MAC_Valid->isChecked() ) Settings.setValue( "MAC_Generation_Mode", "Model" );
	
	// Save to Log File
	if( ui.CH_Log_Save_in_File->isChecked() ) Settings.setValue( "Log/Save_In_File", "yes" );
	else Settings.setValue( "Log/Save_In_File", "no" );
	
	// Print In StdOut
	if( ui.CH_Log_Print_in_STDIO->isChecked() ) Settings.setValue( "Log/Print_In_STDOUT", "yes" );
	else Settings.setValue( "Log/Print_In_STDOUT", "no" );
	
	// Log File Path
	Settings.setValue( "Log/Log_Path", ui.Edit_Log_Path->text() );
	
	// Save to AQEMU Log
	if( ui.CH_Log_Debug->isChecked() ) Settings.setValue( "Log/Save_Debug", "yes" );
	else Settings.setValue( "Log/Save_Debug", "no" );
	
	if( ui.CH_Log_Warning->isChecked() ) Settings.setValue( "Log/Save_Warning", "yes" );
	else Settings.setValue( "Log/Save_Warning", "no" );
	
	if( ui.CH_Log_Error->isChecked() ) Settings.setValue( "Log/Save_Error", "yes" );
	else Settings.setValue( "Log/Save_Error", "no" );
	
	// QEMU-IMG Path
	Settings.setValue( "QEMU-IMG_Path", ui.Edit_QEMU_IMG_Path->text() );
	
	// QEMU_AUDIO
	if( ui.CH_Audio_Default->isChecked() )
		Settings.setValue( "QEMU_AUDIO/Use_Default_Driver", "no" );
	else
		Settings.setValue( "QEMU_AUDIO/Use_Default_Driver", "yes" );
	
	// QEMU_AUDIO_DRV
	Settings.setValue( "QEMU_AUDIO/QEMU_AUDIO_DRV", ui.CB_Host_Sound_System->currentText() );
	
	// Recent CD Count
	Settings.setValue( "CD_ROM_Exits_Images/Max", QString::number(ui.SB_Recent_CD_Count->value()) );
	
	// Recent FDD Count
	Settings.setValue( "Floppy_Exits_Images/Max", QString::number(ui.SB_Recent_FDD_Count->value()) );
	
	// First VNC Port for Embedded Display
	Settings.setValue( "First_VNC_Port", QString::number(ui.SB_First_VNC_Port->value()) );
	
	// All OK?
	if( Settings.status() != QSettings::NoError )
		AQError( "void Advanced_Settings_Window::on_Button_OK_clicked()", "QSettings Error!" );
	
	if( Save_Emulators_Info() ) accept();
}

void Advanced_Settings_Window::on_TB_Browse_Before_clicked()
{
	QFileDialog::Options options;
	QString selectedFilter;
	
	QString fileName = QFileDialog::getOpenFileName( this, tr("Select executable"),
													 Get_Last_Dir_Path(ui.Edit_Before_Start_Command->text()),
													 tr("All Files (*);;Scripts (*.sh)"), &selectedFilter, options );
	
	if( ! fileName.isEmpty() )
		ui.Edit_Before_Start_Command->setText( fileName );
}

void Advanced_Settings_Window::on_TB_Browse_After_clicked()
{
	QFileDialog::Options options;
	QString selectedFilter;
	
	QString fileName = QFileDialog::getOpenFileName( this, tr("Select executable"),
													 Get_Last_Dir_Path(ui.Edit_After_Stop_Command->text()),
													 tr("All Files (*);;Scripts (*.sh)"), &selectedFilter, options );
	
	if( ! fileName.isEmpty() )
		ui.Edit_After_Stop_Command->setText( fileName );
}

void Advanced_Settings_Window::on_TB_Log_File_clicked()
{
	QFileDialog::Options options;
	QString selectedFilter;
	
	QString fileName = QFileDialog::getSaveFileName( this, tr("Select or Create Log File"),
													 Get_Last_Dir_Path(ui.Edit_Log_Path->text()),
													 tr("All Files (*)"), &selectedFilter, options );
	
	if( ! fileName.isEmpty() )
		ui.Edit_Log_Path->setText( fileName );
}

void Advanced_Settings_Window::on_TB_QEMU_IMG_Browse_clicked()
{
	QFileDialog::Options options;
	QString selectedFilter;
	
	QString fileName = QFileDialog::getOpenFileName( this, tr("Select executable"),
													 Get_Last_Dir_Path(ui.Edit_After_Stop_Command->text()),
													 tr("All Files (*)"), &selectedFilter, options );
	
	if( ! fileName.isEmpty() )
		ui.Edit_QEMU_IMG_Path->setText( fileName );
}

void Advanced_Settings_Window::on_TB_Add_Emulator_clicked()
{
	Emulator_Options_Window *emulatorOptionsWin = new Emulator_Options_Window( this );
	emulatorOptionsWin->Set_All_Emulators_Names( Get_All_Emulators_Names() );
	emulatorOptionsWin->Set_Emulator( Emulator() );
	
	if( emulatorOptionsWin->exec() == QDialog::Accepted )
	{
		Emulator new_emul = emulatorOptionsWin->Get_Emulator();
		
		// This Default Emulator?
		bool finded = false;
		for( int ix = 0; ix < Emulators.count(); ix++ )
		{
			if( Emulators[ix].Get_Type() == new_emul.Get_Type() &&
				Emulators[ix].Get_Default() )
			{
				finded = true;
			}
		}
		
		if( finded == false ) new_emul.Set_Default( true );
		
		Emulators << new_emul;
		Update_Emulators_Info();
	}
	
	delete emulatorOptionsWin;
}

void Advanced_Settings_Window::on_TB_Delete_Emulator_clicked()
{
	int cur_index = ui.Emulators_Table->currentRow();
	
	if( cur_index >= 0 && cur_index < Emulators.count() )
	{
		Emulators.removeAt( cur_index );
		ui.Emulators_Table->removeRow( cur_index );
		
		// Set new default emulator?
		bool q, k, qd, kd; // q - qemu, k - kvm, d - default
		q = k = qd = kd = false;
		
		for( int ex = 0; ex < Emulators.count(); ++ex )
		{
			if( Emulators[ex].Get_Type() == VM::QEMU )
			{
				q = true;
				if( Emulators[ex].Get_Default() ) qd = true;
			}
			else if( Emulators[ex].Get_Type() == VM::KVM )
			{
				k = true;
				if( Emulators[ex].Get_Default() ) kd = true;
			}
		}
		
		if( q && qd == false )
		{
			for( int ex = 0; ex < Emulators.count(); ++ex )
			{
				if( Emulators[ex].Get_Type() == VM::QEMU )
				{
					Emulators[ ex ].Set_Default( true );
					ui.Emulators_Table->item( ex, 3 )->setText( tr("Yes") );
					break;
				}
			}
		}
		
		if( k && kd == false )
		{
			for( int ex = 0; ex < Emulators.count(); ++ex )
			{
				if( Emulators[ex].Get_Type() == VM::KVM )
				{
					Emulators[ ex ].Set_Default( true );
					ui.Emulators_Table->item( ex, 3 )->setText( tr("Yes") );
					break;
				}
			}
		}
	}
}

void Advanced_Settings_Window::on_TB_Edit_Emulator_clicked()
{
	int cur_index = ui.Emulators_Table->currentRow();
	
	if( cur_index >= 0 && cur_index < Emulators.count() )
	{
		Emulator_Options_Window *emulatorOptionsWin = new Emulator_Options_Window( this );
		emulatorOptionsWin->Set_All_Emulators_Names( Get_All_Emulators_Names() );
		emulatorOptionsWin->Set_Emulator( Emulators[cur_index] );
		
		if( emulatorOptionsWin->exec() == QDialog::Accepted )
		{
			Emulators[ cur_index ] = emulatorOptionsWin->Get_Emulator();
			Update_Emulators_Info();
		}
		
		delete emulatorOptionsWin;
	}
}

void Advanced_Settings_Window::on_TB_Use_Default_clicked()
{
	int cur_index = ui.Emulators_Table->currentRow();
	
	if( cur_index >= 0 && cur_index < Emulators.count() )
	{
		Emulators[ cur_index ].Set_Default( true );
		
		for( int ix = 0; ix < Emulators.count(); ix++ )
		{
			if( ix != cur_index &&
				Emulators[cur_index].Get_Type() == Emulators[ix].Get_Type() )
			{
				Emulators[ ix ].Set_Default( false );
			}
		}
		
		Update_Emulators_Info();
	}
}

void Advanced_Settings_Window::on_TB_Find_All_Emulators_clicked()
{
	First_Start_Wizard *first_start_win = new First_Start_Wizard( NULL );
	
	if( first_start_win->Find_Emulators() )
	{
		AQDebug( "void Advanced_Settings_Window::on_TB_Find_All_Emulators_clicked()",
				 "Find Emulators and Save Settings Complete" );
		
		// Update Emulators List
		if( Load_Emulators_Info() ) Update_Emulators_Info();
		
		// QEMU-IMG Path
		ui.Edit_QEMU_IMG_Path->setText( Settings.value("QEMU-IMG_Path", "qemu-img").toString() );
	}
	else
	{
		AQGraphic_Error( "void Advanced_Settings_Window::on_TB_Find_All_Emulators_clicked()", QObject::tr("Error!"),
						 QObject::tr("Cannot Find Emulators in This System! Please Add Emulators Manual!"), false );
	}
	
	delete first_start_win;
}

void Advanced_Settings_Window::on_Emulators_Table_cellDoubleClicked( int row, int column )
{
	on_TB_Edit_Emulator_clicked();
}

void Advanced_Settings_Window::on_Button_CDROM_Add_clicked()
{
	bool ok = false;
	QString text = QInputDialog::getText( this, tr("Add CD/DVD Device"), tr("Enter Device Name. Sample: /dev/cdrom"),
										  QLineEdit::Normal, "", &ok );
	
	if( ok && ! text.isEmpty() ) ui.CDROM_List->addItem( text );
}

void Advanced_Settings_Window::on_Button_CDROM_Edit_clicked()
{
	if( ui.CDROM_List->currentRow() >= 0 )
	{
		bool ok = false;
		QString text = QInputDialog::getText( this, tr("Add CD/DVD Device"), tr("Enter Device Name. Sample: /dev/cdrom"),
											  QLineEdit::Normal, ui.CDROM_List->currentItem()->text(), &ok );
	
		if( ok && ! text.isEmpty() ) ui.CDROM_List->currentItem()->setText( text );
	}
}

void Advanced_Settings_Window::on_Button_CDROM_Delete_clicked()
{
	if( ui.CDROM_List->currentRow() >= 0 )
		ui.CDROM_List->takeItem( ui.CDROM_List->currentRow() );
}

bool Advanced_Settings_Window::Load_Emulators_Info()
{
	if( Emulators.count() > 0 ) Emulators.clear();

	Emulators = Get_Emulators_List();
	if( Emulators.count() <= 0 ) return false;
	else return true;
}

bool Advanced_Settings_Window::Save_Emulators_Info()
{
	// FIXME save only if emulators changed
	
	// Check defaults emulators
	bool installed_kvm, installed_qemu, default_kvm, default_qemu;
	installed_kvm = installed_qemu = default_kvm = default_qemu = false;
	
	for( int ix = 0; ix < Emulators.count(); ++ix )
	{
		if( Emulators[ix].Get_Type() == VM::QEMU )
		{
			installed_qemu = true;

			if( Emulators[ix].Get_Default() ) default_qemu = true;
		}
		else if( Emulators[ix].Get_Type() == VM::KVM )
		{
			installed_kvm = true;
			
			if( Emulators[ix].Get_Default() ) default_kvm = true;
		}
	}
	
	if( installed_qemu && default_qemu == false )
	{
		AQGraphic_Warning( tr("Error!"), tr("Not Select Default QEMU Emulator!") );
		return false;
	}
	
	if( installed_kvm && default_kvm == false )
	{
		AQGraphic_Warning( tr("Error!"), tr("Not Select Default KVM Emulator!") );
		return false;
	}
	
	// Remove old emulators files
	if( ! Remove_All_Emulators_Files() )
	{
		AQWarning( "bool Advanced_Settings_Window::Save_Emulators_Info()",
				   "Not all old emulators files removed!" );
	}
	
	// Save new files
	for( int ix = 0; ix < Emulators.count(); ++ix )
	{
		if( ! Emulators[ ix ].Save() )
		{
			AQGraphic_Warning( tr("Error!"),
							   tr("Cannot save emulator \"%1\"!").arg(Emulators[ix].Get_Name()) );
		}
	}
	
	return true;
}

void Advanced_Settings_Window::Update_Emulators_Info()
{
	ui.Emulators_Table->clearContents();
	while( ui.Emulators_Table->rowCount() > 0 ) ui.Emulators_Table->removeRow( 0 );
	
	for( int ix = 0; ix < Emulators.count(); ++ix )
	{
		ui.Emulators_Table->insertRow( ui.Emulators_Table->rowCount() );
		
		QTableWidgetItem *newItem = new QTableWidgetItem( Emulators[ix].Get_Name() );
		ui.Emulators_Table->setItem( ui.Emulators_Table->rowCount()-1, 0, newItem );
		
		newItem = new QTableWidgetItem( Emulator_Version_To_String(Emulators[ix].Get_Version()) ); // FIXME version,check,force
		ui.Emulators_Table->setItem( ui.Emulators_Table->rowCount()-1, 1, newItem );
		
		newItem = new QTableWidgetItem( Emulators[ix].Get_Path() );
		ui.Emulators_Table->setItem( ui.Emulators_Table->rowCount()-1, 2, newItem );
		
		newItem = new QTableWidgetItem( Emulators[ix].Get_Default() ? tr("Yes") : tr("No") );
		ui.Emulators_Table->setItem( ui.Emulators_Table->rowCount()-1, 3, newItem );
	}
}

void Advanced_Settings_Window::on_TB_Screenshot_Folder_clicked()
{
	QString folder = QFileDialog::getExistingDirectory( this, tr("Select Folder for Screenshots"),
														Settings.value("Screenshot_Folder_Path", "~").toString() );
	
	if( ! folder.isEmpty() )
		ui.Edit_Screenshot_Folder->setText( folder );
}

QStringList Advanced_Settings_Window::Get_All_Emulators_Names() const
{
	QStringList list;
	for( int ix = 0; ix < Emulators.count(); ix++ ) list << Emulators[ ix ].Get_Name();
	return list;
}
