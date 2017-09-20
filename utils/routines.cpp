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

#include "routines.h"
#include <QTextStream>

int genFileId()
{
	static int globalFileId = 0;
	return ++globalFileId;
}

int genGuiQueryId()
{
	static int globalGuiQueryId = 0;
	return ++globalGuiQueryId;
}

int genElementId()
{
	static int globalBookmarkId = 0;
	return ++ globalBookmarkId;
}

bool isValidSoundExt(const QString &ext)
{
	if (Global::AudioFormats.contains(ext))
		return true;
	else
		return false;
}

QString dm_home()
{
	return QDir::toNativeSeparators(QDir::homePath());
}


int getMatchCotextLength()
{
	QSettings settings("Dolmen", "Dolmen");
	return settings.value("MatchCotextLength").toInt();
}

QString getMetadataDir()
{
	QDir dir(Global::AppDataDir + QDir::separator() + "Metadata");

	if (! dir.exists())
		dir.mkpath(dir.path());

	return dir.path();
}

QString getUserPluginDir()
{
    QDir dir(Global::AppDataDir + QDir::separator() + "Plugins");

    if (! dir.exists())
        dir.mkpath(dir.path());

    return dir.path();
}

QString getMainDatabase()
{
    QDir dir(Global::AppDataDir + QDir::separator() + "Database");

    if (! dir.exists())
        dir.mkpath(dir.path());

    return dir.path() + QDir::separator() + "dolmen.db";
}

QString getDefaultResourcesDir()
{
#ifdef Q_OS_WIN
    return Global::AppRootDir;
#elif defined(Q_OS_MAC)
    return Global::MacResourcesDir;
#else
    return "/usr/local/share/dolmen";
#endif
}

QString getSystemPluginDir()
{
#ifdef Q_OS_LINUX
    QString tail("plugins");
#else
    QString tail("Plugins");
#endif

    return Global::ResourcesPath + QDir::separator() + tail;
}

QString getHelpDir()
{
return Global::ResourcesPath + QDir::separator() + "html";
}

QString interpolatePath(QString path)
{
	if (path.startsWith(VAR_PROJECT))
		path.replace(VAR_PROJECT, Global::CurrentProjectDir);

	else if (path.startsWith(VAR_APPDIR))
		path.replace(VAR_APPDIR, Global::AppDataDir);

	else
		path.replace(VAR_HOME, dm_home());

	return nat(path);
}

QString compressPath(QString path)
{
	if (path.startsWith(Global::CurrentProjectDir))
		path.replace(Global::CurrentProjectDir, VAR_PROJECT);

	else if (path.startsWith(Global::AppDataDir))
		path.replace(Global::AppDataDir, VAR_APPDIR);

	else
		path.replace(dm_home(), VAR_HOME);

	return nat(path);
}

QString dm_escapeHtml(QString str)
{
	str.replace("<", "&lt;");
	str.replace(">", "&gt;");
	str.replace(" ", "&nbsp;");

	return str;
}


QString dm_time2string(double time)
{
	QString result;

	int timeLeft = (int)time;
	double milliseconds = time - timeLeft;

	int hours = timeLeft / 3600;
	timeLeft = timeLeft % 3600;
	int minutes = timeLeft / 60;
	int seconds = timeLeft % 60;

	if (hours != 0)
		result = QString::number(hours) + " h ";

    result += QString::number(minutes) + ":";
    result += QString::number(seconds) + ".";

	if (milliseconds > 0.)
	{
        QString ms = QString::number(milliseconds); //, 'g', 4);
		ms.replace("0.", "");
		result += ms;
	}

	return result;
}


bool dm_isNumeric(const QString &s)
{
	QRegExp rx("^[\\d\\.]+$");

	if (rx.indexIn(s) != -1)
		return true;
	else
		return false;
}

bool dm_isListNumeric(const QStringList &l)
{
	foreach (QString s, l)
	{
		if (! dm_isNumeric(s))
			return false;
	}

	return true;
}

bool dm_isDate(const QString &date)
{
	QStringList d = date.split('/');

	if (QDate::isValid(d.at(0).toInt(), d.at(1).toInt(), d.at(2).toInt())
		|| QDate::isValid(d.at(2).toInt(), d.at(1).toInt(), d.at(0).toInt()))
		return true;
	else
		return false;
}

bool dm_isDateList(const QStringList &l)
{
	foreach (QString s, l)
		if (! dm_isDate(s))
			return false;

	return true;
}

void dm_escape(QString &str)
{
	str.replace(" ", "__");
	str.replace('(', "_Dm_Op_Par_");
	str.replace(')', "_Dm_Cl_Par_");
    str.replace('<', "_Dm_Op_AngBr_");
    str.replace('>', "_Dm_Cl_AngBr_");
    str.replace('=', "_Dm_Eq_Sign_");
    str.replace('~', "_Dm_Tilde_");
}

void dm_restore(QString &str)
{
	str.replace("__", " ");
	str.replace("_Dm_Op_Par_", "(");
	str.replace("_Dm_Cl_Par_", ")");
    str.replace("_Dm_Op_AngBr_", "<");
    str.replace("_Dm_Cl_AngBr_", ">");
    str.replace("_Dm_Eq_Sign_", "=");
    str.replace("_Dm_Tilde_", "~");
    str.replace("{{{", "(?:");
    str.replace("}}}", ")");
}


QList<QStringList> parseCsv(const QString &path)
{
    QList<QStringList> csv;
    QStringList fields;
    QFile data(path);
    QString line;

    if (data.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&data);
        stream.setCodec("UTF-8");
        stream.setAutoDetectUnicode(true);

        do
        {
            line = stream.readLine();
            fields = line.split(',');
            csv.append(fields);
        }
        while (! stream.atEnd());
        data.close();
    }

    return csv;
}


