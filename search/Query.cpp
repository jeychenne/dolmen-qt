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
 * Purpose:
 * Created: //
 */

#include "Query.h"
#include <QRegularExpression>
#include <QProgressDialog>


struct MatchLess {
    bool operator()(const SearchMatchPtr &a, const SearchMatchPtr &b)
    {
        return *a.get() < *b.get();
    }
};



Query::Query(QString searchString, Grammar *gr) :
    m_separator(" ")
{
	this->search_string = searchString;
	list_index = -1; // by default, search all objects
    case_sensitive_search = false;
	search_style = DM_SEARCH_FLAG_PERL;
	return_parameters = QPair<int,int>();
    m_grammar = gr;
    m_annotators = QStringList();
    m_ref_annotator = "";
    m_results_annotators = QHash<QString, QList<SearchMatchPtr> >();
}

QString Query::toString() const
{
	return search_string;
}

QStringList Query::filePaths() const
{
	QStringList paths;

	foreach (DFile *f, m_files)
		paths << f->path();

	paths.sort();

	return paths;
}

QList<DFile*> Query::files() const
{
	return QList<DFile*>::fromSet(m_files);
}


QStringList Query::warnings() const
{
	return errors;
}

void Query::setCaseSensitive(const bool value)
{
	case_sensitive_search = value;
}

QStringList Query::annotators() const
{
    return m_annotators;
}

int Query::countAnnotators() const
{
    return m_annotators.size();
}

void Query::setAnnotators(const QStringList &annotators)
{
    m_annotators = annotators;
}

QString Query::referenceAnnotator() const
{
    return m_ref_annotator;
}

void Query::setReferenceAnnotator(const QString &a)
{
    m_ref_annotator = a;
}

void Query::setSearchStyle(const QString &value)
{
	search_style = value;
}

void Query::run()
{   
    if (countAnnotators() > 0)
    {
        runComparison();
    }
    else
    {
        // filter files for "normal" queries without comparisons
        m_files = searchMetaTree(meta_node, m_files);
        int counter = 0, size = m_files.size();
        QProgressDialog progress(tr("Loading annotations..."), QString(), 0, size, NULL);
        progress.setWindowModality(Qt::WindowModal);

        for (DFile *f : m_files)
        {
            progress.setValue(counter++);
            f->open();
        }
        progress.setValue(size);

        // set results
        m_results = QList<SearchMatchPtr>::fromSet(searchDataTree(data_node));

        this->applyCrossTextDisplay();

        // Make sure results are displayed in a predictable order
        std::sort(m_results.begin(), m_results.end(), MatchLess());
    }
}

void Query::runComparison()
{
    QSet<DFile*> all_files = m_files;

    // Filter files for each annotator
    foreach (QString annotator, m_annotators)
    {
        QSet<DFile*> files = QSet<DFile*>();

        foreach (DFile *f, all_files)
        {
            if (f->hasProperty("Annotator", annotator))
                files.insert(f);
        }
        files = searchMetaTree(meta_node, files);

        foreach (DFile *f, files) {
            if (!f->isOpen()) f->open();
        }

        // set m_files for searchDataTree()
        m_files = files;
        QList<SearchMatchPtr> results = QList<SearchMatchPtr>::fromSet(searchDataTree(data_node));
        m_results_annotators[annotator] = results;
    }

    groupSearchMatches();
    // Make sure results are displayed in a predictable order
    std::sort(m_results.begin(), m_results.end(), MatchLess());
}

void Query::groupSearchMatches()
{
    m_results = QList<SearchMatchPtr>();
    QList<SearchMatchPtr> ref_matches = m_results_annotators[m_ref_annotator];
    int countGroupings = 0;

    // compare all possible matches
    foreach (auto &ref_match, ref_matches)
    {
        foreach (QString annotator, m_annotators)
        {
            if (annotator == m_ref_annotator)
                continue;

            QList<SearchMatchPtr> candidate_matches = m_results_annotators[annotator];

            foreach (auto &candidate_match, candidate_matches)
            {
                // Compare up to a certain precision
                QString ref_start, cand_start, ref_end, cand_end;

                ref_start.sprintf("%.6f", ref_match->itemStart());
                cand_start.sprintf("%.6f", candidate_match->itemStart());
                ref_end.sprintf("%.6f", ref_match->itemEnd());
                cand_end.sprintf("%.6f", candidate_match->itemEnd());

                if (ref_start == cand_start && ref_end == cand_end && ref_match->nth() == candidate_match->nth())
                {
                    ref_match->addSibling(candidate_match);
                }
            }
        }

        m_results.append(ref_match);
        countGroupings++;
    }

//    qDebug() << "Groupings in Query::groupSearchMatches()" << countGroupings;
}

