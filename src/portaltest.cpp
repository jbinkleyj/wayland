/*
 * Copyright © 2016 Red Hat, Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *       Jan Grulich <jgrulich@redhat.com>
 */

#include "portaltest.h"
#include "ui_portaltest.h"

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusUnixFileDescriptor>
#include <QWindow>

#include <gst/gst.h>

Q_LOGGING_CATEGORY(PortalTestKde, "portal-test-kde")

Q_DECLARE_METATYPE(PortalTest::Stream);
Q_DECLARE_METATYPE(PortalTest::Streams);

const QDBusArgument &operator >> (const QDBusArgument &arg, PortalTest::Stream &stream)
{
    arg.beginStructure();
    arg >> stream.node_id;

    arg.beginMap();
    while (!arg.atEnd()) {
        QString key;
        QVariant map;
        arg.beginMapEntry();
        arg >> key >> map;
        arg.endMapEntry();
        stream.map.insert(key, map);
    }
    arg.endMap();
    arg.endStructure();

    return arg;
}

PortalTest::PortalTest(QWidget *parent, Qt::WindowFlags f)
    : QMainWindow(parent, f)
    , m_mainWindow(new Ui::PortalTest)
    , m_sessionTokenCounter(0)
    , m_requestTokenCounter(0)
{
    QLoggingCategory::setFilterRules(QStringLiteral("portal-test-kde.debug = true"));

    m_mainWindow->setupUi(this);

    connect(m_mainWindow->requestDeviceAccess, &QPushButton::clicked, this, &PortalTest::requestDeviceAccess);
    connect(m_mainWindow->screenShareButton, &QPushButton::clicked, this, &PortalTest::requestScreenSharing);

    connect( m_mainWindow->pushButtonStop, &QPushButton::clicked, this, &PortalTest::slot_Stop );

    gst_init(nullptr, nullptr);
}

PortalTest::~PortalTest()
{
    delete m_mainWindow;
}

void PortalTest::requestDeviceAccess()
{
    qWarning() << "Request device access";
    const QString device = m_mainWindow->deviceCombobox->currentIndex() == 0 ? QLatin1String("microphone") :
                                                                               m_mainWindow->deviceCombobox->currentIndex() == 1 ? QLatin1String("speakers") : QLatin1String("camera");

qDebug() <<  device << "111111111111111111111111111111111111111111111111111111111111111111";
    QDBusMessage message = QDBusMessage::createMethodCall(QLatin1String("org.freedesktop.portal.Desktop"),
                                                          QLatin1String("/org/freedesktop/portal/desktop"),
                                                          QLatin1String("org.freedesktop.portal.Device"),
                                                          QLatin1String("AccessDevice"));
    message << (uint)QApplication::applicationPid() << QStringList {device} << QVariantMap();
qDebug() << message << "222222222222222222222222222222222222222222222222222222222222222222";
    QDBusPendingCall pendingCall = QDBusConnection::sessionBus().asyncCall(message);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingCall);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this] (QDBusPendingCallWatcher *watcher)
    {
        QDBusPendingReply<QDBusObjectPath> reply = *watcher;
        if (reply.isError()) {
            qWarning() << "Couldn't get reply";
            qWarning() << "Error: " << reply.error().message();
        } else {
            qWarning() << reply.value().path();
        }
    });
}

void PortalTest::requestScreenSharing()
{
    QDBusMessage message = QDBusMessage::createMethodCall(QLatin1String("org.freedesktop.portal.Desktop"),
                                                          QLatin1String("/org/freedesktop/portal/desktop"),
                                                          QLatin1String("org.freedesktop.portal.ScreenCast"),
                                                          QLatin1String("CreateSession"));

    message << QVariantMap { { QLatin1String("session_handle_token"), getSessionToken() }, { QLatin1String("handle_token"), getRequestToken() } };

    QDBusPendingCall pendingCall = QDBusConnection::sessionBus().asyncCall(message);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingCall);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this] (QDBusPendingCallWatcher *watcher) {
        QDBusPendingReply<QDBusObjectPath> reply = *watcher;
        if (reply.isError()) {
            qWarning() << "Couldn't get reply";
            qWarning() << "Error: " << reply.error().message();
        } else {
            QDBusConnection::sessionBus().connect(QString(),
                                                reply.value().path(),
                                                QLatin1String("org.freedesktop.portal.Request"),
                                                QLatin1String("Response"),
                                                this,
                                                SLOT(gotCreateSessionResponse(uint,QVariantMap)));
        }
    });
}

