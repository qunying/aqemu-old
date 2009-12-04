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
#include <QFileDialog>
#include <QFileInfo>

#include "Properties_Window.h"
#include "Create_HDD_Image_Window.h"
#include "Utils.h"
#include "System_Info.h"

Properties_Window::Properties_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
	
	HDD_Info = new HDD_Image_Info();
}

const VM_Floppy &Properties_Window::Get_Floppy()
{
	PW_Floppy = VM_Floppy( true, ui.CB_FD_Host_Devices->currentText(),
						   ui.Edit_FD_Image_Path->text(), ui.RB_FD_Host->isChecked() );
	
	return PW_Floppy;
}

const VM_CDROM &Properties_Window::Get_CD_ROM()
{
	PW_CDROM = VM_CDROM( true, ui.CB_CDROM_Host_Devices->currentText(),
						 ui.Edit_CDROM_Image_Path->text(), ui.RB_CDROM_Host->isChecked() );
	
	return PW_CDROM;
}

const VM_HDD &Properties_Window::Get_HDD()
{
	PW_HDD = VM_HDD( true, ui.Edit_HDD_Image_Path->text() );
	
	return PW_HDD;
}

void Properties_Window::Set_Floppy( const VM_Floppy &fd, const QString &name )
{
	ui.Label_Name->setText( ui.Label_Name->text() + name );
	
	ui.GB_Floppy->setVisible( true );
	ui.GB_CDROM->setVisible( false );
	ui.GB_HDD->setVisible( false );
	
	ui.GB_Floppy->setEnabled( true );
	ui.GB_CDROM->setEnabled( false );
	ui.GB_HDD->setEnabled( false );
	
	// Find Floppy's
	QStringList fd_list = System_Info::Get_Host_FDD_List();
	
	ui.CB_FD_Host_Devices->clear();
	
	if( fd_list.count() < 1 )
	{
		AQDebug( "void Properties_Window::Set_Floppy( const VM_Floppy &fd, const QString &name )",
				 "Cannot Find Host Floppy Devices!" );
		
		ui.RB_FD_Host->setChecked( false );
		ui.RB_FD_Host->setEnabled( false );
		
		on_RB_FD_Host_toggled( false );
	}
	else
	{
		ui.RB_FD_Host->setChecked( fd.Get_Host_Device() );
		on_RB_FD_Host_toggled( fd.Get_Host_Device() );
		
		for( int d = 0; d < fd_list.count(); ++d )
		{
			ui.CB_FD_Host_Devices->addItem( fd_list[d] );
		}
		
		int fd0_ix = ui.CB_FD_Host_Devices->findText( fd.Get_Host_File_Name() );
		
		if( fd0_ix != -1 ) ui.CB_FD_Host_Devices->setCurrentIndex( fd0_ix );
	}
	
	ui.Edit_FD_Image_Path->setText( fd.Get_Image_File_Name() );
	
	on_Button_Update_Info_clicked();
	
	//setFixedHeight( minimumSizeHint().height() );
	resize( width(), minimumSizeHint().height() );
}