Grammar * Query::grammar() const
{
    return m_grammar;
}

QList<SearchMatchPtr>& Query::results()
{
	return m_results;
}

QString Query::separator() const
{
    return m_separator;
}

void Query::setSeparator(const QString &separator)
{
    m_separator = separator;
}

void Query::setListIndex(int index)
{
    list_index = index - 1; // list indices are 1-based in the GUI but 0-based in a project
}


void Query::setFiles(QList<DFile*> &files)
{
	m_files = QSet<DFile*>::fromList(files);
}

void Query::setSearchData(SearchNode *node)
{
	data_node = node;
}

void Query::setSearchMetadata(SearchNode *node)
{
	meta_node = node;
}

void Query::setReturnCode(SearchObjectCode object, SearchAttributeCode attribute)
{
	return_object_code = object;
	return_attribute_code = attribute;
}


QSet<DFile*> Query::searchMetaTree(SearchNode *node, QSet<DFile *> files)
{
	/* Note: Terminals can only be combined with the same operator within a node.
	   They are processed from left to right. */

	QSet<DFile*> subset;

	switch (node->opcode())
	{

	 /** AND and OR operators run down the search tree performing a depth-first search **/
	case AndOperator:
		//qDebug() << QString("Parsing AND: #%1").arg(files.size());
		subset = files;
		for (int d = 0; d < node->countDaughters(); d++)
			subset = searchMetaTree(node->daughter(d), subset); // intersect by sending the subset to the next daughter

		//qDebug() << QString("end of AND: #%1").arg(subset.size());
		break;

	case OrOperator:
		//qDebug() << QString("Parsing OR: #%1").arg(files.size());
		for (int d = 0; d < node->countDaughters(); d++)
			subset.unite(searchMetaTree(node->daughter(d), files));

		//qDebug() << QString("end of OR: #%1").arg(subset.size());

		break;

	case EqualOperator:
	case MatchOperator:
	case NumEqOperator:
	case NumGeOperator:
	case NumGtOperator:
	case NumLeOperator:
	case NumLtOperator:
	case NumNeOperator:
				//qDebug() << QString("Parsing terminal");
		foreach(DFile *file, files)
		{
			if (testFile(node, file) == true)
				subset << file;
		}
		break;

	case NullOperator:
		return files; // no search criteria

	default:
		qDebug("searchMetaTree() unhandled default case");
	}

	if (node->truth() == true)
		return subset;
	else
		return files.subtract(subset);
}

QSet<SearchMatchPtr> Query::searchDataTree(SearchNode *node, QSet<SearchMatchPtr> results, SearchTierRelation relation)
{
    // The logical node AND bears the relation in a cross-tier search, but it needs to be percolated to the right daughter
    // because it will try to find submatches based on the previous tier's matches.

    QSet<SearchMatchPtr> subset;

	switch (node->opcode())
	{
	case AndOperator:
		subset = results;
		for (int d = 0; d < node->countDaughters(); d++)
            subset = searchDataTree(node->daughter(d), subset, node->crossTierSpecifier()); // intersect
		break;

	case OrOperator:
		for (int d = 0; d < node->countDaughters(); d++)
			subset.unite(searchDataTree(node->daughter(d), results));
		break;

	case EqualOperator: case MatchOperator:
        for (DFile *file : m_files)
		{
            QSet<SearchMatchPtr> matches = searchFile(node, file, results, relation);

			if (! matches.isEmpty())
				subset.unite(matches);
		}
		break;

	case NullOperator:
        return QSet<SearchMatchPtr>(); // no search criteria
		break;

	default:
		break;
	}

	if (node->truth() == true)
		return subset;
	else
		return results.subtract(subset);
}

