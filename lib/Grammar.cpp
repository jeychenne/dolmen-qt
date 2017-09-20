/*
 * Grammar.cpp
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

#include <QRegularExpression>
#include "Grammar.h"

Grammar::Grammar(QString path, QObject *parent) :
    QObject(parent)
{
    m_path = path;
    cs = true;
    m_tier = -1;
    m_tier_name = "";
    m_active_fields = QList<int>();
    m_tier_selecting_field = -1;
    JsonObject json;
    if (!json.load(path))
        return;

    if (!json.map.contains("Header") || !json.map.contains("FileType") || !json.map.contains("Fields"))
        return;

    // Header
    QVariantMap header = json.map["Header"].toMap();
    m_name = header["DisplayName"].toString();
    m_type = header["Object"].toString();
    m_version = header["Version"].toString();

    // translations
    if (json.map.contains("Translation"))
    {
        QVariantMap trans = json.map["Translation"].toMap();
        foreach (QVariant k, trans.keys())
        {
            QString key = k.toString();
            m_translations.insert(key, trans.value(key).toString());
        }
    }


    m_separator = json.map["Separator"].toString();
    m_filetype = json.map["FileType"].toString();

    if (json.map.contains("Tier"))
        m_tier = json.map["Tier"].toInt();

    if (json.map.contains("TierName"))
    {
        m_tier_name = json.map["TierName"].toString();
    }

    if (json.map.contains("TierSelectingField"))
    {
        bool ok;
        m_tier_selecting_field = json.map["TierSelectingField"].toInt(&ok);
        if (!ok) { m_tier_selecting_field = -1; }
    }

    if (json.map.contains("FieldsPerRow"))
        m_fields_per_row = json.map["FieldsPerRow"].toInt();
    else
        m_fields_per_row = 3;

    if (json.map.contains("CaseSensitive"))
        cs = json.map["CaseSensitive"].toBool();

    // for now, assume we have a SearchGrammar
    QVariantList fieldList = json.map["Fields"].toList();
    parseFields(fieldList);

//    foreach(SearchField f, m_fields)
//    {
//        qDebug() << "Name:" << f.name << "match:" << f.matchAll;
//        foreach(SearchValue v, f.values)
//            qDebug() << "\tMatch:" << v.match << "Text" << v.text;
//    }
}


void Grammar::parseFields(QVariantList fieldList)
{
    for (int count = 0; count < fieldList.size(); ++count)
    {
        const QVariant variant = fieldList.at(count);
        QVariantMap field_map = variant.toMap();
        SearchField field;

        field.name = field_map["Name"].toString();
        field.matchAll = field_map["MatchAll"].toString();
        // mangle non-capturing parentheses so that they are properly parsed by the search engine
        field.matchAll.replace("(?:", "{{{"); // DM_NONCAPT_LPAR
        field.matchAll.replace(")",  "}}}");  // DM_NONCAPT_RPAR

        if (count == m_tier_selecting_field)
        {
            field.matchTierLabels = field_map["TierNamePattern"].toString();
        }

        // get all values
        QVariantList values = field_map["Values"].toList();

        foreach(QVariant v, values)
        {
            QVariantMap val_map = v.toMap();
            SearchValue value;

            value.match = val_map["Match"].toString();
            value.match.replace("(?:", "{{{"); // DM_NONCAPT_LPAR
            value.match.replace(")",  "}}}");  // DM_NONCAPT_RPAR
            value.text  = val_map["Text"].toString();

            if (count == m_tier_selecting_field)
            {
                value.tierName = val_map["TierName"].toString();
            }

            if (val_map.contains("Choices") && val_map.contains("Display"))
            {
                QStringList choices = val_map["Choices"].toString().split('|');
                QStringList displays = val_map["Display"].toString().split('|');

                for (int i = 0; i < choices.count(); ++i)
                {
                    SearchChoice choice;

                    choice.match = choices.at(i);
                    choice.match.replace("(?:", "{{{"); // DM_NONCAPT_LPAR
                    choice.match.replace(")",  "}}}");  // DM_NONCAPT_RPAR;
                    choice.text  = displays.at(i);
                    value.choices << choice;
                }
            }

            field.values << value;
        }

        m_fields << field;
    }

    setAllFieldsActive();
}


bool Grammar::caseSensitive() const
{
    return cs;
}

QString Grammar::name() const
{
    return m_name;
}

QString Grammar::separator() const
{
    return m_separator;
}

QList<SearchField> Grammar::fields() const
{
    return m_fields;
}

QStringList Grammar::matchFields(const QString &s) const
{
    QStringList matches, ptn;

    foreach(SearchField field, m_fields)
    {
        ptn << QString("(%1)").arg(field.matchAll);
    }
    QString pattern = ptn.join(m_separator);
    pattern.replace("{{{", "(?:"); // TODO: handle mangling in a more elegant way
    pattern.replace("}}}", ")");
    QRegularExpression re(pattern, QRegularExpression::CaseInsensitiveOption);
    auto match = re.match(s);

    if (match.hasMatch())
    {
        matches = match.capturedTexts();
        matches.removeFirst();
        Q_ASSERT(m_fields.count() == re.captureCount());
        return matches;
    }
    else
        qDebug() << "FAILED TO MATCH PATTERN" << re.pattern() << "IN" << s;

    // Return an empty match
    for (int i = 0; i < m_fields.count(); ++i)
        matches << "";

    return matches;
}

QStringList Grammar::matchActiveFields(const QString &s) const
{
    QStringList fields = matchFields(s);
    QStringList active_fields;

    // Active fields must correspond to captures, e.g. (1,3,4)
    for (int i = 1; i <= m_fields.count(); ++i)
    {
        if (m_active_fields.contains(i))
            active_fields << fields.at(i-1);
    }

    return active_fields;
}

void Grammar::setActiveFields(QList<int> fields)
{
    m_active_fields.clear();
    m_active_fields = fields;
}

QString Grammar::type() const
{
    return m_type;
}

QString Grammar::version() const
{
    return m_version;
}

int Grammar::tier() const
{
    return m_tier;
}

QString Grammar::tierName() const
{
    return m_tier_name;
}

QString Grammar::translate(const QString &s) const
{
    if (m_translations.contains(s))
        return m_translations.value(s);
    else
        return s;
}

int Grammar::fieldsPerRow() const
{
    return m_fields_per_row;
}

bool Grammar::isValid() const
{
    return !m_name.isEmpty() && !m_fields.isEmpty() && (! m_tier_name.isEmpty() || m_tier != -1 || hasTierSelectingField());
}

void Grammar::setAllFieldsActive()
{
    m_active_fields.clear();

    for (int i = 1; i <= m_fields.count(); ++i)
    {
        m_active_fields << i;
    }
}


