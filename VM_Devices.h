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

#ifndef VM_DEVICES_H
#define VM_DEVICES_H

#include <QObject>
#include <QList>
#include <QString>
#include <QFlags>
#include <QStringList>
#include <QMap>

class VM
{
	public:
		// Emulators Version
		enum QEMU_Version { QEMU_Old, QEMU_0_9_0, QEMU_0_9_1, QEMU_0_10, QEMU_New };
		enum KVM_Version { KVM_Old, KVM_7X, KVM_8X, KVM_New };
		
		// Virtual Machine State
		enum VM_State { VMS_Running, VMS_Power_Off, VMS_Pause, VMS_Saved, VMS_In_Error };
		
		// x86 audio cars ( one or more )
		class Sound_Cards
		{
			public:
				bool Audio_sb16;
				bool Audio_es1370;
				bool Audio_Adlib;
				bool Audio_PC_Speaker;
				bool Audio_GUS;
				bool Audio_AC97;
				
				Sound_Cards()
				{
					Audio_sb16 = Audio_es1370 = Audio_Adlib = Audio_PC_Speaker = Audio_GUS = Audio_AC97 = false;
				}
				
				bool operator==( const Sound_Cards &v ) const
				{
					if( Audio_sb16 == v.Audio_sb16 &&
						Audio_es1370 == v.Audio_es1370 &&
						Audio_Adlib == v.Audio_Adlib &&
						Audio_PC_Speaker == v.Audio_PC_Speaker &&
						Audio_GUS == v.Audio_GUS &&
						Audio_AC97 == v.Audio_AC97 )
					{
						return true;
					}
					else
					{
						return false;
					}
				}
				
				bool operator!=( const Sound_Cards &v ) const
				{
					return ! operator==(v);
				}
				
		};
		
		// KQEMU module mode
		enum Acseleration_Mode { KQEMU_Disabled, KQEMU_Enabled, KQEMU_Full, KQEMU_Default };
		
		// Boot Prioritet
		enum Boot_Device { Boot_From_FDD, Boot_From_CDROM, Boot_From_HDD,
						   Boot_From_Network, Boot_None };
		
		// Kilobyte, Megabyte, Gigabyte
		enum Size_Suffix { Size_Suf_Kb, Size_Suf_Mb, Size_Suf_Gb };
		
		// Size for Storage Devices
		struct Device_Size
		{
			double Size;
			Size_Suffix Suffix;
		};
		
		// All HDD Image Disk Information
		struct Disk_Info
		{
			QString Image_File_Name, Disk_Format;
			VM::Device_Size Virtual_Size, Disk_Size;
			int Cluster_Size;
		};
		
		// Storage Device Interfaces
		enum Device_Interface { DI_IDE, DI_SCSI, DI_SD, DI_MTD, DI_Floppy, DI_PFlash };
		
		// Storage Device Media
		enum Device_Media { DM_Disk, DM_CD_ROM };
		
		// QEMU Serial Port Redirections
		enum Port_Redirection { PR_Default, PR_vc, PR_pty, PR_none, PR_null, PR_dev,
								PR_host_port, PR_file, PR_stdio, PR_pipe, PR_udp,
								PR_tcp, PR_telnet, PR_unix, PR_com, PR_msmouse, PR_mon, PR_braille };
		
		// QEMU/KVM Network Modes
		enum Network_Mode { Net_Mode_None, Net_Mode_Usermode, Net_Mode_Tuntap,
							Net_Mode_Tuntapfd, Net_Mode_Tcplisten, Net_Mode_Tcpfd,
							Net_Mode_Tcpconnect, Net_Mode_Multicast, Net_Mode_Multicastfd,
							Net_Mode_Dummy };
		
