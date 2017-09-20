/*
 * Grammar.h
 *
 * Copyright (C) 2010-2017 Julien Eychenne
 * Copyright (C) 2012 Japanese Society for the Promotion of Science (JSPS)
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
 * Purpose: this module provides a way to specify additional functionality
 * in a declarative format (based on JSON).
 * Created: 12/04/2012
 *
 * The development of this module has been supported by the Japanese
 * Society for the Promotion of Science (JSPS), Grant-in-Aid for Scientific
 * Research (B) n°23320121 (2011-2014).
 * Project title: A corpus-based longitudinal study of the interphonological
 * features of Japanese learners of French.
 * Project leader: Sylvain DETEY (Waseda University, Tokyo, Japan).
 */


#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <memory>
#include <QObject>
#include <QStringList>

#include "lib/JsonObject.h"


struct SearchChoice
{
    QString match, text;
};

struct SearchValue
{
    QString match, text, tierName;
    QList<SearchChoice> choices;
};

struct SearchField
{
    QString name, matchAll, matchTierLabels;
    QList<SearchValue> values;
};

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

class Grammar : public QObject
{
    Q_OBJECT
public:
    Grammar(QString path, QObject *parent = 0);

    QString name() const;

    QString separator() const;

    QString type() const;

    QString version() const;

    QList<SearchField> fields() const;

    QStringList matchFields(const QString &s) const;

    QStringList matchActiveFields(const QString &s) const; // used to compare siblings of a SearchMatch

    int tier() const;

    QString tierName() const; // when searching by tier name rather than number

    QString translate(const QString &s) const;

    int fieldsPerRow() const;

    bool isValid() const;

    bool caseSensitive() const;

    int tierSelectingField() const { return m_tier_selecting_field; }

    bool hasTierSelectingField() const { return m_tier_selecting_field > 0; }

    void setAllFieldsActive();
    
public slots:

    void setActiveFields(QList<int> fields);

private:

    QString m_path, m_separator, m_filetype, m_name, m_type, m_version, m_tier_name;

    int m_tier, m_fields_per_row, m_tier_selecting_field;

    QList<SearchField> m_fields;

    QHash<QString,QString> m_translations;

    bool cs;

    QList<int> m_active_fields;

    void parseFields(QVariantList fieldList);
};

namespace std {

template<>
struct hash<std::unique_ptr<Grammar>>
{
    size_t operator()(const std::unique_ptr<Grammar> &p) const noexcept {
        return std::hash<Grammar*>()(p.get());
    }
};

}

#endif // GRAMMAR_H
