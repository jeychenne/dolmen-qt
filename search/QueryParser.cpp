/*
 * dolmen.h
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
 * Purpose:
 * Created: //
 */

#include <QRegularExpression>
#include "QueryParser.h"
#include "Query.h"

QueryParser::QueryParser(Project *project)
{
	this->project = project;
}

Query* QueryParser::parse(QString queryString, Grammar *grammar)
{
	QString err = "";
//	qDebug() << queryString;
	queryString = queryString.trimmed();
    Query *query = new Query(queryString, grammar);
    QRegularExpression regex_query("SEARCH\\s+(.+)\\s+"
						"FROM\\s+(.+)\\s+"
						"WHERE\\s+(.+)\\s+"
                        "RETURN\\s+<(.*?)>\\s+(.+)\\s*;", QRegularExpression::CaseInsensitiveOption);

    auto match = regex_query.match(queryString);

    if (!match.hasMatch() || match.lastCapturedIndex() != 5)
	{
		QString msg("Search error: invalid syntax");
		emit error_search(msg);
		return NULL;
	}

    parseSearchStatement(query, match.captured(1));
	err = this->error();

	if (err != "")
	{
		emit error_search(err); return NULL;
	}
    parseFromStatement(query, match.captured(2));
    parseWhereStatement(query, match.captured(3));
	err = this->error();
	if (err != "")
	{
		emit error_search(err); return NULL;
	}
    QString sep = match.captured(4);
    sep.replace("%SPACE%", " ");
    query->setSeparator(sep);
    parseReturnStatement(query, match.captured(5));

	return query;
}

void QueryParser::parseSearchStatement(Query *query, QString statement)
{	
	// for now, only tiers (x = cross-tier)
    QRegularExpression regex("@(.+)\\[(\\d+|x|\\$.+)\\]"); // index 0 means all instances

    auto match = regex.match(statement);

    if (!match.hasMatch())
	{
		m_error = tr("Search error: no tier number indicated"); return;
	}

    auto tier = match.captured(2);

    if (tier == "x")
		query->setListIndex(-1); // invalid index
	else
        query->setListIndex(tier.toInt());
	// process flags
	if (statement.contains("-cs"))
		query->setCaseSensitive(true);

    if (statement.contains("-annotators"))
    {
        QRegularExpression rx_annot("-annotators-ref/(.*)//(.+)/");
        auto match = rx_annot.match(statement);

        if (match.hasMatch())
        {
            QString ref_annotator = match.captured(1);
            QStringList annotators = match.captured(2).split('/');

            /* We need at least two annotators */
            if (annotators.size() >= 2)
            {
                query->setAnnotators(annotators);

                /* Pick the first annotator if none was provided by the user (or if it doesn't exist) */
                if (ref_annotator.isEmpty() || ! annotators.contains(ref_annotator))
                    ref_annotator = annotators.at(0);
                query->setReferenceAnnotator(ref_annotator);
            }
        }
    }

	QStringList styles;
    styles << DM_SEARCH_FLAG_PERL << DM_SEARCH_FLAG_TEXT;

	foreach (QString flag, styles)
	{
		if (statement.contains(flag))
			query->setSearchStyle(flag);
	}
}

void QueryParser::parseReturnStatement(Query *query, QString statement)
{
	if (statement == "@file")
		query->setReturnCode(DFileObject, NullAttribute);

	else
	{
		SearchObjectCode object = parseObjectCode(statement);
        SearchAttributeCode attribute = parseAttributeCode(statement);

		/* parameters */
        QRegularExpression regex_params("\\{(.+)\\}");
        auto match = regex_params.match(statement);

        if (match.hasMatch())
            query->parseReturnParameters(match.captured(1));

		query->setReturnCode(object, attribute);
	}
}


void QueryParser::parseFromStatement(Query *query, QString statement)
{
	QList<DFile*> files;

	if (statement == "*") // all files
	{
		files = project->files();
	}
	else if (statement == "%annotation") // all annotations
	{
		foreach (DFile *f, project->files())
		{
			if (isInstance(f, Annotation))
				files << f;
		}
	}
	else if (statement == "%sound") // all sound files
	{
		foreach (DFile *f, project->files())
		{
			if (isInstance(f, Sound))
				files << f;
		}
	}
	else if (statement == "%document") // all documents
	{
		foreach (DFile *f, project->files())
		{
			if (isInstance(f, Document))
				files << f;
		}
	}
	else
	{
        foreach (QString path, statement.split(QRegularExpression("\\s*,\\s*")))
			files << project->file(path);
	}

	query->setFiles(files);
}

void QueryParser::parseWhereStatement(Query *query, QString statement)
{
	QStringList parts, data_tokens, meta_tokens;

    parts = statement.split(QRegularExpression("\\s*&&\\s*")); // separate data and metadata

	if (parts.size() != 2)
	{
		m_error = tr("Search error: bad syntax in WHERE statement"); return;
	}

	data_tokens = tokenizeString(parts[0]);
	meta_tokens = tokenizeString(parts[1]);

	query->setSearchData(buildSearchNode(data_tokens));
	query->setSearchMetadata(buildSearchNode(meta_tokens));
}

