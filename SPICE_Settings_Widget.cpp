/****************************************************************************
**
** Copyright (C) 2010 Andrey Rijov <ANDron142@yandex.ru>
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

#include "SPICE_Settings_Widget.h"

SPICE_Settings_Widget::SPICE_Settings_Widget( QWidget *parent )
	: QWidget( parent )
{
	ui.setupUi( this );
	
	// Connect slots
	connect( ui.CH_Use_SPICE, SIGNAL(clicked()), this, SLOT(State_Changet()) );
	
	connect( ui.SB_QXL_Num, SIGNAL(valueChanged(int)), this, SLOT(State_Changet()) );
	connect( ui.CB_RAM_Size, SIGNAL(currentIndexChanged(int)), this, SLOT(State_Changet()) );
	
	connect( ui.Edit_SPICE_Port, SIGNAL(textChanged(const QString &)), this, SLOT(State_Changet()) );
	connect( ui.CH_SPICE_SPort, SIGNAL(clicked()), this, SLOT(State_Changet()) );
	connect( ui.Edit_SPICE_SPort, SIGNAL(textChanged(const QString &)), this, SLOT(State_Changet()) );
	connect( ui.CH_SPICE_Host, SIGNAL(clicked()), this, SLOT(State_Changet()) );
	connect( ui.Edit_SPICE_Host, SIGNAL(textChanged(const QString &)), this, SLOT(State_Changet()) );
	
	connect( ui.CH_Use_Image_Compression, SIGNAL(clicked()), this, SLOT(State_Changet()) );
	connect( ui.CB_Video_Compression, SIGNAL(currentIndexChanged(int)), this, SLOT(State_Changet()) );
	
	connect( ui.CH_Set_Renderer, SIGNAL(clicked()), this, SLOT(State_Changet()) );
	connect( ui.TB_Up, SIGNAL(clicked()), this, SLOT(State_Changet()) );
	connect( ui.TB_Down, SIGNAL(clicked()), this, SLOT(State_Changet()) );
	
	connect( ui.CH_Use_Video_Compression, SIGNAL(clicked()), this, SLOT(State_Changet()) );
	connect( ui.CH_Use_Playback_Compression, SIGNAL(clicked()), this, SLOT(State_Changet()) );
	
	connect( ui.RB_No_Password, SIGNAL(toggled(bool)), this, SLOT(State_Changet()) );
	connect( ui.Edit_Password, SIGNAL(textChanged(const QString &)), this, SLOT(State_Changet()) );
}

const VM_SPICE &SPICE_Settings_Widget::Get_Settigns() const
{
	// FIXME
}

void SPICE_Settings_Widget::Set_Settings( const VM_SPICE &settings )
{
	ui.CH_Use_SPICE->setChecked( settings.Use_SPICE() );
	
	ui.SB_QXL_Num->setValue( settings.Get_GXL_Devices_Count() );
	ui.CB_RAM_Size->setEditText( QString::number(settings.Get_RAM_Size()) );
	
	ui.Edit_SPICE_Port->setText( QString::number(settings.Get_Port()) );
	ui.CH_SPICE_SPort->setChecked( settings.Use_SPort() );
	ui.Edit_SPICE_SPort->setText( QString::number(settings.Get_SPort()) );
	ui.CH_SPICE_Host->setChecked( settings.Use_Hostname() );
	ui.Edit_SPICE_Host->setText( settings.Get_Hostname() );
	
	ui.CH_Use_Image_Compression->setChecked( settings.Use_Image_Compression() );
	switch( settings.Get_Image_Compression() )
	{
		case VM::SPICE_IC_Type_on:
		case VM::SPICE_IC_Type_auto_glz:
			ui.CB_Video_Compression->setCurrentIndex( 0 );
			break;
		
		case VM::SPICE_IC_Type_auto_lz:
			ui.CB_Video_Compression->setCurrentIndex( 1 );
			break;
	
		case VM::SPICE_IC_Type_quic:
			ui.CB_Video_Compression->setCurrentIndex( 2 );
			break;
	
		case VM::SPICE_IC_Type_glz:
			ui.CB_Video_Compression->setCurrentIndex( 3 );
			break;
			
		case VM::SPICE_IC_Type_lz:
			ui.CB_Video_Compression->setCurrentIndex( 4 );
			break;
			
		case VM::SPICE_IC_Type_off:
			ui.CB_Video_Compression->setCurrentIndex( 5 );
			break;
	
		default:
			ui.CB_Video_Compression->setCurrentIndex( 0 );
			break;
	}
	
	ui.CH_Set_Renderer->setChecked( settings.Use_Renderer() );
	Show_Renderer_List( settings.Get_Renderer_List() );
	
	ui.CH_Use_Video_Compression->setChecked( settings.Use_Video_Stream_Compression() );
	ui.CH_Use_Playback_Compression->setChecked( settings.Use_Playback_Compression() );
	
	ui.RB_No_Password->setChecked( ! settings.Use_Password() );
	ui.Edit_Password->setText( settings.Get_Password() );
}

void SPICE_Settings_Widget::on_TB_Up_clicked()
{
	
}

void SPICE_Settings_Widget::on_TB_Down_clicked()
{
	
}
	
void SPICE_Settings_Widget::Show_Renderer_List( const QList<VM::SPICE_Renderer> &list )
{
	// FIXME
}
