/*
 * Global.h
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
 * Purpose: define global variables
 * Created: September 2010
 */

#ifndef GLOBAL_H
#define GLOBAL_H

#include <QObject>
#include <QDate>
#include <QAction>
#include <QMap>
#include <QHash>
#include <QStatusBar>
#include "lib/praat.h"
#include "lib/Plugin.h"
#include "lib/Grammar.h"
//#include "gui/OutputTab.h"


class VFileNode;
class DFile;
class OutputTab;


class Global
{
public:
	static QString	AppVersionString;
	static QDate	AppBuildDate;

	static QString	AppDataDir; // store application data
	static QString	AppRootDir; // directory containing the program (or the app bundle on mac)
	static QString	CurrentProjectDir, CurrentProjectPath;
	static QString	MacResourcesDir;
    static const char *AudioDriver;


	// Actions in the main menu that can be accessed from different places are declared as
	// global for convenience.
	static QHash<QString, DFile*> FileHash;
    // Register VFileNodes by id. The id is stored in / retreived from the data()
    // property of QTreeWidgetItems in the file browser.
	static QMap<int, VFileNode*> IdMap;
	static QString DefaultSearchStyle; // "Perl-like" | "UNIX Shell" | "Plain Text"
	static QStringList RecentProjects, AudioFormats, CommonAudioFormats;
	static int MatchCotextLength;

	// Allow special modes for the "PFC" ("Phonologie du français contemporain") and "PAC" ("phonologie de
	// l'anglais contemporain") projects. Other user should use the "Default" mode.
	static QString ApplicationMode;

	static QString PraatPrefsDir;
	static QString PraatPath;
    static bool PraatIsInstalled, UseAntialiasingInPlots, EnableStatusBar;
    static QStatusBar *Output;
    static PraatInstance *Praat;

    inline static bool fileExists(const QString &path) { return FileHash.contains(path); }

#ifdef Q_OS_MAC
    // there's a bug in Qt so that the window geometry is not properly remembered
    // accross session. We maximize the window for (my own) convenience.
    static bool Maximize;
#endif

};


#endif // GLOBAL_H