void Properties_Window::Set_CD_ROM( const VM_CDROM &cd, const QString &name )
{
	ui.Label_Name->setText( ui.Label_Name->text() + name );
	
	ui.GB_Floppy->setVisible( false );
	ui.GB_CDROM->setVisible( true );
	ui.GB_HDD->setVisible( false );
	
	ui.GB_Floppy->setEnabled( false );
	ui.GB_CDROM->setEnabled( true );
	ui.GB_HDD->setEnabled( false );
	
	// Find CD_ROM's
	QStringList cd_list = System_Info::Get_Host_CDROM_List();
	
	if( cd_list.count() < 1 )
	{
		AQDebug( "void Properties_Window::Set_CD_ROM( const VM_CDROM &cd, const QString &name )",
				 "Cannot Find Host CD-ROM Devices!" );
		
		ui.RB_CDROM_Host->setChecked( false );
		ui.RB_CDROM_Host->setEnabled( false );
		
		on_RB_CDROM_Host_toggled( false );
	}
	else
	{
		ui.RB_CDROM_Host->setChecked( cd.Get_Host_Device() );
		on_RB_CDROM_Host_toggled( cd.Get_Host_Device() );
		
		ui.CB_CDROM_Host_Devices->clear();
		
		for( int d = 0; d < cd_list.count(); ++d )
		{
			ui.CB_CDROM_Host_Devices->addItem( cd_list[d] );
		}
		
		int cd_ix = ui.CB_CDROM_Host_Devices->findText( cd.Get_Host_File_Name() );
		
		if( cd_ix != -1 ) ui.CB_CDROM_Host_Devices->setCurrentIndex( cd_ix );
	}
	
	ui.Edit_CDROM_Image_Path->setText( cd.Get_Image_File_Name() );
	
	on_Button_Update_Info_clicked();
	
	//setFixedHeight( minimumSizeHint().height() );
	resize( width(), minimumSizeHint().height() );
}

void Properties_Window::Set_HDD( const VM_HDD &hd, const QString &name )
{
	connect( HDD_Info, SIGNAL(Completed()),
			 this, SLOT(Update_HDD()) );
	
	ui.Label_Name->setText( ui.Label_Name->text() + name );
	
	ui.GB_Floppy->setVisible( false );
	ui.GB_CDROM->setVisible( false );
	ui.GB_HDD->setVisible( true );
	
	ui.GB_Floppy->setEnabled( false );
	ui.GB_CDROM->setEnabled( false );
	ui.GB_HDD->setEnabled( true );
	
	ui.Edit_HDD_Image_Path->setText( hd.Get_Image_File_Name() );
	
	on_Button_Update_Info_clicked();
	
	resize( width(), minimumSizeHint().height() );
}

void Properties_Window::on_Button_OK_clicked()
{
	if( ui.GB_Floppy->isEnabled() )
	{
		if( ui.RB_FD_Host->isChecked() )
		{
			accept();
		}
		else
		{
			if( ! QFile::exists(ui.Edit_FD_Image_Path->text()) )
			{
				AQGraphic_Warning( tr("Warning"),
								   tr("Image File Not Exists!") );
			}
			else
			{
				accept();
			}
		}
	}
	else if( ui.GB_CDROM->isEnabled() )
	{
		if( ui.RB_CDROM_Host->isChecked() )
		{
			accept();
		}
		else
		{
			if( ! QFile::exists(ui.Edit_CDROM_Image_Path->text()) )
			{
				AQGraphic_Warning( tr("Warning"),
								   tr("Image File Not Exists!") );
			}
			else
			{
				Add_To_Recent_Files( ui.Edit_CDROM_Image_Path->text() );
				accept();
			}
		}
	}
	else if( ui.GB_HDD->isEnabled() )
	{
		if( ! QFile::exists(ui.Edit_HDD_Image_Path->text()) )
		{
			AQGraphic_Warning( tr("Warning"),
							   tr("Image File Not Exists!") );
		}
		else
		{
			accept();
		}
	}
	else
	{
		AQError( "void Properties_Window::on_Button_OK_clicked()",
				 "Default Section!" );
	}
}

void Properties_Window::on_Button_Cancel_clicked()
{
	reject();
}

