/*
 * DMOutputApi.cpp
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

#include "DMOutputApi.h"

DMOutputApi::DMOutputApi(QObject *parent):
    QObject(parent)
{
    setObjectName("output");
}

void DMOutputApi::write(QString msg)
{
    Global::Output.write(msg);
}

void DMOutputApi::error(QString msg)
{
    Global::Output.error(msg);
}

void DMOutputApi::clear()
{
    Global::Output.clear();
}
