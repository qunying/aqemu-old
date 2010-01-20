/****************************************************************************
**
** Copyright (C) 2009-2010 Andrey Rijov <ANDron142@yandex.ru>
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

#ifndef SMP_SETTINGS_WINDOW_H
#define SMP_SETTINGS_WINDOW_H

#include "ui_SMP_Settings_Window.h"

class SMP_Settings_Window: public QDialog
{
	Q_OBJECT
	
	public:
		SMP_Settings_Window( QWidget *parent = 0 );
	
	private slots:
		void on_Button_OK_clicked();
	
	private:
		Ui::SMP_Settings_Window ui;
};

#endif
