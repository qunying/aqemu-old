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

#include <QSettings>
#include <QRegExp>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QTextStream>

#include "System_Info.h"

System_Info::System_Info()
{
}

bool System_Info::Update_VM_Computers_List()
{
	Averable_Devices tmp_sys;
	
	// Devices Lists
	QList<Device_Map> CPU_x86;
	QList<Device_Map> CPU_ARM;
	QList<Device_Map> CPU_MIPS;
	QList<Device_Map> CPU_PPC;
	QList<Device_Map> CPU_Sparc;
	
	QList<Device_Map> Machine_x86;
	QList<Device_Map> Machine_ARM;
	QList<Device_Map> Machine_M68K;
	QList<Device_Map> Machine_MIPS;
	QList<Device_Map> Machine_PPC;
	QList<Device_Map> Machine_SH4;
	QList<Device_Map> Machine_Sparc;
	
	QList<Device_Map> Network_Card_x86;
	QList<Device_Map> Network_Card_x86_v090;
	QList<Device_Map> Network_Card_ARM;
	QList<Device_Map> Network_Card_M68K;
	QList<Device_Map> Network_Card_MIPS;
	QList<Device_Map> Network_Card_PPC;
	QList<Device_Map> Network_Card_Sparc;
	
	QList<Device_Map> Video_Card_Standart;
	QList<Device_Map> Video_Card_x86;
	
	VM::Sound_Cards Audio_Card_x86;
	VM::Sound_Cards Audio_Card_PPC;
	
	CPU_x86 << Device_Map( QObject::tr("Default"), "" );
	CPU_x86 << Device_Map( "QEMU 32Bit", "qemu32" );
	CPU_x86 << Device_Map( "Intel 486", "486" );
	CPU_x86 << Device_Map( "Pentium", "pentium" );
	CPU_x86 << Device_Map( "Pentium 2", "pentium2" );
	CPU_x86 << Device_Map( "Pentium 3", "pentium3" );
	
	CPU_ARM << Device_Map( QObject::tr("Default"), "" );
	CPU_ARM << Device_Map( "ARM 926", "arm926" );
	CPU_ARM << Device_Map( "ARM 946", "arm946" );
	CPU_ARM << Device_Map( "ARM 1026", "arm1026" );
	CPU_ARM << Device_Map( "ARM 1136", "arm1136" );
	CPU_ARM << Device_Map( "ARM 11 MPCORE", "arm11mpcore" );
	CPU_ARM << Device_Map( "Cortex-M3", "cortex-m3" );
	CPU_ARM << Device_Map( "Cortex-A8", "cortex-a8" );
	CPU_ARM << Device_Map( "Ti925T", "ti925t" );
	CPU_ARM << Device_Map( "PXA 250", "pxa250" );
	CPU_ARM << Device_Map( "PXA 255", "pxa255" );
	CPU_ARM << Device_Map( "PXA 260", "pxa260" );
	CPU_ARM << Device_Map( "PXA 261", "pxa261" );
	CPU_ARM << Device_Map( "PXA 262", "pxa262" );
	CPU_ARM << Device_Map( "PXA 270", "pxa270" );
	CPU_ARM << Device_Map( "PXA 270-a0", "pxa270-a0" );
	CPU_ARM << Device_Map( "PXA 270-a1", "pxa270-a1" );
	CPU_ARM << Device_Map( "PXA 270-b0", "pxa270-b0" );
	CPU_ARM << Device_Map( "PXA 270-b1", "pxa270-b1" );
	CPU_ARM << Device_Map( "PXA 270-c0", "pxa270-c0" );
	CPU_ARM << Device_Map( "PXA 270-c5", "pxa270-c5" );
	CPU_ARM << Device_Map( "Any", "any" );
	
	CPU_MIPS << Device_Map( QObject::tr("Default"), "" );
	CPU_MIPS << Device_Map( "4Kc", "4Kc" );
	CPU_MIPS << Device_Map( "4Km", "4Km" );
	CPU_MIPS << Device_Map( "4KEcR1", "4KEcR1" );
	CPU_MIPS << Device_Map( "4KEmR1", "4KEmR1" );
	CPU_MIPS << Device_Map( "4KEc", "4KEc" );
	CPU_MIPS << Device_Map( "4KEm", "4KEm" );
	CPU_MIPS << Device_Map( "24Kc", "24Kc" );
	CPU_MIPS << Device_Map( "24Kf", "24Kf" );
	CPU_MIPS << Device_Map( "34Kf", "34Kf" );
	
	CPU_PPC << Device_Map( QObject::tr("Default"), "default" );
	CPU_PPC << Device_Map( "401", "401" );
	CPU_PPC << Device_Map( "401A1", "401A1" );
	CPU_PPC << Device_Map( "401B2", "401B2" );
	CPU_PPC << Device_Map( "401C2", "401C2" );
	CPU_PPC << Device_Map( "401D2", "401D2" );
	CPU_PPC << Device_Map( "401E2", "401E2" );
	CPU_PPC << Device_Map( "401F2", "401F2" );
	CPU_PPC << Device_Map( "401G2", "401G2" );
	CPU_PPC << Device_Map( "IOP480", "IOP480" );
	CPU_PPC << Device_Map( "Cobra", "Cobra" );
	CPU_PPC << Device_Map( "403", "403" );
	CPU_PPC << Device_Map( "403GA", "403GA" );
	CPU_PPC << Device_Map( "403GB", "403GB" );
	CPU_PPC << Device_Map( "403GC", "403GC" );
	CPU_PPC << Device_Map( "403GCX", "403GCX" );
	CPU_PPC << Device_Map( "405", "405" );
	CPU_PPC << Device_Map( "405D2", "405D2" );
	CPU_PPC << Device_Map( "405D4", "405D4" );
	CPU_PPC << Device_Map( "405CR", "405CR" );
	CPU_PPC << Device_Map( "405CRa", "405CRa" );
	CPU_PPC << Device_Map( "405CRb", "405CRb" );
	CPU_PPC << Device_Map( "405CRc", "405CRc" );
	CPU_PPC << Device_Map( "405EP", "405EP" );
	CPU_PPC << Device_Map( "405EZ", "405EZ" );
	CPU_PPC << Device_Map( "405GP", "405GP" );
	CPU_PPC << Device_Map( "405GPa", "405GPa" );
	CPU_PPC << Device_Map( "405GPb", "405GPb" );
	CPU_PPC << Device_Map( "405GPc", "405GPc" );
	CPU_PPC << Device_Map( "405GPd", "405GPd" );
	CPU_PPC << Device_Map( "405GPe", "405GPe" );
	CPU_PPC << Device_Map( "405GPR", "405GPR" );
	CPU_PPC << Device_Map( "405LP", "405LP" );
	CPU_PPC << Device_Map( "Npe405H", "Npe405H" );
	CPU_PPC << Device_Map( "Npe405H2", "Npe405H2" );
	CPU_PPC << Device_Map( "Npe405L", "Npe405L" );
	CPU_PPC << Device_Map( "Npe4GS3", "Npe4GS3" );
	CPU_PPC << Device_Map( "STB03", "STB03" );
	CPU_PPC << Device_Map( "STB04", "STB04" );
	CPU_PPC << Device_Map( "STB25", "STB25" );
	CPU_PPC << Device_Map( "x2vp4", "x2vp4" );
	CPU_PPC << Device_Map( "x2vp7", "x2vp7" );
	CPU_PPC << Device_Map( "x2vp20", "x2vp20" );
	CPU_PPC << Device_Map( "x2vp50", "x2vp50" );
	CPU_PPC << Device_Map( "MPC52xx", "MPC52xx" );
	CPU_PPC << Device_Map( "MPC82xx", "MPC82xx" );
	CPU_PPC << Device_Map( "PowerQUICC-II", "\"PowerQUICC-II\"" );
	CPU_PPC << Device_Map( "G2", "G2" );
	CPU_PPC << Device_Map( "G2H4", "G2H4" );
	CPU_PPC << Device_Map( "G2GP", "G2GP" );
	CPU_PPC << Device_Map( "G2LS", "G2LS" );
	CPU_PPC << Device_Map( "G2HiP3", "G2HiP3" );
	CPU_PPC << Device_Map( "G2HiP4", "G2HiP4" );
	CPU_PPC << Device_Map( "MPC603", "MPC603" );
	CPU_PPC << Device_Map( "G2le", "G2le" );
	CPU_PPC << Device_Map( "G2leGP", "G2leGP" );
	CPU_PPC << Device_Map( "G2leLS", "G2leLS" );
	CPU_PPC << Device_Map( "G2leGP1", "G2leGP1" );
	CPU_PPC << Device_Map( "G2leGP3", "G2leGP3" );
	CPU_PPC << Device_Map( "MPC8240", "MPC8240" );
	CPU_PPC << Device_Map( "MPC5200", "MPC5200" );
	CPU_PPC << Device_Map( "MPC5200_v10", "MPC5200_v10" );
	CPU_PPC << Device_Map( "MPC5200_v11", "MPC5200_v11" );
	CPU_PPC << Device_Map( "MPC5200_v12", "MPC5200_v12" );
	CPU_PPC << Device_Map( "MPC5200B", "MPC5200B" );
	CPU_PPC << Device_Map( "MPC5200B_v20", "MPC5200B_v20" );
	CPU_PPC << Device_Map( "MPC5200B_v21", "MPC5200B_v21" );
	CPU_PPC << Device_Map( "MPC8241", "MPC8241" );
	CPU_PPC << Device_Map( "MPC8245", "MPC8245" );
	CPU_PPC << Device_Map( "MPC8247", "MPC8247" );
	CPU_PPC << Device_Map( "MPC8248", "MPC8248" );
	CPU_PPC << Device_Map( "MPC8250", "MPC8250" );
	CPU_PPC << Device_Map( "MPC8250_HiP3", "MPC8250_HiP3" );
	CPU_PPC << Device_Map( "MPC8250_HiP4", "MPC8250_HiP4" );
	CPU_PPC << Device_Map( "MPC8255", "MPC8255" );
	CPU_PPC << Device_Map( "MPC8255_HiP3", "MPC8255_HiP3" );
	CPU_PPC << Device_Map( "MPC8255_HiP4", "MPC8255_HiP4" );
	CPU_PPC << Device_Map( "MPC8260", "MPC8260" );
	CPU_PPC << Device_Map( "MPC8260_HiP3", "MPC8260_HiP3" );
	CPU_PPC << Device_Map( "MPC8260_HiP4", "MPC8260_HiP4" );
	CPU_PPC << Device_Map( "MPC8264", "MPC8264" );
	CPU_PPC << Device_Map( "MPC8264_HiP3", "MPC8264_HiP3" );
	CPU_PPC << Device_Map( "MPC8264_HiP4", "MPC8264_HiP4" );
	CPU_PPC << Device_Map( "MPC8265", "MPC8265" );
	CPU_PPC << Device_Map( "MPC8265_HiP3", "MPC8265_HiP3" );
	CPU_PPC << Device_Map( "MPC8265_HiP4", "MPC8265_HiP4" );
	CPU_PPC << Device_Map( "MPC8266", "MPC8266" );
	CPU_PPC << Device_Map( "MPC8266_HiP3", "MPC8266_HiP3" );
	CPU_PPC << Device_Map( "MPC8266_HiP4", "MPC8266_HiP4" );
	CPU_PPC << Device_Map( "MPC8270", "MPC8270" );
	CPU_PPC << Device_Map( "MPC8271", "MPC8271" );
	CPU_PPC << Device_Map( "MPC8272", "MPC8272" );
	CPU_PPC << Device_Map( "MPC8275", "MPC8275" );
	CPU_PPC << Device_Map( "MPC8280", "MPC8280" );
	CPU_PPC << Device_Map( "e200", "e200" );
	CPU_PPC << Device_Map( "e200z5", "e200z5" );
	CPU_PPC << Device_Map( "e200z6", "e200z6" );
	CPU_PPC << Device_Map( "e300", "e300" );
	CPU_PPC << Device_Map( "e300c1", "e300c1" );
	CPU_PPC << Device_Map( "e300c2", "e300c2" );
	CPU_PPC << Device_Map( "e300c3", "e300c3" );
	CPU_PPC << Device_Map( "e300c4", "e300c4" );
	CPU_PPC << Device_Map( "MPC8343A", "MPC8343A" );
	CPU_PPC << Device_Map( "MPC8343EA", "MPC8343EA" );
	CPU_PPC << Device_Map( "MPC8347A", "MPC8347A" );
	CPU_PPC << Device_Map( "MPC8347AT", "MPC8347AT" );
	CPU_PPC << Device_Map( "MPC8347AP", "MPC8347AP" );
	CPU_PPC << Device_Map( "MPC8347EA", "MPC8347EA" );
	CPU_PPC << Device_Map( "MPC8347EAT", "MPC8347EAT" );
	CPU_PPC << Device_Map( "MPC8347EAP", "MPC8347EAP" );
	CPU_PPC << Device_Map( "MPC8349", "MPC8349" );
	CPU_PPC << Device_Map( "MPC8349A", "MPC8349A" );
	CPU_PPC << Device_Map( "MPC8349E", "MPC8349E" );
	CPU_PPC << Device_Map( "MPC8349EA", "MPC8349EA" );
	CPU_PPC << Device_Map( "MPC8377", "MPC8377" );
	CPU_PPC << Device_Map( "MPC8377E", "MPC8377E" );
	CPU_PPC << Device_Map( "MPC8378", "MPC8378" );
	CPU_PPC << Device_Map( "MPC8378E", "MPC8378E" );
	CPU_PPC << Device_Map( "MPC8379", "MPC8379" );
	CPU_PPC << Device_Map( "MPC8379E", "MPC8379E" );
	CPU_PPC << Device_Map( "e500", "e500" );
	CPU_PPC << Device_Map( "e500_v10", "e500_v10" );
	CPU_PPC << Device_Map( "e500_v20", "e500_v20" );
	CPU_PPC << Device_Map( "e500v2", "e500v2" );
	CPU_PPC << Device_Map( "e500v2_v10", "e500v2_v10" );
	CPU_PPC << Device_Map( "e500v2_v20", "e500v2_v20" );
	CPU_PPC << Device_Map( "e500v2_v21", "e500v2_v21" );
	CPU_PPC << Device_Map( "e500v2_v22", "e500v2_v22" );
	CPU_PPC << Device_Map( "e500v2_v30", "e500v2_v30" );
	CPU_PPC << Device_Map( "MPC8533", "MPC8533" );
	CPU_PPC << Device_Map( "MPC8533_v10", "MPC8533_v10" );
	CPU_PPC << Device_Map( "MPC8533_v11", "MPC8533_v11" );
	CPU_PPC << Device_Map( "MPC8533E", "MPC8533E" );
	CPU_PPC << Device_Map( "MPC8533E_v10", "MPC8533E_v10" );
	CPU_PPC << Device_Map( "MPC8533E_v11", "MPC8533E_v11" );
	CPU_PPC << Device_Map( "MPC8540", "MPC8540" );
	CPU_PPC << Device_Map( "MPC8540_v10", "MPC8540_v10" );
	CPU_PPC << Device_Map( "MPC8540_v20", "MPC8540_v20" );
	CPU_PPC << Device_Map( "MPC8540_v21", "MPC8540_v21" );
	CPU_PPC << Device_Map( "MPC8541", "MPC8541" );
	CPU_PPC << Device_Map( "MPC8541_v10", "MPC8541_v10" );
	CPU_PPC << Device_Map( "MPC8541_v11", "MPC8541_v11" );
	CPU_PPC << Device_Map( "MPC8541E", "MPC8541E" );
	CPU_PPC << Device_Map( "MPC8541E_v10", "MPC8541E_v10" );
	CPU_PPC << Device_Map( "MPC8541E_v11", "MPC8541E_v11" );
	CPU_PPC << Device_Map( "MPC8543", "MPC8543" );
	CPU_PPC << Device_Map( "MPC8543_v10", "MPC8543_v10" );
	CPU_PPC << Device_Map( "MPC8543_v11", "MPC8543_v11" );
	CPU_PPC << Device_Map( "MPC8543_v20", "MPC8543_v20" );
	CPU_PPC << Device_Map( "MPC8543_v21", "MPC8543_v21" );
	CPU_PPC << Device_Map( "MPC8543E", "MPC8543E" );
	CPU_PPC << Device_Map( "MPC8543E_v10", "MPC8543E_v10" );
	CPU_PPC << Device_Map( "MPC8543E_v11", "MPC8543E_v11" );
	CPU_PPC << Device_Map( "MPC8543E_v20", "MPC8543E_v20" );
	CPU_PPC << Device_Map( "MPC8543E_v21", "MPC8543E_v21" );
	CPU_PPC << Device_Map( "MPC8544", "MPC8544" );
	CPU_PPC << Device_Map( "MPC8544_v10", "MPC8544_v10" );
	CPU_PPC << Device_Map( "MPC8544_v11", "MPC8544_v11" );
	CPU_PPC << Device_Map( "MPC8544E", "MPC8544E" );
	CPU_PPC << Device_Map( "MPC8544E_v10", "MPC8544E_v10" );
	CPU_PPC << Device_Map( "MPC8544E_v11", "MPC8544E_v11" );
	CPU_PPC << Device_Map( "MPC8545", "MPC8545" );
	CPU_PPC << Device_Map( "MPC8545_v20", "MPC8545_v20" );
	CPU_PPC << Device_Map( "MPC8545_v21", "MPC8545_v21" );
	CPU_PPC << Device_Map( "MPC8545E", "MPC8545E" );
	CPU_PPC << Device_Map( "MPC8545E_v20", "MPC8545E_v20" );
	CPU_PPC << Device_Map( "MPC8545E_v21", "MPC8545E_v21" );
	CPU_PPC << Device_Map( "MPC8547E", "MPC8547E" );
	CPU_PPC << Device_Map( "MPC8547E_v20", "MPC8547E_v20" );
	CPU_PPC << Device_Map( "MPC8547E_v21", "MPC8547E_v21" );
	CPU_PPC << Device_Map( "MPC8548", "MPC8548" );
	CPU_PPC << Device_Map( "MPC8548_v10", "MPC8548_v10" );
	CPU_PPC << Device_Map( "MPC8548_v11", "MPC8548_v11" );
	CPU_PPC << Device_Map( "MPC8548_v20", "MPC8548_v20" );
	CPU_PPC << Device_Map( "MPC8548_v21", "MPC8548_v21" );
	CPU_PPC << Device_Map( "MPC8548E", "MPC8548E" );
	CPU_PPC << Device_Map( "MPC8548E_v10", "MPC8548E_v10" );
	CPU_PPC << Device_Map( "MPC8548E_v11", "MPC8548E_v11" );
	CPU_PPC << Device_Map( "MPC8548E_v20", "MPC8548E_v20" );
	CPU_PPC << Device_Map( "MPC8548E_v21", "MPC8548E_v21" );
	CPU_PPC << Device_Map( "MPC8555", "MPC8555" );
	CPU_PPC << Device_Map( "MPC8555_v10", "MPC8555_v10" );
	CPU_PPC << Device_Map( "MPC8555_v11", "MPC8555_v11" );
	CPU_PPC << Device_Map( "MPC8555E", "MPC8555E" );
	CPU_PPC << Device_Map( "MPC8555E_v10", "MPC8555E_v10" );
	CPU_PPC << Device_Map( "MPC8555E_v11", "MPC8555E_v11" );
	CPU_PPC << Device_Map( "MPC8560", "MPC8560" );
	CPU_PPC << Device_Map( "MPC8560_v10", "MPC8560_v10" );
	CPU_PPC << Device_Map( "MPC8560_v20", "MPC8560_v20" );
	CPU_PPC << Device_Map( "MPC8560_v21", "MPC8560_v21" );
	CPU_PPC << Device_Map( "MPC8567", "MPC8567" );
	CPU_PPC << Device_Map( "MPC8567E", "MPC8567E" );
	CPU_PPC << Device_Map( "MPC8568", "MPC8568" );
	CPU_PPC << Device_Map( "MPC8568E", "MPC8568E" );
	CPU_PPC << Device_Map( "MPC8572", "MPC8572" );
	CPU_PPC << Device_Map( "MPC8572E", "MPC8572E" );
	CPU_PPC << Device_Map( "e600", "e600" );
	CPU_PPC << Device_Map( "MPC8641", "MPC8641" );
	CPU_PPC << Device_Map( "MPC8641D", "MPC8641D" );
	CPU_PPC << Device_Map( "601", "601" );
	CPU_PPC << Device_Map( "601_v0", "601_v0" );
	CPU_PPC << Device_Map( "601_v1", "601_v1" );
	CPU_PPC << Device_Map( "601v", "601v" );
	CPU_PPC << Device_Map( "601_v2", "601_v2" );
	CPU_PPC << Device_Map( "602", "602" );
	CPU_PPC << Device_Map( "603", "603" );
	CPU_PPC << Device_Map( "Vanilla", "Vanilla" );
	CPU_PPC << Device_Map( "603e", "603e" );
	CPU_PPC << Device_Map( "Stretch", "Stretch" );
	CPU_PPC << Device_Map( "603e_v1.1", "\"603e_v1.1\"" );
	CPU_PPC << Device_Map( "603e_v1.2", "\"603e_v1.2\"" );
	CPU_PPC << Device_Map( "603e_v1.3", "\"603e_v1.3\"" );
	CPU_PPC << Device_Map( "603e_v1.4", "\"603e_v1.4\"" );
	CPU_PPC << Device_Map( "603e_v2.2", "\"603e_v2.2\"" );
	CPU_PPC << Device_Map( "603e_v3", "603e_v3" );
	CPU_PPC << Device_Map( "603e_v4", "603e_v4" );
	CPU_PPC << Device_Map( "603e_v4.1", "\"603e_v4.1\"" );
	CPU_PPC << Device_Map( "603e7", "603e7" );
	CPU_PPC << Device_Map( "603e7t", "603e7t" );
	CPU_PPC << Device_Map( "603e7v", "603e7v" );
	CPU_PPC << Device_Map( "Vaillant", "Vaillant" );
	CPU_PPC << Device_Map( "603e7v1", "603e7v1" );
	CPU_PPC << Device_Map( "603e7v2", "603e7v2" );
	CPU_PPC << Device_Map( "603p", "603p" );
	CPU_PPC << Device_Map( "603r", "603r" );
	CPU_PPC << Device_Map( "Goldeneye", "Goldeneye" );
	CPU_PPC << Device_Map( "604", "604" );
	CPU_PPC << Device_Map( "604e", "604e" );
	CPU_PPC << Device_Map( "Sirocco", "Sirocco" );
	CPU_PPC << Device_Map( "604e_v1.0", "\"604e_v1.0\"" );
	CPU_PPC << Device_Map( "604e_v2.2", "\"604e_v2.2\"" );
	CPU_PPC << Device_Map( "604e_v2.4", "\"604e_v2.4\"" );
	CPU_PPC << Device_Map( "604r", "604r" );
	CPU_PPC << Device_Map( "Mach5", "Mach5" );
	CPU_PPC << Device_Map( "740", "740" );
	CPU_PPC << Device_Map( "Arthur", "Arthur" );
	CPU_PPC << Device_Map( "750", "750" );
	CPU_PPC << Device_Map( "Typhoon", "Typhoon" );
	CPU_PPC << Device_Map( "G3", "G3" );
	CPU_PPC << Device_Map( "740_v1.0", "\"740_v1.0\"" );
	CPU_PPC << Device_Map( "750_v1.0", "\"750_v1.0\"" );
	CPU_PPC << Device_Map( "740_v2.0", "\"740_v2.0\"" );
	CPU_PPC << Device_Map( "750_v2.0", "\"750_v2.0\"" );
	CPU_PPC << Device_Map( "740_v2.1", "\"740_v2.1\"" );
	CPU_PPC << Device_Map( "750_v2.1", "\"750_v2.1\"" );
	CPU_PPC << Device_Map( "740_v2.2", "\"740_v2.2\"" );
	CPU_PPC << Device_Map( "750_v2.2", "\"750_v2.2\"" );
	CPU_PPC << Device_Map( "740_v3.0", "\"740_v3.0\"" );
	CPU_PPC << Device_Map( "750_v3.0", "\"750_v3.0\"" );
	CPU_PPC << Device_Map( "740_v3.1", "\"740_v3.1\"" );
	CPU_PPC << Device_Map( "750_v3.1", "\"750_v3.1\"" );
	CPU_PPC << Device_Map( "740e", "740e" );
	CPU_PPC << Device_Map( "750e", "750e" );
	CPU_PPC << Device_Map( "740p", "740p" );
	CPU_PPC << Device_Map( "750p", "750p" );
	CPU_PPC << Device_Map( "Conan/Doyle", "\"Conan/Doyle\"" );
	CPU_PPC << Device_Map( "750cl", "750cl" );
	CPU_PPC << Device_Map( "750cl_v1.0", "\"750cl_v1.0\"" );
	CPU_PPC << Device_Map( "750cl_v2.0", "\"750cl_v2.0\"" );
	CPU_PPC << Device_Map( "750cx", "750cx" );
	CPU_PPC << Device_Map( "750cx_v1.0", "\"750cx_v1.0\"" );
	CPU_PPC << Device_Map( "750cx_v2.0", "\"750cx_v2.0\"" );
	CPU_PPC << Device_Map( "750cx_v2.1", "\"750cx_v2.1\"" );
	CPU_PPC << Device_Map( "750cx_v2.2", "\"750cx_v2.2\"" );
	CPU_PPC << Device_Map( "750cxe", "750cxe" );
	CPU_PPC << Device_Map( "750cxe_v2.1", "\"750cxe_v2.1\"" );
	CPU_PPC << Device_Map( "750cxe_v2.2", "\"750cxe_v2.2\"" );
	CPU_PPC << Device_Map( "750cxe_v2.3", "\"750cxe_v2.3\"" );
	CPU_PPC << Device_Map( "750cxe_v2.4", "\"750cxe_v2.4\"" );
	CPU_PPC << Device_Map( "750cxe_v2.4b", "\"750cxe_v2.4b\"" );
	CPU_PPC << Device_Map( "750cxe_v3.0", "\"750cxe_v3.0\"" );
	CPU_PPC << Device_Map( "750cxe_v3.1", "\"750cxe_v3.1\"" );
	CPU_PPC << Device_Map( "750cxe_v3.1b", "\"750cxe_v3.1b\"" );
	CPU_PPC << Device_Map( "750cxr", "750cxr" );
	CPU_PPC << Device_Map( "750fl", "750fl" );
	CPU_PPC << Device_Map( "750fx", "750fx" );
	CPU_PPC << Device_Map( "750fx_v1.0", "\"750fx_v1.0\"" );
	CPU_PPC << Device_Map( "750fx_v2.0", "\"750fx_v2.0\"" );
	CPU_PPC << Device_Map( "750fx_v2.1", "\"750fx_v2.1\"" );
	CPU_PPC << Device_Map( "750fx_v2.2", "\"750fx_v2.2\"" );
	CPU_PPC << Device_Map( "750fx_v2.3", "\"750fx_v2.3\"" );
	CPU_PPC << Device_Map( "750gl", "750gl" );
	CPU_PPC << Device_Map( "750gx", "750gx" );
	CPU_PPC << Device_Map( "750gx_v1.0", "\"750gx_v1.0\"" );
	CPU_PPC << Device_Map( "750gx_v1.1", "\"750gx_v1.1\"" );
	CPU_PPC << Device_Map( "750gx_v1.2", "\"750gx_v1.2\"" );
	CPU_PPC << Device_Map( "750l", "750l" );
	CPU_PPC << Device_Map( "LoneStar", "LoneStar" );
	CPU_PPC << Device_Map( "750l_v2.0", "\"750l_v2.0\"" );
	CPU_PPC << Device_Map( "750l_v2.1", "\"750l_v2.1\"" );
	CPU_PPC << Device_Map( "750l_v2.2", "\"750l_v2.2\"" );
	CPU_PPC << Device_Map( "750l_v3.0", "\"750l_v3.0\"" );
	CPU_PPC << Device_Map( "750l_v3.2", "\"750l_v3.2\"" );
	CPU_PPC << Device_Map( "745", "745" );
	CPU_PPC << Device_Map( "755", "755" );
	CPU_PPC << Device_Map( "Goldfinger", "Goldfinger" );
	CPU_PPC << Device_Map( "745_v1.0", "\"745_v1.0\"" );
	CPU_PPC << Device_Map( "755_v1.0", "\"755_v1.0\"" );
	CPU_PPC << Device_Map( "745_v1.1", "\"745_v1.1\"" );
	CPU_PPC << Device_Map( "755_v1.1", "\"755_v1.1\"" );
	CPU_PPC << Device_Map( "745_v2.0", "\"745_v2.0\"" );
	CPU_PPC << Device_Map( "755_v2.0", "\"755_v2.0\"" );
	CPU_PPC << Device_Map( "745_v2.1", "\"745_v2.1\"" );
	CPU_PPC << Device_Map( "755_v2.1", "\"755_v2.1\"" );
	CPU_PPC << Device_Map( "745_v2.2", "\"745_v2.2\"" );
	CPU_PPC << Device_Map( "755_v2.2", "\"755_v2.2\"" );
	CPU_PPC << Device_Map( "745_v2.3", "\"745_v2.3\"" );
	CPU_PPC << Device_Map( "755_v2.3", "\"755_v2.3\"" );
	CPU_PPC << Device_Map( "745_v2.4", "\"745_v2.4\"" );
	CPU_PPC << Device_Map( "755_v2.4", "\"755_v2.4\"" );
	CPU_PPC << Device_Map( "745_v2.5", "\"745_v2.5\"" );
	CPU_PPC << Device_Map( "755_v2.5", "\"755_v2.5\"" );
	CPU_PPC << Device_Map( "745_v2.6", "\"745_v2.6\"" );
	CPU_PPC << Device_Map( "755_v2.6", "\"755_v2.6\"" );
	CPU_PPC << Device_Map( "745_v2.7", "\"745_v2.7\"" );
	CPU_PPC << Device_Map( "755_v2.7", "\"755_v2.7\"" );
	CPU_PPC << Device_Map( "745_v2.8", "\"745_v2.8\"" );
	CPU_PPC << Device_Map( "755_v2.8", "\"755_v2.8\"" );
	CPU_PPC << Device_Map( "7400", "7400" );
	CPU_PPC << Device_Map( "Max", "Max" );
	CPU_PPC << Device_Map( "G4", "G4" );
	CPU_PPC << Device_Map( "7400_v1.0", "\"7400_v1.0\"" );
	CPU_PPC << Device_Map( "7400_v1.1", "\"7400_v1.1\"" );
	CPU_PPC << Device_Map( "7400_v2.0", "\"7400_v2.0\"" );
	CPU_PPC << Device_Map( "7400_v2.1", "\"7400_v2.1\"" );
	CPU_PPC << Device_Map( "7400_v2.2", "\"7400_v2.2\"" );
	CPU_PPC << Device_Map( "7400_v2.6", "\"7400_v2.6\"" );
	CPU_PPC << Device_Map( "7400_v2.7", "\"7400_v2.7\"" );
	CPU_PPC << Device_Map( "7400_v2.8", "\"7400_v2.8\"" );
	CPU_PPC << Device_Map( "7400_v2.9", "\"7400_v2.9\"" );
	CPU_PPC << Device_Map( "7410", "7410" );
	CPU_PPC << Device_Map( "Nitro", "Nitro" );
	CPU_PPC << Device_Map( "7410_v1.0", "\"7410_v1.0\"" );
	CPU_PPC << Device_Map( "7410_v1.1", "\"7410_v1.1\"" );
	CPU_PPC << Device_Map( "7410_v1.2", "\"7410_v1.2\"" );
	CPU_PPC << Device_Map( "7410_v1.3", "\"7410_v1.3\"" );
	CPU_PPC << Device_Map( "7410_v1.4", "\"7410_v1.4\"" );
	CPU_PPC << Device_Map( "7448", "7448" );
	CPU_PPC << Device_Map( "7448_v1.0", "\"7448_v1.0\"" );
	CPU_PPC << Device_Map( "7448_v1.1", "\"7448_v1.1\"" );
	CPU_PPC << Device_Map( "7448_v2.0", "\"7448_v2.0\"" );
	CPU_PPC << Device_Map( "7448_v2.1", "\"7448_v2.1\"" );
	CPU_PPC << Device_Map( "7450", "7450" );
	CPU_PPC << Device_Map( "Vger", "Vger" );
	CPU_PPC << Device_Map( "7450_v1.0", "\"7450_v1.0\"" );
	CPU_PPC << Device_Map( "7450_v1.1", "\"7450_v1.1\"" );
	CPU_PPC << Device_Map( "7450_v1.2", "\"7450_v1.2\"" );
	CPU_PPC << Device_Map( "7450_v2.0", "\"7450_v2.0\"" );
	CPU_PPC << Device_Map( "7450_v2.1", "\"7450_v2.1\"" );
	CPU_PPC << Device_Map( "7441", "7441" );
	CPU_PPC << Device_Map( "7451", "7451" );
	CPU_PPC << Device_Map( "7441_v2.1", "\"7441_v2.1\"" );
	CPU_PPC << Device_Map( "7441_v2.3", "\"7441_v2.3\"" );
	CPU_PPC << Device_Map( "7451_v2.3", "\"7451_v2.3\"" );
	CPU_PPC << Device_Map( "7441_v2.10", "\"7441_v2.10\"" );
	CPU_PPC << Device_Map( "7451_v2.10", "\"7451_v2.10\"" );
	CPU_PPC << Device_Map( "7445", "7445" );
	CPU_PPC << Device_Map( "7455", "7455" );
	CPU_PPC << Device_Map( "Apollo6", "Apollo6" );
	CPU_PPC << Device_Map( "7445_v1.0", "\"7445_v1.0\"" );
	CPU_PPC << Device_Map( "7455_v1.0", "\"7455_v1.0\"" );
	CPU_PPC << Device_Map( "7445_v2.1", "\"7445_v2.1\"" );
	CPU_PPC << Device_Map( "7455_v2.1", "\"7455_v2.1\"" );
	CPU_PPC << Device_Map( "7445_v3.2", "\"7445_v3.2\"" );
	CPU_PPC << Device_Map( "7455_v3.2", "\"7455_v3.2\"" );
	CPU_PPC << Device_Map( "7445_v3.3", "\"7445_v3.3\"" );
	CPU_PPC << Device_Map( "7455_v3.3", "\"7455_v3.3\"" );
	CPU_PPC << Device_Map( "7445_v3.4", "\"7445_v3.4\"" );
	CPU_PPC << Device_Map( "7455_v3.4", "\"7455_v3.4\"" );
	CPU_PPC << Device_Map( "7447", "7447" );
	CPU_PPC << Device_Map( "7457", "7457" );
	CPU_PPC << Device_Map( "Apollo7", "Apollo7" );
	CPU_PPC << Device_Map( "7447_v1.0", "\"7447_v1.0\"" );
	CPU_PPC << Device_Map( "7457_v1.0", "\"7457_v1.0\"" );
	CPU_PPC << Device_Map( "7447_v1.1", "\"7447_v1.1\"" );
	CPU_PPC << Device_Map( "7457_v1.1", "\"7457_v1.1\"" );
	CPU_PPC << Device_Map( "7457_v1.2", "\"7457_v1.2\"" );
	CPU_PPC << Device_Map( "7447A", "7447A" );
	CPU_PPC << Device_Map( "7457A", "7457A" );
	CPU_PPC << Device_Map( "7447A_v1.0", "\"7447A_v1.0\"" );
	CPU_PPC << Device_Map( "7457A_v1.0", "\"7457A_v1.0\"" );
	CPU_PPC << Device_Map( "Apollo7PM", "Apollo7PM" );
	CPU_PPC << Device_Map( "7447A_v1.1", "\"7447A_v1.1\"" );
	CPU_PPC << Device_Map( "7457A_v1.1", "\"7457A_v1.1\"" );
	CPU_PPC << Device_Map( "7447A_v1.2", "\"7447A_v1.2\"" );
	CPU_PPC << Device_Map( "7457A_v1.2", "\"7457A_v1.2\"" );
	CPU_PPC << Device_Map( "ppc32", "ppc32" );
	CPU_PPC << Device_Map( "ppc", "ppc" );
	
	CPU_Sparc << Device_Map( QObject::tr("Default"), "" );
	CPU_Sparc << Device_Map( "Fujitsu MB86900", "\"Fujitsu MB86900\"" );
	CPU_Sparc << Device_Map( "Fujitsu MB86904", "\"Fujitsu MB86904\"" );
	CPU_Sparc << Device_Map( "Fujitsu MB86907", "\"Fujitsu MB86907\"" );
	CPU_Sparc << Device_Map( "LSI L64811", "\"LSI L64811\"" );
	CPU_Sparc << Device_Map( "Cypress CY7C601", "\"Cypress CY7C601\"" );
	CPU_Sparc << Device_Map( "Cypress CY7C611", "\"Cypress CY7C611\"" );
	CPU_Sparc << Device_Map( "TI SuperSparc II", "\"TI SuperSparc II\"" );
	CPU_Sparc << Device_Map( "TI MicroSparc I", "\"TI MicroSparc I\"" );
	CPU_Sparc << Device_Map( "TI MicroSparc II", "\"TI MicroSparc II\"" );
	CPU_Sparc << Device_Map( "TI MicroSparc IIep", "\"TI MicroSparc IIep\"" );
	CPU_Sparc << Device_Map( "TI SuperSparc 51", "\"TI SuperSparc 51\"" );
	CPU_Sparc << Device_Map( "TI SuperSparc 61", "\"TI SuperSparc 61\"" );
	CPU_Sparc << Device_Map( "Ross RT625", "\"Ross RT625\"" );
	CPU_Sparc << Device_Map( "Ross RT620", "\"Ross RT620\"" );
	CPU_Sparc << Device_Map( "BIT", "\"BIT\"" );
	CPU_Sparc << Device_Map( "Matsushita MN10501", "\"Matsushita MN10501\"" );
	CPU_Sparc << Device_Map( "Weitek W8601", "\"Weitek W8601\"" );
	CPU_Sparc << Device_Map( "LEON2", "\"LEON2\"" );
	CPU_Sparc << Device_Map( "LEON3", "\"LEON3\"" );
	
	// Machines
	Machine_x86 << Device_Map( QObject::tr("Standard PC (Default)"), "" );
	Machine_x86 << Device_Map( QObject::tr("Standard PC"), "pc" );
	Machine_x86 << Device_Map( QObject::tr("ISA-only PC"), "isapc" );
	
	Machine_ARM << Device_Map( QObject::tr("Default"), "" );
	Machine_ARM << Device_Map( QObject::tr("Integrator/CP (ARM926EJ-S)"), "integratorcp926" );
	Machine_ARM << Device_Map( QObject::tr("Integrator/CP (ARM1026EJ-S)"), "integratorcp1026" );
	Machine_ARM << Device_Map( QObject::tr("Versatile/PB (ARM926EJ-S)"), "versatilepb" );
	Machine_ARM << Device_Map( QObject::tr("Versatile/AB (ARM926EJ-S)"), "versatileab" );
	Machine_ARM << Device_Map( QObject::tr("RealView (ARM926EJ-S)"), "realview" );
	
	Machine_MIPS << Device_Map( QObject::tr("Default"), "" );
	Machine_MIPS << Device_Map( QObject::tr("R4k platform"), "mips" );
	Machine_MIPS << Device_Map( QObject::tr("Malta Core LV"), "malta" );
	
	Machine_PPC << Device_Map( QObject::tr("Default"), "" );
	Machine_PPC << Device_Map( QObject::tr("Heathrow PowerMAC"), "g3bw" );
	Machine_PPC << Device_Map( QObject::tr("Mac99 PowerMAC"), "mac99" );
	Machine_PPC << Device_Map( QObject::tr("PowerPC PREP"), "prep" );
	
	Machine_Sparc << Device_Map( QObject::tr("Default"), "" );
	Machine_Sparc << Device_Map( QObject::tr("SPARCstation 2 (Sun4c)"), "SS-2" );
	Machine_Sparc << Device_Map( QObject::tr("SPARCstation 5 (Sun4m)"), "SS-5" );
	Machine_Sparc << Device_Map( QObject::tr("SPARCstation 10 (Sun4m)"), "SS-10" );
	Machine_Sparc << Device_Map( QObject::tr("SPARCstation 20 (Sun4m)"), "SS-20" );
	Machine_Sparc << Device_Map( QObject::tr("SPARCserver 600MP (Sun4m)"), "SS-600MP" );
	Machine_Sparc << Device_Map( QObject::tr("SPARCserver 1000 (Sun4d)"), "SS-1000" );
	Machine_Sparc << Device_Map( QObject::tr("SPARCcenter 2000 (Sun4d)"), "SS-2000" );
	
	// Net Cards
	Network_Card_x86 << Device_Map( QObject::tr("NE2000 (Default)"), "" );
	Network_Card_x86 << Device_Map( QObject::tr("NE2000 PCI"), "ne2k_pci" );
	Network_Card_x86 << Device_Map( QObject::tr("NE2000 ISA"), "ne2k_isa" );
	Network_Card_x86 << Device_Map( QObject::tr("i82551"), "i82551" );
	Network_Card_x86 << Device_Map( QObject::tr("i82557B"), "i82557b" );
	Network_Card_x86 << Device_Map( QObject::tr("i82559ER"), "i82559er" );
	Network_Card_x86 << Device_Map( QObject::tr("PCnet32 PCI"), "pcnet" );
	Network_Card_x86 << Device_Map( QObject::tr("RTL8139"), "rtl8139" );
	
	Network_Card_x86_v090 << Device_Map( QObject::tr("NE2000 (Default)"), "" );
	Network_Card_x86_v090 << Device_Map( QObject::tr("NE2000 PCI"), "ne2k_pci" );
	Network_Card_x86_v090 << Device_Map( QObject::tr("NE2000 ISA"), "ne2k_isa" );
	Network_Card_x86_v090 << Device_Map( QObject::tr("RTL8139"), "rtl8139" );
	
	Network_Card_ARM << Device_Map( QObject::tr("Default"), "" );
	Network_Card_ARM << Device_Map( QObject::tr("SMC91C111"), "smc91c111" );
	
	Network_Card_M68K << Device_Map( QObject::tr("Default"), "" );
	Network_Card_M68K << Device_Map( QObject::tr("MCF FEC"), "mcf_fec" );
	
	Network_Card_MIPS << Device_Map( QObject::tr("Default"), "" );
	Network_Card_MIPS << Device_Map( QObject::tr("NE2000 ISA"), "ne2k_isa" );
	
	Network_Card_PPC << Device_Map( QObject::tr("Default"), "" );
	Network_Card_PPC << Device_Map( QObject::tr("i82551"), "i82551" );
	Network_Card_PPC << Device_Map( QObject::tr("i82557B"), "i82557b" );
	Network_Card_PPC << Device_Map( QObject::tr("i82559ER"), "i82559er" );
	Network_Card_PPC << Device_Map( QObject::tr("NE2000 PCI"), "ne2k_pci" );
	Network_Card_PPC << Device_Map( QObject::tr("PCnet32 PCI"), "pcnet" );
	Network_Card_PPC << Device_Map( QObject::tr("RTL8139"), "rtl8139" );
	
	Machine_SH4 << Device_Map( QObject::tr("shix card (Default)"), "" );
	Machine_SH4 << Device_Map( QObject::tr("shix card"), "shix" );
	Machine_SH4 << Device_Map( QObject::tr("r2d-plus board"), "r2d" );
	
	Network_Card_Sparc << Device_Map( QObject::tr("Default"), "" );
	Network_Card_Sparc << Device_Map( QObject::tr("Lance"), "lance" );
	
	// Video
	Video_Card_Standart << Device_Map( QObject::tr("Default"), "" );
	Video_Card_Standart << Device_Map( QObject::tr("No Video Card"), "-nographic" );
	
	Video_Card_x86 << Device_Map( QObject::tr("Cirrus CLGD 5446 (Default)"), "" );
	Video_Card_x86 << Device_Map( QObject::tr("No Video Card"), "-nographic" );
	Video_Card_x86 << Device_Map( QObject::tr("Standard VGA"), "-std-vga" );
	
	// Sound Cards
	Audio_Card_x86.Audio_PC_Speaker = true;
	Audio_Card_x86.Audio_sb16 = true;
	Audio_Card_x86.Audio_Adlib = true;
	Audio_Card_x86.Audio_es1370 = true;
	
	Audio_Card_PPC.Audio_sb16 = true;
	Audio_Card_PPC.Audio_Adlib = true;
	Audio_Card_PPC.Audio_es1370 = true;
	
	// QEMU 0.9.0
	Averable_Devices ad;
	ad.System = Device_Map( "IBM PC 32Bit", "qemu-system-x86" );
	ad.CPU_List << Device_Map( QObject::tr("Default"), "" );
	ad.Machine_List = Machine_x86;
	ad.Network_Card_List = Network_Card_x86_v090;
	ad.Video_Card_List = Video_Card_x86;
	ad.Audio_Card_List = Audio_Card_x86;
	ad.PSO_SMP_Count = 255;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = true;
	ad.PSO_Win2K_Hack = true;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = true;
	ad.PSO_No_ACPI = true;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_9_0 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "IBM PC 64Bit", "qemu-system-x86_64" );
	ad.CPU_List << Device_Map( QObject::tr("Default"), "" );
	ad.Machine_List = Machine_x86;
	ad.Network_Card_List = Network_Card_x86_v090;
	ad.Video_Card_List = Video_Card_x86;
	ad.Audio_Card_List = Audio_Card_x86;
	ad.PSO_SMP_Count = 255;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = true;
	ad.PSO_Win2K_Hack = true;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = true;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_9_0 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "ARM", "qemu-system-arm" );
	ad.CPU_List << Device_Map( QObject::tr("Default"), "" );
	ad.Machine_List = Machine_ARM;
	ad.Network_Card_List << Device_Map( QObject::tr("Default"), "" );
	ad.Video_Card_List = Video_Card_Standart;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_9_0 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "MIPS 32Bit", "qemu-system-mips" );
	ad.CPU_List << Device_Map( QObject::tr("Default"), "" );
	ad.Machine_List = Machine_MIPS;
	ad.Network_Card_List << Device_Map( QObject::tr("Default"), "" );
	ad.Video_Card_List = Video_Card_Standart;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_9_0 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "MIPS 32Bit EL", "qemu-system-mipsel" );
	ad.CPU_List << Device_Map( QObject::tr("Default"), "" );
	ad.Machine_List = Machine_MIPS;
	ad.Network_Card_List << Device_Map( QObject::tr("Default"), "" );
	ad.Video_Card_List = Video_Card_Standart;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_9_0 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "Power PC 32Bit", "qemu-system-ppc" );
	ad.CPU_List << Device_Map( QObject::tr("Default"), "" );
	ad.Machine_List = Machine_PPC;
	ad.Network_Card_List << Device_Map( QObject::tr("Default"), "" );
	ad.Video_Card_List = Video_Card_Standart;
	ad.Audio_Card_List = Audio_Card_PPC;
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = true;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_9_0 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "Sparc 32Bit", "qemu-system-sparc" );
	ad.CPU_List << Device_Map( QObject::tr("Default"), "" );
	ad.Machine_List << Device_Map( QObject::tr("Sun4m"), "sun4m" );
	ad.Network_Card_List << Device_Map( QObject::tr("Default"), "" );
	ad.Video_Card_List = Video_Card_Standart;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = true;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_9_0 << ad;
	
	// QEMU 0.9.1
	ad = Averable_Devices();
	ad.System = Device_Map( "IBM PC 32Bit", "qemu-system-x86" );
	ad.CPU_List = CPU_x86;
	ad.Machine_List = Machine_x86;
	ad.Network_Card_List = Network_Card_x86;
	ad.Video_Card_List = Video_Card_x86;
	ad.Audio_Card_List = Audio_Card_x86;
	ad.PSO_SMP_Count = 255;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = true;
	ad.PSO_Win2K_Hack = true;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = true;
	ad.PSO_No_ACPI = true;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_9_1 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "IBM PC 64Bit", "qemu-system-x86_64" );
	
	QList<Device_Map> CPU_x86_64 = CPU_x86;
	CPU_x86_64.insert( 1, Device_Map(QObject::tr("qemu64"), "qemu64") );
	CPU_x86_64 << Device_Map( QObject::tr("Athlon"), "athlon" );
	ad.CPU_List = CPU_x86_64;
	
	ad.Machine_List = Machine_x86;
	ad.Network_Card_List = Network_Card_x86;
	ad.Network_Card_List << Device_Map( "E1000", "e1000" );
	ad.Network_Card_List << Device_Map( "Virtio", "virtio" );
	ad.Video_Card_List = Video_Card_x86;
	ad.Audio_Card_List = Audio_Card_x86;
	ad.Audio_Card_List.Audio_GUS = true;
	ad.Audio_Card_List.Audio_AC97 = true;
	ad.PSO_SMP_Count = 255;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = true;
	ad.PSO_Win2K_Hack = true;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = true;
	ad.PSO_No_ACPI = true;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_9_1 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "ARM", "qemu-system-arm" );
	ad.CPU_List = CPU_ARM;
	
	QList<Device_Map> Machine_ARM_0_9_1;
	Machine_ARM_0_9_1 << Device_Map( QObject::tr("Integrator/CP (ARM926EJ-S)"), "integratorcp" );
	Machine_ARM_0_9_1 << Device_Map( QObject::tr("Versatile/PB (ARM926EJ-S)"), "versatilepb" );
	Machine_ARM_0_9_1 << Device_Map( QObject::tr("Versatile/AB (ARM926EJ-S)"), "versatileab" );
	Machine_ARM_0_9_1 << Device_Map( QObject::tr("RealView (ARM926EJ-S)"), "realview" );
	Machine_ARM_0_9_1 << Device_Map( QObject::tr("Akita PDA (PXA270)"), "akita" );
	Machine_ARM_0_9_1 << Device_Map( QObject::tr("Spitz PDA (PXA270)"), "spitz" );
	Machine_ARM_0_9_1 << Device_Map( QObject::tr("Borzoi PDA (PXA270)"), "borzoi" );
	Machine_ARM_0_9_1 << Device_Map( QObject::tr("Terrier PDA (PXA270)"), "terrier" );
	Machine_ARM_0_9_1 << Device_Map( QObject::tr("Palm Tungsten (OMAP310)"), "cheetah" );
	Machine_ARM_0_9_1 << Device_Map( QObject::tr("Stellaris LM3S811EVB"), "lm3s811evb" );
	Machine_ARM_0_9_1 << Device_Map( QObject::tr("Stellaris LM3S6965EVB"), "lm3s6965evb" );
	Machine_ARM_0_9_1 << Device_Map( QObject::tr("Gumstix Connex (PXA255)"), "connex" );
	Machine_ARM_0_9_1 << Device_Map( QObject::tr("Gumstix Verdex (PXA270)"), "verdex" );
	Machine_ARM_0_9_1 << Device_Map( QObject::tr("Mainstone II (PXA27x)"), "mainstone" );
	ad.Machine_List = Machine_ARM_0_9_1;
	
	ad.Network_Card_List = Network_Card_ARM;
	ad.Video_Card_List = Video_Card_Standart;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_9_1 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "Cris", "qemu-system-cris" );
	ad.CPU_List << Device_Map( QObject::tr("Default"), "" );
	ad.Machine_List << Device_Map( QObject::tr("Bare ETRAX FS board"), "bareetraxfs" );
	ad.Network_Card_List << Device_Map( QObject::tr("Default"), "" );
	ad.Video_Card_List = Video_Card_Standart;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_9_1 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "ColdFire (M68K)", "qemu-system-m68k" );
	ad.CPU_List << Device_Map( QObject::tr("Default"), "" );
	ad.Machine_List = Machine_M68K;
	ad.Network_Card_List = Network_Card_M68K;
	ad.Video_Card_List = Video_Card_Standart;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_9_1 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "MIPS", "qemu-system-mips" );
	ad.CPU_List = CPU_MIPS;
	
	QList<Device_Map> Machine_MIPS_0_9_1 = Machine_MIPS;
	Machine_MIPS_0_9_1 << Device_Map( QObject::tr("Acer Pica 61"), "pica61" );
	Machine_MIPS_0_9_1 << Device_Map( QObject::tr("MIPS MIPSsim platform"), "mipssim" );
	ad.Machine_List = Machine_MIPS_0_9_1;
	
	ad.Network_Card_List = Network_Card_MIPS;
	ad.Video_Card_List = Video_Card_Standart;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_9_1 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "MIPS EL", "qemu-system-mipsel" );
	ad.CPU_List = CPU_MIPS;
	ad.Machine_List = Machine_MIPS_0_9_1;
	ad.Network_Card_List = Network_Card_MIPS;
	ad.Video_Card_List = Video_Card_Standart;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_9_1 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "MIPS64", "qemu-system-mips64" );
	
	QList<Device_Map> CPU_MIPS64_0_9_1 = CPU_MIPS;
	CPU_MIPS64_0_9_1 << Device_Map( QObject::tr("R4000"), "R4000" );
	CPU_MIPS64_0_9_1 << Device_Map( QObject::tr("VR5432"), "VR5432" );
	CPU_MIPS64_0_9_1 << Device_Map( QObject::tr("5Kc"), "5Kc" );
	CPU_MIPS64_0_9_1 << Device_Map( QObject::tr("5Kf"), "5Kf" );
	CPU_MIPS64_0_9_1 << Device_Map( QObject::tr("20Kc"), "20Kc" );
	CPU_MIPS64_0_9_1 << Device_Map( QObject::tr("MIPS64R2-generic"), "MIPS64R2-generic" );
	ad.CPU_List = CPU_MIPS64_0_9_1;
	
	ad.Machine_List = Machine_MIPS_0_9_1;
	ad.Network_Card_List = Network_Card_MIPS;
	ad.Video_Card_List = Video_Card_Standart;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_9_1 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "MIPS64 EL", "qemu-system-mips64el" );
	ad.CPU_List = CPU_MIPS64_0_9_1;
	ad.Machine_List = Machine_MIPS_0_9_1;
	ad.Network_Card_List = Network_Card_MIPS;
	ad.Video_Card_List = Video_Card_Standart;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_9_1 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "Power PC 32Bit", "qemu-system-ppc" );
	ad.CPU_List = CPU_PPC;
	
	QList<Device_Map> Machine_PPC_0_9_1 = Machine_PPC;
	Machine_PPC_0_9_1 << Device_Map( QObject::tr("Ref405ep"), "ref405ep" );
	Machine_PPC_0_9_1 << Device_Map( QObject::tr("Taihu"), "taihu" );
	ad.Machine_List = Machine_PPC_0_9_1;
	
	ad.Network_Card_List = Network_Card_PPC;
	ad.Video_Card_List = Video_Card_Standart;
	ad.Audio_Card_List = Audio_Card_PPC;
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = true;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_9_1 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "Power PC 64Bit", "qemu-system-ppc64" );
	
	QList<Device_Map> CPU_PPC_64 = CPU_PPC;
	CPU_PPC_64 << Device_Map( "620", "620" );
	CPU_PPC_64 << Device_Map( "Trident", "Trident" );
	CPU_PPC_64 << Device_Map( "970", "970" );
	CPU_PPC_64 << Device_Map( "970fx", "970fx" );
	CPU_PPC_64 << Device_Map( "970fx_v1.0", "\"970fx_v1.0\"" );
	CPU_PPC_64 << Device_Map( "970fx_v2.0", "\"970fx_v2.0\"" );
	CPU_PPC_64 << Device_Map( "970fx_v2.1", "\"970fx_v2.1\"" );
	CPU_PPC_64 << Device_Map( "970fx_v3.0", "\"970fx_v3.0\"" );
	CPU_PPC_64 << Device_Map( "970fx_v3.1", "\"970fx_v3.1\"" );
	CPU_PPC_64 << Device_Map( "970gx", "970gx" );
	CPU_PPC_64 << Device_Map( "970mp", "970mp" );
	CPU_PPC_64 << Device_Map( "970mp_v1.0", "\"970mp_v1.0\"" );
	CPU_PPC_64 << Device_Map( "970mp_v1.1", "\"970mp_v1.1\"" );
	CPU_PPC_64 << Device_Map( "PPC64", "ppc64" );
	ad.CPU_List = CPU_PPC_64;
	
	ad.Machine_List = Machine_PPC_0_9_1;
	ad.Network_Card_List = Network_Card_PPC;
	ad.Video_Card_List = Video_Card_Standart;
	ad.Audio_Card_List = Audio_Card_PPC;
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = true;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_9_1 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "Power PC EMB 32Bit", "qemu-system-ppcemb" );
	ad.CPU_List = CPU_PPC;
	ad.Machine_List = Machine_PPC_0_9_1;
	ad.Network_Card_List = Network_Card_PPC;
	ad.Video_Card_List = Video_Card_Standart;
	ad.Audio_Card_List = Audio_Card_PPC;
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = true;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_9_1 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "SH4", "qemu-system-sh4" );
	ad.CPU_List << Device_Map( QObject::tr("Default"), "" );;
	ad.Machine_List = Machine_SH4;
	ad.Network_Card_List << Device_Map( QObject::tr("Default"), "" );;
	ad.Video_Card_List = Video_Card_Standart;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_9_1 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "SH4 EB", "qemu-system-sh4eb" );
	ad.CPU_List << Device_Map( QObject::tr("Default"), "" );;
	ad.Machine_List = Machine_SH4;
	ad.Network_Card_List << Device_Map( QObject::tr("Default"), "" );;
	ad.Video_Card_List = Video_Card_Standart;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_9_1 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "Sparc 32Bit", "qemu-system-sparc" );
	ad.CPU_List = CPU_Sparc;
	ad.Machine_List = Machine_Sparc;
	ad.Network_Card_List = Network_Card_Sparc;
	ad.Video_Card_List = Video_Card_Standart;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = true;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = true;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_9_1 << ad;
	
	// QEMU 0.10.0
	QList<Device_Map> QEMU_Video_Cards_v0_10_0;
	QEMU_Video_Cards_v0_10_0 << Device_Map( QObject::tr("Default"), "" );
	QEMU_Video_Cards_v0_10_0 << Device_Map( QObject::tr("StdVGA (VESA 2.0)"), "std" );
	QEMU_Video_Cards_v0_10_0 << Device_Map( QObject::tr("Cirrus CLGD 5446"), "cirrus" );
	QEMU_Video_Cards_v0_10_0 << Device_Map( QObject::tr("VMWare Video Card"), "vmware" );
	QEMU_Video_Cards_v0_10_0 << Device_Map( QObject::tr("None Video Card"), "none" );
	
	QList<Device_Map> CPU_x86_v0_10_0 = CPU_x86;
	CPU_x86_v0_10_0 << Device_Map( QObject::tr("Core Duo"), "coreduo" );
	CPU_x86_v0_10_0 << Device_Map( QObject::tr("Athlon"), "athlon" );
	CPU_x86_v0_10_0 << Device_Map( QObject::tr("Atom N270"), "n270" );
	
	QList<Device_Map> Network_Card_v0_10_0 = Network_Card_x86;
	Network_Card_v0_10_0 << Device_Map( QObject::tr("e1000"), "e1000" );
	Network_Card_v0_10_0 << Device_Map( QObject::tr("virtio"), "virtio" );
	
	ad = Averable_Devices();
	ad.System = Device_Map( "IBM PC 32Bit", "qemu-system-x86" );
	ad.CPU_List = CPU_x86_v0_10_0;
	ad.Machine_List = Machine_x86;
	ad.Network_Card_List = Network_Card_v0_10_0;
	ad.Video_Card_List = QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = Audio_Card_x86;
	ad.PSO_SMP_Count = 255;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = true;
	ad.PSO_Win2K_Hack = true;
	ad.PSO_RTC_TD_Hack = true;
	ad.PSO_Kernel_KQEMU = true;
	ad.PSO_No_ACPI = true;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_10 << ad;
	
	QList<Device_Map> CPU_x86_64_v0_10_0 = CPU_x86_v0_10_0;
	CPU_x86_64_v0_10_0 << Device_Map( QObject::tr("Phenom"), "phenom" );
	CPU_x86_64_v0_10_0 << Device_Map( QObject::tr("Core 2 Duo"), "core2duo" );
	CPU_x86_64_v0_10_0 << Device_Map( QObject::tr("QEMU 64Bit"), "qemu64" );
	
	ad = Averable_Devices();
	ad.System = Device_Map( "IBM PC 64Bit", "qemu-system-x86_64" );
	ad.CPU_List = CPU_x86_64_v0_10_0;
	ad.Machine_List = Machine_x86;
	ad.Network_Card_List = Network_Card_v0_10_0;
	ad.Video_Card_List = QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = Audio_Card_x86;
	ad.Audio_Card_List.Audio_GUS = true;
	ad.Audio_Card_List.Audio_AC97 = true;
	ad.PSO_SMP_Count = 255;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = true;
	ad.PSO_Win2K_Hack = true;
	ad.PSO_RTC_TD_Hack = true;
	ad.PSO_Kernel_KQEMU = true;
	ad.PSO_No_ACPI = true;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_10 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "ARM", "qemu-system-arm" );
	
	CPU_ARM.insert( 1, Device_Map(QObject::tr("ARM 1136 R2"), "arm1136-r2") );
	ad.CPU_List = CPU_ARM;
	
	QList<Device_Map> Machine_ARM_0_10_0;
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Integrator/CP (ARM926EJ-S)"), "integratorcp" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Versatile/PB (ARM926EJ-S)"), "versatilepb" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Versatile/AB (ARM926EJ-S)"), "versatileab" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("RealView (ARM926EJ-S)"), "realview" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Akita PDA (PXA270)"), "akita" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Spitz PDA (PXA270)"), "spitz" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Borzoi PDA (PXA270)"), "borzoi" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Terrier PDA (PXA270)"), "terrier" );
	
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Siemens SX1 (OMAP310) V1"), "sx1-v1" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Siemens SX1 (OMAP310) V2"), "sx1" );
	
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Palm Tungsten (OMAP310)"), "cheetah" );
	
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Nokia N800"), "n800" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Nokia N810"), "n810" );
	
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Stellaris LM3S811EVB"), "lm3s811evb" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Stellaris LM3S6965EVB"), "lm3s6965evb" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Gumstix Connex (PXA255)"), "connex" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Gumstix Verdex (PXA270)"), "verdex" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Mainstone II (PXA27x)"), "mainstone" );
	
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Marvell 88w8618 / MusicPal"), "musicpal" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Tosa PDA (PXA255)"), "tosa" );
	
	ad.Machine_List = Machine_ARM_0_10_0;
	
	ad.Network_Card_List = Network_Card_ARM;
	ad.Video_Card_List = QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_10 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "Cris", "qemu-system-cris" );
	ad.CPU_List << Device_Map( QObject::tr("Default"), "" );
	ad.Machine_List << Device_Map( QObject::tr("Bare ETRAX FS board"), "bareetraxfs" );
	ad.Machine_List << Device_Map( QObject::tr("AXIS devboard 88"), "axis-dev88" );
	ad.Network_Card_List << Device_Map( QObject::tr("fseth (Default)"), "" );
	ad.Network_Card_List << Device_Map( QObject::tr("fseth"), "fseth" );
	ad.Video_Card_List = QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_10 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "ColdFire (M68K)", "qemu-system-m68k" );
	ad.CPU_List << Device_Map( QObject::tr("Default"), "" );
	ad.Machine_List << Device_Map( QObject::tr("Default (MCF5206EVB)"), "" );
	ad.Machine_List << Device_Map( QObject::tr("MCF5206EVB"), "mcf5208evb" );
	ad.Machine_List << Device_Map( QObject::tr("Arnewsh 5206"), "an5206 " );
	ad.Machine_List << Device_Map( QObject::tr("Dummy board"), "dummy" );
	ad.Network_Card_List = Network_Card_M68K;
	ad.Video_Card_List = QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_10 << ad;
	
	QList<Device_Map> Machine_MIPS_v0_10_0;
	Machine_MIPS << Device_Map( QObject::tr("Default (MIPS Malta Core LV)"), "" );
	Machine_MIPS << Device_Map( QObject::tr("MIPS Malta Core LV"), "malta" );
	Machine_MIPS << Device_Map( QObject::tr("MIPS Magnum"), "magnum" );
	Machine_MIPS << Device_Map( QObject::tr("Acer Pica 61"), "pica61" );
	Machine_MIPS << Device_Map( QObject::tr("MIPS MIPSsim platform"), "mipssim" );
	Machine_MIPS << Device_Map( QObject::tr("mips r4k platform"), "mips" );
	
	ad = Averable_Devices();
	ad.System = Device_Map( "MIPS", "qemu-system-mips" );
	ad.CPU_List = CPU_MIPS;
	ad.Machine_List = Machine_MIPS_v0_10_0;
	ad.Network_Card_List = Network_Card_MIPS;
	ad.Video_Card_List = QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_10 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "MIPS EL", "qemu-system-mipsel" );
	ad.CPU_List = CPU_MIPS;
	ad.Machine_List = Machine_MIPS_v0_10_0;
	ad.Network_Card_List = Network_Card_MIPS;
	ad.Video_Card_List = QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_10 << ad;
	
	QList<Device_Map> MIPS_CPU_64Bit = CPU_MIPS;
	MIPS_CPU_64Bit << Device_Map( QObject::tr("MIPS R4000"), "R4000" );
	MIPS_CPU_64Bit << Device_Map( QObject::tr("MIPS VR5432"), "VR5432" );
	MIPS_CPU_64Bit << Device_Map( QObject::tr("MIPS 5Kc"), "5Kc" );
	MIPS_CPU_64Bit << Device_Map( QObject::tr("MIPS 5Kf"), "5Kf" );
	MIPS_CPU_64Bit << Device_Map( QObject::tr("MIPS 20Kc"), "20Kc" );
	MIPS_CPU_64Bit << Device_Map( QObject::tr("MIPS MIPS64R2-generic"), "MIPS64R2-generic" );
	
	ad = Averable_Devices();
	ad.System = Device_Map( "MIPS64", "qemu-system-mips64" );
	ad.CPU_List = MIPS_CPU_64Bit;
	ad.Machine_List = Machine_MIPS_v0_10_0;
	ad.Network_Card_List = Network_Card_MIPS;
	ad.Video_Card_List = QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_10 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "MIPS64 EL", "qemu-system-mips64el" );
	ad.CPU_List = MIPS_CPU_64Bit;
	ad.Machine_List = Machine_MIPS_v0_10_0;
	ad.Network_Card_List = Network_Card_MIPS;
	ad.Video_Card_List = QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_10 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "Power PC 32Bit", "qemu-system-ppc" );
	
	CPU_PPC.insert( 121, Device_Map(QObject::tr("e500v1"), "e500v1") );
	ad.CPU_List = CPU_PPC;
	
	QList<Device_Map> Machine_PPC_0_10_0;
	Machine_PPC_0_10_0 << Device_Map( QObject::tr("Heathrow PowerMAC (Default)"), "" );
	Machine_PPC_0_10_0 << Device_Map( QObject::tr("Heathrow PowerMAC"), "g3beige" );
	Machine_PPC_0_10_0 << Device_Map( QObject::tr("Mac99 based PowerMAC"), "mac99" );
	Machine_PPC_0_10_0 << Device_Map( QObject::tr("PowerPC PREP platform"), "prep" );
	Machine_PPC_0_10_0 << Device_Map( QObject::tr("ref405ep"), "ref405ep" );
	Machine_PPC_0_10_0 << Device_Map( QObject::tr("taihu"), "taihu" );
	Machine_PPC_0_10_0 << Device_Map( QObject::tr("bamboo "), "bamboo " );
	Machine_PPC_0_10_0 << Device_Map( QObject::tr("mpc8544ds"), "mpc8544ds" );
	ad.Machine_List = Machine_PPC_0_10_0;
	
	ad.Network_Card_List = Network_Card_v0_10_0;
	ad.Video_Card_List = QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = Audio_Card_PPC;
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = true;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_10 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "Power PC EMB 32Bit", "qemu-system-ppcemb" );
	ad.CPU_List = CPU_PPC;
	ad.Machine_List = Machine_PPC_0_10_0;
	ad.Network_Card_List = Network_Card_PPC;
	ad.Video_Card_List = QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = Audio_Card_PPC;
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = true;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_10 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "Power PC 64Bit", "qemu-system-ppc64" );
	
	QList<Device_Map> CPU_PPC_64_v0_10_0 = CPU_PPC;
	CPU_PPC_64 << Device_Map( "620", "620" );
	CPU_PPC_64 << Device_Map( "Trident", "Trident" );
	CPU_PPC_64 << Device_Map( "970", "970" );
	CPU_PPC_64 << Device_Map( "970fx", "970fx" );
	CPU_PPC_64 << Device_Map( "970fx_v1.0", "\"970fx_v1.0\"" );
	CPU_PPC_64 << Device_Map( "970fx_v2.0", "\"970fx_v2.0\"" );
	CPU_PPC_64 << Device_Map( "970fx_v2.1", "\"970fx_v2.1\"" );
	CPU_PPC_64 << Device_Map( "970fx_v3.0", "\"970fx_v3.0\"" );
	CPU_PPC_64 << Device_Map( "970fx_v3.1", "\"970fx_v3.1\"" );
	CPU_PPC_64 << Device_Map( "970gx", "970gx" );
	CPU_PPC_64 << Device_Map( "970mp", "970mp" );
	CPU_PPC_64 << Device_Map( "970mp_v1.0", "\"970mp_v1.0\"" );
	CPU_PPC_64 << Device_Map( "970mp_v1.1", "\"970mp_v1.1\"" );
	CPU_PPC_64 << Device_Map( "PPC64", "ppc64" );
	ad.CPU_List = CPU_PPC_64_v0_10_0;
	
	ad.Machine_List = Machine_PPC_0_10_0;
	ad.Network_Card_List = Network_Card_PPC;
	ad.Video_Card_List = QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = Audio_Card_PPC;
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = true;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_10 << ad;
	
	QList<Device_Map> CPU_SH4;
	CPU_SH4 << Device_Map( QObject::tr("Default"), "" );
	CPU_SH4 << Device_Map( QObject::tr("SH7750R"), "SH7750R" );
	CPU_SH4 << Device_Map( QObject::tr("SH7751R"), "SH7751R" );
	CPU_SH4 << Device_Map( QObject::tr("SH7785"), "SH7785" );
	
	ad = Averable_Devices();
	ad.System = Device_Map( "SH4", "qemu-system-sh4" );
	ad.CPU_List = CPU_SH4;
	ad.Machine_List = Machine_SH4;
	ad.Network_Card_List << Device_Map( QObject::tr("Default"), "" );
	ad.Video_Card_List = QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_10 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "SH4 EB", "qemu-system-sh4eb" );
	ad.CPU_List = CPU_SH4;
	ad.Machine_List = Machine_SH4;
	ad.Network_Card_List << Device_Map( QObject::tr("Default"), "" );
	ad.Video_Card_List = QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_SMP_Count = 1;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_10 << ad;
	
	ad = Averable_Devices();
	ad.System = Device_Map( "Sparc 32Bit", "qemu-system-sparc" );
	ad.CPU_List = CPU_Sparc;
	
	ad.Machine_List = Machine_Sparc;
	ad.Machine_List << Device_Map( QObject::tr("SPARCstation Voyager (Sun4m)"), "Voyager" );
	ad.Machine_List << Device_Map( QObject::tr("SPARCstation LX (Sun4m)"), "LX" );
	ad.Machine_List << Device_Map( QObject::tr("SPARCstation 4 (Sun4m)"), "SS-4" );
	ad.Machine_List << Device_Map( QObject::tr("SPARCClassic (Sun4m)"), "SPARCClassic" );
	ad.Machine_List << Device_Map( QObject::tr("SPARCbook (Sun4m)"), "SPARCbook" );
	
	ad.Network_Card_List = Network_Card_Sparc;
	ad.Video_Card_List = QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_SMP_Count = 4;
	ad.PSO_Initial_Graphic_Mode = true;
	ad.PSO_No_FB_Boot_Check = false;
	ad.PSO_Win2K_Hack = false;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = false;
	ad.PSO_PROM_ENV = true;
	ad.PSO_KVM = false;
	System_Info::Emulator_QEMU_0_10 << ad;
	
	// KVM Old
	ad = Averable_Devices();
	ad.System = Device_Map( "KVM (Intel VT/AMD SVM)", "qemu-kvm" );
	ad.CPU_List << Device_Map( QObject::tr("Default"), "" );
	ad.Machine_List = Machine_x86;
	ad.Network_Card_List << Device_Map( QObject::tr("Default"), "" );
	ad.Video_Card_List = Video_Card_x86;
	ad.Audio_Card_List = Audio_Card_x86;
	ad.PSO_SMP_Count = 255;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = true;
	ad.PSO_Win2K_Hack = true;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = true;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = true;
	System_Info::Emulator_KVM_Old << ad;
	
	// KVM 7X
	ad = Averable_Devices();
	ad.System = Device_Map( "KVM (Intel VT/AMD SVM)", "qemu-kvm" );
	ad.CPU_List = CPU_x86_64_v0_10_0;
	ad.Machine_List = Machine_x86;
	ad.Network_Card_List = Network_Card_x86;
	ad.Network_Card_List << Device_Map( "E1000", "e1000" );
	ad.Network_Card_List << Device_Map( "Virtio", "virtio" );
	ad.Video_Card_List = Video_Card_x86;
	ad.Audio_Card_List = Audio_Card_x86;
	ad.Audio_Card_List.Audio_GUS = true;
	ad.Audio_Card_List.Audio_AC97 = true;
	ad.PSO_SMP_Count = 255;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = true;
	ad.PSO_Win2K_Hack = true;
	ad.PSO_RTC_TD_Hack = false;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = true;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = true;
	System_Info::Emulator_KVM_7X << ad;
	
	// KVM 8X
	ad = Averable_Devices();
	ad.System = Device_Map( "KVM (Intel VT/AMD SVM)", "qemu-kvm" );
	ad.CPU_List = CPU_x86_64_v0_10_0;
	ad.Machine_List = Machine_x86;
	ad.Network_Card_List = Network_Card_v0_10_0;
	ad.Video_Card_List = QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = Audio_Card_x86;
	ad.Audio_Card_List.Audio_GUS = true;
	ad.Audio_Card_List.Audio_AC97 = true;
	ad.PSO_SMP_Count = 255;
	ad.PSO_Initial_Graphic_Mode = false;
	ad.PSO_No_FB_Boot_Check = true;
	ad.PSO_Win2K_Hack = true;
	ad.PSO_RTC_TD_Hack = true;
	ad.PSO_Kernel_KQEMU = false;
	ad.PSO_No_ACPI = true;
	ad.PSO_PROM_ENV = false;
	ad.PSO_KVM = true;
	System_Info::Emulator_KVM_8X << ad;
	
	return true;
}

