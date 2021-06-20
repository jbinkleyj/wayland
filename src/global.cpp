/* vokoscreenNG - A desktop recorder
 * Copyright (C) 2017-2021 Volker Kohaupt
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
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include <QFile>

#include <iostream>

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


    QFile logFile;
    bool log_filename_exists = false;
    Ui::formMainWindow_wl *ui;
    void vk_out( QString value )
    {
        // Output terminal
        std::cout << value.toStdString() << std::endl << std::flush;

        // Output log file
        if ( log_filename_exists == false )
        {
            QDateTime dateTime = QDateTime::currentDateTime();
            QString stringDateTime = dateTime.toString( "yyyy-MM-dd_hh-mm-ss" );
            QString path = QStandardPaths::writableLocation( QStandardPaths::AppConfigLocation );

            QString logDirName = path + "/" + "log";
            if( !QDir( logDirName ).exists() )
            {
                QDir().mkpath( logDirName );
            }

            logFile.setFileName( path + "/" + "log" + "/" + stringDateTime + ".log" );

            log_filename_exists = true;
        }

        QString eol = "\n";
        logFile.open( QIODevice::Append | QIODevice::Text | QIODevice::Unbuffered );
        logFile.write( value.toUtf8() );
        logFile.write( eol.toUtf8() );
        logFile.close();

        // Output GUI
        ui->textBrowserLog->append( value );
    }
}
