/*
 * JsonObject.cpp
 *
 * Copyright (C) 2010-2013 Julien Eychenne
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
 * Purpose: json wrapper
 * Created: 24/02/2012
 */

#include "JsonObject.h"


JsonObject::JsonObject()
{
    m_path = m_text = "";
}

bool JsonObject::load(QString path)
{
    QFile data(path);
    QString line;


    if (!data.open(QIODevice::ReadOnly))
    {
        QString msg = QString("Cannot read TextGrid %1").arg(path);
        ///TODO ..................emit file_error(msg); return;
    }
    else
    {
        QTextStream stream(&data);
        stream.setCodec("UTF-8");
        stream.setAutoDetectUnicode(true);

        do
        {
            line = stream.readLine().trimmed();
            if (!line.startsWith("//"))
                m_text += line;
        }
        while (! stream.atEnd());
    }
    data.close();

    bool ok;
    map = Json::parse(m_text, ok).toMap();

    if (ok) m_path = path;
    else
        qDebug() << "Problem parsing the plugin" << path;

    return ok;
}

void JsonObject::dump()
{
//    if (!m_path.isEmpty())
//        ;
}