QString System_Info::Get_QEMU_Version( const QString &path )
{
	QSettings settings;
	
	if( path.isEmpty() )
	{
		AQWarning( "QString System_Info::Get_QEMU_Version( const QString &path )",
				   "emulator_path is Empty" );
		return "0.9.0";
	}
	
	QProcess *qemu_pr = new QProcess();
	
	qemu_pr->start( path, QStringList("-h") );
	
	if( ! qemu_pr->waitForFinished(1000) )
	{
		qemu_pr->kill();
		delete qemu_pr;
		qemu_pr = new QProcess();
		
		qemu_pr->start( path, QStringList("-nographic") );
		
		if( ! qemu_pr->waitForFinished(1000) )
		{
			AQError( "QString System_Info::Get_QEMU_Version( const QString &path )",
					 "QEMU Not Finished!" );
			qemu_pr->terminate();
			delete qemu_pr;
			return "0.9.0";
		}
	}
	
	QRegExp qemu_ver = QRegExp( ".*version\\s+([\\d]+)[.]([\\d]+)[.]([\\d]+).*" );
	
	bool find_done = false;
	
	for( int ix = 0; ix < 5; ++ix )
	{
		QString result = qemu_pr->readLine();
		
		if( result.isEmpty() )
		{
			AQError( "QString System_Info::Get_QEMU_Version( const QString &path )",
					 "Result String is Empty!" );
		}
		
		if( ! qemu_ver.exactMatch(result) )
		{
			AQError( "QString System_Info::Get_QEMU_Version( const QString &path )",
					 "Cannot Match RegExp!\nData: " + result );
		}
		else
		{
			find_done = true;
			break;
		}
	}
	
	delete qemu_pr;
	
	if( find_done == false )
	{
		AQError( "QString System_Info::Get_QEMU_Version( const QString &path )",
				 "Cannot Find QEMU Version!" );
		return "0.9.0";
	}
	
	QStringList version_lines = qemu_ver.capturedTexts();
	
	int major_ver, minor_ver, bugfix_ver;
	
	bool ok1, ok2, ok3;
	
	major_ver = version_lines[1].toInt( &ok1, 10 );
	minor_ver = version_lines[2].toInt( &ok2, 10 );
	bugfix_ver = version_lines[3].toInt( &ok3, 10 );
	
	if( ! (ok1 && ok2 && ok3) )
	{
		AQError( "QString System_Info::Get_QEMU_Version( const QString &path )",
				 "Cannot Convert to Int!" );
		return "0.9.0";
	}
	
	if( major_ver == 0 && minor_ver < 9 )
	{
		return "Old";
	}
	else if( major_ver == 0 && minor_ver == 9 && bugfix_ver == 0 )
	{
		return "0.9.0";
	}
	else if( major_ver == 0 && minor_ver == 9 && bugfix_ver == 1 )
	{
		return  "0.9.1";
	}
	else if( major_ver == 0 && minor_ver == 10 && bugfix_ver >= 0 && bugfix_ver <= 5 )
	{
		return "0.10";
	}
	else if(  major_ver >  0 ||
			 (major_ver == 0 && minor_ver > 10) ||
			 (major_ver == 0 && minor_ver == 10 && bugfix_ver > 5) )
	{
		return "New";
	}
	else
	{
		AQError( "QString System_Info::Get_QEMU_Version( const QString &path )",
				 "Version not Checked!" );
		return "0.9.0";
	}
}

