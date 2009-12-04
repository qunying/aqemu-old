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

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <QApplication>
#include <QSettings>
#include <QRegExp>
#include <QProcess>
#include <QStringList>

#include "Utils.h"
#include "System_Info.h"

static uint Messages_Index = 0;

static bool Save_Messages_To_Log = true;
static QString Log_Path = "";

static bool Use_Stdout;
static bool Stdout_Debug;
static bool Stdout_Warning;
static bool Stdout_Error;

static QStringList Recent_CD_Images;

void AQDebug( const QString &sender, const QString &mes )
{
	if( Use_Stdout && Stdout_Debug )
	{
		qDebug( qPrintable(QString(
			"\n\33[32mAQEMU Debug\33[0m [%1] >>>\n\33[32mSender:\33[0m %2\n\33[32mMessage:\33[0m %3")
			.arg(Messages_Index).arg(sender).arg(mes)) );
	}
	
	if( Save_Messages_To_Log && Stdout_Debug )
	{
		AQSave_To_Log( "Debug", sender, mes );
	}
	
	Messages_Index++;
}

void AQWarning( const QString &sender, const QString &mes )
{
	if( Use_Stdout && Stdout_Warning )
	{
		qDebug( qPrintable(QString(
			"\n\33[34mAQEMU Warning\33[0m [%1] >>>\n\33[34mSender:\33[0m %2\n\33[34mMessage:\33[0m %3")
			.arg(Messages_Index).arg(sender).arg(mes)) );
	}
	
	if( Save_Messages_To_Log && Stdout_Warning )
	{
		AQSave_To_Log( "Warning", sender, mes );
	}
	
	Messages_Index++;
}

void AQError( const QString &sender, const QString &mes )
{
	if( Use_Stdout && Stdout_Error )
	{
		qDebug( qPrintable(QString(
			"\n\33[31mAQEMU Error\33[0m [%1] >>>\n\33[31mSender:\33[0m %2\n\33[31mMessage:\33[0m %3")
			.arg(Messages_Index).arg(sender).arg(mes)) );
	}
	
	if( Save_Messages_To_Log && Stdout_Error )
	{
		AQSave_To_Log( "Error", sender, mes );
	}
	
	Messages_Index++;
}

void AQGraphic_Warning( const QString &caption, const QString &mes )
{
	QMessageBox::warning( NULL, caption, mes, QMessageBox::Ok );
}

void AQGraphic_Warning( const QString &sender, const QString &caption, const QString &mes, bool fatal )
{
	if( fatal )
	{
		QMessageBox::warning( NULL, caption,
					QString("Sender: %1\nMessage: %2\n").arg(sender).arg(mes) +
					QObject::tr("This Fatal Error. Recomend Close AQEMU."),
					QMessageBox::Ok );
	}
	else
	{
		QMessageBox::warning( NULL, caption,
					QString("Sender: %1\nMessage: %2").arg(sender).arg(mes),
					QMessageBox::Ok );
	}
	
	if( Save_Messages_To_Log )
	{
		AQSave_To_Log( "Warning", sender, mes );
	}
}

void AQGraphic_Error( const QString &sender, const QString &caption, const QString &mes, bool fatal )
{
	if( fatal )
	{
		QMessageBox::critical( NULL, caption,
					QString("Sender: %1\nMessage: %2\n").arg(sender).arg(mes) +
					QObject::tr("This Fatal Error. Recomend Close AQEMU."),
					QMessageBox::Ok );
	}
	else
	{
		QMessageBox::critical( NULL, caption,
					QString("Sender: %1\nMessage: %2").arg(sender).arg(mes),
					QMessageBox::Ok );
	}
	
	if( Save_Messages_To_Log )
	{
		AQSave_To_Log( "Error", sender, mes );
	}
}

void AQUse_Log( bool use )
{
	Save_Messages_To_Log = use;
}

void AQUse_Debug_Output( bool use, bool d, bool w, bool e )
{
	Use_Stdout = use;
	Stdout_Debug = d;
	Stdout_Warning = w;
	Stdout_Error = e;
}

void AQLog_Path( const QString& path )
{
	Log_Path = path;
}

