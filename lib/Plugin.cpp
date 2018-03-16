/*
 * Plugin.cpp
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
 * Purpose: Plugins are extensions loaded at run-time
 * Created: 24/02/2012
 */

#include <QtAlgorithms>

#include "Plugin.h"


static bool grammarIsLessThan(const Grammar *g1, const Grammar *g2)
{
    return g1->name() < g2->name();
}

Plugin::Plugin(QDir dir)
{
    m_dir = dir;
    m_files = m_dir.entryList();
    m_files.removeAll(".");
    m_files.removeAll("..");
    m_name = m_version = "";
    m_menu = NULL;
}

bool Plugin::isValid() const
{
    return m_files.contains("description.json");
}

bool Plugin::initialize()
{
    JsonObject json;
    if (!json.load(absoluteFilePath("description.json")))
        return false;

    if (!json.map.contains("PluginInfo"))
        return false;

    QVariantMap info = json.map["PluginInfo"].toMap();
    if (!info.contains("Name"))
        return false;

    m_is_main = info.contains("MainApplication") && info["MainApplication"].toString() == "true";
    m_name = info["Name"].toString();

    if (json.map.contains("Menu"))
    {
        QVariantMap map = json.map["Menu"].toMap();
        parseMenu(map);
    }

    return true;
}

bool Plugin::postInitialize()
{
    QString script("init.lua");
    auto content = scriptContent(script);
    emit scriptCalled(script, content);

    QString msg = tr("Plugin \"%1\" successfully loaded...").arg(m_name);
    emit output(msg);

    return true;
}

QList<Grammar*> Plugin::getGrammars()
{
    QList<Grammar*> grammars;

	if (m_grammars.empty())
        this->loadGrammars();

	for (auto &g : m_grammars) {
		grammars.append(g.get());
	}
    qSort(grammars.begin(), grammars.end(), grammarIsLessThan);

    return grammars;
}

bool Plugin::loadGrammars()
{
    QDir grammarDir(this->absoluteFilePath("Grammars"));
    if (!grammarDir.exists())
        return false;
    QStringList filters;
    filters << "*.json";

    foreach(QString filename, grammarDir.entryList(filters))
    {
        QString path = grammarDir.path() + QDir::separator() + filename;
		auto g = std::make_unique<Grammar>(path);
        if (g->isValid())
			m_grammars.insert(std::move(g));
    }

    return true;
}

bool Plugin::isMain() const
{
    return m_is_main;
}

QString Plugin::dir() const
{
    return m_dir.absolutePath();
}

QString Plugin::name() const
{
    return m_name;
}

QString Plugin::scriptPath(QString basename) const
{
    // A script is either "main.lua" or "init.lua" at the root of the plugin, or a *.lua
    // file in the "Scripts" subdirectory
    QString script = dir() + QDir::separator();

    if (basename != "main.lua" && basename != "init.lua")
        script += QString("Scripts") + QDir::separator();

    script += basename;
    QFileInfo info(script);

    if (info.exists())
        return script;
    else
        return QString();
}

QString Plugin::scriptContent(QString basename) const
{
    QString path = scriptPath(basename);
    if (path.isEmpty())
        return QString();

    QFile file(path);
    if (! file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();

    QTextStream stream(&file);
    QString text = stream.readAll();
    file.close();

    return text;
}


QString Plugin::absoluteFilePath(QString file) const
{
    return m_dir.absolutePath() + QDir::separator() + file;
}

bool Plugin::hasMenu() const
{
    return m_menu != NULL;
}

QMenu * Plugin::menu() const
{
    return m_menu;
}

void Plugin::parseMenu(QVariantMap map)
{
    m_menu = new QMenu;

    QString name = map["Text"].toString();
    m_menu->setTitle(name);

    QVariantList actions = map["Actions"].toList();

    foreach(QVariant var, actions)
    {
        QVariantMap actionDict = var.toMap();
        if (actionDict["Type"].toString() == "Action")
        {
            QAction *action = new QAction(m_menu);
            QString text = actionDict["Text"].toString();
            QVariant script(actionDict["Script"].toString());

            action->setText(text);
            action->setData(script);
            m_menu->addAction(action);

            if (actionDict.contains("Shortcut"))
            {
                QString shortcut = actionDict["Shortcut"].toString();
                action->setShortcut(QKeySequence(shortcut));
            }

            connect(action, SIGNAL(triggered()), this, SLOT(setupScript()));
        }
        // TODO: implement submenus

    }

}

void Plugin::errorMessage() const
{
    QMessageBox::critical(NULL, "Invalid Plugin", m_name + tr("is not a valid plugin"));
}

void Plugin::setupScript()
{
    QAction *action = qobject_cast<QAction*>(sender());
    QString script = action->data().toString();

    emit scriptCalled(script, scriptContent(script));
}
