void Properties_Window::on_Button_Update_Info_clicked()
{
	if( ui.GB_Floppy->isEnabled() )
	{
		VM_Floppy *fd = new VM_Floppy( true,
			ui.CB_FD_Host_Devices->currentText(),
			ui.Edit_FD_Image_Path->text(), ui.RB_FD_Host->isChecked() );
		
		if( ! fd->Get_Host_Device() )
		{
			if( ui.Edit_FD_Image_Path->text().isEmpty() )
			{
				ui.Label_Info->setText( tr("Image Size: ") + QString::number(0) + tr("Mb") );
				return;
			}
			
			if( ! QFile::exists(ui.Edit_FD_Image_Path->text()) )
			{
				AQGraphic_Warning( tr("Error!"),
								   tr("Cannot Locate Image File!") );
				ui.Label_Info->setText( tr("Image Size: ") + QString::number(0) + tr("Mb") );
				return;
			}
			
			QFileInfo fd_img = QFileInfo( fd->Get_Image_File_Name() );
			qint64 size_in_bytes = fd_img.size();
			
			if( size_in_bytes <= 0 )
			{
				ui.Label_Info->setText( tr("Image Size: ") + QString::number(0) + tr("Mb") );
			}
			else
			{
				ui.Label_Info->setText( tr("Image Size: ") +
						QString::number((float)size_in_bytes / 1024.0 / 1024.0) + tr("Mb") );
			}
		}
		
		delete fd;
	}
	else if( ui.GB_CDROM->isEnabled() )
	{
		VM_CDROM *cd = new VM_CDROM( true,
			ui.CB_CDROM_Host_Devices->currentText(),
			ui.Edit_CDROM_Image_Path->text(), ui.RB_CDROM_Host->isChecked() );
		
		if( ! cd->Get_Host_Device() )
		{
			if( ui.Edit_CDROM_Image_Path->text().isEmpty() )
			{
				ui.Label_Info->setText( tr("Image Size: ") + QString::number(0)  + tr("Mb") );
				return;
			}
			
			if( ! QFile::exists(ui.Edit_CDROM_Image_Path->text()) )
			{
				AQGraphic_Warning( tr("Error!"),
								   tr("Cannot Locate Image File!") );
				ui.Label_Info->setText( tr("Image Size: ") + QString::number(0) + tr("Mb") );
				return;
			}
			
			QFileInfo cd_img = QFileInfo( cd->Get_Image_File_Name() );
			qint64 size_in_bytes = cd_img.size();
			
			if( size_in_bytes <= 0 )
			{
				ui.Label_Info->setText( tr("Image Size: ") + QString::number(0) + tr("Mb") );
			}
			else
			{
				ui.Label_Info->setText( tr("Image Size: ") +
						QString::number((float)size_in_bytes / 1024.0 / 1024.0) + tr("Mb") );
			}
		}
		
		delete cd;
	}
	else if( ui.GB_HDD->isEnabled() )
	{
		if( ui.Edit_HDD_Image_Path->text().isEmpty() )
		{
			ui.Label_Info->setText( tr("Format: ") +
									tr(" Virtual Size: ") +
									tr("\nOn Disk Size: ") +
									tr(" Cluster Size: ") );
			return;
		}
		
		if( ! QFile::exists(ui.Edit_HDD_Image_Path->text()) )
		{
			AQGraphic_Warning( tr("Error!"),
							   tr("Cannot Locate Image File!") );
			return;
		}
		
		HDD_Info->Update_Disk_Info( ui.Edit_HDD_Image_Path->text() );
		
		/*
		VM_HDD hd = VM_HDD( true, ui.Edit_HDD_Image_Path->text() );
		
		QString suf_v = Get_TR_Size_Suffix( hd.Get_Virtual_Size() );
		QString suf_d = Get_TR_Size_Suffix( hd.Get_Disk_Size() );
		
		ui.Label_Info->setText( tr("Format: ") + hd.Get_Image_Format() +
								tr(" Virtual Size: ") + QString::number(hd.Get_Virtual_Size().Size) + suf_v +
								tr("\nOn Disk Size: ") + QString::number(hd.Get_Disk_Size().Size) + suf_d +
								tr(" Cluster Size: ") + QString::number(hd.Get_Cluster_Size()) );*/
		
	}
	else
	{
		AQError( "void Properties_Window::on_Button_Update_Info_clicked()",
				 "No Enabled GB!" );
	}
}

void Properties_Window::on_TB_FD_Image_Browse_clicked()
{
	QFileDialog::Options options;
	QString selectedFilter;
	
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open Floppy Image File"),
													 Get_Last_Dir_Path(ui.Edit_FD_Image_Path->text()),
													 tr("All Files (*);;Images Files (*.img *.ima)"), &selectedFilter, options );
	
	if( ! (fileName.isNull() || fileName.isEmpty()) )
	{
		ui.Edit_FD_Image_Path->setText( fileName );
		on_Button_Update_Info_clicked();
	}
}