SearchObjectCode QueryParser::parseObjectCode(QString statement)
{
	SearchObjectCode obj = NullObject;

	if (statement.startsWith("@file"))
		obj = DFileObject;

    else if (statement.startsWith("@document"))
        obj = DocumentObject;

	else if (statement.startsWith("@tier"))
		obj = TierObject;

	else if (statement.startsWith("@item"))
		obj = ItemObject;

	else if (statement.startsWith("@span"))
		obj = SpanObject;

	else if (statement.startsWith("@point"))
		obj = PointObject;

	else if (statement.startsWith("@tag"))
		obj = PropertyObject;

	else if (statement.startsWith("@graphnode"))
		obj = GraphNodeObject;

	else
		m_error = tr("Search error: unknown object \"%1\"").arg(statement);

	return obj;
}

SearchAttributeCode QueryParser::parseAttributeCode(QString statement)
{
	SearchAttributeCode attr = NullAttribute;

	if  (statement.endsWith(".text"))
		attr = TextAttribute;

	else if (statement.endsWith(".match"))
		attr = TextMatchAttribute;

	else if (statement.endsWith(".time"))
		attr = TimeAttribute;

	else if (statement.endsWith(".start"))
		attr = StartAttribute;

	else if (statement.endsWith(".end"))
		attr = EndAttribute;

	else if (statement.endsWith(".category"))
		attr = CategoryAttribute;

	else if (statement.endsWith(".label"))
		attr = LabelAttribute;

	else if (statement.endsWith(".description"))
		attr = DescriptionAttribute;

    else if (statement.endsWith(".crosstext"))
        attr = CrossTextAttribute;

	return attr;
}

/*
	A search expression is represented as a tree structure. All atomic expressions
	must be bracketed.
	Ex: (@item.text ~ "[12]124") AND ((speaker = "John") OR (speaker = "Jane"));

								AND
								/\
							   /  \
							  /    \
							 /      \
							/        \
						   ~         OR
						  /\          /\
						 /  \        /  \
						/    \      /    \
				 item.text [12]124 /      \
								  =        =
								 /\        /\
								/  \      /  \
							   /    \    /    \
						  speaker John speaker Jane
*/
SearchNode* QueryParser::buildSearchNode(QStringList &tokens)
{
	SearchNode *currentNode, *mother;
	currentNode = new SearchNode(NULL);

	// remove top-level bracketing
	tokens.removeFirst();
	tokens.removeLast();

	foreach(QString token, tokens)
	{
		if (token == "(")
		{
			// create a new daughter
			mother = currentNode;
			currentNode = new SearchNode(mother);
			mother->addDaughter(currentNode);
		}
		else if (token == ")")
		{
			// end of a node: go up one level
			currentNode = currentNode->mother();
			if (currentNode == NULL)
			{
				m_error = tr("Search error: parsing problem in WHERE statement <%1>").arg(token);
				return NULL;
			}
		}

		/* text operators */

		else if (token == "=")
			currentNode->setOpcode(EqualOperator);

		else if (token == "~")
			currentNode->setOpcode(MatchOperator);

		/* boolean operators */

        else if (token.startsWith("AND"))
        {
			currentNode->setOpcode(AndOperator);
            auto parts = token.split(":");

            if (parts.size() == 2) {
                currentNode->setCrossTierSpecifier(parts.at(1));
            }
        }

		else if (token == "OR")
			currentNode->setOpcode(OrOperator);

        else if (token == "NOT")
			currentNode->setTruth(false);

		/* values */

		else if (token.startsWith('"') && token.endsWith('"'))
		{
			dm_restore(token);
			token.remove(0,1); token.chop(1);
			currentNode->setValue(token);
		}

		/* numeric operators */

		else if (token == DM_LE)
			currentNode->setOpcode(NumLeOperator);

		else if (token == DM_GE)
			currentNode->setOpcode(NumGeOperator);

		else if (token == DM_LT)
			currentNode->setOpcode(NumLtOperator);

		else if (token == DM_GT)
			currentNode->setOpcode(NumGtOperator);

		else if (token == DM_EQ)
			currentNode->setOpcode(NumEqOperator);

		else if (token == DM_NE)
			currentNode->setOpcode(NumNeOperator);

		/* @object.attribute */

		else
		{
			currentNode->setObjectCode(parseObjectCode(token));
            currentNode->setAttributeCode(parseAttributeCode(token));
		}

		/* parameters */
        QRegularExpression regex_params("\\{(.+)\\}");
        auto match = regex_params.match(token);

        if (match.hasMatch())
		{
            // Tier name or number.
            QString txt = match.captured(1);
            if (txt.startsWith("$")) // look for a text variable $item{$tier_name}.text
            {
                txt.remove(0,1);
                currentNode->setTierName(txt);
            }
            else
                currentNode->setTierIndex(txt.toInt());
		}
	}

	if (currentNode->mother() != NULL)
	{
		m_error = tr("Search error: parsing problem in WHERE statement (did you forget a bracket?)");
		return NULL;
	}

	//Q_ASSERT(currentNode->opcode() != NullOperator);

	return currentNode;
}

QStringList QueryParser::tokenizeString(QString statement)
{
	statement.replace('(', "( ");
	statement.replace(')', " )");
	statement.replace('=', " = ");
    statement.replace('<', " < ");
    statement.replace('>', " > ");
    statement.replace("<  =", "<=");
    statement.replace(">  =", ">=");
	statement.replace('~', " ~ ");

    return statement.split(QRegularExpression("\\s+"));
}

QString QueryParser::error()
{
	QString error = m_error;
	m_error = "";

	return error;
}