QString System_Info::Get_KVM_Version( const QString &path )
{
	if( path.isEmpty() )
	{
		AQWarning( "QString System_Info::Get_KVM_Version( const QString &path )",
				   "emulator_path is Empty" );
		return "7X";
	}
	
	QProcess *kvm_pr = new QProcess();
	
	kvm_pr->start( path, QStringList("-h") );
	//kvm_pr->start( path, QStringList("-nographic") ); // FIXME
	
	if( ! kvm_pr->waitForFinished(1000) )
	{
		// Alternative
		kvm_pr->kill();
		delete kvm_pr;
		kvm_pr = new QProcess();
		
		kvm_pr->start( path, QStringList("-version") );
		
		if( ! kvm_pr->waitForFinished(1000) )
		{
			AQError( "QString System_Info::Get_KVM_Version( const QString &path )",
					 "KVM Not Finished!" );
			kvm_pr->terminate();
			delete kvm_pr;
			return "7X";
		}
	}
	
	QRegExp kvm_ver = QRegExp( ".*version.*kvm-([\\d]+).*" );
	QRegExp kvm_ver_v2 = QRegExp( ".*version.*\\(.*([\\d]+)\\).*" );
	QStringList version_lines;
	
	for( int ix = 0; ix < 5; ++ix )
	{
		QString result = kvm_pr->readLine();
		
		if( result.isEmpty() )
		{
			AQWarning( "QString System_Info::Get_KVM_Version( const QString &path )",
					   "Result String is Empty!" );
			continue;
		}
		
		if( ! kvm_ver.exactMatch(result) )
		{
			if( ! kvm_ver_v2.exactMatch(result) )
			{
				AQError( "QString System_Info::Get_KVM_Version( const QString &path )",
						 "Cannot Match RegExp!\nData: " + result );
				
				AQError( "QString System_Info::Get_KVM_Version( const QString &path )",
						 "Cannot Find KVM Version!" );
				
				delete kvm_pr;
				
				return "7X";
			}
			else
			{
				version_lines = kvm_ver_v2.capturedTexts();
				break;
				
			}
		}
		else
		{
			version_lines = kvm_ver.capturedTexts();
			break;
		}
	}
	
	delete kvm_pr;
	
	int kvm_version;
	bool ok = false;
	
	kvm_version = version_lines[1].toInt( &ok, 10 );
	
	if( ! ok )
	{
		AQError( "QString System_Info::Get_KVM_Version( const QString &path )",
				 "Cannot Convert to Int!" );
		return "7X";
	}
	
	if( kvm_version < 70 )
	{
		return "Old";
	}
	if( kvm_version >= 70 && kvm_version < 80 )
	{
		return "7X";
	}
	else if( kvm_version >= 80 && kvm_version <= 88 )
	{
		return "8X";
	}
	else if( kvm_version > 88 )
	{
		return "New";
	}
	else
	{
		AQError( "QString System_Info::Get_KVM_Version( const QString &path )",
				 "Version not Checked!" );
		return "7X";
	}
}