void AQSave_To_Log( const QString &mes_type, const QString &sender, const QString &mes )
{
	if( Log_Path.isEmpty() ) return;
	
	QFile log_file( Log_Path );
	
	if( ! log_file.open(QIODevice::Append | QIODevice::Text) )
	{
		AQUse_Log( false ); // off loging
		AQError( "void AQSave_To_Log( const QString& mes_type, const QString& sender, const QString& mes )",
				 "Cannot Open Log file to Write! Log Path: \"" + Log_Path + "\"" );
	}
	else
	{
		QTextStream out( &log_file );
		out << "Type: " << mes_type << " Num: " << Messages_Index << "\nDate: "
			<< QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss zzz")
			<< "\nSender: " << sender << "\nMessage: " << mes << "\n\n";
	}
}

bool Create_New_HDD_Image( bool encrypted, const QString &base_image,
						   const QString &file_name, const QString &format, VM::Device_Size size, bool verbose )
{
	QStringList args;
	args << "create";
	
	if( encrypted ) args << "-e"; // encrypted
	
	if( ! base_image.isEmpty() )
	{
		args << "-b" << base_image;
	}
	
	args << "-f" << format; // format
	
	args << file_name; // file name
	 
	switch( size.Suffix )
	{
		case VM::Size_Suf_Mb: // MB
			args << QString::number( (long)(size.Size * 1024) );
			break;
			
		case VM::Size_Suf_Gb: // GB
			args << QString::number( (long)(size.Size * 1024) ) + "M";
			break;
			
		default: // KG
			args << QString::number( (long)size.Size );
			break;
	}
	
	QProcess qemu_img;
	QSettings settings;
	qemu_img.start( Get_Default_QEMU_IMG_Path(), args );
	
	if( ! qemu_img.waitForStarted(200) )
	{
		AQGraphic_Error( "bool Create_New_HDD_Image( bool encrypted, const QString &base_image,"
						 "const QString &file_name, const QString &format, VM::Device_Size size, bool verbose )",
						 QObject::tr("Error!"), QObject::tr("Cannot Start qemu-img! Image not Created!") );
		return false;
	}
	
	if( ! qemu_img.waitForFinished(10000) )
	{
		AQGraphic_Error( "bool Create_New_HDD_Image( bool encrypted, const QString &base_image,"
						 "const QString &file_name, const QString &format, VM::Device_Size size, bool verbose )",
						 QObject::tr("Error!"), QObject::tr("qemu-img Cannot Finish! Image not Created!") );
		return false;
	}
	else
	{
		QByteArray err = qemu_img.readAllStandardError();
		QByteArray out = qemu_img.readAllStandardOutput();
		
		if( err.count() > 0 )
		{
			AQGraphic_Error( "bool Create_New_HDD_Image( bool encrypted, const QString &base_image,"
							 "const QString &file_name, const QString &format, VM::Device_Size size, bool verbose )",
							 QObject::tr("Error!"), QObject::tr("Cannot Create Image!\nInformation: ") + err );
		}
		
		QRegExp rx( "Format*ing*fmt*size*" );
		rx.setPatternSyntax( QRegExp::Wildcard );
		
		if( verbose )
		{
			if( rx.exactMatch( out ) )
			{
				QMessageBox::information( NULL, QObject::tr("Complete!"),
						QObject::tr("QEMU-IMG is Create HDD Image.\nAdditional Information:\n") + out );
			}
			else
			{
				QMessageBox::information( NULL, QObject::tr("Warning!"),
						 QObject::tr("QEMU-IMG is Return non Standard Message!.\nAdditional Information:\n") + out );
			}
		}
		
		return true;
	}
}

bool Create_New_HDD_Image( const QString &file_name, VM::Device_Size size )
{
	QSettings settings;
	
	QString format = settings.value( "Default_HDD_Image_Format", "qcow2" ).toString();
	
	return Create_New_HDD_Image( false, "", file_name, format, size, false );
}

