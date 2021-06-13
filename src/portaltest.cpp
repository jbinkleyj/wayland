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

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusUnixFileDescriptor>

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

PortalTest::PortalTest()
    : m_sessionTokenCounter(0)
    , m_requestTokenCounter(0)
{
}

PortalTest::~PortalTest()
{
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
        if (reply.isError())
        {
            qWarning() << "Couldn't get reply";
            qWarning() << "Error: " << reply.error().message();
        } else
        {
            QDBusConnection::sessionBus().connect(QString(),
                                                  reply.value().path(),
                                                  QLatin1String("org.freedesktop.portal.Request"),
                                                  QLatin1String("Response"),
                                                  this,
                                                  SLOT(gotCreateSessionResponse(uint,QVariantMap)));
        }
        qDebug() << "reply.value().path():" << reply.value().path();
    });
}

void PortalTest::gotCreateSessionResponse(uint response, const QVariantMap &results)
{
qDebug() << "1111111111111111111111111111111111111111111111111111111111 response:" << response << "results:" << results;
    if (response != 0)
    {
        qWarning() << "Failed to create session: " << response;
        return;
    }

    QDBusMessage message = QDBusMessage::createMethodCall(QLatin1String("org.freedesktop.portal.Desktop"),
                                                          QLatin1String("/org/freedesktop/portal/desktop"),
                                                          QLatin1String("org.freedesktop.portal.ScreenCast"),
                                                          QLatin1String("SelectSources"));

    m_session = results.value(QLatin1String("session_handle")).toString();

    message << QVariant::fromValue(QDBusObjectPath(m_session))
            << QVariantMap { { QLatin1String("multiple"), true},
                             { QLatin1String("types"), (uint)1 }, //(uint)m_mainWindow->screenShareCombobox->currentIndex() + 1}, // 1 = Monitor
                             { QLatin1String("handle_token"), getRequestToken() } };

qDebug() << "1111111111111111111111111111111111111111111111111111111111 message:"<< message;

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
                                                SLOT(gotSelectSourcesResponse(uint,QVariantMap)));
        }
    });
qDebug() << "2222222222222222222222222222222222222222222222222222222222222222222";
}

void PortalTest::gotSelectSourcesResponse( uint response, const QVariantMap &results )
{
qDebug() << "3333333333333333333333333333333333333333333333333333333333333333333333 response:" << response << "results:" << results;
    if ( response != 0 )
    {
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
qDebug() << "44444444444444444444444444444444444444444444444444444444444444444444444";
}


void PortalTest::gotStartResponse( uint response, const QVariantMap &results )
{
qDebug() << "55555555555555555555555555555555555555555555555555555555555555 response:" << response << "results:" << results;
    if ( response != 0 )
    {
        // KDE-Dialog wird per Button <Abbruch> abgeprochen
        qWarning() << "Failed to start: " << response;
        return;
    }

    Streams streams = qdbus_cast<Streams>(results.value(QLatin1String("streams")));
    Stream stream = streams.at( vk_startCounter );
    vk_startCounter++;

    QDBusMessage message = QDBusMessage::createMethodCall(QLatin1String("org.freedesktop.portal.Desktop"),
                                                          QLatin1String("/org/freedesktop/portal/desktop"),
                                                          QLatin1String("org.freedesktop.portal.ScreenCast"),
                                                          QLatin1String("OpenPipeWireRemote"));

    message << QVariant::fromValue(QDBusObjectPath(m_session)) << QVariantMap();

    QDBusPendingCall pendingCall = QDBusConnection::sessionBus().asyncCall( message );
    pendingCall.waitForFinished();
    QDBusPendingReply<QDBusUnixFileDescriptor> reply = pendingCall.reply();
    if (reply.isError())
    {
        qWarning() << "Failed to get fd for node_id " << stream.node_id;
    }

    vk_fd = QString::number( reply.value().fileDescriptor() );
    vk_path = QString::number( stream.node_id );

    emit signal_fd_path( vk_fd, vk_path );
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