		// New Network Modes
		enum Network_Mode_Nativ { Net_Mode_Nativ_NIC, Net_Mode_Nativ_User, Net_Mode_Nativ_Chanel,
								  Net_Mode_Nativ_TAP, Net_Mode_Nativ_Socket, Net_Mode_Nativ_MulticastSocket,
		  						  Net_Mode_Nativ_VDE, Net_Mode_Nativ_Dump };
};

class Device_Map
{
	public:
		Device_Map() {};
		Device_Map( QString cp, QString nm ) : Caption(cp), QEMU_Name(nm) {};
		
		QString Caption;
		QString QEMU_Name;
};

class Emulator
{
	public:
		Emulator();
		Emulator( const Emulator &emul );
		
		bool operator==( const Emulator &emul ) const;
		bool operator!=( const Emulator &emul ) const;
		
		const QString& Get_Type() const;
		void Set_Type( const QString &type );
		
		const QString& Get_Name() const;
		void Set_Name( const QString &name );
		
		const QString& Get_Default() const;
		void Set_Default( const QString &use );
		
		const QString& Get_Path() const;
		void Set_Path( const QString &path );
		
		const QString& Get_Check_QEMU_Version() const;
		void Set_Check_QEMU_Version( const QString &check );
		
		const QString& Get_QEMU_Version() const;
		void Set_QEMU_Version( const QString &ver );
		
		const QString& Get_Check_KVM_Version() const;
		void Set_Check_KVM_Version( const QString &check );
		
		const QString& Get_KVM_Version() const;
		void Set_KVM_Version( const QString &ver );
		
		const QString& Get_Check_Available_Audio_Cards() const;
		void Set_Check_Available_Audio_Cards( const QString &check );
		
		const QMap<QString, QString>& Get_Binary_Files() const;
		void Set_Binary_Files( const QMap<QString, QString> &files );
		
	private:
		QString Type, Name, Default, Path, Check_QEMU_Version, QEMU_Version,
				Check_KVM_Version, KVM_Version, Check_Available_Audio_Cards;
		QMap<QString, QString> Binary_Files;
};

class Averable_Devices
{
	public:
		// QEMU System
		Device_Map System;
		
		// QEMU CPU
		QList<Device_Map> CPU_List;
		
		// QEMU Machines
		QList<Device_Map> Machine_List;
		
		// QEMU Network Card
		QList<Device_Map> Network_Card_List;
		
		// QEMU Audio Card
		VM::Sound_Cards Audio_Card_List;
		
		// QEMU Video Card
		QList<Device_Map> Video_Card_List;
		
		// Platform Specific Options
		unsigned short PSO_SMP_Count;
		bool PSO_Initial_Graphic_Mode;
		bool PSO_No_FB_Boot_Check;
		bool PSO_Win2K_Hack;
		bool PSO_Kernel_KQEMU;
		bool PSO_No_ACPI;
		bool PSO_PROM_ENV;
		bool PSO_KVM;
		bool PSO_RTC_TD_Hack;
};

// Storage Device (For QEMU 0.9.1 Device Style)
class VM_Storage_Device
{
	public:
		VM_Storage_Device();
		VM_Storage_Device( const VM_Storage_Device &sd );
		
		QString Get_QEMU_Device_Name() const;
		
		bool operator==( const VM_Storage_Device &sd ) const;
		bool operator!=( const VM_Storage_Device &sd ) const;
		
		bool Use_File_Path() const;
		void Use_File_Path( bool use );
		
		const QString &Get_File_Path() const;
		void Set_File_Path( const QString &path );
		
		bool Use_Interface() const;
		void Use_Interface( bool use );
		
		VM::Device_Interface Get_Interface() const;
		void Set_Interface( VM::Device_Interface di );
		
		bool Use_Bus_Unit() const;
		void Use_Bus_Unit( bool use );
		
		int Get_Bus() const;
		void Set_Bus( int bus );
		
		int Get_Unit() const;
		void Set_Unit( int unit );
		
		bool Use_Index() const;
		void Use_Index( bool use );
		
