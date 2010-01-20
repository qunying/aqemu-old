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

#include <QApplication>
#include <QResource>
#include <QMessageBox>
#include <QTranslator>
#include <QFileDialog>
#include <QDir>
#include <QFile>

// For Check User UID
#ifdef Q_OS_LINUX
#include <unistd.h>
#include <sys/types.h>
#endif

#include "Utils.h"
#include "Main_Window.h"
#include "Settings_Window.h"
#include "First_Start_Wizard.h"

int main( int argc, char *argv[] )
{
	// Set QSettings Data
	QCoreApplication::setOrganizationName( "ANDronSoft" );
	QCoreApplication::setApplicationName( "AQEMU" );
	QSettings settings;
	
	// Create QApplication
	QApplication app( argc, argv );
	
	Set_Show_Error_Window( true );
	
	// Log Filter
	AQUse_Debug_Output( settings.value("Log/Print_In_STDOUT", "yes").toString() == "yes",
						settings.value("Log/Save_Debug", "yes").toString() == "yes",
						settings.value("Log/Save_Warning", "yes").toString() == "yes",
						settings.value("Log/Save_Error", "yes").toString() == "yes" );
	
	// Check For First Start in root Mode
	#ifdef Q_OS_LINUX
	if( settings.value("First_Start", "yes").toString() == "yes" ) // This is a first start AQEMU on this computer?
	{
		uid_t user_uid = getuid();
		
		if( user_uid == 0 )
		{
			int ret = QMessageBox::question( NULL, QObject::tr("Warning!"),
											 QObject::tr("This is a First Start AQEMU and Program Running in root Mode.\n"
														 "In Some Linux Distributions That Can Lead to Inability to Save Configuration!"),
											 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
			
			if( ret == QMessageBox::Yes ) return 0;
		}
	}
	#endif
	
	// This is Upgrade AQEMU? Find Previous Confing...
	if( QFile::exists(QDir::homePath() + "/.config/ANDronSoft/AQEMU.conf") )
	{
		QString conf_ver = settings.value( "AQEMU_Config_Version", "0.7.2" ).toString();
		
		if( conf_ver == "0.5" )
		{
			AQDebug( "int main( int argc, char *argv[] )",
					 "AQEMU Config Version: 0.5\nRun Firt Start Wizard" );
			
			settings.setValue( "First_Start", "yes" );
			settings.setValue( "AQEMU_Config_Version", "0.7.3" );
		}
		else if( conf_ver == "0.7.2" )
		{
			AQDebug( "int main( int argc, char *argv[] )",
					 "AQEMU Config Version: 0.7.2\nIn Version 0.7.3 Emulator Version Check Off In Default" );
			
			First_Start_Wizard *first_start_win = new First_Start_Wizard( NULL );
			
			if( first_start_win->Find_Emulators() )
			{
				AQDebug( "int main( int argc, char *argv[] )", "Find Emulators and Save Settings Complete" );
			}
			else
			{
				AQGraphic_Error( "int main( int argc, char *argv[] )", QObject::tr("Error!"),
								 QObject::tr("Cannot Find Emulators in This System! You Most Set It In Advanced Settings!"), false );
			}
			
			delete first_start_win;
			
			settings.setValue( "AQEMU_Config_Version", "0.7.3" );
		}
		else if( conf_ver == "0.7.3" )
		{
			AQDebug( "int main( int argc, char *argv[] )",  "AQEMU Config Version: 0.7.3" );
		}
		else
		{
			// Remove Old Config!
			if( QFile::copy(QDir::homePath() + "/.config/ANDronSoft/AQEMU.conf",
				QDir::homePath() + "/.config/ANDronSoft/AQEMU.conf.bak") )
			{
				AQWarning( "int main( int argc, char *argv[] )",
						   "AQEMU Configuration File No Version 0.5. File Saved: AQEMU.conf.bak" );
				
				settings.clear();
				settings.sync();
				
				settings.setValue( "AQEMU_Config_Version", "0.7.3" );
			}
			else AQError( "int main( int argc, char *argv[] )", "Cannot Save Old Version AQEMU Configuration File!" );
		}
	}
	else
	{
		// Config File Not Found. This First Install
		settings.setValue( "AQEMU_Config_Version", "0.7.3" );
	}
	
	// Use Log
	if( settings.value( "Log/Save_In_File", "yes" ).toString() == "yes" )
	{
		// Log Size
		if( QFile::exists(settings.value("Log/Log_Path", "").toString()) )
		{
			QFileInfo log_info( settings.value("Log/Log_Path", "").toString() );
			
			// Log > 1MB
			if( log_info.size() > 1 * 1024 * 1024 )
			{
				// FIXME Delete Half Log Size
				QFile::remove( settings.value("Log/Log_Path", "").toString() );
			}
		}
		
		AQUse_Log( true );
	}
	else AQUse_Log( false );
	
	// Log File Name
	AQLog_Path( settings.value("Log/Log_Path", "").toString() );
	
	// Find Data Folder
	if( settings.value("AQEMU_Data_Folder", "").toString().isEmpty() )
	{
		#ifdef Q_OS_WIN32
		if( QDir(QDir::currentPath() + "\\os_icons").exists() &&
			QDir(QDir::currentPath() + "\\os_templates").exists() )
		{
			settings.setValue( "AQEMU_Data_Folder", QDir::currentPath() );
			AQDebug( "int main( int argc, char *argv[] )", "Use Data Folder: " + QDir::currentPath() );
		}
		else
		{
			AQGraphic_Error( "int main( int argc, char *argv[] )", QObject::tr("Error!"),
							 QObject::tr("Cannot Find AQEMU Data!"), false );
		}
		#else
		
		QDir data_dir;
		
		if( data_dir.exists("/usr/share/aqemu/") )
		{
			settings.setValue( "AQEMU_Data_Folder", "/usr/share/aqemu/" );
			AQDebug( "int main( int argc, char *argv[] )", "Use Data Folder: /usr/share/aqemu/" );
		}
		else if( data_dir.exists("/usr/share/apps/aqemu/") )
		{
			settings.setValue( "AQEMU_Data_Folder", "/usr/share/apps/aqemu/" );
			AQDebug( "int main( int argc, char *argv[] )", "Use Data Folder: /usr/share/apps/aqemu/" );
		}
		else if( data_dir.exists("/usr/local/share/aqemu/") )
		{
			settings.setValue( "AQEMU_Data_Folder", "/usr/local/share/aqemu/" );
			AQDebug( "int main( int argc, char *argv[] )", "Use Data Folder: /usr/local/share/aqemu/" );
		}
		else
		{
			QMessageBox::information( NULL, QObject::tr("Error!"),
				QObject::tr("Cannot Locate AQEMU Data Folder!\nYou Should Select This Folder in Next Window!"),
				QMessageBox::Ok );
			
			QString aqemu_data_dir = QFileDialog::getExistingDirectory( NULL, QObject::tr("Please Select AQEMU Data Folder:"),
																		"/", QFileDialog::ShowDirsOnly );
			
			if( aqemu_data_dir.isEmpty() )
			{
				QMessageBox::critical( NULL, QObject::tr("Error!"),
									   QObject::tr("AQEMU Doesn't Work If Data Folder Not Selected!") );
				return -1;
			}
			else
			{
				if( ! aqemu_data_dir.endsWith("/") ) aqemu_data_dir += "/";
				
				settings.setValue( "AQEMU_Data_Folder", aqemu_data_dir );
			}
		}
		
		#endif
	}
	
	// Load Images
	QString iconsThemeFile = "";
	
	if( settings.value("Icon_Theme", "crystalsvg").toString() == "crystalsvg" )
	{
		iconsThemeFile = settings.value( "AQEMU_Data_Folder", "" ).toString() + "crystalsvg_icons.rcc";
	}
	else
	{
		iconsThemeFile = settings.value( "AQEMU_Data_Folder", "" ).toString() + "oxygen_icons.rcc";
	}
		
	if( ! QResource::registerResource(iconsThemeFile) )
	{
		AQGraphic_Error( "int main( int argc, char *argv[] )", QObject::tr("Error!"),
						 QObject::tr("Cannot Load AQEMU Icon Theme!\nFile \"%1\" Not Found!").arg(iconsThemeFile), false );
	}
	
	// This is a first start AQEMU on this computer?
	if( settings.value("First_Start", "yes").toString() == "yes" )
	{
		First_Start_Wizard *first_start_win = new First_Start_Wizard( NULL );
		
		if( first_start_win->exec() == QDialog::Accepted ) AQDebug( "int main( int argc, char *argv[] )", "Fisrt Start Wizard Complete" );
		else AQWarning( "int main( int argc, char *argv[] )", "Fisrt Start Wizard Canceled!" );
		
		delete first_start_win;
	}
	
	// Load Language
	QTranslator appTranslator;
	
	if( settings.value("Language", "").toString() != "en" )
	{
		appTranslator.load( settings.value("AQEMU_Data_Folder", "").toString() + settings.value("Language", "").toString() + ".qm",
							app.applicationDirPath() );
			
		app.installTranslator( &appTranslator );
	}
	
	// VM Directory Exists?
	QDir vm_dir;
	if( ! vm_dir.exists(settings.value("VM_Directory", "").toString()) )
	{
		int ret = QMessageBox::question( NULL, QObject::tr("Warning!"),
										 QObject::tr("AQEMU VM Folder Not Exists! Create It?"),
										 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
		
		if( ret == QMessageBox::Yes )
		{
			vm_dir.mkdir( QDir::homePath() + "/.aqemu" );
		}
	}
	
	// Check QEMU and KVM Versions
	Update_Emulators_List();
	
	Main_Window Window;
	Window.show();
	
	return app.exec();
}