bool Format_HDD_Image( const QString &file_name, VM::Disk_Info info )
{
	if( file_name.isEmpty() )
	{
		AQError( "bool Format_HDD_Image( const QString &file_name )",
				 "File Name is Empty!" );
		
		return false;
	}
	
	VM_HDD tmp_hd = VM_HDD( true, file_name );
	tmp_hd.Set_Disk_Info( info );
	QString hd_format = tmp_hd.Get_Image_Format();
	
	if( hd_format.isEmpty() )
	{
		AQError( "bool Format_HDD_Image( const QString &file_name )",
				 "Format is Empty!" );
		
		return false;
	}
	
	return Create_New_HDD_Image( false, "", file_name, hd_format, tmp_hd.Get_Virtual_Size(), false );
}

QList<QString> Get_Templates_List()
{
	QList<QString> all_templates;
	QSettings settings;
	
	// VM Templates
	QDir sys_templates_dir( settings.value("AQEMU_Data_Folder", "").toString() + "os_templates/" );
	
	QFileInfoList sys_templates_list = sys_templates_dir.entryInfoList(
			QStringList("*.aqvmt"), QDir::Files, QDir::Unsorted );
	
	QDir user_templates_dir( settings.value("VM_Directory", "").toString() + "os_templates/" );
	
	QFileInfoList user_templates_list = user_templates_dir.entryInfoList(
			QStringList("*.aqvmt"), QDir::Files, QDir::Unsorted );
	
	for( int tx = 0; tx < sys_templates_list.count(); ++tx )
	{
		for( int ux = 0; ux < user_templates_list.count(); ++ux )
		{
			if( sys_templates_list[tx].completeBaseName() ==
						 user_templates_list[ux].completeBaseName() )
			{
				sys_templates_list.takeAt( tx ); // delete system template
				tx -= 1;
				ux = user_templates_list.count();
			}
		}
	}
	
	// OK. In Template Lists Only Unique Values
	for( int ix = 0; ix < sys_templates_list.count(); ++ix )
	{
		all_templates.append( sys_templates_list[ix].absoluteFilePath() );
	}
	
	for( int ix = 0; ix < user_templates_list.count(); ++ix )
	{
		all_templates.append( user_templates_list[ix].absoluteFilePath() );
	}
	
	// sort
	qSort( all_templates );
	
	return all_templates;
}


QString Get_FS_Compatible_VM_Name( const QString &name )
{
	//QRegExp vm_name_val = QRegExp( "[^a-zA-Z0-9_]" ); // old style
	QRegExp vm_name_val = QRegExp( "[^\\w\\.]" );
	
	QString tmp = name;
	tmp = tmp.replace( vm_name_val, "_" );
	
	return tmp.replace( "__", "_" );
}

QString Get_Complete_VM_File_Path( const QString &vm_name )
{
	//QRegExp vm_name_val = QRegExp( "[^a-zA-Z0-9_]" ); // old style
	QRegExp vm_name_val = QRegExp( "[^\\w]" );
	
	QString tmp = vm_name;
	tmp = tmp.replace( vm_name_val, "_" );
	
	QString new_name = tmp.replace( "__", "_" );
	QString tmp_str = new_name;
	
	QSettings settings;
	
	QString ret_str = settings.value("VM_Directory", "").toString() + tmp_str;
	
	if( ! ret_str.endsWith(".aqemu") )
	{
		ret_str += ".aqemu";
	}
	
	for( int ix = 0; QFile::exists(ret_str); ++ix )
	{
		tmp_str = new_name + QString::number( ix );
	}
	
	return ret_str;
}

QString Get_TR_Size_Suffix( VM::Device_Size suf )
{
	switch( suf.Suffix )
	{
		case VM::Size_Suf_Kb:
			return QObject::tr("Kb");
				
		case VM::Size_Suf_Mb:
			return QObject::tr("Mb");
				
		case VM::Size_Suf_Gb:
			return QObject::tr("Gb");
				
		default:
			AQError( "QString Get_TR_Size_Suffix( VM::Device_Size suf )",
					 "Virtual Size Suffix Default Section!" );
			return "";
	}
}

QString Get_Last_Dir_Path( const QString &path )
{
	QFileInfo info( path );
	QString dir = info.path();
	
	if( dir.isEmpty() )
	{
		return "/";
	}
	else
	{
		return dir;
	}
}

