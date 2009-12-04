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

#include "VNC_Client_Thread.h"
#include "Utils.h"

#include <QMutexLocker>
#include <QTimer>

static QString outputErrorMessageString;

rfbBool VncClientThread::newclient(rfbClient *cl)
{
    VncClientThread *t = (VncClientThread*)rfbClientGetClientData(cl, 0);
    Q_ASSERT(t);

    int width = cl->width, height = cl->height, depth = cl->format.bitsPerPixel;
    int size = width * height * (depth / 8);
    if (t->frameBuffer)
        delete [] t->frameBuffer; // do not leak if we get a new framebuffer size
    t->frameBuffer = new uint8_t[size];
    cl->frameBuffer = t->frameBuffer;
    memset(cl->frameBuffer, '\0', size);
    cl->format.bitsPerPixel = 32;
    cl->format.redShift = 16;
    cl->format.greenShift = 8;
    cl->format.blueShift = 0;
    cl->format.redMax = 0xff;
    cl->format.greenMax = 0xff;
    cl->format.blueMax = 0xff;

    switch (t->quality()) {
    case RemoteView::High:
        cl->appData.useBGR233 = 0;
        cl->appData.encodingsString = "copyrect hextile raw";
        cl->appData.compressLevel = 0;
        cl->appData.qualityLevel = 9;
        break;
    case RemoteView::Medium:
        cl->appData.useBGR233 = 0;
        cl->appData.encodingsString = "tight zrle ultra copyrect hextile zlib corre rre raw";
        cl->appData.compressLevel = 5;
        cl->appData.qualityLevel = 7;
        break;
    case RemoteView::Low:
    case RemoteView::Unknown:
    default:
        cl->appData.useBGR233 = 1;
        cl->appData.encodingsString = "tight zrle ultra copyrect hextile zlib corre rre raw";
        cl->appData.compressLevel = 9;
        cl->appData.qualityLevel = 1;
    }

    SetFormatAndEncodings(cl);

    return true;
}

void VncClientThread::updatefb(rfbClient* cl, int x, int y, int w, int h)
{
    int width = cl->width, height = cl->height;

    QImage img(cl->frameBuffer, width, height, QImage::Format_RGB32);

    if (img.isNull())
        AQDebug( "void VncClientThread::updatefb(rfbClient* cl, int x, int y, int w, int h)",
				 "image not loaded" );

    VncClientThread *t = (VncClientThread*)rfbClientGetClientData(cl, 0);
    Q_ASSERT(t);

    t->setImage(img);

    t->emitUpdated(x, y, w, h);
}

void VncClientThread::cuttext(rfbClient* cl, const char *text, int textlen)
{
    QString cutText = QString::fromUtf8(text, textlen);
	
	AQDebug( "void VncClientThread::cuttext(rfbClient* cl, const char *text, int textlen)",
			 cutText );

    if (!cutText.isEmpty()) {
        VncClientThread *t = (VncClientThread*)rfbClientGetClientData(cl, 0);
        Q_ASSERT(t);

        t->emitGotCut(cutText);
    }
}

char *VncClientThread::passwdHandler(rfbClient *cl)
{
	AQDebug( "char *VncClientThread::passwdHandler(rfbClient *cl)",
			 "password request" );

    VncClientThread *t = (VncClientThread*)rfbClientGetClientData(cl, 0);
    Q_ASSERT(t);

    t->passwordRequest();
    t->m_passwordError = true;

    return strdup(t->password().toLocal8Bit());
}

void VncClientThread::outputHandler(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    QString message;
    message.vsprintf(format, args);

    va_end(args);

    message = message.trimmed();

	AQDebug( "void VncClientThread::outputHandler(const char *format, ...)",
			 message );

    if ((message.contains("Couldn't convert ")) ||
            (message.contains("Unable to connect to VNC server")))
        outputErrorMessageString = tr("Server not found.");

    if ((message.contains("VNC connection failed: Authentication failed, too many tries")) ||
            (message.contains("VNC connection failed: Too many authentication failures")))
        outputErrorMessageString = tr("VNC authentication failed because of too many authentication tries.");

    if (message.contains("VNC connection failed: Authentication failed"))
        outputErrorMessageString = tr("VNC authentication failed.");

    if (message.contains("VNC server closed connection"))
        outputErrorMessageString = "VNC server closed connection.";

    // internal messages, not displayed to user
    if (message.contains("VNC server supports protocol version 3.889")) // see http://bugs.kde.org/162640
        outputErrorMessageString = "INTERNAL:APPLE_VNC_COMPATIBILTY";
}

VncClientThread::VncClientThread(QObject *parent)
        : QThread(parent)
        , frameBuffer(0)
{
    QMutexLocker locker(&mutex);
    m_stopped = false;

    QTimer *outputErrorMessagesCheckTimer = new QTimer(this);
    outputErrorMessagesCheckTimer->setInterval(500);
    connect(outputErrorMessagesCheckTimer, SIGNAL(timeout()), this, SLOT(checkOutputErrorMessage()));
    outputErrorMessagesCheckTimer->start();
}

