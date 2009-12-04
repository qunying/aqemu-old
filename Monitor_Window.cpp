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

#include <QCloseEvent>

#include "Monitor_Window.h"

Monitor_Window::Monitor_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
}

void Monitor_Window::Add_QEMU_Out( const QString &new_text )
{
	ui.Edit_Monitor_Out->insertPlainText( new_text );
}

void Monitor_Window::on_Button_Run_clicked()
{
	ui.Edit_Monitor_Out->append( ui.CB_Com_Line->currentText() + "\n" );
	emit Command_Sended( ui.CB_Com_Line->currentText() );
	ui.CB_Com_Line->clearEditText();
}

void Monitor_Window::closeEvent( QCloseEvent *event )
{
	emit Closing_Win();
	event->accept();
}