void Check_AQEMU_Permissions()
{
	QSettings settings;
	QFileInfo test_perm;
	
	#ifndef Q_OS_WIN32
	// This Section For Unix Like OS.
	test_perm = QFileInfo( settings.fileName() );
	
	if( test_perm.exists() )
	{
		if( ! test_perm.isWritable() )
		{
			AQGraphic_Error( "void Check_AQEMU_Permissions()", QObject::tr("Error!"),
							 QObject::tr("AQEMU Config File is Read Only!\nCheck Permissions For File: ") +
							 settings.fileName(), true );
		}
	}
	#endif
	
	// Check VM Dir Permissions
	test_perm = QFileInfo( settings.value("VM_Directory", "~").toString() );
	
	if( test_perm.exists() )
	{
		if( ! test_perm.isWritable() )
		{
			AQGraphic_Error( "void Check_AQEMU_Permissions()", QObject::tr("Error!"),
							 QObject::tr("AQEMU VM Directory is Read Only!\nCheck Permissions For: ") +
							 settings.value("VM_Directory", "~").toString(), true );
		}
	}
	
	// Check VM Templates Dir Permissions
	test_perm = QFileInfo( settings.value("VM_Directory", "~").toString() + "os_templates" );
	
	if( test_perm.exists() )
	{
		if( ! test_perm.isWritable() )
		{
			AQGraphic_Error( "void Check_AQEMU_Permissions()", QObject::tr("Error!"),
							 QObject::tr("AQEMU VM Template Directory is Read Only!\nCheck Permissions For: ") +
							 settings.value("VM_Directory", "~").toString() + "os_templates", true );
		}
	}
	
	// Check AQEMU Log File Permissions
	if( ! settings.value("Log_Path", "").toString().isEmpty() )
	{
		test_perm = QFileInfo( settings.value("Log_Path", "").toString() );
		
		if( test_perm.exists() )
		{
			if( ! test_perm.isWritable() )
			{
				AQGraphic_Error( "void Check_AQEMU_Permissions()", QObject::tr("Error!"),
								 QObject::tr("AQEMU Log File is Read Only!\nCheck Permissions For File: ") +
								 settings.value("Log_Path", "").toString(), false );
			}
		}
	}
}

VM::QEMU_Version String_To_QEMU_Version( const QString &str )
{
	if( str.isEmpty() )
	{
		AQError( "VM::QEMU_Version String_To_QEMU_Version( const QString &str )",
				 "Cannot Convert String to QEMU_Version! String is Empty! Use Default QEMU Version: 0.9.0" );
		return VM::QEMU_0_9_0;
	}
	
	if( str == "Old" )
	{
		return VM::QEMU_Old;
	}
	else if( str == "0.9.0" )
	{
		return VM::QEMU_0_9_0;
	}
	else if( str == "0.9.1" )
	{
		return VM::QEMU_0_9_1;
	}
	else if( str == "0.10" )
	{
		return VM::QEMU_0_10;
	}
	else if( str == "New" )
	{
		return VM::QEMU_New;
	}
	else
	{
		AQError( "VM::QEMU_Version String_To_QEMU_Version( const QString &str )",
				 "Cannot Convert String to QEMU_Version! Use Default QEMU Version: 0.9.0" );
		return VM::QEMU_0_9_0;
	}
}

VM::KVM_Version String_To_KVM_Version( const QString &str )
{
	if( str.isEmpty() )
	{
		AQError( "VM::KVM_Version String_To_KVM_Version( const QString &str )",
				 "Cannot Convert String to KVM_Version! String is Empty! Use Default KVM Version: 7X" );
		return VM::KVM_7X;
	}
	
	if( str == "Old" )
	{
		return VM::KVM_Old;
	}
	else if( str == "7X" )
	{
		return VM::KVM_7X;
	}
	else if( str == "8X" )
	{
		return VM::KVM_8X;
	}
	else if( str == "New" )
	{
		return VM::KVM_New;
	}
	else
	{
		AQError( "VM::KVM_Version String_To_KVM_Version( const QString &str )",
				 "Cannot Convert String to KVM_Version! Use Default KVM Version: 7X" );
		return VM::KVM_7X;
	}
}

static QList<Emulator> Emulators_List;
static QList<Emulator> Empty_Emul_List;
static Emulator Empty_Emul;

