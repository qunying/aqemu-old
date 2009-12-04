/****************************************************************************
**
** Copyright (C) 2007-2008 Urs Wolfer <uwolfer @ kde.org>
** Copyright (C) 2009 Andrey Rijov <ANDron142@yandex.ru>
**
** This file is part of KDE, QtEMU, AQEMU.
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
** You should have received a copy of the GNU General Public License
** along with this program; see the file COPYING. If not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
** Boston, MA 02110-1301, USA.
**
****************************************************************************/

#include "VNC_View.h"
#include "Utils.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QApplication>
#include <QImage>
#include <QPainter>
#include <QMouseEvent>

// Definition of key modifier mask constants
#define KMOD_Alt_R 	0x01
#define KMOD_Alt_L 	0x02
#define KMOD_Meta_L 	0x04
#define KMOD_Control_L 	0x08
#define KMOD_Shift_L	0x10

VncView::VncView(QWidget *parent, const QUrl &url)
        : RemoteView(parent),
        m_initDone(false),
        m_buttonMask(0),
        m_modifiersMask(0),
        m_repaint(false),
        m_quitFlag(false),
        m_firstPasswordTry(true),
        m_authenticaionCanceled(false),
        m_dontSendClipboard(false),
        m_horizontalFactor(1.0),
        m_verticalFactor(1.0),
        m_forceLocalCursor(false)
{
    m_url = url;
    m_host = url.host();
    m_port = url.port();

    connect(&vncThread, SIGNAL(imageUpdated(int, int, int, int)), this, SLOT(updateImage(int, int, int, int)), Qt::BlockingQueuedConnection);
    connect(&vncThread, SIGNAL(gotCut(const QString&)), this, SLOT(setCut(const QString&)), Qt::BlockingQueuedConnection);
    connect(&vncThread, SIGNAL(passwordRequest()), this, SLOT(requestPassword()), Qt::BlockingQueuedConnection);
    connect(&vncThread, SIGNAL(outputErrorMessage(QString)), this, SLOT(outputErrorMessage(QString)));

    m_clipboard = QApplication::clipboard();
    connect(m_clipboard, SIGNAL(selectionChanged()), this, SLOT(clipboardSelectionChanged()));
    connect(m_clipboard, SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()));
}

VncView::~VncView()
{
    // Disconnect all signals so that we don't get any more callbacks from the client thread
    disconnect(&vncThread, SIGNAL(imageUpdated(int, int, int, int)), this, SLOT(updateImage(int, int, int, int)));
    disconnect(&vncThread, SIGNAL(gotCut(const QString&)), this, SLOT(setCut(const QString&)));
    disconnect(&vncThread, SIGNAL(passwordRequest()), this, SLOT(requestPassword()));
    disconnect(&vncThread, SIGNAL(outputErrorMessage(QString)), this, SLOT(outputErrorMessage(QString)));

    startQuitting();
}

bool VncView::eventFilter(QObject *obj, QEvent *event)
{
    if (m_viewOnly) {
        if (event->type() == QEvent::KeyPress ||
                event->type() == QEvent::KeyRelease ||
                event->type() == QEvent::MouseButtonDblClick ||
                event->type() == QEvent::MouseButtonPress ||
                event->type() == QEvent::MouseButtonRelease ||
                event->type() == QEvent::Wheel ||
                event->type() == QEvent::MouseMove)
            return true;
    }

    return RemoteView::eventFilter(obj, event);
}

QSize VncView::framebufferSize()
{
    return m_frame.size();
}

QSize VncView::sizeHint() const
{
    return size();
}

QSize VncView::minimumSizeHint() const
{
    return size();
}