QStringList System_Info::Find_QEMU_Binary_Files( const QString &path )
{
	QStringList find_names, return_list;
	find_names << "qemu";
	find_names << "qemu-img";
	find_names << "qemu-system-arm";
	find_names << "qemu-system-cris";
	find_names << "qemu-system-m68k";
	find_names << "qemu-system-mips";
	find_names << "qemu-system-mipsel";
	find_names << "qemu-system-mips64";
	find_names << "qemu-system-mips64el";
	find_names << "qemu-system-ppc";
	find_names << "qemu-system-ppc64";
	find_names << "qemu-system-ppcemb";
	find_names << "qemu-system-sh4";
	find_names << "qemu-system-sh4eb";
	find_names << "qemu-system-sparc";
	find_names << "qemu-system-x86_64";
	
	// Init List
	for( int ix = 0; ix < find_names.count(); ix++ ) return_list << QString();
	
	if( path.isEmpty() )
	{
		AQError( "QStringList System_Info::Find_QEMU_Binary_Files( const QString &path )",
				 "Path is Empty!" );
		return QStringList();
	}
	
	QDir bin_dir = QDir( path );
	
	for( int ix = 0; ix < find_names.count(); ix++ )
	{
		QStringList tmp_list = bin_dir.entryList( QStringList(find_names[ix]) );
		
		if( tmp_list.count() > 0 )
		{
			return_list[ ix ] = tmp_list[ 0 ];
		}
	}
	
	return return_list;
}