void Query::applyCrossTextDisplay()
{
	if (return_object_code != GraphNodeObject || return_attribute_code != CrossTextAttribute
		|| m_results.size() == 0)
		return;

    QList<SearchMatchPtr> newResults;

    for (auto &match : m_results)
	{
        double start = match->itemStart();
        double end = match->itemEnd();
        Annotation *annot = qobject_cast<Annotation*>(match->file());
        QString text = annot->getTextSpan(return_parameters.second, start, end, separator());
        newResults << make_shared<SearchMatch>(annot, match->items(), match->tier0(), text);

//        Item *item = match->lastItem();

//		if (isInstance(item, DSpan))
//		{
//			DSpan *span = qobject_cast<DSpan*>(item);
//			Annotation *annot = qobject_cast<Annotation*>(match->file());
//            QString text = annot->getTextSpan(return_parameters.second, span->left(), span->right(), separator());
//            newResults << make_shared<SearchMatch>(annot, span, match->tier0(), text); // texts.join(" "));
//		}
	}

	m_results = newResults;
}

void Query::parseReturnParameters(const QString &s)
{
	QStringList lst = s.split(',');
	if (lst.size() != 2) return;

	return_parameters.first  = lst.at(0).toInt() - 1;
	return_parameters.second = lst.at(1).toInt() - 1;
}


/* TEST OBJECTS */

bool Query::testFile(SearchNode *node, DFile *file)
{
	switch (node->object())
	{
	case DFileObject:
		return testObject_dfile(node, file);

	case TierObject:
		return testObject_tier(node, file);

	case ItemObject:
		return testObject_item(node, file);

	case SpanObject:
		return testObject_span(node, file);

	case PointObject:
		return testObject_point(node, file);

	case PropertyObject:
		return testObject_tag(node, file);

	default:
		return false;
	}
}


bool Query::testObject_dfile(SearchNode *node, DFile *file)
{
	// Currently only handles @file.description
    QRegularExpression re(node->value(), QRegularExpression::CaseInsensitiveOption);

	switch (node->opcode())
	{

	case EqualOperator:
            return file->description() == node->value();

	case MatchOperator:
            return re.match(file->description()).hasMatch();

	default:
		break;
	}

	return false; // only files that match the description are returned
}

bool Query::testObject_tier(SearchNode *node, DFile *file)
{
	Q_UNUSED(node);
	Q_UNUSED(file);
	return true;
}

bool Query::testObject_item(SearchNode *node, DFile *file)
{
	Q_UNUSED(node);
	Q_UNUSED(file);

	return true;
}

bool Query::testObject_span(SearchNode *node, DFile *file)
{
	Q_UNUSED(file);

	bool result = true;

	switch (node->attribute())
	{
	case TextAttribute:
		break;
//	case StartAttribute:
//		break;
//	case EndAttribute:
//		break;
    default:
        break;
	}

	return result;
}

bool Query::testObject_point(SearchNode *node, DFile *file)
{
	Q_UNUSED(node);
	Q_UNUSED(file);

	return true;
}

bool Query::testObject_tag(SearchNode *node, DFile *file)
{
	/* regular properties are normally tested for text equality ('='). Numeric properties
	   allow basic numeric comparisons. */

	switch(node->opcode())
	{
	case EqualOperator:
//		qDebug() << QString("Regular property '%1' (op %2, attr %3)").arg(node->value()).arg(node->opcode()).arg(node->attribute());
		switch (node->attribute())
		{
		case CategoryAttribute:
			//qDebug() << QString("testing cat %1 for %2").arg(node->value(), file->path());
			return file->hasCategory(node->value());

		case LabelAttribute:
			return file->hasLabel(node->value());
		default:
			return false;
		}

	// only labels contain numeric values
	case NumEqOperator:
	case NumGeOperator:
	case NumGtOperator:
	case NumLeOperator:
	case NumLtOperator:
	case NumNeOperator:
		return file->cmpLabel(node->opcode(), node->value().toDouble());

	default:
		return false;
	}
}

/* TEST ATTRIBUTES */