void VncView::scaleResize(int w, int h)
{
	AQDebug( "void VncView::scaleResize(int w, int h)",  QString::number(w) + "x" + QString::number(h) );
    if (m_scale) {
        m_verticalFactor = (qreal) h / m_frame.height();
        m_horizontalFactor = (qreal) w / m_frame.width();

        m_verticalFactor = m_horizontalFactor = qMin(m_verticalFactor, m_horizontalFactor);

        qreal newW = m_frame.width() * m_horizontalFactor;
        qreal newH = m_frame.height() * m_verticalFactor;
        setMaximumSize(newW, newH); //This is a hack to force Qt to center the view in the scroll area
        resize(newW, newH);
    }
}

void VncView::updateConfiguration()
{
    RemoteView::updateConfiguration();

    // Update the scaling mode in case KeepAspectRatio changed
    scaleResize(parentWidget()->width(), parentWidget()->height());
}

void VncView::startQuitting()
{
	AQDebug( "void VncView::startQuitting()", "about to quit" );

    bool connected = status() == RemoteView::Connected;

    setStatus(Disconnecting);

    m_quitFlag = true;

    if (connected) {
        vncThread.stop();
    } else {
        vncThread.quit();
    }

    vncThread.wait(500);

    setStatus(Disconnected);
}

bool VncView::isQuitting()
{
    return m_quitFlag;
}

bool VncView::start()
{
    vncThread.setHost(m_host);
    vncThread.setPort(m_port);
    RemoteView::Quality quality;

    quality = (RemoteView::Quality)((QCoreApplication::arguments().count() > 2) ?
        QCoreApplication::arguments().at(2).toInt() : 2);

    vncThread.setQuality(quality);

    // set local cursor on by default because low quality mostly means slow internet connection
    if (quality == RemoteView::Low) {
        showDotCursor(RemoteView::CursorOn);
    }

    setStatus(Connecting);

	setGrabAllKeys( true );
	
    vncThread.start();
    return true;
}

bool VncView::supportsScaling() const
{
    return true;
}

bool VncView::supportsLocalCursor() const
{
    return true;
}

void VncView::requestPassword()
{
    AQDebug( "void VncView::requestPassword()", "request password" );

    if (m_authenticaionCanceled) {
        startQuitting();
        return;
    }

    setStatus(Authenticating);

    if (!m_url.password().isNull()) {
        vncThread.setPassword(m_url.password());
        return;
    }

    bool ok;
    QString password = QInputDialog::getText(this, //krazy:exclude=qclasses
                                             tr("Password required"),
                                             tr("Please enter the password for the remote desktop:"),
                                             QLineEdit::Password, QString(), &ok);
    m_firstPasswordTry = false;
    if (ok)
        vncThread.setPassword(password);
    else
        m_authenticaionCanceled = true;
}

void VncView::outputErrorMessage(const QString &message)
{
    AQDebug( "void VncView::outputErrorMessage(const QString &message)", message );

    if (message == "INTERNAL:APPLE_VNC_COMPATIBILTY") {
        setCursor(localDotCursor());
        m_forceLocalCursor = true;
        return;
    }

    startQuitting();

    AQError( "void VncView::outputErrorMessage(const QString &message)",
             message );

    emit reinit_me();

    //emit errorMessage(tr("VNC failure"), message);
}

void VncView::updateImage(int x, int y, int w, int h)
{
    m_x = x;
    m_y = y;
    m_w = w;
    m_h = h;

    if (m_horizontalFactor != 1.0 || m_verticalFactor != 1.0) {
        // If the view is scaled, grow the update rectangle to avoid artifacts
        m_x-=1;
        m_y-=1;
        m_w+=2;
        m_h+=2;
    }

    m_frame = vncThread.image();

    if (!m_initDone) {
        setAttribute(Qt::WA_StaticContents);
        setAttribute(Qt::WA_OpaquePaintEvent);
        installEventFilter(this);

        setCursor(((m_dotCursorState == CursorOn) || m_forceLocalCursor) ? localDotCursor() : Qt::BlankCursor);

        setMouseTracking(true); // get mouse events even when there is no mousebutton pressed
        setFocusPolicy(Qt::WheelFocus);
        resize(m_frame.width(), m_frame.height());
        setStatus(Connected);
        emit changeSize(m_frame.width(), m_frame.height());
        emit connected();
        m_initDone = true;
    }

    if ((y == 0 && x == 0) && (m_frame.size() != size())) {
        resize(m_frame.width(), m_frame.height());
        emit changeSize(m_frame.width(), m_frame.height());
    }

    m_repaint = true;
    repaint(qRound(m_x * m_horizontalFactor), qRound(m_y * m_verticalFactor), qRound(m_w * m_horizontalFactor), qRound(m_h * m_verticalFactor));
    m_repaint = false;
}