QStringList System_Info::Find_KVM_Binary_Files( const QString &path )
{
	QStringList kvm_names, kvm_img_names, return_list;
	kvm_names << "kvm";
	kvm_names << "qemu-kvm";
	kvm_img_names << "kvm-img";
	kvm_img_names << "qemu-img";
	return_list << QString();
	return_list << QString();
	
	if( path.isEmpty() )
	{
		AQError( "QStringList System_Info::Find_KVM_Binary_Files( const QString &path )",
				 "Path is Empty!" );
		return return_list;
	}
	
	QDir bin_dir = QDir( path );
	
	for( int ix = 0; ix < kvm_names.count(); ix++ )
	{
		QStringList tmp_list = bin_dir.entryList( QStringList(kvm_names[ix]) );
		
		if( tmp_list.count() > 0 )
		{
			return_list[ 0 ] = tmp_list[ 0 ];
			break;
		}
	}
	
	for( int ix = 0; ix < kvm_img_names.count(); ix++ )
	{
		QStringList tmp_list = bin_dir.entryList( QStringList(kvm_img_names[ix]) );
		
		if( tmp_list.count() > 0 )
		{
			return_list[ 1 ] =  tmp_list[ 0 ];
			break;
		}
	}
	
	return return_list;
}

const QList<VM_USB> &System_Info::Get_All_Host_USB()
{
	if( All_Host_USB.count() <= 0 ) Update_Host_USB();
	
	return All_Host_USB;
}

