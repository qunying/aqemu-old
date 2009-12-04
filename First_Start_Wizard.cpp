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

#include <QDir>
#include <QProcess>
#include <QMessageBox>
#include <QTranslator>
#include <QFileDialog>

#include "First_Start_Wizard.h"
#include "Utils.h"
#include "System_Info.h"
#include "Settings_Window.h"
#include "Advanced_Settings_Window.h"
#include "Edit_Emulator_Version_Window.h"

First_Start_Wizard::First_Start_Wizard( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
	
	Emulators_Find_Done = false;
	Next_Move = true;
	
	retranslateUi();
	Load_Settings();
}

bool First_Start_Wizard::Find_Emulators()
{
	on_Button_Find_Emulators_clicked();
	return Save_Settings();
}

void First_Start_Wizard::on_Button_Cancel_clicked()
{
	if( QMessageBox::information(this, tr("Warning!"), tr("You sure? You Can Configure These Options In The Settings Window."),
		QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes )
	{
		reject();
	}
}

void First_Start_Wizard::on_Button_Back_clicked()
{
	Next_Move = false;
	ui.All_Pages->setCurrentIndex( ui.All_Pages->currentIndex() -1 );
}

void First_Start_Wizard::on_Button_Next_clicked()
{
	if( ui.All_Pages->currentWidget() == ui.Welcome_Page )
	{
		retranslateUi();
	}
	else if( ui.All_Pages->currentIndex() == ui.All_Pages->count()-1 )
	{
		if( Save_Settings() )
		{
			Settings_Window *sw = new Settings_Window();
			sw->on_Button_OK_clicked();
			delete sw;
			
			Advanced_Settings_Window *asw = new Advanced_Settings_Window();
			asw->on_Button_OK_clicked();
			delete asw;
			
			accept();
		}
	}
	
	Next_Move = true;
	ui.All_Pages->setCurrentIndex( ui.All_Pages->currentIndex() +1 );
	
	if( ui.All_Pages->currentWidget() == ui.Find_Emulators_Page )
	{
		if( ! Emulators_Find_Done ) ui.Button_Next->setEnabled( false );
	}
}

void First_Start_Wizard::on_TB_Browse_VM_Dir_clicked()
{
	QString folder = QFileDialog::getExistingDirectory( this, tr("Set Folder for you VM's"),
														Settings.value("VM_Directory", "~").toString() );
	
	if( ! (folder.isNull() || folder.isEmpty()) )
	{
		if( ! folder.endsWith("/") || folder.endsWith("\\") )
		{
			#ifdef Q_OS_WIN32
			folder += "\\";
			#else
			folder += "/";
			#endif
		}
		
		ui.Edit_VM_Dir->setText( folder );
	}
}

void First_Start_Wizard::on_Button_Find_Emulators_clicked()
{
	Emulators_Find_Done = true;
	ui.Button_Next->setEnabled( true );
	
	ui.Edit_Enulators_List->clear();
	
	for( int rx = 0; rx < Settings.value( "Emulators_Count", "0" ).toString().toInt(); rx++ )
	{
		Settings.remove( "Emulator_" + QString::number(rx) );
	}
	
	Settings.setValue( "Emulators_Count", "0" );
	
	QStringList sys_env = QProcess::systemEnvironment();
	
	if( sys_env.count() <= 0 )
	{
		AQError( "void First_Start_Wizard::on_Button_Find_clicked()",
				 "Cannot Get System Environment Information!" );
		return;
	}
	else
	{
		QStringList paths;
		
		for( int ix = 0; ix < sys_env.count(); ix++ )
		{
			if( sys_env[ix].startsWith("PATH=") )
			{
				QString tmp = sys_env[ ix ].remove( "PATH=" );
				paths = tmp.split( ":", QString::SkipEmptyParts );
				break;
			}
		}
		
		// Delete /usr/bin/X11/
		for( int ix = 0; ix < paths.count(); ix++ )
		{
			if( paths[ix].indexOf("/usr/bin/X11") >= 0 )
			{
				paths.removeAt( ix );
			}
		}
		
		if( paths.count() <= 0 )
		{
			AQError( "void First_Start_Wizard::on_Button_Find_clicked()",
					 "Cannot Find PATH in System Environment Information!" );
			return;
		}
		else
		{
			int qemu_emulators_count = 0;
			
			// Find QEMU
			for( int qx = 0; qx < paths.count(); ++qx )
			{
				QStringList qemu_list = System_Info::Find_QEMU_Binary_Files( paths[qx] );
				
				if( qemu_list.count() < 1 || qemu_list[0].isEmpty() )
				{
					AQDebug( "void First_Start_Wizard::on_Button_Find_clicked()",
							 "In " + paths[qx] + " QEMU Not Found" );
				}
				else
				{
					AQDebug( "void First_Start_Wizard::on_Button_Find_clicked()",
							 "QEMU Finded. Path: " + paths[qx] );
					
					QString path = "";
					
					if( paths[qx].endsWith("/") || paths[qx].endsWith("\\") )
					{
						path = paths[qx];
					}
					else
					{
					#ifdef Q_OS_WIN32
						path = paths[qx] + "\\";
					#else
						path = paths[qx] + "/";
					#endif
					}
					
					// Check Version
					QString qemu_version = System_Info::Get_QEMU_Version( path + qemu_list[0] );
					
					if( qemu_version.isEmpty() )
					{
						AQError( "void First_Start_Wizard::on_Button_Find_clicked()",
								 "Cannot Get QEMU Version! Using Default: 0.9.0" );
						
						qemu_version = "0.9.0";
					}
					
					// Add Text
					ui.Edit_Enulators_List->appendPlainText( tr("Finded QEMU in %1, version: %2").arg(paths[qx]).arg(qemu_version) );
					
					// Create New Emulator
					Settings.setValue( "Emulators_Count", Settings.value("Emulators_Count", "0").toString().toInt() +1 );
					int emul_count = Settings.value( "Emulators_Count", "0" ).toString().toInt();
					
					// Load Emulator Data
					QString prefix = "Emulator_" + QString::number( emul_count-1 ) + "/";
					
					// Emulator Type
					Settings.setValue( prefix + "Type", "QEMU" );
					
					// Name
					Settings.setValue( prefix + "Name", "QEMU " + qemu_version );
					
					// Default
					if( qemu_emulators_count == 0 ) Settings.setValue( prefix + "Default", "yes" );
					else Settings.setValue( prefix + "Default", "no" );
					
					// Path
					Settings.setValue( prefix + "Path", paths[qx] );
					
					// Check Version on Start AQEMU
					Settings.setValue( prefix + "Check_QEMU_Version", "no" );
					
					// Force QEMU Version
					Settings.setValue( prefix + "QEMU_Version", qemu_version );
					
					// Check Available Audio Cards
					Settings.setValue( prefix + "Check_Available_Audio_Cards", "no" );
					
					// Bin files
					QStringList QEMU_Binary_Names;
					QEMU_Binary_Names << "qemu";
					QEMU_Binary_Names << "qemu-img";
					QEMU_Binary_Names << "qemu-system-arm";
					QEMU_Binary_Names << "qemu-system-cris";
					QEMU_Binary_Names << "qemu-system-m68k";
					QEMU_Binary_Names << "qemu-system-mips";
					QEMU_Binary_Names << "qemu-system-mipsel";
					QEMU_Binary_Names << "qemu-system-mips64";
					QEMU_Binary_Names << "qemu-system-mips64el";
					QEMU_Binary_Names << "qemu-system-ppc";
					QEMU_Binary_Names << "qemu-system-ppc64";
					QEMU_Binary_Names << "qemu-system-ppcemb";
					QEMU_Binary_Names << "qemu-system-sh4";
					QEMU_Binary_Names << "qemu-system-sh4eb";
					QEMU_Binary_Names << "qemu-system-sparc";
					QEMU_Binary_Names << "qemu-system-x86_64";
					
					for( int ix = 0; ix < QEMU_Binary_Names.count(); ++ix )
					{
						Settings.setValue( prefix + QEMU_Binary_Names[ix], path + qemu_list[ix] );
					}
					
					qemu_emulators_count++;
				}
			}
			
			int kvm_emulators_count = 0;
			
			// Find KVM
			for( int kx = 0; kx < paths.count(); ++kx )
			{
				QStringList kvm_list = System_Info::Find_KVM_Binary_Files( paths[kx] );
				
				if( kvm_list.count() < 1 || kvm_list[0].isEmpty() )
				{
					AQDebug( "void First_Start_Wizard::on_Button_Find_clicked()",
							 "In " + paths[kx] + " KVM Not Found" );
				}
				else
				{
					AQDebug( "void First_Start_Wizard::on_Button_Find_clicked()",
							 "KVM Finded. Path: " + paths[kx] );
					
					QString path = "";
					
					if( paths[kx].endsWith("/") || paths[kx].endsWith("\\") )
					{
						path = paths[kx];
					}
					else
					{
					#ifdef Q_OS_WIN32
						path = paths[kx] + "\\";
					#else
						path = paths[kx] + "/";
					#endif
					}
					
					// Check Version
					QString kvm_version = System_Info::Get_KVM_Version( path + kvm_list[0] );
					
					if( kvm_version.isEmpty() )
					{
						AQError( "void First_Start_Wizard::on_Button_Find_clicked()",
								 "Cannot Get KVM Version! Using Default: 7X" );
						
						kvm_version = "7X";
					}
					
					// Add Text
					ui.Edit_Enulators_List->appendPlainText( tr("Finded KVM in %1, version: %2").arg(paths[kx]).arg(kvm_version) );
					
					// Create New Emulator
					Settings.setValue( "Emulators_Count", Settings.value("Emulators_Count", "0").toString().toInt() +1 );
					int emul_count = Settings.value( "Emulators_Count", "0" ).toString().toInt();
					
					// Load Emulator Data
					QString prefix = "Emulator_" + QString::number( emul_count-1 ) + "/";
					
					// Emulator Type
					Settings.setValue( prefix + "Type", "KVM" );
					
					// Name
					Settings.setValue( prefix + "Name", "KVM " + kvm_version );
					
					// Default
					if( kvm_emulators_count == 0 ) Settings.setValue( prefix + "Default", "yes" );
					else Settings.setValue( prefix + "Default", "no" );
					
					// Path
					Settings.setValue( prefix + "Path", paths[kx] );
					
					// Check Version on Start AQEMU
					Settings.setValue( prefix + "Check_KVM_Version", "no" );
					
					// Force KVM Version
					Settings.setValue( prefix + "KVM_Version", kvm_version );
					
					// Check Available Audio Cards
					Settings.setValue( prefix + "Check_Available_Audio_Cards", "no" );
					
					// Bin files
					QStringList KVM_Binary_Names;
					KVM_Binary_Names << "kvm";
					KVM_Binary_Names << "kvm-img";
					
					for( int ix = 0; ix < KVM_Binary_Names.count(); ++ix )
					{
						Settings.setValue( prefix + KVM_Binary_Names[ix], path + kvm_list[ix] );
					}
					
					kvm_emulators_count++;
				}
			}
			
			// Enable Edit Emulator Version Manualy Button
			if( qemu_emulators_count > 0 || kvm_emulators_count > 0 )
			{
				ui.Button_Edit->setEnabled( true );
			}
		}
	}
}

void First_Start_Wizard::on_Button_Skip_Find_clicked()
{
	ui.Button_Next->setEnabled( true );
}

void First_Start_Wizard::on_Button_Edit_clicked()
{
	Edit_Emulator_Version_Window *edit_win = new Edit_Emulator_Version_Window();
	
	if( edit_win->exec() == QDialog::Accepted )
	{
		// Update Emulators Info Text
		ui.Edit_Enulators_List->setPlainText( tr("Emulator Version Modified By User") );
	}
	
	delete edit_win;
}

void First_Start_Wizard::on_All_Pages_currentChanged( int index )
{
	// Back, Next Buttons State
	if( index == 0 )
	{
		ui.Button_Back->setEnabled( false );
	}
	else if( index == ui.All_Pages->count()-1 )
	{
		ui.Button_Next->setText( tr("&Finish") );
	}
	else
	{
		ui.Button_Back->setEnabled( true );
		ui.Button_Next->setText( tr("&Next") );
	}
	
	// Set Header Text
	ui.Label_Caption->setText( Header_Captions[ui.All_Pages->currentIndex()] );
	
	// Pages Actions
	if( ui.All_Pages->currentWidget() == ui.General_Settings_Page && Next_Move )
	{
		retranslateUi();
	}
}

void First_Start_Wizard::Load_Settings()
{
	// Find All Language Files (*.qm)
	QDir data_dir( Settings.value("AQEMU_Data_Folder", "/usr/share/aqemu/").toString() );
	QFileInfoList lang_files = data_dir.entryInfoList( QStringList("*.qm"), QDir::Files, QDir::Name );
	
	if( lang_files.count() > 0 )
	{
		// Add Languages to List
		for( int dd = 0; dd < lang_files.count(); ++dd )
		{
			ui.CB_Language->addItem( lang_files[dd].completeBaseName() );
			
			if( lang_files[dd].completeBaseName() == Settings.value( "Language", "en" ).toString() )
			{
				ui.CB_Language->setCurrentIndex( dd + 1 ); // First Item 'English'
			}
		}
	}
	
	// Virtual Machines Folder
	ui.Edit_VM_Dir->setText( Settings.value("VM_Directory", QDir::homePath() + "/.aqemu/").toString() );
	
	// Use Device Manager
	ui.CH_Device_Manager->setChecked( Settings.value("Use_Device_Manager", "no").toString() == "yes" );
	
	// Use VNC Embedded Display
	#ifdef VNC_DISPLAY
	ui.CH_Embedded_VNC->setEnabled( true );
	ui.CH_Embedded_VNC->setChecked( Settings.value("Use_VNC_Display", "no").toString() == "yes" );
	#endif
}

bool First_Start_Wizard::Save_Settings()
{
	// Virtual Machines Folder
	QDir dir;
	
	if( ! ui.Edit_VM_Dir->text().endsWith("/") ) ui.Edit_VM_Dir->setText( ui.Edit_VM_Dir->text() + "/" );
	
	if( dir.exists(ui.Edit_VM_Dir->text()) )
	{
		if( ! dir.exists(ui.Edit_VM_Dir->text() + "os_templates/") )
		{
			dir.mkdir( ui.Edit_VM_Dir->text() + "os_templates/" );
		}
	}
	else if( ! (dir.mkdir(ui.Edit_VM_Dir->text()) && dir.mkdir(ui.Edit_VM_Dir->text() + "os_templates/")) )
	{
		AQGraphic_Warning( tr("Error!"), tr("Cannot Create New Folder!") );
		return false;
	}
	
	Settings.setValue( "VM_Directory", ui.Edit_VM_Dir->text() );
	
	// Use Device Manager
	if( ui.CH_Device_Manager->isChecked() ) Settings.setValue( "Use_Device_Manager", "yes" );
	else Settings.setValue( "Use_Device_Manager", "no" );
	
	// Use VNC Embedded Display
	#ifdef VNC_DISPLAY
	if( ui.CH_Embedded_VNC->isChecked() ) Settings.setValue( "Use_VNC_Display", "yes" );
	else Settings.setValue( "Use_VNC_Display", "no" );
	#endif
	
	// Interface Language
	if( ui.CB_Language->currentIndex() == 0 ) Settings.setValue( "Language", "en" );
	else Settings.setValue( "Language", ui.CB_Language->itemText(ui.CB_Language->currentIndex()) );
	
	// Off First Start
	Settings.setValue( "First_Start", "no" );
	
	return true;
}

void First_Start_Wizard::retranslateUi()
{
	// Load Language
	QTranslator appTranslator;
	
	if( ui.CB_Language->currentIndex() != 0 )
	{
		if( appTranslator.load(Settings.value("AQEMU_Data_Folder", "").toString() + ui.CB_Language->itemText(ui.CB_Language->currentIndex()) + ".qm") )
		{
			QCoreApplication::installTranslator( &appTranslator );
			
			AQDebug( "void First_Start_Wizard::retranslateUi()",
					 "Translation File\"" + Settings.value("AQEMU_Data_Folder", "").toString() +
					 ui.CB_Language->itemText(ui.CB_Language->currentIndex()) + ".qm" + "\" Loaded" );
		}
		else AQError( "void First_Start_Wizard::retranslateUi()", "Cannot Load Translations!" );
	}
	
	Header_Captions.clear();
	Header_Captions << tr( "Welcome" );
	Header_Captions << tr( "General Settings" );
	Header_Captions << tr( "Find Emulators" );
	Header_Captions << tr( "Finish" );
	
	ui.Label_Caption->setText( Header_Captions[0] );
	
	ui.Button_Back->setText( tr("&Back") );
	ui.Button_Next->setText( tr("&Next") );
	ui.Button_Cancel->setText( tr("&Cancel") );
	
	ui.Label_Welcome_Text->setText( tr("Welcome to AQEMU settings wizard!\nThis wizard will help you to choose options that need to AQEMU work correctly.\nPush the \"Next\" button to go to next page or \"Back\" button to go back to previous page.") );
	ui.Label_Select_Language->setText( tr("Here you can choose interface language") );
	ui.Label_VM_Dir->setText( tr("Please select folder for AQEMU virtual machines") );
	ui.Label_Device_Manager->setText( tr("If the \"Device manager\" mode is active, storage devices (Floppy, CD/DVD, HDD) will displayed in virtual machine as icons, like in file manager window.") );
	ui.CH_Device_Manager->setText( tr("Use Device manager") );
	ui.Label_Embedded_VNC->setText( tr("New experimental work mode is accessible in AQEMU. In this mode QEMU/KVM display will displayed in AQEMU main window. Note that this mode is still unstable so here may be some problems. If you want to enable this function you must mark the \"Include Emdedded VNC Display in Main Window\" flag.") );
	ui.CH_Embedded_VNC->setText( tr("Include Emdedded VNC Display in Main Window") );
	ui.Label_Find_Emulators->setText( tr("To work correctly, AQEMU must know your emulators' arrangement and its' versions. For searching push the \"Search\" button. If automatic search can't find out your emulators, you must configure AQEMU yourself later. You can do it in \"File->Advanced Settings\" menu.") );
	ui.Button_Find_Emulators->setText( tr("&Search") );
	ui.Button_Skip_Find->setText( tr("S&kip Search") );
	ui.Button_Edit->setText( tr("Set &Versions Manualy") );
	ui.Label_Finish_Text->setText( tr("Congratulations! The Wizard has got all necessary settings for configuring AQEMU. Push \"Finish\" button to save your settings.") );
}
