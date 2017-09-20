/*
 * ScriptBrowser.h
 *
 * Copyright (C) 2010-2017 Julien Eychenne
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
 * Purpose: display scripts in the leftmost panel.
 * Created: 15/09/2017
 */

#ifndef SCRIPTBROWSER_H
#define SCRIPTBROWSER_H

#include "ElementBrowser.h"
#include "lib/Script.h"

class ScriptBrowser : public ElementBrowser
{
    Q_OBJECT

public:
    ScriptBrowser(QWidget *parent = NULL);

protected:
    void displayElements() override;
    void finalizeElements() override;


};

#endif // SCRIPTBROWSER_H