VncClientThread::~VncClientThread()
{
    stop();
    wait(500);
    
    delete [] frameBuffer;
}

void VncClientThread::checkOutputErrorMessage()
{
    if (!outputErrorMessageString.isEmpty()) {
		AQDebug( "void VncClientThread::checkOutputErrorMessage()", outputErrorMessageString );
        QString errorMessage = outputErrorMessageString;
        outputErrorMessageString.clear();
        // show authentication failure error only after the 3rd unsuccessful try
        if ((errorMessage != tr("VNC authentication failed.")) || m_passwordError)
            outputErrorMessage(errorMessage);
    }
}

void VncClientThread::setHost(const QString &host)
{
    QMutexLocker locker(&mutex);
    m_host = host;
}

void VncClientThread::setPort(int port)
{
    QMutexLocker locker(&mutex);
    m_port = port;
}

void VncClientThread::setQuality(RemoteView::Quality quality)
{
    m_quality = quality;
}

RemoteView::Quality VncClientThread::quality() const
{
    return m_quality;
}

void VncClientThread::setImage(const QImage &img)
{
    QMutexLocker locker(&mutex);
    m_image = img;
}

const QImage VncClientThread::image(int x, int y, int w, int h)
{
    QMutexLocker locker(&mutex);

    if (w == 0) // full image requested
        return m_image;
    else
        return m_image.copy(x, y, w, h);
}

void VncClientThread::emitUpdated(int x, int y, int w, int h)
{
    emit imageUpdated(x, y, w, h);
}

void VncClientThread::emitGotCut(const QString &text)
{
    emit gotCut(text);
}

void VncClientThread::stop()
{
    QMutexLocker locker(&mutex);
    m_stopped = true;
}

void VncClientThread::run()
{
    QMutexLocker locker(&mutex);

    while (!m_stopped) { // try to connect as long as the server allows
        m_passwordError = false;

        rfbClientLog = outputHandler;
        rfbClientErr = outputHandler;
        cl = rfbGetClient(8, 3, 4);
        cl->MallocFrameBuffer = newclient;
        cl->canHandleNewFBSize = true;
        cl->GetPassword = passwdHandler;
        cl->GotFrameBufferUpdate = updatefb;
        cl->GotXCutText = cuttext;
        rfbClientSetClientData(cl, 0, this);

        cl->serverHost = strdup(m_host.toUtf8().constData());

        if (m_port < 0 || !m_port) // port is invalid or empty...
            m_port = 5900; // fallback: try an often used VNC port

        if (m_port >= 0 && m_port < 100) // the user most likely used the short form (e.g. :1)
            m_port += 5900;
        cl->serverPort = m_port;

		AQDebug( "void VncClientThread::run()", "Init..." );

        if (rfbInitClient(cl, 0, 0))
            break;

        if (m_passwordError)
            continue;

        return;
    }

    locker.unlock();

    // Main VNC event loop
    while (!m_stopped) {
        int i = WaitForMessage(cl, 500);
        if (i < 0)
            break;
        if (i)
            if (!HandleRFBServerMessage(cl))
                break;

        locker.relock();

        while (!m_eventQueue.isEmpty()) {
            ClientEvent* clientEvent = m_eventQueue.dequeue();
            clientEvent->fire(cl);
            delete clientEvent;
        }

        locker.unlock();
    }

    // Cleanup allocated resources
    locker.relock();
    rfbClientCleanup(cl);
    m_stopped = true;
}

ClientEvent::~ClientEvent()
{
}

void PointerClientEvent::fire(rfbClient* cl)
{
    SendPointerEvent(cl, m_x, m_y, m_buttonMask);
}

void KeyClientEvent::fire(rfbClient* cl)
{
    SendKeyEvent(cl, m_key, m_pressed);
}

void ClientCutEvent::fire(rfbClient* cl)
{
    SendClientCutText(cl, text, qstrlen(text));
}

void VncClientThread::mouseEvent(int x, int y, int buttonMask)
{
    QMutexLocker lock(&mutex);
    if (m_stopped)
        return;

    m_eventQueue.enqueue(new PointerClientEvent(x, y, buttonMask));
}

void VncClientThread::keyEvent(int key, bool pressed)
{
    QMutexLocker lock(&mutex);
    if (m_stopped)
        return;

    m_eventQueue.enqueue(new KeyClientEvent(key, pressed));
}

void VncClientThread::clientCut(const QString &text)
{
    QMutexLocker lock(&mutex);
    if (m_stopped)
        return;

    m_eventQueue.enqueue(new ClientCutEvent(strdup(text.toUtf8())));
}