bool Update_Emulators_List()
{
	QSettings Settings;
	
	int emul_count = Settings.value( "Emulators_Count", "0" ).toString().toInt();
	
	if( emul_count <= 0 )
	{
		AQWarning( "bool Update_Emulators_List()", "Emulators Count == 0" );
		return false;
	}
	
	if( Emulators_List.count() > 0 ) Emulators_List.clear();
	
	for( int ix = 0; ix < emul_count; ix++ )
	{
		QString prefix = "Emulator_" + QString::number( ix ) + "/";
		Emulator tmp_emul;
		
		// Emulator Type
		tmp_emul.Set_Type( Settings.value(prefix + "Type").toString() );
		
		// Name
		tmp_emul.Set_Name( Settings.value(prefix + "Name").toString() );
		
		// Default
		tmp_emul.Set_Default( Settings.value(prefix + "Default").toString() );
		
		// Path
		tmp_emul.Set_Path( Settings.value(prefix + "Path").toString() );
		
		// Check Version on Start AQEMU
		tmp_emul.Set_Check_QEMU_Version( Settings.value(prefix + "Check_QEMU_Version").toString() );
		
		// QEMU Version
		tmp_emul.Set_QEMU_Version( Settings.value(prefix + "QEMU_Version").toString() );
		
		// Check Version KVM on Start AQEMU
		tmp_emul.Set_Check_KVM_Version( Settings.value(prefix + "Check_KVM_Version").toString() );
		
		// KVM Version
		tmp_emul.Set_KVM_Version( Settings.value(prefix + "KVM_Version").toString() );
		
		// Check Available Audio Cards
		tmp_emul.Set_Check_Available_Audio_Cards( Settings.value(prefix + "Check_Available_Audio_Cards").toString() );
		
		// Binary Files
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
		
		QStringList KVM_Binary_Names;
		KVM_Binary_Names << "kvm";
		KVM_Binary_Names << "kvm-img";
		
		QMap<QString, QString> bin_list;
		
		if( tmp_emul.Get_Type() == "QEMU" )
		{
			for( int ix = 0; ix < QEMU_Binary_Names.count(); ix++ )
				bin_list[ QEMU_Binary_Names[ix] ] = Settings.value( prefix + QEMU_Binary_Names[ix] ).toString();
		}
		else
		{
			for( int ix = 0; ix < KVM_Binary_Names.count(); ix++ )
				bin_list[ KVM_Binary_Names[ix] ] = Settings.value( prefix + KVM_Binary_Names[ix] ).toString();
		}
		
		tmp_emul.Set_Binary_Files( bin_list );
		
		// Update Versions
		if( tmp_emul.Get_Type() == "QEMU" && tmp_emul.Get_Check_QEMU_Version() == "yes" )
		{
			QString tmp_ver = System_Info::Get_QEMU_Version( bin_list[QEMU_Binary_Names[0]] );
			
			if( ! tmp_ver.isEmpty() ) tmp_emul.Set_QEMU_Version( tmp_ver );
			else AQError( "bool Update_Emulators_List()", "Cannot Update Version! Use Old." );
		}
		else if( tmp_emul.Get_Type() == "KVM" && tmp_emul.Get_Check_KVM_Version() == "yes" )
		{
			QString tmp_ver = System_Info::Get_KVM_Version( bin_list[KVM_Binary_Names[0]] );
			
			if( ! tmp_ver.isEmpty() ) tmp_emul.Set_KVM_Version( tmp_ver );
			else AQError( "bool Update_Emulators_List()", "Cannot Update Version! Use Old." );
		}
		
		Emulators_List << tmp_emul;
	}
	
	return true;
}

const QList<Emulator> &Get_Emulators_List()
{
	if( Update_Emulators_List() ) return Emulators_List;
	else
	{
		AQError( "QList<Emulator> Get_Emulators_List()", "Cannot Update Emulators List" );
		return Empty_Emul_List;
	}
}

const Emulator &Get_Default_Emulator( const QString &type )
{
	for( int ix = 0; ix < Emulators_List.count(); ix++ )
	{
		if( Emulators_List[ix].Get_Type() == type && Emulators_List[ix].Get_Default() == "yes" )
		{
			return Emulators_List[ ix ];
		}
	}
	
	AQError( "const Emulator Get_Default_Emulator( const QString &type )", "Cannot Find!" );
	return Empty_Emul;
}