const QList<VM_USB> &System_Info::Get_Used_USB_List()
{
	return Used_Host_USB;
}

bool System_Info::Add_To_Used_USB_List( const VM_USB &device )
{
	if( Used_Host_USB.indexOf(device) == -1 ) Used_Host_USB << device;
	else return false;
}

bool System_Info::Delete_From_Used_USB_List( const VM_USB &device )
{
	return Used_Host_USB.removeOne( device );
}

bool System_Info::Update_Host_USB()
{
	QList<VM_USB> list;
	
	if( QFile::exists("/sys/bus/usb") )
	{
		if( Scan_USB_Sys(list) )
		{
			All_Host_USB = list;
			return true;
		}
	}
	
	if( QFile::exists("/proc/bus/usb/devices") )
	{
		if( Scan_USB_Proc(list) )
		{
			All_Host_USB = list;
			return true;
		}
	}
	
	// Error...
	AQError( "bool System_Info::Update_Host_USB()",
			 "Cannot read USB information from /sys, /proc, /dev!" );
	return false;
}

bool System_Info::Scan_USB_Sys( QList<VM_USB> &list )
{
	list.clear();
	QDir dir("/sys/bus/usb/devices/");
	
	QStringList usb_dirs = dir.entryList( QStringList("usb*"), QDir::Dirs, QDir::Name );
	
	if( usb_dirs.isEmpty() )
	{
		AQWarning( "bool System_Info::Scan_USB_Sys( QList<VM_USB> &list )",
				   "No USB Folders in /sys/bus/usb/devices/" );
		return false;
	}
	
	for( int dx = 0; dx < usb_dirs.count(); dx++ )
	{
		QString usb_path = "/sys/bus/usb/devices/" + usb_dirs[ dx ] + "/";
		QString data = "";
		VM_USB tmp_usb;
		
		// idVendor
		if( Read_SysFS_File(usb_path + "idVendor", data) )
		{
			tmp_usb.Set_Vendor_ID( data );
		}
		else
		{
			AQError( "bool System_Info::Scan_USB_Sys( QList<VM_USB> &list )",
					 "" );
			continue;
		}
		
		// idProduct
		if( Read_SysFS_File(usb_path + "idProduct", data) )
		{
			tmp_usb.Set_Product_ID( data );
		}
		else
		{
			AQError( "bool System_Info::Scan_USB_Sys( QList<VM_USB> &list )",
					 "" );
			continue;
		}
		
		// manufacturer
		if( Read_SysFS_File(usb_path + "manufacturer", data) )
		{
			tmp_usb.Set_Manufacturer_Name( data );
		}
		else
		{
			AQError( "bool System_Info::Scan_USB_Sys( QList<VM_USB> &list )",
					 "" );
			continue;
		}
		
		// product
		if( Read_SysFS_File(usb_path + "product", data) )
		{
			tmp_usb.Set_Product_Name( data );
		}
		else
		{
			AQError( "bool System_Info::Scan_USB_Sys( QList<VM_USB> &list )",
					 "" );
			continue;
		}
		
		// speed
		if( Read_SysFS_File(usb_path + "speed", data) )
		{
			tmp_usb.Set_Speed( data.toInt() );
		}
		else
		{
			AQError( "bool System_Info::Scan_USB_Sys( QList<VM_USB> &list )",
					 "" );
			continue;
		}
		
		// BusAddr
		if( Read_SysFS_File(usb_path + "busnum", data) )
		{
			QString busAddr = data;
			
			if( Read_SysFS_File(usb_path + "devnum", data) )
			{
				busAddr += "." + data;
				tmp_usb.Set_BusAddr( busAddr );
			}
			else
			{
				AQError( "bool System_Info::Scan_USB_Sys( QList<VM_USB> &list )",
						 "" );
				continue;
			}
		}
		else
		{
			AQError( "bool System_Info::Scan_USB_Sys( QList<VM_USB> &list )",
					 "" );
			continue;
		}
		
		// All Data Read
		tmp_usb.Set_Use_Host_Device( true );
		list << tmp_usb;
	}
	
	return true;
}

