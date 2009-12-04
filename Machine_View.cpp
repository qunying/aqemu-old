/****************************************************************************
**
** Copyright (C) 2008 Ben Klopfenstein <benklop@gmail.com>
** Copyright (C) 2009 Andrey Rijov <ANDron142@yandex.ru>
**
** This file is part of QtEMU, AQEMU.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU Library General Public License
** along with this library; see the file COPYING.LIB.  If not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
** Boston, MA 02110-1301, USA.
**
****************************************************************************/

#include <QUrl>
#include <QSize>
#include <QColor>
#include <QLabel>
#include <QAction>
#include <QTimer>
#include <QKeySequence>
#include <QTest>

#include "Utils.h"
#include "Machine_View.h"

MachineView::MachineView( QWidget *parent ) : QScrollArea( parent )
{
	View = new VncView( this );
	splashShown = true;
	fullscreenEnabled = false;
	showSplash( true );
	setFrameShape( QFrame::NoFrame );
	Scaling = false;
	Reinit_Timer = new QTimer( this );
	
	connect( View, SIGNAL(reinit_me()),
			 this, SLOT(reinitView()) );
}

void MachineView::Set_VNC_URL( const QString &host, int port )
{
	VNC_Host = host;
	VNC_Port = port;
}

void MachineView::Set_Scaling( bool s )
{
	Scaling = s;
	
	if( Scaling )
	{
		newViewSize( maximumViewportSize().width(),
					 maximumViewportSize().height() );
	}
	else
	{
		newViewSize( -1, -1 );
	}
}

void MachineView::Set_Fullscreen( bool on )
{
	fullscreen( on );
}

void MachineView::resizeEvent( QResizeEvent *event )
{
	resizeView( event->size().width(), event->size().height() );
	QScrollArea::resizeEvent( event );
}

void MachineView::resizeView( int widgetWidth, int widgetHeight )
{
	View->blockSignals( true );
	
	if( Scaling )
	{
		View->enableScaling( true );
		View->scaleResize( widgetWidth, widgetHeight );
	}
	else
	{
		View->enableScaling( false );
	}
	
	View->blockSignals( false );
}

void MachineView::initView()
{
	showSplash( true );
	delete View;
	
	QUrl url;
	url.setScheme( "vnc" );
	url.setHost( VNC_Host );
	url.setPort( VNC_Port );
	
	View = new VncView( this, url );
	View->start();
	showSplash( false );
	
	connect( View, SIGNAL(connected()),
			 this, SIGNAL(Connected()) );
	
	connect( View, SIGNAL(connected()),
			 this, SLOT(VNC_Connected_OK()) );
	
	connect( View, SIGNAL(changeSize(int, int)),
			 this, SLOT(newViewSize(int, int)) );
	
	connect( View, SIGNAL(reinit_me()),
			 this, SLOT(reinitView()) );
}

void MachineView::reinitView()
{
	connect( Reinit_Timer, SIGNAL(timeout()),
			 this, SLOT(reinitVNC()) );
	
	Reinit_Timer->start( 1000 );
}

void MachineView::reinitVNC()
{
	if( stop_reinit ) return;
	
	static int init_count = 0;
	
	AQWarning( "void MachineView::reinitVNC()",
			   "Reinit VNC. Attempt #" + QString::number(init_count+1) );
	
	if( init_count < 10 )
	{
		++init_count;
		
		disconnect( View, SIGNAL(connected()),
					this, SIGNAL(Connected()) );
		
		disconnect( View, SIGNAL(connected()),
					this, SLOT(VNC_Connected_OK()) );
		
		disconnect( View, SIGNAL(changeSize(int, int)),
					this, SLOT(newViewSize(int, int)) );
		
		disconnect( View, SIGNAL(reinit_me()),
					this, SLOT(reinitView()) );
		
		View = new VncView( this );
		splashShown = true;
		fullscreenEnabled = false;
		showSplash( true );
		setFrameShape( QFrame::NoFrame );
		Scaling = false;
		
		initView();
	}
	else
	{
		Reinit_Timer->stop();
		
		AQGraphic_Error( "void MachineView::reinitVNC()", tr("Error!"),
						 tr("Cannot Reinit VNC Connection!"), false );
	}
}

void MachineView::disconnectVNC()
{
	stop_reinit = true;
	Reinit_Timer->stop();
}

void MachineView::VNC_Connected_OK()
{
	disconnect( Reinit_Timer, SIGNAL(timeout()),
				this, SLOT(reinitVNC()) );
}

void MachineView::showSplash( bool show )
{
	if( ! show )
	{
		takeWidget();
		setWidget( View );
		splashShown = false;
		View->show();
	}
	else
	{
		fullscreen( false );
		View->hide();
		takeWidget();
		splashShown = true;
	}
}

void MachineView::fullscreen( bool enable )
{
	if( splashShown ) return;

	if( enable )
	{
		setWindowFlags( Qt::Window );
		showFullScreen();
		QPalette p;
		p.setColor( QPalette::Background, QColor(22,22,22) );
		setPalette( p );
	}
	else if( fullscreenEnabled )
	{
		setWindowFlags( Qt::Widget );
		showNormal();
		setPalette( QPalette() );
	}
	
	fullscreenEnabled = enable;
	emit fullscreenToggled( enable );
	show();
	View->switchFullscreen( enable );
}

void MachineView::captureAllKeys( bool enabled )
{
	View->setGrabAllKeys( enabled );
}

void MachineView::sendKey( QKeyEvent *event )
{
	View->keyEvent( event );
}

void MachineView::newViewSize( int w, int h )
{
	static int vnc_width = 0;
	static int vnc_height = 0;
	
	if( w > 0 && h > 0 )
	{
		vnc_width = w;
		vnc_height = h;
	}
	
	resizeView( maximumViewportSize().width(),
				maximumViewportSize().height() );
	
	emit Full_Size( vnc_width, vnc_height );
}

bool MachineView::event( QEvent *event )
{
	if( event->type() == QEvent::DynamicPropertyChange )
	{
		//any property changes dealt with in here
		QDynamicPropertyChangeEvent *propEvent = static_cast<QDynamicPropertyChangeEvent*>( event );
		
		if( propEvent->propertyName() == "scaleEmbeddedDisplay" )
		{
			newViewSize( -1, -1 );
		}
		
		return false;
	}
	else if( event->type() == QEvent::Enter && splashShown == false )
	{
		View->setFocus();
		View->grabKeyboard();
		
		return true;
	}
	else if( event->type() == QEvent::Leave )
	{
		View->clearFocus();
		View->releaseKeyboard();
		
		return true;
	}
	else if( event->type() == QEvent::KeyPress )
	{
		QKeyEvent *ke = static_cast<QKeyEvent*>( event );
		 
		if( ke->key() == Qt::Key_Return && ke->modifiers() == Qt::ControlModifier + Qt::AltModifier )
		{
			fullscreen( false );
			
			return true;
		}
	}
	
	return QScrollArea::event( event );
}