		int Get_Index() const;
		void Set_Index( int index );
		
		bool Use_Media() const;
		void Use_Media( bool use );
		
		VM::Device_Media Get_Media() const;
		void Set_Media( VM::Device_Media media );
		
		bool Use_hdachs() const;
		void Use_hdachs( bool use );
		
		qulonglong Get_Cyls() const;
		void Set_Cyls( qulonglong cyls );
		
		qulonglong Get_Heads() const;
		void Set_Heads( qulonglong heads );
		
		qulonglong Get_Secs() const;
		void Set_Secs( qulonglong secs );
		
		qulonglong Get_Trans() const;
		void Set_Trans( qulonglong trans );
		
		bool Get_Snapshot() const;
		void Set_Snapshot( bool snapshot );
		
		bool Get_Cache() const;
		void Set_Cache( bool cache );
		
	private:
		bool UFile_Path;
		QString File_Path;
		
		bool UInterface;
		VM::Device_Interface Interface; // ide, scsi, sd, mtd, floppy, pflash
		
		bool UBus_Unit;
		int Bus, Unit; // For SCSI
		
		bool UIndex;
		int Index; // For IDE, Floppy
		
		bool UMedia;
		VM::Device_Media Media; // disk or cdrom
		
		bool Uhdachs;
		int Cyls, Heads, Secs, Trans; // For -hdachs
		
		bool Snapshot;
		bool Cache;
};

// Virtual Machine Floppy Device
class VM_Floppy
{
	public:
		VM_Floppy();
		VM_Floppy( const VM_Floppy &fd );
		VM_Floppy( bool enabled, const QString &host_path,
				   const QString &image_path, bool host_device );
		
		bool operator==( const VM_Floppy &v ) const;
		bool operator!=( const VM_Floppy &v ) const;
		
		bool Get_Enabled() const;
		void Set_Enabled( bool enabled );
		
		const QString &Get_Host_File_Name() const;
		void Set_Host_File_Name( const QString &file_path );
		
		const QString &Get_Image_File_Name() const;
		void Set_Image_File_Name( const QString &file_path );
		
		bool Get_Host_Device() const;
		void Set_Host_Device( bool enabled );
		
	private:
		bool Enabled;
		QString Host_File_Name;
		QString Image_File_Name;
		bool Host_Device;
};

// Virtual Machine CD-ROM Device
class VM_CDROM
{
	public:
		VM_CDROM();
		VM_CDROM( const VM_CDROM &cd );
		VM_CDROM( bool enabled, const QString &host_path,
				  const QString &image_path, bool host_device );
		
		bool operator==( const VM_CDROM& v ) const;
		bool operator!=( const VM_CDROM& v ) const;
		
		bool Get_Enabled() const;
		void Set_Enabled( bool enabled );
		
		const QString &Get_Host_File_Name() const;
		void Set_Host_File_Name( const QString &file_path );
		
		const QString &Get_Image_File_Name() const;
		void Set_Image_File_Name( const QString &file_path );
		
		bool Get_Host_Device() const;
		void Set_Host_Device( bool enabled );
		
	private:
		bool Enabled;
		QString Host_File_Name;
		QString Image_File_Name;
		bool Host_Device;
};

// Virtual Machine Hard Drive Device
class VM_HDD
{
	public:
		VM_HDD();
		VM_HDD( const VM_HDD &hd );
		VM_HDD( bool enabled, const QString &im_pach );
		
		bool operator==( const VM_HDD &v ) const;
		bool operator!=( const VM_HDD &v ) const;
		
		bool Get_Enabled() const;
		void Set_Enabled( bool enabled );
		
		const QString &Get_Image_File_Name() const;
		void Set_Image_File_Name( const QString &file_name );
		
		void Set_Disk_Info( VM::Disk_Info info );
		const QString &Get_Image_Format() const;
		const VM::Device_Size &Get_Virtual_Size() const;
		const VM::Device_Size &Get_Disk_Size() const;
		int Get_Cluster_Size() const;
		
