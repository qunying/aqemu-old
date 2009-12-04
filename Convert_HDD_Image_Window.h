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

#ifndef CONVERT_HDD_IMAGE_WINDOW_H
#define CONVERT_HDD_IMAGE_WINDOW_H

#include <QProgressDialog>
#include <QThread>
#include "ui_Convert_HDD_Image_Window.h"

class Convert_HDD_Thread : public QThread
{
	Q_OBJECT
	
	public:
		Convert_HDD_Thread();
		void Set_Args( const QStringList &args );
		
	protected:
		void run();
		
	signals:
		void Conversion_Complete( bool ok );
		
	private:
		QStringList Arguments;
};

class Convert_HDD_Image_Window: public QDialog
{
	Q_OBJECT
	
	public:
		Convert_HDD_Image_Window( QWidget *parent = 0 );
		
	private slots:
		void on_Button_Browse_Base_clicked();
		void on_Button_Browse_Output_clicked();
		void on_CB_Output_Format_currentIndexChanged( const QString &text );
		void on_Button_Convert_clicked();
		void on_Button_Identify_clicked();
		
		void Conversion_Done( bool ok );
		void Cancel_Convertion();
		
	private:
		Ui::Convert_HDD_Image_Window ui;
		Convert_HDD_Thread Conv_Thread;
		QProgressDialog *ProgDial;
};

#endif
