/*
 * DMScriptApi.h
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
 * Purpose: Top-level scripting API
 * Created: 06/03/2012
 */

#ifndef DMSCRIPTAPI_H
#define DMSCRIPTAPI_H

#include <QObject>
#include <QList>
#include <QMessageBox>

#include "scripting.h"
#include "lib/Plugin.h"


class PluginHandler : public QObject
{
    Q_OBJECT

public:
    PluginHandler(QWidget *parent, sol::state_view lua);
    ~PluginHandler();
    QList<Plugin*> plugins() const;
    void setMainPlugin(Plugin *p);
    Plugin * mainPlugin() const;
    void addPlugin(Plugin *p);
    void executeMainPlugin();

private slots:
    void executeScript(const QString &script, const QString &content);

    
private:
    QWidget *m_widget;
    QList<Plugin*> m_plugins;
    Plugin *m_mainPlugin;
    sol::state_view m_lua;
};

#endif // DMSCRIPTAPI_H