bool Query::testAttribute_text(SearchNode *node, DFile *file)
{
	Q_UNUSED(file);

	switch (node->opcode())
	{
	case EqualOperator:
		break;
	case MatchOperator:
		break;
    default:
        break;
	}

    return false;
}

/* SEARCH FILE */

QSet<SearchMatchPtr> Query::searchFile(SearchNode *node, DFile *file, QSet<SearchMatchPtr> results, SearchTierRelation relation)
{
    //qDebug() << QString("searchFile() in %1").arg(file->path());

	switch (node->object())
	{
	case ItemObject:
		switch (node->attribute())
		{
		case TextAttribute:
            return searchItem_text(node, static_cast<Annotation*>(file), results, relation);
		default:
			break;
		}
    case DocumentObject:
        switch (node->attribute())
        {
        case TextAttribute:
            return searchDocument_text(node, static_cast<Document*>(file), results);
        default:
            break;
        }

	default:
		break;
	}

	// TODO: implement other searches
    return QSet<SearchMatchPtr>();
}


QSet<SearchMatchPtr> Query::searchItem_text(SearchNode *node, Annotation *annot, QSet<SearchMatchPtr> results, SearchTierRelation relation)
{
    QSet<SearchMatchPtr> matches;
	Tier *tier;
	int start, end;

	switch(list_index)
	{
	case -2:
        start = node->tierIndex();
		end = start + 1;
		break;
    case -1: // either search in any tier or in a tier specified by name (filtered later)
        start = 0;
		end = annot->countTiers();
		break;
	default:
		start = list_index;
		end = start + 1;
	}
	// cross-tier search
	if (list_index == -2 && results.size() != 0)
	{
        assert(relation != NullRelation);
        int tier_no = node->tierIndex();

		if (tier_no >= annot->tiers().size())
		{
			errors << annot->error() + ": file ignored";
			return matches;
		}

        if (relation == AlignmentRelation)
        {
            QRegularExpression pattern;

            if (node->opcode() == MatchOperator) {
                pattern = buildPattern(node->value());
            }

            for (auto &match : results)
            {
                if (match->file() != annot)
                    continue;

                Item *item = match->lastItem();
                auto gnode = annot->graphNode(item, tier_no);
                if (! gnode) continue;

                auto daughters = gnode->daughters();
                if (daughters.size() != 1) continue;

                Item *other_item = static_cast<Item*>(daughters.at(0));
                if (item->left() != other_item->left() || item->right() != other_item->right()) {
                    continue;
                }

                // We now have two items strictly aligned -> check that the text of the second matches.
                if (node->opcode() == MatchOperator)
                {
                    auto re_match = pattern.match(other_item->text());

                    if (re_match.hasMatch()) {
                        matches << match;
                    }
                }
                else if (node->opcode() == EqualOperator)
                {
                    if (node->value() == other_item->text()) {
                        matches << match;
                    }
                }
            }
        }
        else if (relation == PrecedenceRelation)
        {
            QRegularExpression pattern;

            if (node->opcode() == MatchOperator) {
                pattern = buildPattern(node->value());
            }

            for (auto &match : results)
            {
                Item *item = match->lastItem();
                Item *next_item = annot->nextItem(tier_no, item);

                if (next_item == nullptr) {
                    continue;
                }

                if (node->opcode() == MatchOperator)
                {
                    auto re_match = pattern.match(next_item->text());

                    if (re_match.hasMatch())
                    {
                        match->addItem(next_item);
                        matches << match;
                    }
                }
                else if (node->opcode() == EqualOperator)
                {
                    if (node->value() == next_item->text())
                    {
                        match->addItem(next_item);
                        matches << match;
                    }
                }
            }
        }
        if (relation == DominanceRelation)
        {
            QRegularExpression re;

            if (node->opcode() == MatchOperator) {
                re = buildPattern(node->value());
            }

            for (auto &match : results)
            {
                if (match->file() != annot) {
                    continue;
                }

                Item *item  = match->lastItem();
                //qDebug() << "Cross search, tier no: " << tier_no+1;
                auto gnode = annot->graphNode(item, tier_no);
                if (! gnode) continue;
                auto daughters = gnode->daughters();

                // If a daughter matches the search text, add SearchMatch to matches.
                for (Vertex *v : daughters)
                {
                    Item *sub_item = static_cast<Item*>(v);

                    if (node->opcode() == MatchOperator)
                    {
                        auto re_match = re.match(sub_item->text());

                        if (re_match.hasMatch()) {
                            matches << match;
                            break;
                        }
                    }
                    else if (node->opcode() == EqualOperator)
                    {
                        if (node->value() == sub_item->text()) {
                            matches << match;
                            break;
                        }
                    }
                }
            }
        }
	}
	// pass if there are no matches in the 1st tier of a cross-tier search
	else if (list_index == -2 && start != return_parameters.first) {}
	// single-tier search
	else
	{
        QRegularExpression tierName(node->tierName());

		for (int tier_no = start; tier_no < end; tier_no++)
		{
			tier = annot->tier(tier_no);

			if (tier == NULL)
			{
				errors << annot->error() + ": file ignored";
				return matches;
			}

            // When searching by name, we go through all tiers and discard
            // tiers whose name doesn't match
            if (node->searchByName() && !tierName.match(tier->label()).hasMatch())
                continue;

			QList<Item*> items = tier->items();

			if (node->opcode() == MatchOperator)
			{
				//	FIXME: regexps capture from left to right. Even when they're non-greedy,
				//	it is not fully appropriate for natural language search. For instance, "je.*pas"
				//	will match the whole string in "je crois, je, je sais pas" instead of "je sais pas",
				//	which is what most people will want.
				//	TODO: find the best way to get the rightmost match, and perhaps let users choose
				//	(there might be cases when we want the default behaviour).
                auto pattern = buildPattern(node->value());

				for (int j = 0; j < items.size(); j++)
				{
					int pos = 0;
                    int nth = 1; // nth match in item
                    QRegularExpressionMatch re_match;

                    while ((re_match = pattern.match(items[j]->text(), pos)).hasMatch())
					{
                        QString match = re_match.captured(0);
                        QString left  = annot->leftCotext(tier_no, j, re_match.capturedStart(), separator());
                        pos = re_match.capturedEnd();
                        QString right = annot->rightCotext(tier_no, j, pos, separator());

                        SearchMatchPtr m = make_shared<SearchMatch>(annot, items[j], tier_no, match, left, right);
                        m->setNth(nth++);
                        matches << m;
					}
				}
			}
			else if (node->opcode() == EqualOperator)
			{
				QString value = node->value();

				for (int j = 0; j < items.size(); j++)
				{
					if (items[j]->text() == value)
                        matches << make_shared<SearchMatch>(annot, items[j], tier_no, value); // Pad with space???
				}
			}
		}
	} // end single-tier search

	return matches;
}