const Emulator &Get_Emulator_By_Name( const QString &name )
{
	if( Emulators_List.count() <= 0 )
		AQError( "const Emulator Get_Emulator_By_Name( const QString &name )", "Emulator Count == 0" );
	else
	{
		for( int ix = 0; ix < Emulators_List.count(); ix++ )
		{
			if( Emulators_List[ix].Get_Name() == name ) return Emulators_List[ ix ];
		}
	}
	
	AQError( "const Emulator Get_Emulator_By_Name( const QString &name )", "Cannot Find!" );
	return Empty_Emul;
}

QString Get_Default_QEMU_IMG_Path( QString emul_name )
{
	Emulator tmp;
	
	if( emul_name.isEmpty() )
	{
		tmp = Get_Default_Emulator( "QEMU" );
		
		if( tmp.Get_Type().isEmpty() && tmp.Get_Name().isEmpty() )
		{
			tmp = Get_Default_Emulator( "KVM" );
			
			if( tmp.Get_Type().isEmpty() && tmp.Get_Name().isEmpty() )
			{
				AQError( "QString Get_Default_QEMU_IMG_Path()", "Cannot Get Emulator!" );
				return "qemu-img";
			}
		}
	}
	else
	{
		tmp = Get_Emulator_By_Name( emul_name );
		
		if( tmp.Get_Type().isEmpty() && tmp.Get_Name().isEmpty() )
		{
			AQError( "QString Get_Default_QEMU_IMG_Path()", "Cannot Get Emulator!" );
			return "qemu-img";
		}
	}
	
	QMap<QString, QString> bin_list = tmp.Get_Binary_Files();
	
	for( QMap<QString, QString>::const_iterator iter = bin_list.constBegin(); iter != bin_list.constEnd(); iter++ )
	{
		if( iter.key() == "qemu-img" || iter.key() == "kvm-img" )
		{
			return iter.value();
		}
	}
	
	AQError( "QString Get_Default_QEMU_IMG_Path()", "Cannot Find qemu-img and kmv-img!" );
	return "qemu-img";
}

int Get_Random( int min, int max )
{
	if( min < 0 || max > RAND_MAX || min > max )
	{
		return -1;
	}
	
	qsrand( QTime::currentTime().msec() );
	
	return int( qrand() / (RAND_MAX + 1.0) * (max + 1 - min) + min );
}

void Load_Recent_Images_List()
{
	QSettings settings;
	int max = settings.value( "CD_ROM_Exits_Images/Max", "5" ).toString().toInt();
	
	Recent_CD_Images.clear();
	
	for( int ix = 0; ix < max; ++ix )
	{
		Recent_CD_Images << settings.value( "CD_ROM_Exits_Images/item" + QString::number(ix), "" ).toString();
	}
}

const QStringList &Get_CD_Recent_Images_List()
{
	return Recent_CD_Images;
}

void Add_To_Recent_Files( const QString &path )
{
	QSettings settings;
	int max = settings.value( "CD_ROM_Exits_Images/Max", "5" ).toString().toInt();
	
	// This Unique Path?
	for( int fx = 0; fx < Recent_CD_Images.count() && fx < max; ++fx )
	{
		if( Recent_CD_Images[fx] == path )
		{
			AQDebug( "void Add_To_Recent_Files( const QString &path )",
					 "CD-ROM Path Not Unique." );
			return;
		}
	}
	
	// Add to List
	if( Recent_CD_Images.count() < max )
	{
		Recent_CD_Images << path;
	}
	else
	{
		for( int ix = 0; ix < Recent_CD_Images.count() -1 && ix < max -1; ++ix )
		{
			Recent_CD_Images[ ix ] = Recent_CD_Images[ ix +1 ];
		}
		
		Recent_CD_Images[ max -1 ] = path;
	}
	
	settings.setValue( "CD_ROM_Exits_Images/item" + QString::number(Recent_CD_Images.count()-1), path );
}

static bool Show_Error_Window;

bool Get_Show_Error_Window()
{
	return Show_Error_Window;
}

void Set_Show_Error_Window( bool show )
{
	Show_Error_Window = show;
}