void PortalTest::gotCreateSessionResponse(uint response, const QVariantMap &results)
{
    if (response != 0) {
        qWarning() << "Failed to create session: " << response;
        return;
    }

    QDBusMessage message = QDBusMessage::createMethodCall(QLatin1String("org.freedesktop.portal.Desktop"),
                                                          QLatin1String("/org/freedesktop/portal/desktop"),
                                                          QLatin1String("org.freedesktop.portal.ScreenCast"),
                                                          QLatin1String("SelectSources"));

    m_session = results.value(QLatin1String("session_handle")).toString();

    message << QVariant::fromValue(QDBusObjectPath(m_session))
            << QVariantMap { { QLatin1String("multiple"), false},
                             { QLatin1String("types"), (uint)m_mainWindow->screenShareCombobox->currentIndex() + 1},
                             { QLatin1String("handle_token"), getRequestToken() } };

    QDBusPendingCall pendingCall = QDBusConnection::sessionBus().asyncCall(message);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingCall);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this] (QDBusPendingCallWatcher *watcher) {
        QDBusPendingReply<QDBusObjectPath> reply = *watcher;
        if (reply.isError()) {
            qWarning() << "Couldn't get reply";
            qWarning() << "Error: " << reply.error().message();
        } else {
            QDBusConnection::sessionBus().connect(QString(),
                                                reply.value().path(),
                                                QLatin1String("org.freedesktop.portal.Request"),
                                                QLatin1String("Response"),
                                                this,
                                                SLOT(gotSelectSourcesResponse(uint,QVariantMap)));
        }
    });
}

void PortalTest::gotSelectSourcesResponse(uint response, const QVariantMap &results)
{
    if (response != 0) {
        qWarning() << "Failed to select sources: " << response;
        return;
    }

    QDBusMessage message = QDBusMessage::createMethodCall(QLatin1String("org.freedesktop.portal.Desktop"),
                                                          QLatin1String("/org/freedesktop/portal/desktop"),
                                                          QLatin1String("org.freedesktop.portal.ScreenCast"),
                                                          QLatin1String("Start"));

    message << QVariant::fromValue(QDBusObjectPath(m_session))
            << QString() // parent_window
            << QVariantMap { { QLatin1String("handle_token"), getRequestToken() } };

    QDBusPendingCall pendingCall = QDBusConnection::sessionBus().asyncCall(message);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingCall);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this] (QDBusPendingCallWatcher *watcher)
    {
        QDBusPendingReply<QDBusObjectPath> reply = *watcher;
        if (reply.isError()) {
            qWarning() << "Couldn't get reply";
            qWarning() << "Error: " << reply.error().message();
        } else {
            QDBusConnection::sessionBus().connect(QString(),
                                                reply.value().path(),
                                                QLatin1String("org.freedesktop.portal.Request"),
                                                QLatin1String("Response"),
                                                this,
                                                SLOT(gotStartResponse(uint,QVariantMap)));
        }
    });
}

void PortalTest::gotStartResponse(uint response, const QVariantMap &results)
{
    if (response != 0) {
        qWarning() << "Failed to start: " << response;
    }

    Streams streams = qdbus_cast<Streams>(results.value(QLatin1String("streams")));
    Q_FOREACH (Stream stream, streams)
    {
        QDBusMessage message = QDBusMessage::createMethodCall(QLatin1String("org.freedesktop.portal.Desktop"),
                                                              QLatin1String("/org/freedesktop/portal/desktop"),
                                                              QLatin1String("org.freedesktop.portal.ScreenCast"),
                                                              QLatin1String("OpenPipeWireRemote"));

        message << QVariant::fromValue(QDBusObjectPath(m_session)) << QVariantMap();
qDebug() << message;
        QDBusPendingCall pendingCall = QDBusConnection::sessionBus().asyncCall(message);
        pendingCall.waitForFinished();
        QDBusPendingReply<QDBusUnixFileDescriptor> reply = pendingCall.reply();
        if (reply.isError()) {
            qWarning() << "Failed to get fd for node_id " << stream.node_id;
        }

        QString gstLaunch = QString("pipewiresrc fd=%1 path=%2 do-timestamp=true ! queue ! videoconvert n-threads=4 ! x264enc key-int-max=1 qp-min=17 qp-max=17 speed-preset=medium threads=4 ! video/x-h264, profile=baseline ! matroskamux ! filesink location=vokoscreenNG.mkv" ).arg(reply.value().fileDescriptor()).arg(stream.node_id);

qDebug();
        qDebug() << gstLaunch;
qDebug();
        element = gst_parse_launch(gstLaunch.toUtf8(), nullptr);
        gst_element_set_state( element, GST_STATE_PLAYING);
    }
}

void PortalTest::slot_Stop()
{
    GstStateChangeReturn ret ;
    ret = gst_element_set_state( element, GST_STATE_PAUSED );
    ret = gst_element_set_state( element, GST_STATE_READY );
    ret = gst_element_set_state( element, GST_STATE_NULL );
    gst_object_unref( element );
    qDebug().noquote() << "Stop record";
    make_time_true();
}

void PortalTest::make_time_true()
{
    QString gstLaunch = QString( "filesrc location=vokoscreenNG.mkv ! matroskademux name=d d.video_0 ! matroskamux ! filesink location=voko-1.mkv" );
    element = gst_parse_launch(gstLaunch.toUtf8(), nullptr);
    gst_element_set_state( element, GST_STATE_PLAYING);
}


QString PortalTest::getSessionToken()
{
    m_sessionTokenCounter += 1;
    return QString("u%1").arg(m_sessionTokenCounter);
}

QString PortalTest::getRequestToken()
{
    m_requestTokenCounter += 1;
    return QString("u%1").arg(m_requestTokenCounter);
}