QSet<SearchMatchPtr> Query::searchDocument_text(SearchNode *node, Document *file, QSet<SearchMatchPtr> results)
{
    //TODO: results doesn't seem to be necessary and seems to be here to mirror searchItem_text(). Consider refactoring
    Q_UNUSED(results);
    QSet<SearchMatchPtr> matches;
    int pos;
    QString text;

    if (node->opcode() == MatchOperator)
    {
        auto pattern = buildPattern(node->value());

        text = file->text();
        pos = 0;
        QRegularExpressionMatch re_match;

        while ((re_match = pattern.match(text, pos)).hasMatch())
        {
            pos = re_match.capturedEnd();
            QString match = re_match.captured(0);
            QString left  = file->leftCotext(re_match.capturedStart(), separator());
            pos = re_match.capturedEnd(); // update position in the string
            QString right = file->rightCotext(pos, separator());

            matches << make_shared<SearchMatch>(file, match, left, right);
        }

    }

    return matches;
}

QRegularExpression Query::buildPattern(QString pattern)
{

    if (search_style == DM_SEARCH_FLAG_TEXT) {
        pattern = QRegularExpression::escape(pattern);
    }

    QRegularExpression re(pattern);
    int flags = QRegularExpression::PatternOption::NoPatternOption;

    if (! case_sensitive_search)
        flags |= QRegularExpression::PatternOption::CaseInsensitiveOption;

    re.setPatternOptions((QRegularExpression::PatternOption)flags);

    return re;
}

Query::~Query()
{

}
