/*
 * JsonObject.h
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

#ifndef JSONOBJECT_H
#define JSONOBJECT_H

#include <QFile>
#include <QTextStream>
#include <QVariantMap>
#include <QDebug>
#include "extra/json.h"

using namespace QtJson;


class JsonObject
{
public:
    JsonObject();
    bool load(QString path);
    void dump();
    QVariantMap map;

private:
    QString m_path, m_text;
};

#endif // JSONOBJECT_H
