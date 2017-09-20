/*
 * dolmen.h
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
 * Purpose:
 * Created: //
 */

#ifndef ROUTINES_H
#define ROUTINES_H

#include <QString>
#include <QDir>
#include <QDate>

#include "dolmen.h"
#include "utils/Global.h"


int genFileId();
int genGuiQueryId();
int genElementId();
bool isValidSoundExt(const QString &ext);

QString dm_home();

int getMatchCotextLength();
QString getMetadataDir();
QString getUserPluginDir();
QString getMainDatabase();
QString getDefaultResourcesDir();
QString getSystemPluginDir();
QString getHelpDir();

bool dm_isNumeric(const QString &); // int or float value
bool dm_isListNumeric(const QStringList &);

// dd/mm/yyyy or yyyy/mm/dd
bool dm_isDate(const QString &);
bool dm_isDateList(const QStringList &);


QString interpolatePath(QString);
QString compressPath(QString);

// "__" is used to prevent values that contain spaces from being split
// Escape parentheses to allow them in search patterns
void dm_escape(QString &str);
void dm_restore(QString &str);



QString dm_escapeHtml(QString str);

QString dm_time2string(double seconds);

QList<QStringList> parseCsv(const QString &path);


#endif // ROUTINES_H
