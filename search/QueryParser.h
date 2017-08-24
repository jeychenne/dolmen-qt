/*
 * SearchEngine.h
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
 * Purpose: implement search engine
 *
 * All searches in Dolmen are implemented via an abstract query protocol, which is
 * loosely based on SQL but adapted to the needs of the program.
 * This allows queries to be easily remembered and saved, for instance to display
 * frequently used subsets of the corpus.
 * A query can search and/or return any (registered) object or attribute from a
 * file. Objects are preceded by '@' and its attributes are accessed using
 * the dot notation (e.g. @item.text --> "text of an Item object").
 *
 * Syntax:
 *    SEARCH @object FROM files WHERE data_conditions && metadata_conditions RETURN return_val;
 *
 * When there are several instances of an object (e.g. tiers, items), a specific instance
 * may be accessed by its index. The indexing system uses a 1-based array notation (e.g.
 * @item[5] --> "5th item in a tier"). If the index is 0, all objects are processed.
 *
 * The files in the FROM statement are always a list of vfiles. The user selects the files of
 * interest in the search window and the search engine only discards those that don't satisfy
 * the metadata conditions.
 *
 * The RETURN value is always a list of type return_val. Importantly, the SEARCH object and the
 * RETURN object need not be the same. A typical case is when a user wants to retrieve the value
 * of a particular attribute in an object (e.g. "get the start of the 5th span").
 *
 * Example:
 *     SEARCH @tier[3] FROM vfile1, vfile2 WHERE (@item.text ~ "e141[34]") && () RETURN @item.text;

 Extension for cross-tier search:
 - use 'x' instead of number for the @tier object to signal a cross-tier search
 - use braces {} to pass parameters to objects. For instance, "@item{1}.text" is the text of an item
 in the first tier.

Example:
     SEARCH @tier[x] FROM ... WHERE ((@item{1}.text ~ "e141[34]") AND (@item{3}.text ~ "e141[34]")) && () RETURN @graphnode{4}.crosstext;
Another RETURN object: @graphnode = a node from 1 item in a tier dominating other items on another tier
Attribute: crosstext = convenience to get the concatenated text of daughter items on the display tier.
So "@graphnode{1,5}.crosstext" gets a matching item on tier 1, looks up its daughters on tier 5 and concatenates their text.
For now, @graphnode.crosstext should be used only if the 1st search tier and the display tier are different.

Example with a document:
	SEARCH @file[x]  -perl FROM %document WHERE (@document.text ~ "le") && () RETURN @document.match;
for now @file[x] represents a Document (TODO: get rid of "[x]"??). @document.text represents a Document's text property

To look for tiers by name, we use the following syntax:
SEARCH @tier[0] FROM vfile1, vfile2 WHERE (@item{$tier_name}.text ~ "e141[34]") && () RETURN @item.text;
where "tier_name" is the name of the tier, and the $ sign signals that it's a text variable, not a number.
 *
 * Created: 15/09/2010
 */

#ifndef QUERYPARSER_H
#define QUERYPARSER_H

#include <QObject>

#include "dolmen.h"
#include "search/Query.h"
#include "lib/Project.h"
#include "utils/routines.h"

class Grammar;

class QueryParser : public QObject
{
	Q_OBJECT

public:
	QueryParser(Project *project);

    Query* parse(QString, Grammar *grammar = NULL);
	QString error();

signals:
	void error_search(QString &msg);

private:
	Project *project;
	QString m_error;

	void parseSearchStatement(Query *query, QString statement);
	void parseReturnStatement(Query *query, QString statement);
	void parseFromStatement(Query *query, QString statement);
	void parseWhereStatement(Query *query, QString statement);

	SearchObjectCode parseObjectCode(QString);
	SearchAttributeCode parseAttributeCode(QString);
	SearchNode* buildSearchNode(QStringList &tokens);

	QStringList tokenizeString(QString);
};

#endif // QUERYPARSER_H
