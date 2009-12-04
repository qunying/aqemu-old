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

#include <QRegExp>
#include <QUuid>
#include <QSettings>
#include <QFile>
#include <QStringList>

#include "VM_Devices.h"
#include "Utils.h"
#include "HDD_Image_Info.h"

// VM_Storage_Device --------------------------------------------------------

Emulator::Emulator()
{
	Type = Name = Default = Path = Check_QEMU_Version = QEMU_Version = Check_KVM_Version = KVM_Version = Check_Available_Audio_Cards = "";
	Binary_Files = QMap<QString, QString>();
}

Emulator::Emulator( const Emulator &emul )
{
	Type = emul.Get_Type();
	Name = emul.Get_Name();
	Default = emul.Get_Default();
	Path = emul.Get_Path();
	Check_QEMU_Version = emul.Get_Check_QEMU_Version();
	QEMU_Version = emul.Get_QEMU_Version();
	Check_KVM_Version = emul.Get_Check_KVM_Version();
	KVM_Version = emul.Get_KVM_Version();
	Check_Available_Audio_Cards = emul.Get_Check_Available_Audio_Cards();
	Binary_Files = emul.Get_Binary_Files();
}

bool Emulator::operator==( const Emulator &emul ) const
{
	if( Type == emul.Get_Type() &&
		Name == emul.Get_Name() &&
		Default == emul.Get_Default() &&
		Path == emul.Get_Path() &&
		Check_QEMU_Version == emul.Get_Check_QEMU_Version() &&
		QEMU_Version == emul.Get_QEMU_Version() &&
		Check_KVM_Version == emul.Get_Check_KVM_Version() &&
		KVM_Version == emul.Get_KVM_Version() &&
		Check_Available_Audio_Cards == emul.Get_Check_Available_Audio_Cards() &&
		Binary_Files == emul.Get_Binary_Files() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Emulator::operator!=( const Emulator &emul ) const
{
	return ! ( operator==(emul) );
}

const QString& Emulator::Get_Type() const
{
	return Type;
}

void Emulator::Set_Type( const QString &type )
{
	if( Type != type )
	{
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
		
		if( type == "QEMU" )
		{
			Binary_Files.clear();
			for( int ix = 0; ix < QEMU_Binary_Names.count(); ++ix ) Binary_Files[ QEMU_Binary_Names[ix] ] = "";
		}
		else if( type == "KVM" )
		{
			Binary_Files.clear();
			for( int ix = 0; ix < KVM_Binary_Names.count(); ++ix ) Binary_Files[ KVM_Binary_Names[ix] ] = "";
		}
		else
		{
			AQError( "void Emulator::Set_Type( const QString &type )",
					 "Type \"" + type + "\" Incorrect! Use Default: QEMU" );
			
			Binary_Files.clear();
			for( int ix = 0; ix < QEMU_Binary_Names.count(); ++ix ) Binary_Files[ QEMU_Binary_Names[ix] ] = "";
		}
		
		Type = type;
	}
}

const QString& Emulator::Get_Name() const
{
	return Name;
}

void Emulator::Set_Name( const QString &name )
{
	Name = name;
}

const QString& Emulator::Get_Default() const
{
	return Default;
}

void Emulator::Set_Default( const QString &use )
{
	Default = use;
}

const QString& Emulator::Get_Path() const
{
	return Path;
}

void Emulator::Set_Path( const QString &path )
{
	Path = path;
}

const QString& Emulator::Get_Check_QEMU_Version() const
{
	return Check_QEMU_Version;
}

void Emulator::Set_Check_QEMU_Version( const QString &check )
{
	Check_QEMU_Version = check;
}

const QString& Emulator::Get_QEMU_Version() const
{
	return QEMU_Version;
}

void Emulator::Set_QEMU_Version( const QString &ver )
{
	QEMU_Version = ver;
}

const QString& Emulator::Get_Check_KVM_Version() const
{
	return Check_KVM_Version;
}

void Emulator::Set_Check_KVM_Version( const QString &check )
{
	Check_KVM_Version = check;
}

const QString& Emulator::Get_KVM_Version() const
{
	return KVM_Version;
}

void Emulator::Set_KVM_Version( const QString &ver )
{
	KVM_Version = ver;
}

const QString& Emulator::Get_Check_Available_Audio_Cards() const
{
	return Check_Available_Audio_Cards;
}

void Emulator::Set_Check_Available_Audio_Cards( const QString &check )
{
	Check_Available_Audio_Cards = check;
}

const QMap<QString, QString>& Emulator::Get_Binary_Files() const
{
	return Binary_Files;
}

void Emulator::Set_Binary_Files( const QMap<QString, QString> &files )
{
	Binary_Files = files;
}

//---------------------------------------------------------------------------

// VM_Storage_Device --------------------------------------------------------

VM_Storage_Device::VM_Storage_Device()
{
	UFile_Path = false;
	File_Path = "";
	UInterface = false;
	Interface = VM::DI_IDE;
	UBus_Unit = false;
	Bus = 0;
	Unit = 0;
	UIndex = false;
	Index = 0;
	UMedia = false;
	Media = VM::DM_Disk;
	Uhdachs = false;
	Cyls = 0;
	Heads = 0;
	Secs = 0;
	Trans = 0;
	Snapshot = false;
	Cache = false;
}

VM_Storage_Device::VM_Storage_Device( const VM_Storage_Device &sd )
{
	UFile_Path = sd.Use_File_Path();
	File_Path = sd.Get_File_Path();
	UInterface = sd.Use_Interface();
	Interface = sd.Get_Interface();
	UBus_Unit = sd.Use_Bus_Unit();
	Bus = sd.Get_Bus();
	Unit = sd.Get_Unit();
	UIndex = sd.Use_Index();
	Index = sd.Get_Index();
	UMedia = sd.Use_Media();
	Media = sd.Get_Media();
	Uhdachs = sd.Use_hdachs();
	Cyls = sd.Get_Cyls();
	Heads = sd.Get_Heads();
	Secs = sd.Get_Secs();
	Trans = sd.Get_Trans();
	Snapshot = sd.Get_Snapshot();
	Cache = sd.Get_Cache();
}

QString VM_Storage_Device::Get_QEMU_Device_Name() const
{
	// FIXME Get_QEMU_Device_Name()
	return "ide0-hd1";
}

bool VM_Storage_Device::operator==( const VM_Storage_Device &sd ) const
{
	if( UFile_Path == sd.Use_File_Path() &&
		File_Path == sd.Get_File_Path() &&
		UInterface == sd.Use_Interface() &&
		Interface == sd.Get_Interface() &&
		UBus_Unit == sd.Use_Bus_Unit() &&
		Bus == sd.Get_Bus() &&
		Unit == sd.Get_Unit() &&
		UIndex == sd.Use_Index() &&
		Index == sd.Get_Index() &&
		UMedia == sd.Use_Media() &&
		Media == sd.Get_Media() &&
		Uhdachs == sd.Use_hdachs() &&
		Cyls == sd.Get_Cyls() &&
		Heads == sd.Get_Heads() &&
		Secs == sd.Get_Secs() &&
		Trans == sd.Get_Trans() &&
		Snapshot == sd.Get_Snapshot() &&
		Cache == sd.Get_Cache() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool VM_Storage_Device::operator!=( const VM_Storage_Device &sd ) const
{
	return ! ( operator==(sd) );
}

bool VM_Storage_Device::Use_File_Path() const
{
	return UFile_Path;
}

void VM_Storage_Device::Use_File_Path( bool use )
{
	UFile_Path = use;
}

const QString &VM_Storage_Device::Get_File_Path() const
{
	return File_Path;
}

void VM_Storage_Device::Set_File_Path( const QString &path )
{
	File_Path = path;
}

bool VM_Storage_Device::Use_Interface() const
{
	return UInterface;
}

void VM_Storage_Device::Use_Interface( bool use )
{
	UInterface = use;
}

VM::Device_Interface VM_Storage_Device::Get_Interface() const
{
	return Interface;
}

void VM_Storage_Device::Set_Interface( VM::Device_Interface di )
{
	Interface = di;
}

bool VM_Storage_Device::Use_Bus_Unit() const
{
	return UBus_Unit;
}

void VM_Storage_Device::Use_Bus_Unit( bool use )
{
	UBus_Unit = use;
}

int VM_Storage_Device::Get_Bus() const
{
	return Bus;
}

void VM_Storage_Device::Set_Bus( int bus )
{
	Bus = bus;
}

int VM_Storage_Device::Get_Unit() const
{
	return Unit;
}

void VM_Storage_Device::Set_Unit( int unit )
{
	Unit = unit;
}

bool VM_Storage_Device::Use_Index() const
{
	return UIndex;
}

void VM_Storage_Device::Use_Index( bool use )
{
	UIndex = use;
}

int VM_Storage_Device::Get_Index() const
{
	return Index;
}

void VM_Storage_Device::Set_Index( int index )
{
	Index = index;
}

bool VM_Storage_Device::Use_Media() const
{
	return UMedia;
}

void VM_Storage_Device::Use_Media( bool use )
{
	UMedia = use;
}

VM::Device_Media VM_Storage_Device::Get_Media() const
{
	return Media;
}

void VM_Storage_Device::Set_Media( VM::Device_Media media )
{
	Media = media;
}

bool VM_Storage_Device::Use_hdachs() const
{
	return Uhdachs;
}

void VM_Storage_Device::Use_hdachs( bool use )
{
	Uhdachs = use;
}

qulonglong VM_Storage_Device::Get_Cyls() const
{
	return Cyls;
}

void VM_Storage_Device::Set_Cyls( qulonglong cyls )
{
	Cyls = cyls;
}

qulonglong VM_Storage_Device::Get_Heads() const
{
	return Heads;
}

void VM_Storage_Device::Set_Heads( qulonglong heads )
{
	Heads = heads;
}

qulonglong VM_Storage_Device::Get_Secs() const
{
	return Secs;
}

void VM_Storage_Device::Set_Secs( qulonglong secs )
{
	Secs = secs;
}

qulonglong VM_Storage_Device::Get_Trans() const
{
	return Trans;
}

void VM_Storage_Device::Set_Trans( qulonglong trans )
{
	Trans = trans;
}

bool VM_Storage_Device::Get_Snapshot() const
{
	return Snapshot;
}

void VM_Storage_Device::Set_Snapshot( bool snapshot )
{
	Snapshot = snapshot;
}

bool VM_Storage_Device::Get_Cache() const
{
	return Cache;
}

void VM_Storage_Device::Set_Cache( bool cache )
{
	Cache = cache;
}

//---------------------------------------------------------------------------

// VM_Floppy Class ----------------------------------------------------------

VM_Floppy::VM_Floppy()
{
	Enabled = false;
	Host_Device = true;
	Host_File_Name = "";
	Image_File_Name = "";
}

VM_Floppy::VM_Floppy( const VM_Floppy &fd )
{
	Enabled = fd.Get_Enabled();
	Host_File_Name = fd.Get_Host_File_Name();
	Image_File_Name = fd.Get_Image_File_Name();
	Host_Device = fd.Get_Host_Device();
}

VM_Floppy::VM_Floppy( bool enabled, const QString &host_path,
					  const QString &image_path, bool host_device )
{
	Enabled = enabled;
	Host_File_Name = host_path;
	Image_File_Name = image_path;
	Host_Device = host_device;
	
	if( Enabled && Host_Device == false && Image_File_Name.isEmpty() )
	{
		AQWarning( "VM_Floppy::VM_Floppy( bool enabled, const QString &host_path,"
				   "const QString &image_path, bool host_device )",
			       "Image File Name is Empty!" );
	}
}

bool VM_Floppy::operator==( const VM_Floppy &v ) const
{
	if( Enabled == v.Get_Enabled() &&
		Image_File_Name == v.Get_Image_File_Name() &&
		Host_Device == v.Get_Host_Device() )
	{
		if( Host_File_Name == v.Get_Host_File_Name() ||
			v.Get_Host_File_Name().isEmpty() ||
			Host_File_Name.isEmpty() )
		{
			return true;
		}
		else return false;
	}
	else
	{
		return false;
	}
}

bool VM_Floppy::operator!=( const VM_Floppy &v ) const
{
	return ! ( operator==(v) );
}

bool VM_Floppy::Get_Enabled() const
{
	return Enabled;
}

void VM_Floppy::Set_Enabled( bool enabled )
{
	Enabled = enabled;
}

const QString &VM_Floppy::Get_Host_File_Name() const
{
	return Host_File_Name;
}

void VM_Floppy::Set_Host_File_Name( const QString &file_path )
{
	if( file_path.isEmpty() && Enabled )
	{
		AQWarning( "void VM_Floppy::Set_Host_File_Name( const QString &file_path )",
				   "File Name is Empty!" );
		Host_File_Name = "";
	}
	else
	{
		Host_File_Name = file_path;
	}
}

const QString &VM_Floppy::Get_Image_File_Name() const
{
	return Image_File_Name;
}

void VM_Floppy::Set_Image_File_Name( const QString &file_path )
{
	if( file_path.isEmpty() && Enabled )
	{
		AQWarning( "void VM_Floppy::Set_Image_File_Name( const QString &file_path )",
				   "File Name is Empty!" );
		Image_File_Name = "";
	}
	else
	{
		Image_File_Name = file_path;
	}
}

bool VM_Floppy::Get_Host_Device() const
{
	return Host_Device;
}

void VM_Floppy::Set_Host_Device( bool enabled )
{
	Host_Device = enabled;
}

//===========================================================================

// VM_CDROM Class -----------------------------------------------------------

VM_CDROM::VM_CDROM()
{
	Enabled = false;
	Host_File_Name = "";
	Image_File_Name = "";
	Host_Device = true;
}

VM_CDROM::VM_CDROM( const VM_CDROM &cd )
{
	Enabled = cd.Get_Enabled();
	Host_File_Name = cd.Get_Host_File_Name();
	Image_File_Name = cd.Get_Image_File_Name();
	Host_Device = cd.Get_Host_Device();
}

VM_CDROM::VM_CDROM( bool enabled, const QString &host_path,
					const QString &image_path, bool host_device )
{
	Enabled = enabled;
	Host_File_Name = host_path;
	Image_File_Name = image_path;
	Host_Device = host_device;
	
	if( Enabled && Host_Device == false && Image_File_Name.isEmpty() )
	{
		AQWarning( "VM_CDROM::VM_CDROM( bool enabled, const QString &host_path,"
				   "const QString &image_path, bool host_device )",
				   "Image File Name is Empty!" );
	}
}

bool VM_CDROM::operator==( const VM_CDROM &v ) const
{
	if( Enabled == v.Get_Enabled() &&
		Image_File_Name == v.Get_Image_File_Name() &&
		Host_Device == v.Get_Host_Device() )
	{
		// This for users without CD/DVD-ROM
		if( Host_File_Name == v.Get_Host_File_Name() ||
			v.Get_Host_File_Name().isEmpty() ||
			Host_File_Name.isEmpty() )
		{
			return true;
		}
		else return false;
	}
	else
	{
		return false;
	}
}

bool VM_CDROM::operator!=( const VM_CDROM &v ) const
{
	return ! ( operator==(v) );
}

bool VM_CDROM::Get_Enabled() const
{
	return Enabled;
}

void VM_CDROM::Set_Enabled( bool enabled )
{
	Enabled = enabled;
}

const QString &VM_CDROM::Get_Host_File_Name() const
{
	return Host_File_Name;
}

void VM_CDROM::Set_Host_File_Name( const QString &file_path )
{
	if( file_path.isEmpty() && Enabled )
	{
		AQWarning( "void VM_CDROM::Set_Host_File_Name( const QString &file_path )",
				   "File Name is Empty!" );
		Host_File_Name = "";
	}
	else
	{
		Host_File_Name = file_path;
	}
}

const QString &VM_CDROM::Get_Image_File_Name() const
{
	return Image_File_Name;
}

void VM_CDROM::Set_Image_File_Name( const QString &file_path )
{
	if( file_path.isEmpty() && Enabled )
	{
		AQWarning( "void VM_CDROM::Set_Image_File_Name( const QString &file_path )",
				   "File Name is Empty!" );
		Image_File_Name = "";
	}
	else
	{
		Image_File_Name = file_path;
	}
}

bool VM_CDROM::Get_Host_Device() const
{
	return Host_Device;
}

void VM_CDROM::Set_Host_Device( bool enabled )
{
	Host_Device = enabled;
}

//===========================================================================

// VM_HDD Class -------------------------------------------------------------

VM_HDD::VM_HDD()
{
	Enabled = false;
	Image_File_Name = "";
	
	Disk_Format = "";
	
	VM::Device_Size zero_size;
	zero_size.Size = 0.0;
	zero_size.Suffix = VM::Size_Suf_Gb;
	
	Virtual_Size = zero_size;
	Disk_Size = zero_size;
	
	Cluster_Size = 0;
}

VM_HDD::VM_HDD( const VM_HDD &hd  )
{
	Enabled = hd.Get_Enabled();
	Image_File_Name = hd.Get_Image_File_Name();
	
	if( Image_File_Name.isEmpty() )
	{
		Disk_Format = "";
		
		VM::Device_Size zero_size;
		zero_size.Size = 0.0;
		zero_size.Suffix = VM::Size_Suf_Gb;
		
		Virtual_Size = zero_size;
		Disk_Size = zero_size;
		
		Cluster_Size = 0;
	}
}

VM_HDD::VM_HDD( bool enabled, const QString &im_pach )
{
	Enabled = enabled;
	Image_File_Name = im_pach;
	
	if( im_pach.isEmpty() )
	{
		Disk_Format = "";
		
		VM::Device_Size zero_size;
		zero_size.Size = 0.0;
		zero_size.Suffix = VM::Size_Suf_Gb;
		
		Virtual_Size = zero_size;
		Disk_Size = zero_size;
		
		Cluster_Size = 0;
	}
}

bool VM_HDD::operator==( const VM_HDD &v ) const
{
	if( Enabled == v.Get_Enabled() &&
		Image_File_Name == v.Get_Image_File_Name() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool VM_HDD::operator!=( const VM_HDD &v ) const
{
	return ! ( operator==(v) );
}

bool VM_HDD::Get_Enabled() const
{
	return Enabled;
}

void VM_HDD::Set_Enabled( bool enabled )
{
	Enabled = enabled;
}

const QString &VM_HDD::Get_Image_File_Name() const
{
	return Image_File_Name;
}

void VM_HDD::Set_Image_File_Name( const QString &file_name )
{
	if( file_name.isEmpty() && Enabled )
	{
		Image_File_Name = "";
		Disk_Format = "";
	
		VM::Device_Size zero_size;
		zero_size.Size = 0.0;
		zero_size.Suffix = VM::Size_Suf_Gb;
	
		Virtual_Size = zero_size;
		Disk_Size = zero_size;
	
		Cluster_Size = 0;
	}
	else
	{
		if( file_name.isEmpty() )
		{
			Image_File_Name = "";
			Disk_Format = "";
	
			VM::Device_Size zero_size;
			zero_size.Size = 0.0;
			zero_size.Suffix = VM::Size_Suf_Gb;
	
			Virtual_Size = zero_size;
			Disk_Size = zero_size;
	
			Cluster_Size = 0;
		}
		else
		{
			Image_File_Name = file_name;
		}
	}
}

void VM_HDD::Set_Disk_Info( VM::Disk_Info info )
{
	Disk_Format = info.Disk_Format;
	Virtual_Size = info.Virtual_Size;
	Disk_Size = info.Disk_Size;
	Cluster_Size = info.Cluster_Size;
}

const QString &VM_HDD::Get_Image_Format() const
{
	return Disk_Format;
}

const VM::Device_Size &VM_HDD::Get_Virtual_Size() const
{
	return Virtual_Size;
}

const VM::Device_Size &VM_HDD::Get_Disk_Size() const
{
	return Disk_Size;
}

int VM_HDD::Get_Cluster_Size() const
{
	return Cluster_Size;
}

double VM_HDD::Get_Virtual_Size_in_GB() const
{
	if( Virtual_Size.Size < 1 ) return 0.0;
	
	int div = 0;
	
	switch( Virtual_Size.Suffix )
	{
		case VM::Size_Suf_Kb:
			div = 1048576;
			break;
			
		case VM::Size_Suf_Mb:
			div = 1024;
			break;
			
		case VM::Size_Suf_Gb:
			div = 1;
			break;
			
		default:
			AQError( "double VM_HDD::Get_Virtual_Size_in_GB() const",
					 "Default Section!" );
			return 0.0;
	}
	
	return Virtual_Size.Size / div;
}

QString VM_HDD::Get_Complete_Virtual_Size() const
{
	return QString::number(Virtual_Size.Size) + Size_Suffix_to_String(Virtual_Size.Suffix);
}

QString VM_HDD::Get_Complete_Disk_Size() const
{
	return QString::number(Disk_Size.Size) + Size_Suffix_to_String(Disk_Size.Suffix);
}

void VM_HDD::Set_Virtual_Size( const VM::Device_Size &size )
{
	Virtual_Size = size;
}

void VM_HDD::Set_Image_Format( const QString &format )
{
	Disk_Format = format;
}

QString VM_HDD::Size_Suffix_to_String( VM::Size_Suffix suf ) const
{
	switch( suf )
	{
		case VM::Size_Suf_Kb:
			return "K";
			
		case VM::Size_Suf_Mb:
			return "M";
			
		case VM::Size_Suf_Gb:
			return "G";
			
		default:
			AQError( "QString VM_HDD::Size_Suffix_to_String( VM::Size_Suffix suf ) const",
					 "Size Suffix Incorrect!" );
			return "";
	}
}

VM::Device_Size VM_HDD::String_to_Device_Size( const QString &size ) const
{
	VM::Device_Size zero_size, hd_size;
	zero_size.Size = 0.0;
	zero_size.Suffix = VM::Size_Suf_Gb;
	
	if( size.isEmpty() )
	{
		AQDebug( "VM::Device_Size VM_HDD::String_to_Device_Size( const QString &size ) const",
				 "Size String is Empty..." );
		return zero_size;
	}
	
	QRegExp RegInfo = QRegExp( "([\\d]+|[\\d]+[.,][\\d]+).*([KMG]+)" );
	
	if( ! RegInfo.exactMatch(size) )
	{
		AQError( "VM::Device_Size VM_HDD::String_to_Device_Size( const QString &size ) const",
				 "Cannot Match RegExp!" );
		return zero_size;
	}
	
	QStringList info_lines = RegInfo.capturedTexts();
	
	bool ok = false;
	hd_size.Size = info_lines[1].toDouble( &ok );
	
	if( ! ok )
	{
		AQError( "VM::Device_Size VM_HDD::String_to_Device_Size( const QString &size ) const",
				 "Cannot Convert String to Double!" );
		return zero_size;
	}
	
	if( info_lines[2] == "K" )
	{
		hd_size.Suffix = VM::Size_Suf_Kb;
	}
	else if( info_lines[2] == "M" )
	{
		hd_size.Suffix = VM::Size_Suf_Mb;
	}
	else if( info_lines[2] == "G" )
	{
		hd_size.Suffix = VM::Size_Suf_Gb;
	}
	else
	{
		AQError( "VM::Device_Size VM_HDD::String_to_Device_Size( const QString &size ) const",
				 "HDD Size Suffix Not K,M,G!" );
		return zero_size;
	}
	
	return hd_size;
}

//===========================================================================

// VM_Snapshot --------------------------------------------------------------

VM_Snapshot::VM_Snapshot()
{
	Tag = "";
	ID = "";
	Size = "";
	Date = "";
	VM_Clock = "";
	Name = "";
	Description = "";
}

VM_Snapshot::VM_Snapshot( const VM_Snapshot &s )
{
	Tag = s.Get_Tag();
	ID = s.Get_ID();
	Size = s.Get_Size();
	Date = s.Get_Date();
	VM_Clock = s.Get_VM_Clock();
	Name = s.Get_Name();
	Description = s.Get_Description();
}

bool VM_Snapshot::operator==( const VM_Snapshot &s ) const
{
	if( Tag == s.Get_Tag() &&
		ID == s.Get_ID() &&
		Size == s.Get_Size() &&
		Date == s.Get_Date() &&
		VM_Clock == s.Get_VM_Clock() &&
		Name == s.Get_Name() &&
		Description == s.Get_Description() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool VM_Snapshot::operator!=( const VM_Snapshot &s ) const
{
	return ! ( operator==(s) );
}

const QString &VM_Snapshot::Get_Tag() const
{
	return Tag;
}

void VM_Snapshot::Set_Tag( const QString &tag )
{
	Tag = tag;
}

const QString &VM_Snapshot::Get_ID() const
{
	return ID;
}

void VM_Snapshot::Set_ID( const QString &id )
{
	ID = id;
}

const QString &VM_Snapshot::Get_Size() const
{
	return Size;
}

void VM_Snapshot::Set_Size( const QString &size )
{
	Size = size;
}

const QString &VM_Snapshot::Get_Date() const
{
	return Date;
}

void VM_Snapshot::Set_Date( const QString &date )
{
	Date = date;
}

const QString &VM_Snapshot::Get_VM_Clock() const
{
	return VM_Clock;
}

void VM_Snapshot::Set_VM_Clock( const QString &vm_clock )
{
	VM_Clock = vm_clock;
}

const QString &VM_Snapshot::Get_Name() const
{
	return Name;
}

void VM_Snapshot::Set_Name( const QString &name )
{
	Name = name;
}

const QString &VM_Snapshot::Get_Description() const
{
	return Description;
}

void VM_Snapshot::Set_Description( const QString &desc )
{
	Description = desc;
}

//===========================================================================

// VM_Net_Card_Nativ --------------------------------------------------------

VM_Net_Card_Nativ::VM_Net_Card_Nativ()
{
	Network_Mode = VM::Net_Mode_Nativ_NIC;
	Card_Model = "";
	MAC = "";
	VLAN = 0;
	Name = "";
	Hostname = "";
	PortDev = "";
	File_Descriptor = 0;
	Interface_Name = "";
	TUN_TAP_Script = "";
	TUN_TAP_Down_Script = "";
	Listen = "";
	Connect = "";
	MCast = "";
	Sock = "";
	Port = 0;
	Group = "";
	Mode = "";
	File = "";
	Len = 0;
	
	_Use_Card_Model = false;
	_Use_MAC = false;
	_Use_VLAN = false;
	_Use_Name = false;
	_Use_Hostname = false;
	_Use_File_Descriptor = false;
	_Use_Interface_Name = false;
	_Use_TUN_TAP_Script = false;
	_Use_TUN_TAP_Down_Script = false;
	_Use_Listen = false;
	_Use_Connect = false;
	_Use_MCast = false;
	_Use_Sock = false;
	_Use_Port = false;
	_Use_Group = false;
	_Use_Mode = false;
	_Use_File = false;
	_Use_Len = false;
}

VM_Net_Card_Nativ::VM_Net_Card_Nativ( const VM_Net_Card_Nativ &nc )
{
	Network_Mode = nc.Get_Network_Type();
	Card_Model = nc.Get_Card_Model();
	MAC = nc.Get_MAC_Address();
	VLAN = nc.Get_VLAN();
	Name = nc.Get_Name();
	Hostname = nc.Get_Hostname();
	PortDev = nc.Get_PortDev();
	File_Descriptor = nc.Get_File_Descriptor();
	Interface_Name = nc.Get_Interface_Name();
	TUN_TAP_Script = nc.Get_TUN_TAP_Script();
	TUN_TAP_Down_Script = nc.Get_TUN_TAP_Down_Script();
	Listen = nc.Get_Listen();
	Connect = nc.Get_Connect();
	MCast = nc.Get_MCast();
	Sock = nc.Get_Sock();
	Port = nc.Get_Port();
	Group = nc.Get_Group();
	Mode = nc.Get_Mode();
	File = nc.Get_File();
	Len = nc.Get_Len();
	
	_Use_Card_Model = nc.Use_Card_Model();
	_Use_MAC = nc.Use_MAC_Address();
	_Use_VLAN = nc.Use_VLAN();
	_Use_Name = nc.Use_Name();
	_Use_Hostname = nc.Use_Hostname();
	_Use_File_Descriptor = nc.Use_File_Descriptor();
	_Use_Interface_Name = nc.Use_Interface_Name();
	_Use_TUN_TAP_Script = nc.Use_TUN_TAP_Script();
	_Use_TUN_TAP_Down_Script = nc.Use_TUN_TAP_Down_Script();
	_Use_Listen = nc.Use_Listen();
	_Use_Connect = nc.Use_Connect();
	_Use_MCast = nc.Use_MCast();
	_Use_Sock = nc.Use_Sock();
	_Use_Port = nc.Use_Port();
	_Use_Group = nc.Use_Group();
	_Use_Mode = nc.Use_Mode();
	_Use_File = nc.Use_File();
	_Use_Len = nc.Use_Len();
}

bool VM_Net_Card_Nativ::operator==( const VM_Net_Card_Nativ &nc ) const
{
	if( Network_Mode == nc.Get_Network_Type() &&
		Card_Model == nc.Get_Card_Model() &&
		MAC == nc.Get_MAC_Address() &&
		VLAN == nc.Get_VLAN() &&
		Name == nc.Get_Name() &&
		Hostname == nc.Get_Hostname() &&
		PortDev == nc.Get_PortDev() &&
		File_Descriptor == nc.Get_File_Descriptor() &&
		Interface_Name == nc.Get_Interface_Name() &&
		TUN_TAP_Script == nc.Get_TUN_TAP_Script() &&
		TUN_TAP_Down_Script == nc.Get_TUN_TAP_Down_Script() &&
		Listen == nc.Get_Listen() &&
		Connect == nc.Get_Connect() &&
		MCast == nc.Get_MCast() &&
		Sock == nc.Get_Sock() &&
		Port == nc.Get_Port() &&
		Group == nc.Get_Group() &&
		Mode == nc.Get_Mode() &&
		File == nc.Get_File() &&
		Len == nc.Get_Len() &&
		_Use_Card_Model == nc.Use_Card_Model() &&
		_Use_MAC == nc.Use_MAC_Address() &&
		_Use_VLAN == nc.Use_VLAN() &&
		_Use_Name == nc.Use_Name() &&
		_Use_Hostname == nc.Use_Hostname() &&
		_Use_File_Descriptor == nc.Use_File_Descriptor() &&
		_Use_Interface_Name == nc.Use_Interface_Name() &&
		_Use_TUN_TAP_Script == nc.Use_TUN_TAP_Script() &&
		_Use_TUN_TAP_Down_Script == nc.Use_TUN_TAP_Down_Script() &&
		_Use_Listen == nc.Use_Listen() &&
		_Use_Connect == nc.Use_Connect() &&
		_Use_MCast == nc.Use_MCast() &&
		_Use_Sock == nc.Use_Sock() &&
		_Use_Port == nc.Use_Port() &&
		_Use_Group == nc.Use_Group() &&
		_Use_Mode == nc.Use_Mode() &&
		_Use_File == nc.Use_File() &&
		_Use_Len == nc.Use_Len() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool VM_Net_Card_Nativ::operator!=( const VM_Net_Card_Nativ &nc ) const
{
	return ! operator ==( nc );
}

QString VM_Net_Card_Nativ::Generate_MAC() const
{
	VM_Net_Card tmp_card;
	tmp_card.Set_Card_Model( Card_Model );
	return tmp_card.Generate_MAC();
}

VM::Network_Mode_Nativ VM_Net_Card_Nativ::Get_Network_Type() const
{
	return Network_Mode;
}

void VM_Net_Card_Nativ::Set_Network_Type( VM::Network_Mode_Nativ mode )
{
	Network_Mode = mode;
}

bool VM_Net_Card_Nativ::Use_Card_Model() const
{
	return _Use_Card_Model;
}

void VM_Net_Card_Nativ::Use_Card_Model( bool use )
{
	_Use_Card_Model = use;
}

const QString &VM_Net_Card_Nativ::Get_Card_Model() const
{
	return Card_Model;
}

void VM_Net_Card_Nativ::Set_Card_Model( const QString &m )
{
	Card_Model = m;
}

bool VM_Net_Card_Nativ::Use_MAC_Address() const
{
	return _Use_MAC;
}

void VM_Net_Card_Nativ::Use_MAC_Address( bool use )
{
	_Use_MAC = use;
}

const QString &VM_Net_Card_Nativ::Get_MAC_Address() const
{
	return MAC;
}

void VM_Net_Card_Nativ::Set_MAC_Address( const QString &ma )
{
	MAC = ma;
}

bool VM_Net_Card_Nativ::Use_VLAN() const
{
	return _Use_VLAN;
}

void VM_Net_Card_Nativ::Use_VLAN( bool use )
{
	_Use_VLAN = use;
}

int VM_Net_Card_Nativ::Get_VLAN() const
{
	return VLAN;
}

void VM_Net_Card_Nativ::Set_VLAN( int vl )
{
	VLAN = vl;
}

bool VM_Net_Card_Nativ::Use_Name() const
{
	return _Use_Name;
}

void VM_Net_Card_Nativ::Use_Name( bool use )
{
	_Use_Name = use;
}

const QString &VM_Net_Card_Nativ::Get_Name() const
{
	return Name;
}

void VM_Net_Card_Nativ::Set_Name( const QString &name )
{
	Name = name;
}

bool VM_Net_Card_Nativ::Use_Hostname() const
{
	return _Use_Hostname;
}

void VM_Net_Card_Nativ::Use_Hostname( bool use )
{
	_Use_Hostname = use;
}

const QString &VM_Net_Card_Nativ::Get_Hostname() const
{
	return Hostname;
}

void VM_Net_Card_Nativ::Set_Hostname( const QString &hostname )
{
	Hostname = hostname;
}

const QString &VM_Net_Card_Nativ::Get_PortDev() const
{
	return PortDev;
}

void VM_Net_Card_Nativ::Set_PortDev( const QString &portDev )
{
	PortDev = portDev;
}

bool VM_Net_Card_Nativ::Use_File_Descriptor() const
{
	return _Use_File_Descriptor;
}

void VM_Net_Card_Nativ::Use_File_Descriptor( bool use )
{
	_Use_File_Descriptor = use;
}

int VM_Net_Card_Nativ::Get_File_Descriptor() const
{
	return File_Descriptor;
}

void VM_Net_Card_Nativ::Set_File_Descriptor( int f )
{
	File_Descriptor = f;
}

bool VM_Net_Card_Nativ::Use_Interface_Name() const
{
	return _Use_Interface_Name;
}

void VM_Net_Card_Nativ::Use_Interface_Name( bool use )
{
	_Use_Interface_Name = use;
}

const QString &VM_Net_Card_Nativ::Get_Interface_Name() const
{
	return Interface_Name;
}

void VM_Net_Card_Nativ::Set_Interface_Name( const QString &n )
{
	Interface_Name = n;
}

bool VM_Net_Card_Nativ::Use_TUN_TAP_Script() const
{
	return _Use_TUN_TAP_Script;
}

void VM_Net_Card_Nativ::Use_TUN_TAP_Script( bool use )
{
	_Use_TUN_TAP_Script = use;
}

const QString &VM_Net_Card_Nativ::Get_TUN_TAP_Script() const
{
	return TUN_TAP_Script;
}

void VM_Net_Card_Nativ::Set_TUN_TAP_Script( const QString &s )
{
	TUN_TAP_Script = s;
}

bool VM_Net_Card_Nativ::Use_TUN_TAP_Down_Script() const
{
	return _Use_TUN_TAP_Down_Script;
}

void VM_Net_Card_Nativ::Use_TUN_TAP_Down_Script( bool use )
{
	_Use_TUN_TAP_Down_Script = use;
}

const QString &VM_Net_Card_Nativ::Get_TUN_TAP_Down_Script() const
{
	return TUN_TAP_Down_Script;
}

void VM_Net_Card_Nativ::Set_TUN_TAP_Down_Script( const QString &s )
{
	TUN_TAP_Down_Script = s;
}

bool VM_Net_Card_Nativ::Use_Listen() const
{
	return _Use_Listen;
}

void VM_Net_Card_Nativ::Use_Listen( bool use )
{
	_Use_Listen = use;
}

const QString &VM_Net_Card_Nativ::Get_Listen() const
{
	return Listen;
}

void VM_Net_Card_Nativ::Set_Listen( const QString &listen )
{
	Listen = listen;
}

bool VM_Net_Card_Nativ::Use_Connect() const
{
	return _Use_Connect;
}

void VM_Net_Card_Nativ::Use_Connect( bool use )
{
	_Use_Connect = use;
}

const QString &VM_Net_Card_Nativ::Get_Connect() const
{
	return Connect;
}

void VM_Net_Card_Nativ::Set_Connect( const QString &con )
{
	Connect = con;
}

bool VM_Net_Card_Nativ::Use_MCast() const
{
	return _Use_MCast;
}

void VM_Net_Card_Nativ::Use_MCast( bool use )
{
	_Use_MCast = use;
}

const QString &VM_Net_Card_Nativ::Get_MCast() const
{
	return MCast;
}

void VM_Net_Card_Nativ::Set_MCast( const QString &mcast )
{
	MCast = mcast;
}

bool VM_Net_Card_Nativ::Use_Sock() const
{
	return _Use_Sock;
}

void VM_Net_Card_Nativ::Use_Sock( bool use )
{
	_Use_Sock = use;
}

const QString &VM_Net_Card_Nativ::Get_Sock() const
{
	return Sock;
}

void VM_Net_Card_Nativ::Set_Sock( const QString &sock )
{
	Sock = sock;
}

bool VM_Net_Card_Nativ::Use_Port() const
{
	return _Use_Port;
}

void VM_Net_Card_Nativ::Use_Port( bool use )
{
	_Use_Port = use;
}

int VM_Net_Card_Nativ::Get_Port() const
{
	return Port;
}

void VM_Net_Card_Nativ::Set_Port( int p )
{
	Port = p;
}

bool VM_Net_Card_Nativ::Use_Group() const
{
	return _Use_Group;
}

void VM_Net_Card_Nativ::Use_Group( bool use )
{
	_Use_Group = use;
}

const QString &VM_Net_Card_Nativ::Get_Group() const
{
	return Group;
}

void VM_Net_Card_Nativ::Set_Group( const QString &group )
{
	Group = group;
}

bool VM_Net_Card_Nativ::Use_Mode() const
{
	return _Use_Mode;
}

void VM_Net_Card_Nativ::Use_Mode( bool use )
{
	_Use_Mode = use;
}

const QString &VM_Net_Card_Nativ::Get_Mode() const
{
	return Mode;
}

void VM_Net_Card_Nativ::Set_Mode( const QString &mode )
{
	Mode = mode;
}

bool VM_Net_Card_Nativ::Use_File() const
{
	return _Use_File;
}

void VM_Net_Card_Nativ::Use_File( bool use )
{
	_Use_File = use;
}

const QString &VM_Net_Card_Nativ::Get_File() const
{
	return File;
}

void VM_Net_Card_Nativ::Set_File( const QString &file )
{
	File = file;
}

bool VM_Net_Card_Nativ::Use_Len() const
{
	return _Use_Len;
}

void VM_Net_Card_Nativ::Use_Len( bool use )
{
	_Use_Len = use;
}

int VM_Net_Card_Nativ::Get_Len() const
{
	return Len;
}

void VM_Net_Card_Nativ::Set_Len( int kb )
{
	Len = kb;
}

//===========================================================================

// VM_Net_Card --------------------------------------------------------------

VM_Net_Card::VM_Net_Card()
{
	Card_Model = "";
	Net_Mode = VM::Net_Mode_None;
	IP = "";
	MAC = "";
	Hostname = "";
	Port = 0;
	VLAN = 0;
	Use_TUN_TAP_Script = true;
	TUN_TAP_Script = "";
	Interface_Name = "";
	File_Descriptor = 0;
}

VM_Net_Card::VM_Net_Card( const VM_Net_Card &nc )
{
	Card_Model = nc.Get_Card_Model();
	Net_Mode = nc.Get_Net_Mode();
	Hostname = nc.Get_Hostname();
	IP = nc.Get_IP_Address();
	MAC = nc.Get_MAC_Address();
	Port = nc.Get_Port();
	VLAN = nc.Get_VLAN();
	Use_TUN_TAP_Script = nc.Get_Use_TUN_TAP_Script();
	TUN_TAP_Script = nc.Get_TUN_TAP_Script();
	Interface_Name = nc.Get_Interface_Name();
	File_Descriptor = nc.Get_File_Descriptor();
}

bool VM_Net_Card::operator==( const VM_Net_Card &nc ) const
{
	if( Card_Model == nc.Get_Card_Model() &&
		Net_Mode == nc.Get_Net_Mode() &&
		Hostname == nc.Get_Hostname() &&
		IP == nc.Get_IP_Address() &&
		MAC == nc.Get_MAC_Address() &&
		Port == nc.Get_Port() &&
		VLAN == nc.Get_VLAN() &&
		Use_TUN_TAP_Script == nc.Get_Use_TUN_TAP_Script() &&
		TUN_TAP_Script == nc.Get_TUN_TAP_Script() &&
		Interface_Name == nc.Get_Interface_Name() &&
		File_Descriptor == nc.Get_File_Descriptor() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool VM_Net_Card::operator!=( const VM_Net_Card &nc ) const
{
	return ! ( operator==(nc) );
}

QString VM_Net_Card::Generate_MAC() const
{
	QSettings settings;
	QString mode = settings.value( "MAC_Generation_Mode", "" ).toString();
	
	QString nmac = QUuid::createUuid(); // Random HEX for MAC
	
	if( mode == "Random" )
	{
		nmac = nmac.mid( 25, 10 );
		nmac.prepend( "00" ); // Two Zero First, For Valid MAC
	}
	else if( mode == "QEMU_Segment" )
	{
		nmac = nmac.mid( 25, 6 );
		nmac.prepend( "525400" ); // QEMU Segment
	}
	else // Default. Valid Card MAC
	{
		nmac = nmac.mid( 25, 6 );
		
		// MAC Data Base
		QStringList novell_mac;
		novell_mac << "00001B"
				   << "0000D8";
		
		QStringList sun_mac;
		sun_mac << "00015D"
				<< "0003BA"
				<< "000782"
				<< "00104F"
				<< "0010E0";
		
		QStringList amd_mac;
		amd_mac << "00001A"
				<< "000C87";
		
		QStringList smc_mac;
		smc_mac << "0004E2"
				<< "000BC5"
				<< "0013F7"
				<< "00222D"
				<< "0023C6";
		
		QStringList motorola_mac;
		motorola_mac << "00080E"
					 << "000A28"
					 << "000B06"
					 << "000CE5"
					 << "000E5C";
		
		QStringList intel_mac;
		intel_mac << "001111"
				  << "0012F0"
				  << "0013CE"
				  << "0013E8"
				  << "001676";
		
		QString model = Get_Card_Model();
		
		if( model == "ne2k_pci" )
		{
			int rm = Get_Random( 0,1 );
			if( rm >= 0 ) nmac.prepend( novell_mac[rm] );
		}
		else if( model == "ne2k_isa" )
		{
			int rm = Get_Random( 0,1 );
			if( rm >= 0 ) nmac.prepend( novell_mac[rm] );
		}
		else if( model == "i82551" )
		{
			int rm = Get_Random( 0,4 );
			if( rm >= 0 ) nmac.prepend( intel_mac[rm] );
		}
		else if( model == "i82557b" )
		{
			int rm = Get_Random( 0,4 );
			if( rm >= 0 ) nmac.prepend( intel_mac[rm] );
		}
		else if( model == "i82559er" )
		{
			int rm = Get_Random( 0,4 );
			if( rm >= 0 ) nmac.prepend( intel_mac[rm] );
		}
		else if( model == "pcnet" )
		{
			int rm = Get_Random( 0,1 );
			if( rm >= 0 ) nmac.prepend( amd_mac[rm] );
		}
		else if( model == "rtl8139" )
		{
			nmac.prepend( "00E04C" );
		}
		else if( model == "smc91c111" )
		{
			int rm = Get_Random( 0,4 );
			if( rm >= 0 ) nmac.prepend( smc_mac[rm] );
		}
		else if( model == "mcf_fec" )
		{
			int rm = Get_Random( 0,4 );
			if( rm >= 0 ) nmac.prepend( motorola_mac[rm] );
		}
		else if( model == "lance" )
		{
			int rm = Get_Random( 0,4 );
			if( rm >= 0 ) nmac.prepend( sun_mac[rm] );
		}
		else if( model == "" ) // Default 00 + Random
		{
			nmac = QUuid::createUuid(); // Random HEX for MAC
			nmac = nmac.mid( 25, 10 );
			nmac.prepend( "00" ); // Two Zero First, For Valid MAC
		}
		else
		{
			AQWarning( "QString VM_Net_Card::Generate_MAC() const",
					   "Net Card Model Invalid!" );
		}
		
		// MAC Finded?
		if( nmac.count() != 12 )
		{
			AQError( "QString VM_Net_Card::Generate_MAC() const",
					 "Cannot Get MAC for This Card!" );
			
			nmac = QUuid::createUuid(); // Random HEX for MAC
			nmac = nmac.mid( 25, 6 );
			nmac.prepend( "525400" ); // QEMU Segment
		}
	}
	
	// Adding separators :
	for( int nx = 2; nx < nmac.count(); nx+=3 )
	{
		nmac = nmac.insert( nx, ":" );
	}
	
	nmac = nmac.toLower();
	return nmac;
}

const QString &VM_Net_Card::Get_Card_Model() const
{
	return Card_Model;
}

void VM_Net_Card::Set_Card_Model( const QString &m )
{
	Card_Model = m;
}

VM::Network_Mode VM_Net_Card::Get_Net_Mode() const
{
	return Net_Mode;
}

void VM_Net_Card::Set_Net_Mode( VM::Network_Mode mode )
{
	Net_Mode = mode;
}

const QString &VM_Net_Card::Get_Hostname() const
{
	return Hostname;
}

void VM_Net_Card::Set_Hostname( const QString &hn )
{
	Hostname = hn;
}

const QString &VM_Net_Card::Get_IP_Address() const
{
	return IP;
}

void VM_Net_Card::Set_IP_Address( const QString &ia )
{
	IP = ia;
}

const QString &VM_Net_Card::Get_MAC_Address() const
{
	return MAC;
}

void VM_Net_Card::Set_MAC_Address( const QString &ma )
{
	MAC = ma;
}

int VM_Net_Card::Get_Port() const
{
	return Port;
}

void VM_Net_Card::Set_Port( int p )
{
	Port = p;
}

int VM_Net_Card::Get_VLAN() const
{
	return VLAN;
}

void VM_Net_Card::Set_VLAN( int vl )
{
	VLAN = vl;
}

bool VM_Net_Card::Get_Use_TUN_TAP_Script() const
{
	return Use_TUN_TAP_Script;
}

void VM_Net_Card::Set_Use_TUN_TAP_Script( bool use )
{
	Use_TUN_TAP_Script = use;
}

const QString &VM_Net_Card::Get_TUN_TAP_Script() const
{
	return TUN_TAP_Script;
}

void VM_Net_Card::Set_TUN_TAP_Script( const QString &s )
{
	TUN_TAP_Script = s;
}

const QString &VM_Net_Card::Get_Interface_Name() const
{
	return Interface_Name;
}

void VM_Net_Card::Set_Interface_Name( const QString &n )
{
	Interface_Name = n;
}

int VM_Net_Card::Get_File_Descriptor() const
{
	return File_Descriptor;
}

void VM_Net_Card::Set_File_Descriptor( int f )
{
	File_Descriptor = f;
}

//===========================================================================

// VM_Redirections Class ----------------------------------------------------

VM_Redirection::VM_Redirection()
{
	Protocol = "NOT";
	Host_Port = 1;
	Guest_IP = "0.0.0.0";
	Guest_Port = 1;
}

VM_Redirection::VM_Redirection( const VM_Redirection &r )
{
	Protocol = r.Get_Protocol();
	Host_Port = r.Get_Host_Port();
	Guest_IP = r.Get_Guest_IP();
	Guest_Port = r.Get_Guest_Port();
}

bool VM_Redirection::operator==( const VM_Redirection &r ) const
{
	if( Protocol == r.Get_Protocol() &&
		Host_Port == r.Get_Host_Port() &&
		Guest_IP == r.Get_Guest_IP() &&
		Guest_Port == r.Get_Guest_Port() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool VM_Redirection::operator!=( const VM_Redirection &r ) const
{
	return ! ( operator==(r) );
}

const QString &VM_Redirection::Get_Protocol() const
{
	return Protocol;
}

void VM_Redirection::Set_Protocol( const QString &p )
{
	Protocol = p;
}

int VM_Redirection::Get_Host_Port() const
{
	return Host_Port;
}

void VM_Redirection::Set_Host_Port( int hp )
{
	Host_Port = hp;
}

const QString &VM_Redirection::Get_Guest_IP() const
{
	return Guest_IP;
}

void VM_Redirection::Set_Guest_IP( const QString &ip )
{
	Guest_IP = ip;
}

int VM_Redirection::Get_Guest_Port() const
{
	return Guest_Port;
}

void VM_Redirection::Set_Guest_Port( int p )
{
	Guest_Port = p;
}

//===========================================================================

// VM_Port ------------------------------------------------------------------

VM_Port::VM_Port()
{
	Port_Redir = VM::PR_Default;
	Param_Line = "";
}

VM_Port::VM_Port( const VM_Port &p )
{
	Port_Redir = p.Get_Port_Redirection();
	Param_Line = p.Get_Parametrs_Line();
}

bool VM_Port::operator==( const VM_Port &p ) const
{
	if( Port_Redir == p.Get_Port_Redirection() &&
		Param_Line == p.Get_Parametrs_Line() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool VM_Port::operator!=( const VM_Port &p ) const
{
	return ! ( operator==(p) );
}

VM::Port_Redirection VM_Port::Get_Port_Redirection() const
{
	return Port_Redir;
}

void VM_Port::Set_Port_Redirection( VM::Port_Redirection pr )
{
	Port_Redir = pr;
}

const QString &VM_Port::Get_Parametrs_Line() const
{
	return Param_Line;
}

void VM_Port::Set_Parametrs_Line( const QString &pl )
{
	Param_Line = pl;
}

//===========================================================================

// VM_USB Class -------------------------------------------------------------

VM_USB::VM_USB()
{
	Use_Host_Device = true;
	Manufacturer_Name = "";
	Product_Name = "";
	Vendor_ID = "";
	Product_ID = "";
	BusAddr = "";
	USB_Keyboard = USB_Tablet = USB_WacomTablet = USB_Braille = false;
	USB_Mouse = true;
	Speed = 0;
}

VM_USB::VM_USB( const VM_USB &u )
{
	Use_Host_Device = u.Get_Use_Host_Device();
	Manufacturer_Name = u.Get_Manufacturer_Name();
	Product_Name = u.Get_Product_Name();
	Vendor_ID = u.Get_Vendor_ID();
	Product_ID = u.Get_Product_ID();
	BusAddr = u.Get_BusAddr();
	u.Get_USB_QEMU_Devices( USB_Keyboard, USB_Mouse, USB_Tablet, USB_WacomTablet, USB_Braille );
	Speed = u.Get_Speed();
}

bool VM_USB::operator==( const VM_USB &u ) const
{
	bool k, m, t, wt, b;
	k = m = t = wt = b = false;
	
	u.Get_USB_QEMU_Devices( k, m, t, wt, b );
	
	if( Use_Host_Device == u.Get_Use_Host_Device() &&
		Manufacturer_Name == u.Get_Manufacturer_Name() &&
		Product_Name == u.Get_Product_Name() &&
		Vendor_ID == u.Get_Vendor_ID() &&
		Product_ID == u.Get_Product_ID() &&
		USB_Keyboard == k &&
		USB_Mouse == m &&
		USB_Tablet == t &&
		USB_WacomTablet == wt &&
		USB_Braille == b &&
		Speed == u.Get_Speed() )
	{
		return true;
	}
	else return false;
}

bool VM_USB::operator!=( const VM_USB &u ) const
{
	return ! ( operator==(u) );
}

bool VM_USB::Get_Use_Host_Device() const
{
	return Use_Host_Device;
}

void VM_USB::Set_Use_Host_Device( bool use )
{
	Use_Host_Device = use;
}

const QString &VM_USB::Get_Manufacturer_Name() const
{
	return Manufacturer_Name;
}

void VM_USB::Set_Manufacturer_Name( const QString &name )
{
	Manufacturer_Name = name;
}

const QString &VM_USB::Get_Product_Name() const
{
	return Product_Name;
}

void VM_USB::Set_Product_Name( const QString &name )
{
	Product_Name = name;
}

const QString &VM_USB::Get_Vendor_ID() const
{
	return Vendor_ID;
}

void VM_USB::Set_Vendor_ID( const QString &dev )
{
	Vendor_ID = dev;
}

const QString &VM_USB::Get_Product_ID() const
{
	return Product_ID;
}

void VM_USB::Set_Product_ID( const QString &dev )
{
	Product_ID = dev;
}

QString VM_USB::Get_ID_Line() const
{
	return Vendor_ID + ":" + Product_ID;
}

void VM_USB::Set_ID_Line( const QString &line )
{
	QRegExp id_line = QRegExp( "([\\dabcdef]{4})[:]{1}([\\dabcdef]{4})" );
	
	if( ! id_line.exactMatch(line) )
	{
		AQError( "void VM_USB::Set_ID_Line( const QString &line )",
				 "Cannot Match QRegExp!" );
		return;
	}
	
	QStringList id_str = id_line.capturedTexts();
	
	Vendor_ID = id_str[1];
	Product_ID = id_str[2];
}

QString VM_USB::Get_BusAddr() const
{
	return BusAddr;
}

void VM_USB::Set_BusAddr( const QString &busAddr )
{
	BusAddr = busAddr;
}

void VM_USB::Get_USB_QEMU_Devices( bool &keyboard, bool &mouse, bool &tablet, bool &wacomTablet, bool &braille ) const
{
	keyboard = USB_Keyboard;
	mouse = USB_Mouse;
	tablet = USB_Tablet;
	wacomTablet = USB_WacomTablet;
	braille = USB_Braille;
}

void VM_USB::Set_USB_QEMU_Devices( bool keyboard, bool mouse, bool tablet, bool wacomTablet, bool braille )
{
	USB_Keyboard = keyboard;
	USB_Mouse = mouse;
	USB_Tablet = tablet;
	USB_WacomTablet = wacomTablet;
	USB_Braille = braille;
}

int VM_USB::Get_Speed() const
{
	return Speed;
}

void VM_USB::Set_Speed( int speed )
{
	Speed = speed;
}

//===========================================================================

// VM_Init_Graphic_Mode -----------------------------------------------------

VM_Init_Graphic_Mode::VM_Init_Graphic_Mode()
{
	Enabled = false;
	Width = 800;
	Height = 600;
	Depth = 16;
}

VM_Init_Graphic_Mode::VM_Init_Graphic_Mode( const VM_Init_Graphic_Mode &mode )
{
	Enabled = mode.Get_Enabled();
	Width = mode.Get_Width();
	Height = mode.Get_Height();
	Depth = mode.Get_Depth();
}

bool VM_Init_Graphic_Mode::operator==( const VM_Init_Graphic_Mode &mode ) const
{
	if( Enabled == mode.Get_Enabled() &&
		Width == mode.Get_Width() &&
		Height == mode.Get_Height() &&
		Depth == mode.Get_Depth() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool VM_Init_Graphic_Mode::operator!=( const VM_Init_Graphic_Mode &mode ) const
{
	return ! ( operator==(mode) );
}

bool VM_Init_Graphic_Mode::Get_Enabled() const
{
	return Enabled;
}

void VM_Init_Graphic_Mode::Set_Enabled( bool use )
{
	Enabled = use;
}

int VM_Init_Graphic_Mode::Get_Width() const
{
	return Width;
}

void VM_Init_Graphic_Mode::Set_Width( int w )
{
	Width = w;
}

int VM_Init_Graphic_Mode::Get_Height() const
{
	return Height;
}

void VM_Init_Graphic_Mode::Set_Height( int h )
{
	Height = h;
}

int VM_Init_Graphic_Mode::Get_Depth() const
{
	return Depth;
}

void VM_Init_Graphic_Mode::Set_Depth( int d )
{
	Depth = d;
}

//===========================================================================
