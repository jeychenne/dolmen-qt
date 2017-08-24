/*
 * DMOutputApi.h
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
 * Purpose: encapsulate standard output in the scripting API
 * Created: Febrary 2012
 */

#ifndef DMOUTPUTAPI_H
#define DMOUTPUTAPI_H

#include <QObject>
#include "utils/Global.h"
#include "gui/OutputTab.h"

class DMOutputApi : public QObject
{
    Q_OBJECT
public:
    explicit DMOutputApi(QObject *parent);

    
signals:
    
public slots:
    void write(QString msg);
    void error(QString msg);
    void clear();
};

#endif // DMOUTPUTAPI_H
