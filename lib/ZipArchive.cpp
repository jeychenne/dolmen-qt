/*
 * ZipArchive.cpp
 *
 * Copyright (C) 2010-2017 Julien Eychenne
 * Copyright (C) 2012 Japanese Society for the Promotion of Science (JSPS)
 *
 * This file is part of Dolmen.
 *
 * Dolmen is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License (GPL) as
 * published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * Dolmen is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Dolmen. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author:  Julien Eychenne
 * Purpose: A simple wrapper over QuaZip to handle compressed directories
 * Created: 14/04/2012
 *
 * The development of this module has been supported by the Japanese
 * Society for the Promotion of Science (JSPS), Grant-in-Aid for Scientific
 * Research (B) n°23320121 (2011-2014).
 * Project title: A corpus-based longitudinal study of the interphonological
 * features of Japanese learners of French.
 * Project leader: Sylvain DETEY (Waseda University, Tokyo, Japan).
 */


#include "ZipArchive.h"

ZipArchive::ZipArchive(QString zippath, QString unzippath, QObject *parent) :
    QObject(parent)
{
    m_zip_file = zippath;
    m_unzip_dir = unzippath;

#ifndef USE_QT5
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForCStrings(codec);
#endif
}

void ZipArchive::zip()
{

}

bool ZipArchive::unzip()
{
    qDebug() << "ZIPF" << m_zip_file;
    qDebug() << "UNZIPF" << m_unzip_dir;
    QuaZip zip(m_zip_file);
    zip.open(QuaZip::mdUnzip);

    if (!zip.goToFirstFile())
        return false;

    QString pluginDir = m_unzip_dir + "/" + zip.getCurrentFileName();
    qDebug() << "PLUGIN DIR" << pluginDir;
    QDir outputDir(pluginDir);

    if (!outputDir.mkpath(pluginDir))
        return false;

    while (zip.goToNextFile())
    {
        QString filename = zip.getCurrentFileName();

        // Ignore Mac junk
        if (filename.startsWith("__MACOSX") || filename.endsWith(".DS_Store"))
            continue;

        if (filename.endsWith('/'))
        {
            QDir subdir(zip.getCurrentFileName());
            outputDir.mkdir(subdir.dirName());
        }
        else
        {
            QuaZipFile file(&zip);

            file.open(QIODevice::ReadOnly);
            QString currentPath = m_unzip_dir + "/" + zip.getCurrentFileName();

            // Get file content
            QByteArray bytes = file.readAll();
            // Open output file
            QFile outfile(currentPath);
            if (!outfile.open(QIODevice::WriteOnly))
            {
                qDebug() << "CAN'T CREATE" << currentPath << qPrintable(outfile.errorString());
                return false;
            }

            // Text file
            if (currentPath.endsWith(".json") || currentPath.endsWith(".js") || currentPath.toLower().endsWith(".txt"))
            {
                QTextStream outstream(&outfile);
                QString content(bytes);
                outstream.setCodec("UTF-8");
                //outstream.setGenerateByteOrderMark(true);
                outstream << content;
                outfile.close();
            }
            // Binary file
            else
            {
                QDataStream outstream(&outfile);
                outstream << bytes;
                outfile.close();
            }
        }
    }

    return true;
}
