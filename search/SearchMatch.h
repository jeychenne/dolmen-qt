/*
 * SearchMatch.h
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
 * Purpose: implement query matches in queries
 * Created: 21/09/2010
 */

#ifndef QUERYMATCH_H
#define QUERYMATCH_H

#include <set>
#include <QObject>
#include <QList>

#ifdef __APPLE__
#include <boost/tr1/memory.hpp>
#else
#include <memory>
#endif

#include "dolmen.h"
#include "lib/DFile.h"
#include "lib/Annotation.h"
#include "lib/Item.h"
#include "lib/Bookmark.h"
#include "utils/routines.h"
#include "lib/Grammar.h"

class SearchMatch;

#ifdef __APPLE__
typedef boost::shared_ptr<SearchMatch> SearchMatchPtr;
#else
typedef std::shared_ptr<SearchMatch> SearchMatchPtr;
#endif

uint qHash(const SearchMatchPtr &p);


class SearchMatch : public QObject, public Bookmark
{
    Q_OBJECT
    Q_INTERFACES(Bookmark)

public:
    SearchMatch(DFile *file, Item* item, int tier, double match);
    SearchMatch(DFile *file, Item* item, int tier, QString match, QString left = "", QString right = "");
    SearchMatch(DFile *file, QString match, QString left = "", QString right = "");
    bool operator< (const SearchMatch &other) const;
    QString   toHtml() const;
    QString   toHtmlWithError() const;
    QString   toString() const;
    QString   left() const;
    QString   right() const;
    double    itemStart() const;
    double    itemEnd() const;
    int       nth() const;
    void      setNth(int n);
    QString   match() const;
    bool      hasSiblings() const;
    void      addSibling(const SearchMatchPtr &m);
    bool      hasDivergingAnnotations(Grammar *grammar) const;
    QList<SearchMatchPtr> siblings() const;
    DFile*    file() const;
    Item*     item() const;
    // Base 0 is used internally. Base 1 is user-visible (as used in Praat).
    // TODO: get rid of that, this was a bad idea...
    int tier0() const; // zero-based
    int tier() const; // one-based
    void setTier0(int tier); // zero-based
    void setTier(int tier); // One-based

    void openInPraat() final;

    /* Bookmark */
    bool readFromXml(QXmlStreamReader *reader);
    void writeToXml(QXmlStreamWriter *writer);
    QString type() const;
    void open();
    QString tooltip() const;

signals:
    void openDFile(DFile *df, double start, double end);

private:
    QString    m_match, m_left, m_right;
    DFile     *m_dfile;
    Item      *matched_item;
    int        m_tier;
    int        m_nth_match; // nth match in item
    QList<SearchMatchPtr> m_siblings; // store (near-)identical matches when comparing annotators

};




#endif // QUERYMATCH_H
