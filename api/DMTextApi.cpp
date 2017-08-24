/*
 * DMTextApi.cpp
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
 * Purpose: implements a text file API exposed to the scripting environment
 * Created: 07/03/2012
 */

#include "DMTextApi.h"

DMTextApi::DMTextApi(QObject *parent) :
    QObject(parent)
{
    setObjectName("text");
}

QString DMTextApi::readAll(QString path, QString encoding) const
{
    Q_UNUSED(encoding);
    QFile file(path);
    if (! file.open(QIODevice::ReadOnly | QIODevice::Text))
        return "";

    QTextStream stream(&file);
    QString text = stream.readAll();
    file.close();

    return text;
}