void VncView::setViewOnly(bool viewOnly)
{
    RemoteView::setViewOnly(viewOnly);

    if (viewOnly)
        setCursor(Qt::ArrowCursor);
    else
        setCursor(m_dotCursorState == CursorOn ? localDotCursor() : Qt::BlankCursor);
}

void VncView::showDotCursor(DotCursorState state)
{
    RemoteView::showDotCursor(state);

    setCursor(state == CursorOn ? localDotCursor() : Qt::BlankCursor);
}

void VncView::enableScaling(bool scale)
{
    RemoteView::enableScaling(scale);

    if (scale) {
        setMaximumSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
        if (parentWidget())
            scaleResize(parentWidget()->width(), parentWidget()->height());
    } else {
        m_verticalFactor = 1.0;
        m_horizontalFactor = 1.0;

        setMaximumSize(m_frame.width(), m_frame.height()); //This is a hack to force Qt to center the view in the scroll area
        resize(m_frame.width(), m_frame.height());
        emit changeSize(m_frame.width(), m_frame.height());
    }
}

void VncView::setCut(const QString &text)
{
    m_dontSendClipboard = true;
    m_clipboard->setText(text, QClipboard::Clipboard);
    m_clipboard->setText(text, QClipboard::Selection);
    m_dontSendClipboard = false;
}

