/*
 * QueryView.h
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
 * Created: 23/09/2010
 */

#ifndef QUERYVIEW_H
#define QUERYVIEW_H

#include <QListWidget>
#include <QLabel>
#include <QPlainTextEdit>
#include <QProcess>
#include <QAction>
#include <QFileDialog>
#include <QInputDialog>
#include <QCheckBox>

#include "dolmen.h"
#include "search/Query.h"
#include "lib/metadata.h"
#include "lib/praat.h"
#include "lib/Annotation.h"
#include "lib/Grammar.h"
#include "lib/DataTable.h"
#include "gui/View.h"
#include "gui/ContextMenu.h"
#include "gui/HtmlItemDelegate.h"
#include "gui/BookmarkEditor.h"
#include "gui/TableView.h"
#include "search/SearchMatch.h"
#include "audio/soundio.h"
#include "utils/routines.h"




using namespace metadata;

class QueryView : public View
{
	Q_OBJECT

public:
    QueryView(QTabWidget *parent, QWidget *wgt_meta, PraatInstance *praat, Query *query, QString title = QString());
	QString id() const;
	void leave();
	bool isQuery(Query *);

signals:
	void output(const QString &);
	void error(const QString &);
	void forwardBookmark(Bookmark *);
	void openMatch(Annotation *a, int tier, double tstart, double tend);
    void itemMatchMetadata(const QString &fileInfo, const QString &propertiesInfo, const QString &startInfo, const QString &endInfo);
    void textMatchMetadata(const QString &fileInfo, const QString &propertiesInfo);
    void forwardDataTable(DataTable *t);

public slots:
	void onItemClicked();
	void playCurrentItem(); // called when item is double-clicked
	void openCurrentMatchInPraat();
	void openCurrentMatch();
//	void cleanUpProcess(int);

private slots:
	void displayMatchContextMenu(const QPoint &);
	void writeMatchesToOutput();
	void exportMatchesToCsv();
	void createBookmark();
	void editItem();
    void createDataTable();
    void hideConsistentCodings(bool discard);

protected:
	void keyReleaseEvent (QKeyEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    Query             *query;
    QListWidget       *list_results;
	QList<SearchMatchPtr> matches;
    QWidget           *wgt_meta;
    QString            id_string;
    QAction           *action_praat;

    PraatInstance     *praat;
    QProcess          *process;
    Sound             *current_sound;
    QCheckBox         *box_hide_consistent_codings;

    SearchMatchPtr getSelectedMatch();
    QStringList getDisplayLines(int *count_mismatches, bool discardCorrect = false);
    QString getMatchLabel(int count_mismatches) const;

	void setupUi();
	void playMatch(SearchMatch *);

    QList<QStringList> matchesToTable(bool toCsv = false);
	void writeMatchesToCsv(const QString &path, const QString &sep = "\t");

	QStringList getCategories();
    void selectPreviousItem();
    void selectNextItem();

};

#endif // QUERYVIEW_H