bool System_Info::Read_SysFS_File( const QString &path, QString &data )
{
	QFile sysfs_file( path );
	
	if( ! sysfs_file.open(QIODevice::ReadOnly | QIODevice::Text) )
	{
		AQError( "bool System_Info::Read_SysFS_File( const QString &path, QString &data )",
				 QString("Cannot Open File \"%1\"! USB information not found!").arg(path) );
		return false;
	}
	
	QTextStream in( &sysfs_file );
	QString line = in.readAll();
	sysfs_file.close();
	
	if( line.isEmpty() )
	{
		AQWarning( "bool System_Info::Read_SysFS_File( const QString &path, QString &data )",
				   "File is Empty!" );
		return false;
	}
	else
	{
		line = line.remove( "\n" );
		data = line;
		return true;
	}
}

bool System_Info::Scan_USB_Proc( QList<VM_USB> &list )
{
	list.clear();
	QFile proc_file( "/proc/bus/usb/devices" );
	
	if( ! proc_file.open(QIODevice::ReadOnly | QIODevice::Text) )
	{
		AQError( "bool System_Info::Scan_USB_Proc( QList<VM_USB> &list )",
				 "Cannot Open File \"/proc/bus/usb/devices\"! USB information not found!" );
		return false;
	}
	
	QTextStream in( &proc_file );
	QList<QStringList> linux_usb_dev;
	
	QString line = in.readLine();
	
	while( ! line.isNull() )
	{
		QStringList tmp_str;
		
		if( line.startsWith("T:") )
		{
			tmp_str << line;
			
			while( ! in.atEnd() )
			{
				line = in.readLine();
				
				if( line.startsWith("P:") ||
					line.startsWith("S:  Manufacturer") ||
					line.startsWith("S:  Product") ) tmp_str << line;
				else if( line.startsWith("E:") ) break;
			}
			
			linux_usb_dev << tmp_str;
		}
		
		line = in.readLine();
	}
	
	proc_file.close();
	
	for( int ex = 0; ex < linux_usb_dev.count(); ++ex )
	{
		if( linux_usb_dev[ex].count() < 4 )
		{
			AQWarning( "bool System_Info::Scan_USB_Proc( QList<VM_USB> &list )",
					   "linux_usb_dev[ex].count() < 4" );
		}
	}
	
	// T:  Bus=01 Lev=00 Prnt=00 Port=00 Cnt=00 Dev#=  1 Spd=480 MxCh= 6
	QRegExp busAddr = QRegExp( "T:[\\s]+Bus=([\\d]{2}).+Dev#=[\\s]*([\\d]+).+Spd=([\\d]{1,3}).*" );
	
	// P:  Vendor=0123 ProdID=abcd Rev= 0.01
	QRegExp idHex = QRegExp( "P:[\\s]+Vendor=([\\dabcdef]{4})[\\s]+ProdID=([\\dabcdef]{4}).+" );
	
	// S:  Manufacturer=Mega Cool Manufacturer
	QRegExp manufacturer = QRegExp( "S:[\\s]+Manufacturer=(.+)" );
	
	// S:  Product=Super USB Device
	QRegExp product = QRegExp( "S:[\\s]+Product=(.+)" );
	
	for( int ix = 0; ix < linux_usb_dev.count(); ix++ )
	{
		if( linux_usb_dev[ix].count() <= 0 )
		{
			AQWarning( "bool System_Info::Scan_USB_Proc( QList<VM_USB> &list )",
					   "linux_usb_dev[ix].count() <= 0" );
			continue;
		}
		
		QStringList busAddr_list;
		QStringList idHex_list;
		QStringList manufacturer_list;
		QStringList product_list;
		
		for( int bx = 0; bx < linux_usb_dev[ix].count(); bx++ )
		{
			if( busAddr_list.count() <= 0 )
			{
				if( busAddr.exactMatch(linux_usb_dev[ix][bx]) )
				{
					busAddr_list = busAddr.capturedTexts();
					continue;
				}
				else
				{
					AQError( "bool System_Info::Scan_USB_Proc( QList<VM_USB> &list )",
							 "Cannot Match busAddr! String: " + linux_usb_dev[ix][bx] );
				}
			}
			
			if( idHex_list.count() <= 0 )
			{
				if( idHex.exactMatch(linux_usb_dev[ix][bx]) )
				{
					idHex_list = idHex.capturedTexts();
					continue;
				}
				else
				{
					AQError( "bool System_Info::Scan_USB_Proc( QList<VM_USB> &list )",
							 "Cannot Match idHex! String: " + linux_usb_dev[ix][bx] );
				}
			}
			
			if( manufacturer_list.count() <= 0 )
			{
				if( manufacturer.exactMatch(linux_usb_dev[ix][bx]) )
				{
					manufacturer_list = manufacturer.capturedTexts();
					continue;
				}
				else
				{
					AQError( "bool System_Info::Scan_USB_Proc( QList<VM_USB> &list )",
							 "Cannot Match manufacturer! String: " + linux_usb_dev[ix][bx] );
				}
			}
			
			if( product_list.count() <= 0 )
			{
				if( product.exactMatch(linux_usb_dev[ix][bx]) )
				{
					product_list = product.capturedTexts();
					continue;
				}
				else
				{
					AQError( "bool System_Info::Scan_USB_Proc( QList<VM_USB> &list )",
							 "Cannot Match product! String: " + linux_usb_dev[ix][bx] );
				}
			}
		}
		
		// Create VM_USB
		if( idHex_list.count() > 1 )
		{
			if( idHex_list[1] != "0000" && idHex_list[2] != "0000" )
			{
				VM_USB tmp_usb;
				
				tmp_usb.Set_Vendor_ID( idHex_list[1] );
				tmp_usb.Set_Product_ID( idHex_list[2] );
				
				if( manufacturer_list.count() > 0 )
				{
					tmp_usb.Set_Manufacturer_Name( manufacturer_list[1] );
				}
				
				if( product_list.count() > 0 )
				{
					tmp_usb.Set_Product_Name( product_list[1] );
				}
				
				if( busAddr_list.count() >= 3 )
				{
					QString busStr = (busAddr_list[1][0] == '0') ? QString(busAddr_list[1][1]) : busAddr_list[1];
					tmp_usb.Set_BusAddr( busStr + ":" + busAddr_list[2] );
					tmp_usb.Set_Speed( busAddr_list[3].toInt() );
				}
				
				list << tmp_usb;
			}
		}
	}
	
	return true;
}