void Properties_Window::on_TB_CDROM_Image_Browse_clicked()
{
	QFileDialog::Options options;
	QString selectedFilter;
	
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open CD\\DVD-ROM Image File"),
													 Get_Last_Dir_Path(ui.Edit_CDROM_Image_Path->text()),
													 tr("All Files (*);;Images Files (*.iso)"), &selectedFilter, options );
	
	if( ! (fileName.isNull() || fileName.isEmpty()) )
	{
		ui.Edit_CDROM_Image_Path->setText( fileName );
		on_Button_Update_Info_clicked();
	}
}

void Properties_Window::on_TB_HDD_Image_Browse_clicked()
{
	QFileDialog::Options options;
	QString selectedFilter;
	
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open HDD Image File"),
													 Get_Last_Dir_Path(ui.Edit_HDD_Image_Path->text()),
													 tr("All Files (*);;Images Files (*.img *.qcow *.wmdk)"), &selectedFilter, options );
	
	if( ! (fileName.isNull() || fileName.isEmpty()) )
	{
		ui.Edit_HDD_Image_Path->setText( fileName );
		on_Button_Update_Info_clicked();
	}
}

void Properties_Window::on_Button_HDD_New_clicked()
{
	Create_HDD_Image_Window *Create_HDD_Win = new Create_HDD_Image_Window( this );
	
	if( Create_HDD_Win->exec() == QDialog::Accepted )
	{
		ui.Edit_HDD_Image_Path->setText( Create_HDD_Win->Get_Image_File_Name() );
		on_Button_Update_Info_clicked();
	}
	
	delete Create_HDD_Win;
}

void Properties_Window::on_Button_HDD_Format_clicked()
{
	Create_HDD_Image_Window *Create_HDD_Win = new Create_HDD_Image_Window( this );
	Create_HDD_Win->Set_Image_File_Name( ui.Edit_HDD_Image_Path->text() );
	
	if( Create_HDD_Win->exec() == QDialog::Accepted )
	{
		on_Button_Update_Info_clicked();
	}
	
	delete Create_HDD_Win;
}

void Properties_Window::on_RB_FD_Host_toggled( bool on )
{
	ui.CB_FD_Host_Devices->setEnabled( on );
	ui.RB_FD_Image->setChecked( ! on );
	ui.Edit_FD_Image_Path->setEnabled( ! on );
	ui.TB_FD_Image_Browse->setEnabled( ! on );
}

void Properties_Window::on_RB_CDROM_Host_toggled( bool on )
{
	ui.CB_CDROM_Host_Devices->setEnabled( on );
	ui.RB_CDROM_Image->setChecked( ! on );
	ui.Edit_CDROM_Image_Path->setEnabled( ! on );
	ui.TB_CDROM_Image_Browse->setEnabled( ! on );
}

void Properties_Window::Update_HDD()
{
	PW_HDD.Set_Disk_Info( HDD_Info->Get_Disk_Info() );
	
	QString suf_v = Get_TR_Size_Suffix( PW_HDD.Get_Virtual_Size() );
	QString suf_d = Get_TR_Size_Suffix( PW_HDD.Get_Disk_Size() );
	
	ui.Label_Info->setText( tr("Format: ") + PW_HDD.Get_Image_Format() +
			tr(" Virtual Size: ") + QString::number(PW_HDD.Get_Virtual_Size().Size) + Get_TR_Size_Suffix(PW_HDD.Get_Virtual_Size()) +
			tr("\nOn Disk Size: ") + QString::number(PW_HDD.Get_Disk_Size().Size) + Get_TR_Size_Suffix(PW_HDD.Get_Disk_Size()) +
			tr(" Cluster Size: ") + QString::number(PW_HDD.Get_Cluster_Size()) );
}
