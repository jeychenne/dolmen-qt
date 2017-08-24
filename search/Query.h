/*
 * dolmen.h
 *
 * Copyright (C) 2010-2013 Julien Eychenne 
 *
 * This file is part of Dolmen.
 *
 * Dolmen is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * Dolmen is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Dolmen. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author:  Julien Eychenne
 * Purpose: implement query object
 * Created: 15/09/2010
 */

#ifndef QUERY_H
#define QUERY_H

#include <QObject>
#include <QPair>
#include <QHash>

#include "dolmen.h"
#include "lib/DFile.h"
#include "lib/Project.h"
#include "lib/Grammar.h"
#include "search/SearchNode.h"
#include "search/SearchMatch.h"


class Query : public QObject
{
public:
    Query(QString search_string, Grammar *gr = NULL);
	~Query();

	void setListIndex(int index); // index of object to search in a list (item, tier, etc.)
	void setFiles(QList<DFile*> &);
	void setSearchData(SearchNode *);
	void setSearchMetadata(SearchNode *);
	void setReturnCode(SearchObjectCode object, SearchAttributeCode attribute);
    void setCaseSensitive(const bool value);
    QStringList annotators() const;
    int countAnnotators() const;
    void setAnnotators (const QStringList &annotators);
    void setReferenceAnnotator(const QString &a);
    QString referenceAnnotator() const;
	void setSearchStyle(const QString &value);
	void run();
    Grammar * grammar() const;
	void parseReturnParameters(const QString &s);
	QStringList warnings() const;
	QStringList filePaths() const;
	QList<DFile*> files() const;
	QString toString() const;
    QList<SearchMatchPtr>& results();


private:
	// 0-based index of the object to search when there are several (tiers, items, etc.)
	// If listIndex == -1, search all objects; if listIndex == -2, cross-tier search
    int                  list_index;
    QPair<int,int>       return_parameters;
    bool                 case_sensitive_search;
    QString              search_style;
    QSet<DFile*>         m_files;
    QList<SearchMatchPtr>   m_results;
    QHash<QString, QList<SearchMatchPtr> > m_results_annotators;// used when comparing annotators
    QString              search_string;
    SearchNode           *data_node, *meta_node;
    SearchObjectCode     return_object_code;
    SearchAttributeCode  return_attribute_code;
    QStringList          errors; // non-fatal errors
    Grammar              *m_grammar;
    QStringList          m_annotators;
    QString              m_ref_annotator;

	// depth-first search
	QSet<DFile*> searchMetaTree(SearchNode *node, QSet<DFile*> files);
    QSet<SearchMatchPtr> searchDataTree(SearchNode *node, QSet<SearchMatchPtr> results = QSet<SearchMatchPtr>());

	// display of cross-text in cross-tier search
	void applyCrossTextDisplay();
	// test whether a file meets the search criteria
	bool testFile(SearchNode *node, DFile *file);
	bool testObject_dfile(SearchNode *node, DFile *file);
	bool testObject_tier(SearchNode *node, DFile *file);
	bool testObject_item(SearchNode *node, DFile *file);
	bool testObject_span(SearchNode *node, DFile *file);
	bool testObject_point(SearchNode *node, DFile *file);
	bool testObject_tag(SearchNode *node, DFile *file);
	bool testAttribute_text(SearchNode *node, DFile *file);

    QRegularExpression buildPattern(QString pattern);

    void runComparison();
    void groupSearchMatches();

	// find all matches in a file (pass results for cross-tier search)
    QSet<SearchMatchPtr> searchFile(SearchNode *node, DFile *file, QSet<SearchMatchPtr> results);
    QSet<SearchMatchPtr> searchItem_text(SearchNode *node, Annotation *file, QSet<SearchMatchPtr> results);
    QSet<SearchMatchPtr> searchDocument_text(SearchNode *node, Document *file, QSet<SearchMatchPtr> results);

};

#endif // QUERY_H