#ifdef Q_OS_LINUX

#include <sys/sysinfo.h>
#include <QDir>
#include <QFileInfoList>

int System_Info::Get_Free_Memory_Size()
{
	// proc fs is exists?
	if( QFile::exists("/proc/meminfo") )
	{
		QFile proc_file( "/proc/meminfo" );
		
		if( ! proc_file.open(QIODevice::ReadOnly | QIODevice::Text) )
		{
			AQDebug( "int System_Info::Get_Free_Memory_Size()",
					 "Cannot Open File \"/proc/meminfo\"! Information not found!" );
		}
		else
		{
			QTextStream in( &proc_file );
			QString line = in.readLine();
			
			int free_mem, cached_mem, buf_mem;
			free_mem = cached_mem = buf_mem = -1;
			
			while( ! line.isNull() )
			{
				if( (free_mem != -1) && (cached_mem != -1) && (buf_mem != -1) )
				{
					return (free_mem + cached_mem + buf_mem) / 1024;
				}
				
				if( line.startsWith("MemFree") )
				{
					QRegExp rx = QRegExp( "MemFree:\\s+(\\d+)\\s+.*" );
					
					if( rx.exactMatch(line) )
					{
						QStringList res = rx.capturedTexts();
						
						if( res.count() != 2 ) break;
						else
						{
							free_mem = res[ 1 ].toInt();
						}
					}
					else break;
				}
				else if( line.startsWith("Cached") )
				{
					QRegExp rx = QRegExp( "Cached:\\s+(\\d+)\\s+.*" );
					
					if( rx.exactMatch(line) )
					{
						QStringList res = rx.capturedTexts();
						
						if( res.count() != 2 ) break;
						else
						{
							cached_mem = res[ 1 ].toInt();
						}
					}
					else break;
				}
				else if( line.startsWith("Buffers") )
				{
					QRegExp rx = QRegExp( "Buffers:\\s+(\\d+)\\s+.*" );
					
					if( rx.exactMatch(line) )
					{
						QStringList res = rx.capturedTexts();
						
						if( res.count() != 2 ) break;
						else
						{
							buf_mem = res[ 1 ].toInt();
						}
					}
					else break;
				}
				
				// Read next line
				line = in.readLine();
			}
		}
	}
	
	// If proc fs not exists
	struct sysinfo sys_info;
	sysinfo( &sys_info );
	
	long memory = ( sys_info.freeram * sys_info.mem_unit ) / 1024.0 / 1024.0;
	
	return (int) memory;
}

QStringList System_Info::Get_Host_FDD_List()
{
	// Find Floppy's
	QDir dev_dir( "/dev/" );
	QFileInfoList devices = dev_dir.entryInfoList( QStringList("fd*"), QDir::System, QDir::Name );
	
	QStringList tmp_list;
	
	for( int d = 0; d < devices.count(); ++d )
	{
		tmp_list << devices[d].absoluteFilePath();
	}
	
	return tmp_list;
}

QStringList System_Info::Get_Host_CDROM_List()
{
	// Find CD-ROM's
	QDir dev_dir( "/dev/" );
	
	QStringList name_filters;
	name_filters << "cdrom*" << "sr*";
	
	QFileInfoList devices = dev_dir.entryInfoList( name_filters, QDir::System, QDir::Name );
	
	QStringList tmp_list;
	
	for( int d = 0; d < devices.count(); ++d )
	{
		tmp_list << devices[ d ].absoluteFilePath();
	}
	
	// Additional CDROM Devices (This Added By User)
	QSettings settings;
	int cdrom_count = settings.value( "Additional_CDROM_Devices/Count", "0" ).toString().toInt();
	
	for( int ix = 0; ix < cdrom_count; ix++ )
	{
		tmp_list << settings.value("Additional_CDROM_Devices/Device" + QString::number(ix), "").toString();
	}
	
	return tmp_list;
}
#endif

#ifdef Q_OS_FREEBSD

#include <sys/sysctl.h>
#include <QDir>
#include <QFileInfoList>

int System_Info::Get_Free_Memory_Size()
{
	size_t free_ram = 0;
	size_t len = sizeof( free_ram );
	
	int page_size = getpagesize();
	
	if( (sysctlbyname("vm.stats.vm.v_free_count", &free_ram, &len, NULL, 0) == -1) || ! len )
	{
		free_ram = 0;
		AQError( "int System_Info::Get_Free_Memory_Size()", "Cannot Get Information on Memory!" );
	}
	
	free_ram *= page_size;
	
	return (int) free_ram / 1024.0 / 1024.0;
}

QStringList System_Info::Get_Host_FDD_List()
{
	// Find Floppy's
	QDir dev_dir( "/dev/fd/" );
	QFileInfoList devices = dev_dir.entryInfoList( QStringList("*"), QDir::System, QDir::Name );
	
	QStringList tmp_list;
	
	for( int d = 0; d < devices.count(); ++d )
	{
		tmp_list << devices[d].absoluteFilePath();
	}
	
	return tmp_list;
}

QStringList System_Info::Get_Host_CDROM_List()
{
	// Find CD-ROM's
	QDir dev_dir( "/dev/" );
	QFileInfoList devices = dev_dir.entryInfoList( QStringList("acd*"), QDir::System, QDir::Name );
	
	QStringList tmp_list;
	
	for( int d = 0; d < devices.count(); ++d )
	{
		tmp_list << devices[d].absoluteFilePath();
	}
	
	return tmp_list;
}
#endif

#ifdef Q_OS_WIN32

#include <windows.h>
#include <tchar.h>

int System_Info::Get_Free_Memory_Size()
{
	MEMORYSTATUS mem;
	memset( (void*)&mem, 0, sizeof(mem) );
	mem.dwLength = sizeof( mem );
	GlobalMemoryStatus( &mem );
	
	return (int) mem.dwAvailPhys / 1024.0 / 1024.0;
}

QStringList System_Info::Get_Host_FDD_List()
{
	DWORD len = GetLogicalDriveStrings( 0, NULL );
	TCHAR buf[ len ];
	int buf_size = sizeof(buf) / sizeof(TCHAR);
	GetLogicalDriveStrings( buf_size, buf );
	
	QStringList ret_list;
	QString tmp = "";
	for( int ix = 0; ix < buf_size-1; ix++ )
	{
		if( buf[ix] != NULL )
		{
			tmp += (char)buf[ix];
		}
		else
		{
			WCHAR *w = new WCHAR[ tmp.count() ];
			tmp.toWCharArray( w );
			UINT uDriveType = GetDriveType( w );
			delete w;
			
			if( uDriveType == DRIVE_REMOVABLE )
			{
				ret_list << tmp;
			}
			
			tmp = "";
		}
	}

	return ret_list;
}

QStringList System_Info::Get_Host_CDROM_List()
{
	DWORD len = GetLogicalDriveStrings( 0, NULL );
	TCHAR buf[ len ];
	int buf_size = sizeof(buf) / sizeof(TCHAR);
	GetLogicalDriveStrings( buf_size, buf );
	
	QStringList ret_list;
	QString tmp = "";
	for( int ix = 0; ix < buf_size-1; ix++ )
	{
		if( buf[ix] != NULL )
		{
			tmp += (char)buf[ix];
		}
		else
		{
			WCHAR *w = new WCHAR[ tmp.count() ];
			tmp.toWCharArray( w );
			UINT uDriveType = GetDriveType( w );
			delete w;
			
			if( uDriveType == DRIVE_CDROM )
			{
				ret_list << tmp;
			}
			
			tmp = "";
		}
	}
	
	return ret_list;
}

#endif