		double Get_Virtual_Size_in_GB() const;
		QString Get_Complete_Virtual_Size() const;
		QString Get_Complete_Disk_Size() const;
		
		void Set_Virtual_Size( const VM::Device_Size &size );
		void Set_Image_Format( const QString &format );
		
		QString Size_Suffix_to_String( VM::Size_Suffix suf ) const;
		VM::Device_Size String_to_Device_Size( const QString &size ) const;
	
	private:
		
		bool Enabled;
		QString Image_File_Name, Disk_Format;
		VM::Device_Size Virtual_Size, Disk_Size;
		int Cluster_Size;
};

// VM Snapshot
class VM_Snapshot
{
	public:
		VM_Snapshot();
		VM_Snapshot( const VM_Snapshot &s );
		
		bool operator==( const VM_Snapshot &s ) const;
		bool operator!=( const VM_Snapshot &s ) const;
		
		const QString &Get_Tag() const;
		void Set_Tag( const QString &tag );
		
		const QString &Get_ID() const;
		void Set_ID( const QString &id );
		
		const QString &Get_Size() const;
		void Set_Size( const QString &size );
		
		const QString &Get_Date() const;
		void Set_Date( const QString &date );
		
		const QString &Get_VM_Clock() const;
		void Set_VM_Clock( const QString &vm_clock );
		
		const QString &Get_Name() const;
		void Set_Name( const QString &name );
		
		const QString &Get_Description() const;
		void Set_Description( const QString &desc );
		
	private:
		QString Tag; // Snapshot Name
		QString ID;
		QString Size;
		QString Date;
		QString VM_Clock;
		
		QString Name; // This name overwrite Tag in GUI
		QString Description;
};

// Network Card
class VM_Net_Card
{
	public:
		VM_Net_Card();
		VM_Net_Card( const VM_Net_Card &nc );
		
		bool operator==( const VM_Net_Card &nc ) const;
		bool operator!=( const VM_Net_Card &nc ) const;
		
		QString Generate_MAC() const;
		
		const QString &Get_Card_Model() const;
		void Set_Card_Model( const QString &m );
		
		VM::Network_Mode Get_Net_Mode() const;
		void Set_Net_Mode( VM::Network_Mode mode );
		
		const QString &Get_Hostname() const;
		void Set_Hostname( const QString &hn );
		
		const QString &Get_IP_Address() const;
		void Set_IP_Address( const QString &ia );
		
		const QString &Get_MAC_Address() const;
		void Set_MAC_Address( const QString &ma );
		
		int Get_Port() const;
		void Set_Port( int p );
		
		int Get_VLAN() const;
		void Set_VLAN( int vl );
		
		bool Get_Use_TUN_TAP_Script() const;
		void Set_Use_TUN_TAP_Script( bool use );
		
		const QString &Get_TUN_TAP_Script() const;
		void Set_TUN_TAP_Script( const QString &s );
		
		const QString &Get_Interface_Name() const;
		void Set_Interface_Name( const QString &n );
		
		int Get_File_Descriptor() const;
		void Set_File_Descriptor( int f );
		
	private:
		QString Card_Model;
		VM::Network_Mode Net_Mode;
		QString Hostname;
		QString IP;
		QString MAC;
		int Port;
		int VLAN;
		bool Use_TUN_TAP_Script;
		QString TUN_TAP_Script;
		QString Interface_Name;
		int File_Descriptor;
};

// Network Card Nativ
class VM_Net_Card_Nativ
{
	public:
		VM_Net_Card_Nativ();
		VM_Net_Card_Nativ( const VM_Net_Card_Nativ &nc );
		
		bool operator==( const VM_Net_Card_Nativ &nc ) const;
		bool operator!=( const VM_Net_Card_Nativ &nc ) const;
		
		QString Generate_MAC() const;
		
