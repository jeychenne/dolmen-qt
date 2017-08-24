/*
 * Plugin.h
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
 * Purpose: Plugins are extensions loaded at run-time
 * Created: 24/02/2012
 */

#ifndef PLUGIN_H
#define PLUGIN_H

#include <memory>
#include <unordered_set>
#include <QDir>
#include <QObject>
#include <QDebug>
#include <QVariantMap>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>
#include <QMenu>
#include <QAction>
#include <QMessageBox>


#include "lib/JsonObject.h"
#include "lib/Grammar.h"

class Plugin : public QObject
{
    Q_OBJECT

public:
    Plugin(QDir dir);
    bool isValid() const;
    QString dir() const;
    QString name() const;
    QString scriptContent(QString basename = "main.js") const;
    bool isMain() const;
    bool hasMenu() const;
    QMenu* menu() const;
    bool initialize();
    bool postInitialize(); // do stuff after the plugin is registered
    QList<Grammar*> getGrammars();

private slots:
    void setupScript(); // retrieve script content and send it to the engine

signals:
    void output(QString msg);
    void scriptCalled(const QString &name, const QString &content);

private:
    QDir m_dir;
    QStringList m_files;
    QString m_name, m_version;
    bool m_is_main; // whether the plugin takes ownership of the application
    QMenu *m_menu;
    std::unordered_set<std::unique_ptr<Grammar>> m_grammars;

    QString absoluteFilePath(QString file) const;
    QString scriptPath(QString basename) const;
    void parseMenu(QVariantMap map);
    void errorMessage() const;
    bool loadGrammars();
};

#endif // PLUGIN_H
