/* vokoscreenNG - A desktop recorder
 * Copyright (C) 2017-2019 Volker Kohaupt
 * 
 * Author:
 *      Volker Kohaupt <vkohaupt@freenet.de>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * --End_License--
 */

#include "global.h"

#include <QString>
#include <iostream>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include <QFile>

namespace global
{
    QString name = "vokoscreenNG";
    QString nameOutput = "[" + name + "]";

    #ifdef Q_OS_LINUX
    QString version = "3.1.0 pre alpha";
    #endif

    #ifdef Q_OS_WIN
    QString version = "3.1.0 pre alpha";
    #endif


void vk_out( QString value )
{
    std::cout << value.toStdString() << std::endl << std::flush;
    vk_create_log_filename();
    write_to_log( value );
}


QFile logFile;
bool log_filename_exists = false;
void vk_create_log_filename()
{
    if ( log_filename_exists == false )
    {
        QDateTime dateTime = QDateTime::currentDateTime();
        QString stringDateTime = dateTime.toString( "yyyy_MM_dd_hh_mm_ss" );
        QString path = QStandardPaths::writableLocation( QStandardPaths::AppConfigLocation );

        QString logFolderName = path + "/" + "log";
        if( !QDir( logFolderName ).exists() )
        {
            QDir().mkpath( logFolderName );
        }

        logFile.setFileName( path + "/" + "log" + "/" + stringDateTime + ".log" );

        log_filename_exists = true;
    }
}


void write_to_log( QString value )
{
#ifdef Q_OS_LINUX
    QString eol = "\n";
#endif
#ifdef Q_OS_WIN
    QString eol = "\r\n";
#endif
    logFile.open( QIODevice::Append | QIODevice::Text | QIODevice::Unbuffered );
    logFile.write( value.toUtf8() );
    logFile.write( eol.toUtf8() );
    logFile.close();
}




}