		VM::Network_Mode_Nativ Get_Network_Type() const;
		void Set_Network_Type( VM::Network_Mode_Nativ mode );
		
		bool Use_Card_Model() const;
		void Use_Card_Model( bool use );
		const QString &Get_Card_Model() const;
		void Set_Card_Model( const QString &m );
		
		bool Use_MAC_Address() const;
		void Use_MAC_Address( bool use );
		const QString &Get_MAC_Address() const;
		void Set_MAC_Address( const QString &ma );
		
		bool Use_VLAN() const;
		void Use_VLAN( bool use );
		int Get_VLAN() const;
		void Set_VLAN( int vl );
		
		bool Use_Name() const;
		void Use_Name( bool use );
		const QString &Get_Name() const;
		void Set_Name( const QString &name );
		
		bool Use_Hostname() const;
		void Use_Hostname( bool use );
		const QString &Get_Hostname() const;
		void Set_Hostname( const QString &hostname );
		
		const QString &Get_PortDev() const;
		void Set_PortDev( const QString &portDev );
		
		bool Use_File_Descriptor() const;
		void Use_File_Descriptor( bool use );
		int Get_File_Descriptor() const;
		void Set_File_Descriptor( int f );
		
		bool Use_Interface_Name() const;
		void Use_Interface_Name( bool use );
		const QString &Get_Interface_Name() const;
		void Set_Interface_Name( const QString &n );
		
		bool Use_TUN_TAP_Script() const;
		void Use_TUN_TAP_Script( bool use );
		const QString &Get_TUN_TAP_Script() const;
		void Set_TUN_TAP_Script( const QString &s );
		
		bool Use_TUN_TAP_Down_Script() const;
		void Use_TUN_TAP_Down_Script( bool use );
		const QString &Get_TUN_TAP_Down_Script() const;
		void Set_TUN_TAP_Down_Script( const QString &s );
		
		bool Use_Listen() const;
		void Use_Listen( bool use );
		const QString &Get_Listen() const;
		void Set_Listen( const QString &listen );
		
		bool Use_Connect() const;
		void Use_Connect( bool use );
		const QString &Get_Connect() const;
		void Set_Connect( const QString &con );
		
		bool Use_MCast() const;
		void Use_MCast( bool use );
		const QString &Get_MCast() const;
		void Set_MCast( const QString &mcast );
		
		bool Use_Sock() const;
		void Use_Sock( bool use );
		const QString &Get_Sock() const;
		void Set_Sock( const QString &sock );
		
		bool Use_Port() const;
		void Use_Port( bool use );
		int Get_Port() const;
		void Set_Port( int p );
		
		bool Use_Group() const;
		void Use_Group( bool use );
		const QString &Get_Group() const;
		void Set_Group( const QString &group );
		
		bool Use_Mode() const;
		void Use_Mode( bool use );
		const QString &Get_Mode() const;
		void Set_Mode( const QString &mode );
		
		bool Use_File() const;
		void Use_File( bool use );
		const QString &Get_File() const;
		void Set_File( const QString &file );
		
		bool Use_Len() const;
		void Use_Len( bool use );
		int Get_Len() const;
		void Set_Len( int kb );
		
	private:
		VM::Network_Mode_Nativ Network_Mode;
		QString Card_Model;
		QString MAC;
		int VLAN;
		QString Name;
		QString Hostname;
		QString PortDev;
		int File_Descriptor;
		QString Interface_Name;
		QString TUN_TAP_Script;
		QString TUN_TAP_Down_Script;
		QString Listen;
		QString Connect;
		QString MCast;
		QString Sock;
		int Port;
		QString Group;
		QString Mode;
		QString File;
		int Len;
		
