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
 * Purpose: define macros and constants
 * Created: 01/09/10
 */

#ifndef DOLMEN_H_
#define DOLMEN_H_

// requires Qt 4.6+ (use '%' instead of '+' for concatenation)
#define QT_USE_FAST_CONCATENATION

#include <QDebug>
#include <QSettings>
#include <cstring>
#include <QDir>

#include "utils/Global.h"
// QT metaobject system
#define isInstance(OBJ, CLASS) strcmp(OBJ->metaObject()->className(), #CLASS) == 0
#define CLASSNAME(OBJ) OBJ->metaObject()->className()


// date
#define DM_DATE_FORMAT "dd/MM/yyyy"

// Nativize path: macro used throughout to make sure Windows paths are always represented as "\\".
#define nat(path) QDir::toNativeSeparators(path)
#define DIR_SEP QDir::separator()


// Operating System
#if defined(Q_OS_WIN)
    #define OS_WINDOWS
#elif defined(Q_OS_UNIX)
#define OS_UNIX
#elif defined(Q_OS_MAC)
#define OS_MACOSX
#else
#define OS_UNKNOWN
#endif


#include <memory>

#ifdef Q_OS_OSX
#include <boost/make_shared.hpp>
using boost::make_shared;
#else
using std::make_shared;
#endif



// application variables
#define VAR_APPDIR "$DM_APPDIR"
#define VAR_HOME "$HOME"
#define VAR_PROJECT "$DM_PROJECT"

// file extensions
#define DM_DOT_PROJECT_EXT ".dmpr"
#define DM_DOT_FILE_EXT ".dmf"
#define DM_DOT_TABLE_EXT ".dmt"
#define DM_DOT_TEXT_EXT ".txt"
#define DM_DOT_METADATA_EXT ".dmm"
#define DM_DOT_QUERY_EXT ".dmq"

#define DM_PROJECT_EXT "dmpr"
#define DM_FILE_EXT "dmf"
#define DM_TABLE_EXT "dmt"
#define DM_TEXT_EXT "txt"
#define DM_METADATA_EXT "dmm"
#define DM_QUERY_EXT "dmq"

// font
#ifdef Q_OS_WIN
#define DM_MONO_FONT "Courier"
#elif defined(Q_OS_MAC)
#define DM_MONO_FONT "Monaco"
#else
#define DM_MONO_FONT "Mono"
#endif

// search
#define DM_SEARCH_STYLE_PERL "Regular Expression"
#define DM_SEARCH_STYLE_TEXT "Plain Text"
// search flags
#define DM_SEARCH_FLAG_PERL "-perl"
#define DM_SEARCH_FLAG_TEXT "-text"
#define DM_SEARCH_FLAG_CASE "-cs"
#define DM_SEARCH_FLAG_GREEDY "-greedy"

// non capturing pathentheses in search
// Note: Grammar.cpp has hard-coded values to avoid circular dependencies
#define DM_NONCAPT_LPAR "{{{"
#define DM_NONCAPT_RPAR "}}}"

// math operators
#define DM_EQ "=="
#define DM_NE "!="
#define DM_LT "<"
#define DM_LE "<="
#define DM_GE ">="
#define DM_GT ">"

// bottom tabs
#define DM_SEARCHES_TAB 0
#define DM_OUTPUT_TAB 1
#define DM_SHELL_TAB 2

// default property category
#define DM_DEFAULT_CAT tr("Default")


#define MATCH_COTEXT_LENGTH 30


#define dm_debug(msg) emit error(msg)
#define inform(msg) emit information(msg)

// special mode for the PFC and PAC projects (enables file name parsing).
// TODO: move this to a plugin.
#define DM_PFC_MODE 1
//#define DM_PAC_MODE 1

/* Curve data classes for signal block */
#define DEFAULT_WINDOW_LENGTH 30.0

/* Invalid units */
#define INVALID_INDEX -1
#define INVALID_POSITION -399
#define INVALID_TIME -1.0


#endif /* DOLMEN_H_ */
