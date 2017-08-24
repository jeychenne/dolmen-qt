/*
 * DMTextApi.h
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

#ifndef DMTEXTAPI_H
#define DMTEXTAPI_H

#include <QObject>
#include <QFile>
#include <QTextStream>

class DMTextApi : public QObject
{
    Q_OBJECT
public:
    explicit DMTextApi(QObject *parent = 0);
    Q_INVOKABLE QString readAll(QString path, QString encoding) const;
    
signals:
    
public slots:
    
};

#endif // DMTEXTAPI_H


/*
 Q_DECLARE_METATYPE(MyObject*)

 QScriptValue myObjectToScriptValue(QScriptEngine *engine, MyObject* const &in)
 { return engine->newQObject(in); }

 void myObjectFromScriptValue(const QScriptValue &object, MyObject* &out)
 { out = qobject_cast<MyObject*>(object.toQObject()); }

 ...

 qScriptRegisterMetaType(&engine, myObjectToScriptValue, myObjectFromScriptValue);
*/