		bool _Use_Card_Model, _Use_MAC, _Use_VLAN, _Use_Name, _Use_Hostname,
			 _Use_File_Descriptor, _Use_Interface_Name, _Use_TUN_TAP_Script,
			 _Use_TUN_TAP_Down_Script, _Use_Listen, _Use_Connect, _Use_MCast,
			 _Use_Sock, _Use_Port, _Use_Group, _Use_Mode, _Use_File, _Use_Len;
};

class VM_Redirection
{
	public:
		VM_Redirection();
		VM_Redirection( const VM_Redirection &r );
		
		bool operator==( const VM_Redirection &r ) const;
		bool operator!=( const VM_Redirection &r ) const;
		
		const QString &Get_Protocol() const;
		void Set_Protocol( const QString &p );
		
		int Get_Host_Port() const;
		void Set_Host_Port( int hp );
		
		const QString &Get_Guest_IP() const;
		void Set_Guest_IP( const QString &ip );
		
		int Get_Guest_Port() const;
		void Set_Guest_Port( int p );
		
	private:
		QString Protocol;
		int Host_Port;
		QString Guest_IP;
		int Guest_Port;
};

// Port (Serial or Parallel)
class VM_Port
{
	public:
		VM_Port();
		VM_Port( const VM_Port &p );
		
		bool operator==( const VM_Port &p ) const;
		bool operator!=( const VM_Port &p ) const;
		
		VM::Port_Redirection Get_Port_Redirection() const;
		void Set_Port_Redirection( VM::Port_Redirection pr );
		
		const QString &Get_Parametrs_Line() const;
		void Set_Parametrs_Line( const QString &pl );
		
	private:
		VM::Port_Redirection Port_Redir;
		QString Param_Line;
};

// USB Port
class VM_USB
{
	public:
		VM_USB();
		VM_USB( const VM_USB &u );
		
		bool operator==( const VM_USB &u ) const;
		bool operator!=( const VM_USB &u ) const;
		
		bool Get_Use_Host_Device() const;
		void Set_Use_Host_Device( bool use );
		
		const QString &Get_Manufacturer_Name() const;
		void Set_Manufacturer_Name( const QString &name );
		
		const QString &Get_Product_Name() const;
		void Set_Product_Name( const QString &name );
		
		const QString &Get_Vendor_ID() const;
		void Set_Vendor_ID( const QString &dev );
		
		const QString &Get_Product_ID() const;
		void Set_Product_ID( const QString &dev );
		
		QString Get_ID_Line() const;
		void Set_ID_Line( const QString &line );
		
		QString Get_BusAddr() const;
		void Set_BusAddr( const QString &busAddr );
		
		void Get_USB_QEMU_Devices( bool &keyboard, bool &mouse, bool &tablet, bool &wacomTablet, bool &braille ) const;
		void Set_USB_QEMU_Devices( bool keyboard, bool mouse, bool tablet, bool wacomTablet, bool braille );
		
		int Get_Speed() const;
		void Set_Speed( int speed );
		
	private:
		bool Use_Host_Device;
		QString Manufacturer_Name;
		QString Product_Name;
		QString Vendor_ID;
		QString Product_ID;
		QString BusAddr;
		bool USB_Keyboard, USB_Mouse, USB_Tablet, USB_WacomTablet, USB_Braille;
		
		int Speed;
};

// Initial Graphical Mode
class VM_Init_Graphic_Mode
{
	public:
		VM_Init_Graphic_Mode();
		VM_Init_Graphic_Mode( const VM_Init_Graphic_Mode &mode );
		
		bool operator==( const VM_Init_Graphic_Mode &mode ) const;
		bool operator!=( const VM_Init_Graphic_Mode &mode ) const;
		
		bool Get_Enabled() const;
		void Set_Enabled( bool use );
		
		int Get_Width() const;
		void Set_Width( int w );
		
		int Get_Height() const;
		void Set_Height( int h );
		
		int Get_Depth() const;
		void Set_Depth( int d );
		
	private:
		bool Enabled;
		int Width;
		int Height;
		int Depth;
};

#endif