void VncView::paintEvent(QPaintEvent *event)
{
    if (m_frame.isNull() || m_frame.format() == QImage::Format_Invalid) {
        AQDebug( "void VncView::paintEvent(QPaintEvent *event)", "no valid image to paint" );
        RemoteView::paintEvent(event);
        event->accept();
        return;
    }

    event->accept();

    QPainter painter(this);

    if (m_repaint) {

        painter.drawImage(QRect(qRound(m_x*m_horizontalFactor), qRound(m_y*m_verticalFactor),
                                qRound(m_w*m_horizontalFactor), qRound(m_h*m_verticalFactor)), 
                          m_frame.copy(m_x, m_y, m_w, m_h).scaled(qRound(m_w*m_horizontalFactor), 
                                                                  qRound(m_h*m_verticalFactor),
                                                                  Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    } else {
        QRect rect = event->rect();
        if (rect.width() != width() || rect.height() != height()) {
            AQDebug( "void VncView::paintEvent(QPaintEvent *event)", "Partial repaint" );
            int sx = rect.x()/m_horizontalFactor;
            int sy = rect.y()/m_verticalFactor;
            int sw = rect.width()/m_horizontalFactor;
            int sh = rect.height()/m_verticalFactor;
            painter.drawImage(rect, 
                              m_frame.copy(sx, sy, sw, sh).scaled(rect.width(), rect.height(),
                                                                  Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        } else {
            AQDebug( "void VncView::paintEvent(QPaintEvent *event)", "Full repaint" );
            painter.drawImage(QRect(0, 0, width(), height()), 
                              m_frame.scaled(m_frame.width() * m_horizontalFactor, m_frame.height() * m_verticalFactor,
                                             Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        }
    }

    RemoteView::paintEvent(event);
}

void VncView::resizeEvent(QResizeEvent *event)
{
    RemoteView::resizeEvent(event);
    update();
}

void VncView::focusOutEvent(QFocusEvent *event)
{
    if (event->reason() == Qt::TabFocusReason || event->reason() == Qt::BacktabFocusReason) {
        event->ignore();
        setFocus(); // get focus back and send tab key event to remote desktop
        vncThread.keyEvent(XK_Tab, true);
        vncThread.keyEvent(XK_Tab, false);
    }

    RemoteView::focusOutEvent(event);
}

void VncView::mouseMoveEvent(QMouseEvent *event)
{
    mouseEvent(event);

    RemoteView::mouseMoveEvent(event);
}

void VncView::mousePressEvent(QMouseEvent *event)
{
    mouseEvent(event);

    RemoteView::mousePressEvent(event);
}

void VncView::mouseDoubleClickEvent(QMouseEvent *event)
{
    mouseEvent(event);

    RemoteView::mouseDoubleClickEvent(event);
}

void VncView::mouseReleaseEvent(QMouseEvent *event)
{
    mouseEvent(event);

    RemoteView::mouseReleaseEvent(event);
}

void VncView::mouseEvent(QMouseEvent *e)
{
    if (e->type() != QEvent::MouseMove) {
        if ((e->type() == QEvent::MouseButtonPress) ||
                (e->type() == QEvent::MouseButtonDblClick)) {
            if (e->button() & Qt::LeftButton)
                m_buttonMask |= 0x01;
            if (e->button() & Qt::MidButton)
                m_buttonMask |= 0x02;
            if (e->button() & Qt::RightButton)
                m_buttonMask |= 0x04;
        } else if (e->type() == QEvent::MouseButtonRelease) {
            if (e->button() & Qt::LeftButton)
                m_buttonMask &= 0xfe;
            if (e->button() & Qt::MidButton)
                m_buttonMask &= 0xfd;
            if (e->button() & Qt::RightButton)
                m_buttonMask &= 0xfb;
        }
    }

    vncThread.mouseEvent(qRound(e->x() / m_horizontalFactor), qRound(e->y() / m_verticalFactor), m_buttonMask);
}

void VncView::wheelEvent(QWheelEvent *event)
{
    int eb = 0;
    if (event->delta() < 0)
        eb |= 0x10;
    else
        eb |= 0x8;

    int x = qRound(event->x() / m_horizontalFactor);
    int y = qRound(event->y() / m_verticalFactor);

    vncThread.mouseEvent(x, y, eb | m_buttonMask);
    vncThread.mouseEvent(x, y, m_buttonMask);

    RemoteView::wheelEvent(event);
}

void VncView::keyEvent(QKeyEvent *e)
{
    int mask = 0;
    
    rfbKeySym k = 0;
    switch (e->key()) {
    case Qt::Key_Backspace: k = XK_BackSpace; break;
    case Qt::Key_Tab: k = XK_Tab; break;
    case Qt::Key_Clear: k = XK_Clear; break;
    case Qt::Key_Return: k = XK_Return; break;
    case Qt::Key_Pause: k = XK_Pause; break;
    case Qt::Key_Escape: k = XK_Escape; break;
    case Qt::Key_Space: k = XK_space; break;
    case Qt::Key_Delete: k = XK_Delete; break;
    case Qt::Key_Enter: k = XK_KP_Enter; break;
    case Qt::Key_Equal: k = XK_equal; break;
    case Qt::Key_Up: k = XK_Up; break;
    case Qt::Key_Down: k = XK_Down; break;
    case Qt::Key_Right: k = XK_Right; break;
    case Qt::Key_Left: k = XK_Left; break;
    case Qt::Key_Insert: k = XK_Insert; break;
    case Qt::Key_Home: k = XK_Home; break;
    case Qt::Key_End: k = XK_End; break;
    case Qt::Key_PageUp: k = XK_Page_Up; break;
    case Qt::Key_PageDown: k = XK_Page_Down; break;
    case Qt::Key_F1: k = XK_F1; break;
    case Qt::Key_F2: k = XK_F2; break;
    case Qt::Key_F3: k = XK_F3; break;
    case Qt::Key_F4: k = XK_F4; break;
    case Qt::Key_F5: k = XK_F5; break;
    case Qt::Key_F6: k = XK_F6; break;
    case Qt::Key_F7: k = XK_F7; break;
    case Qt::Key_F8: k = XK_F8; break;
    case Qt::Key_F9: k = XK_F9; break;
    case Qt::Key_F10: k = XK_F10; break;
    case Qt::Key_F11: k = XK_F11; break;
    case Qt::Key_F12: k = XK_F12; break;
    case Qt::Key_F13: k = XK_F13; break;
    case Qt::Key_F14: k = XK_F14; break;
    case Qt::Key_F15: k = XK_F15; break;
    case Qt::Key_NumLock: k = XK_Num_Lock; break;
    case Qt::Key_CapsLock: k = XK_Caps_Lock; break;
    case Qt::Key_ScrollLock: k = XK_Scroll_Lock; break;
    case Qt::Key_Shift: k = XK_Shift_L; mask |= KMOD_Shift_L; break;
    case Qt::Key_Control: k = XK_Control_L; mask |= KMOD_Control_L; break;
    case Qt::Key_AltGr: k = XK_ISO_Level3_Shift; mask |= KMOD_Alt_R; break;
    case Qt::Key_Alt: k = XK_Alt_L; mask |= KMOD_Alt_L; break;
    case Qt::Key_Meta: k = XK_Meta_L; mask |= KMOD_Meta_L; break;
    case Qt::Key_Mode_switch: k = XK_Mode_switch; break;
    case Qt::Key_Help: k = XK_Help; break;
    case Qt::Key_Print: k = XK_Print; break;
    case Qt::Key_SysReq: k = XK_Sys_Req; break;
    default: break;
    }
    
    // Transform dead keys
    if (e->key() >= Qt::Key_Dead_Grave && e->key() <= Qt::Key_Dead_Horn) {
        k = e->key() - Qt::Key_Dead_Grave + XK_dead_grave;
    }
    
    bool pressed = (e->type() == QEvent::KeyPress) ? true : false;
    m_modifiersMask = pressed ? m_modifiersMask | mask : m_modifiersMask & ~mask;

    bool hasShift = m_modifiersMask & KMOD_Shift_L;
    bool hasOtherMod = m_modifiersMask & (KMOD_Alt_R | KMOD_Alt_L | KMOD_Meta_L | KMOD_Control_L);
    bool isUpper = e->key() >= 'A' && e->key() <= 'Z';
    
    // If shift is pressed, we receive Alt as Meta, but we want to actually send Alt.
    if (k == XK_Meta_L && hasShift) k = XK_Alt_L;

    if (k == 0) {
        if (hasOtherMod && (! isUpper || hasShift)) {
            k = e->key();
        } else {
            if (e->key() < 0x100 && e->text().length() > 0)
                k = QChar(e->text().at(0)).unicode(); //respect upper- / lowercase
            else
                rfbClientLog("Unknown keysym: 0x%x\n", e->key());
        }
    }

    if (k < 26) // workaround for modified keys by pressing CTRL
        k += 96;

    vncThread.keyEvent(k, pressed);
    RemoteView::keyEvent(e);
}

void VncView::keyPressEvent(QKeyEvent *event)
{
    keyEvent(event);

    RemoteView::keyPressEvent(event);
}

void VncView::keyReleaseEvent(QKeyEvent *event)
{
    keyEvent(event);

    RemoteView::keyReleaseEvent(event);
}

void VncView::clipboardSelectionChanged()
{
    if (m_status != Connected)
        return;

    if (m_clipboard->ownsSelection() || m_dontSendClipboard)
        return;

    QString text = m_clipboard->text(QClipboard::Selection);

    vncThread.clientCut(text);
}

void VncView::clipboardDataChanged()
{
    if (m_status != Connected)
        return;

    if (m_clipboard->ownsClipboard() || m_dontSendClipboard)
        return;

    QString text = m_clipboard->text(QClipboard::Clipboard);

    vncThread.clientCut(text);
}
