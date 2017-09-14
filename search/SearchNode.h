/*
 * SearchNode.h
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
 * Purpose: implement search nodes in queries
 * Created: 17/09/2010
 */

#ifndef SEARCHNODE_H
#define SEARCHNODE_H

#include <QList>
#include <QString>
#include <QStringList>
#include <QDebug>

enum SearchOpcode
{
	NullOperator,
	AndOperator,
	OrOperator,
	NotOperator,
	EqualOperator,  // '=' (text)
	MatchOperator,  // '~' (regex)
	NumEqOperator,  // '==' (numeric)
	NumNeOperator,  // '!='
	NumLtOperator,  // '<'
	NumLeOperator,  // '<='
	NumGtOperator,  // '>'
	NumGeOperator   // '>='
};

enum SearchObjectCode
{
	NullObject = 1000,
	DFileObject,
    DocumentObject,
	TierObject,
	ItemObject,
	SpanObject,
	PointObject,
	PropertyObject,
	GraphNodeObject
};

enum SearchAttributeCode
{
	NullAttribute = 2000,
	TextAttribute,
	TextMatchAttribute, // pseudo-attribute that returns a text match within its context
	TimeAttribute,
	StartAttribute,
	EndAttribute,
	CategoryAttribute,
	LabelAttribute,
	DescriptionAttribute, // in DFile objects
	CrossTextAttribute	// text of a sequence of items
};

enum SearchTierRelation
{
    NullRelation,
    AlignmentRelation,
    PrecedenceRelation,
    DominanceRelation
};


class SearchNode
{
public:
	SearchNode(SearchNode *mother);

	SearchNode* mother();
	void addDaughter(SearchNode *node);
	int countDaughters();
	SearchNode* daughter(int index);
	//bool hasDaughters();
	SearchOpcode opcode();
	SearchObjectCode object();
	SearchAttributeCode attribute();
	void setOpcode(SearchOpcode);
	void setObjectCode(SearchObjectCode);
	void setAttributeCode(SearchAttributeCode);
	bool truth();
	void setTruth(bool);
	QString value() const;
	void setValue(QString);
    int tierIndex() const;
    void setTierIndex(int);
    void setTierName(const QString &name);
    SearchTierRelation crossTierSpecifier() const;
    void setCrossTierSpecifier(const QString &value);
    QString tierName() const;
    bool searchByName() const;

private:
    SearchOpcode        m_opcode;
    int                 m_tier_no;          // relevant for searches in annotation data
    QString             m_tier_name;        // used when searching by tier name rather than number
    bool                m_search_by_name;
    QString             m_value;
    bool                m_truth;            // NOT operator
    SearchObjectCode    object_code;
    SearchAttributeCode attribute_code;
    SearchTierRelation  m_cross_tier_spec;  // specify the relation between 2 tiers
    SearchNode         *m_mother;
    QList<SearchNode*>  m_daughters;
};

#endif // SEARCHNODE_H
