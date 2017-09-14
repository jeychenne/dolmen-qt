/*
 * SearchNode.cpp
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
 * Purpose: implement search tree used to represent conditions on data & metadata
 * in queries.
 * Created: 17/09/2010
 */

#include "SearchNode.h"

SearchNode::SearchNode(SearchNode *mother)
{
	m_mother = mother;
	m_opcode = NullOperator;
	object_code = NullObject;
	attribute_code = NullAttribute;
	m_value = "";
	m_truth = true;
    m_tier_no = -1;
    m_search_by_name = false;
    m_cross_tier_spec = NullRelation;
}

SearchNode* SearchNode::mother()
{
	return m_mother;
}

void SearchNode::addDaughter(SearchNode *node)
{
	m_daughters << node;
}

int SearchNode::countDaughters()
{
	return m_daughters.size();
}

SearchNode* SearchNode::daughter(int index)
{
	return m_daughters[index];
}
/*
bool SearchNode::hasDaughters()
{
	if (myDaughters.size() == 0)
		return false;
	else
		return true;
}
*/
SearchOpcode SearchNode::opcode()
{
    return m_opcode;
}

void SearchNode::setOpcode(SearchOpcode op)
{
    m_opcode = op;
}

SearchObjectCode SearchNode::object()
{
	return object_code;
}

SearchAttributeCode SearchNode::attribute()
{
	return attribute_code;
}

void SearchNode::setObjectCode(SearchObjectCode code)
{
	object_code = code;
}

void SearchNode::setAttributeCode(SearchAttributeCode code)
{
	attribute_code = code;
}

bool SearchNode::truth()
{
	return m_truth;
}

void SearchNode::setTruth(bool value)
{
	m_truth = value;
}

QString SearchNode::value() const
{
	return m_value;
}

void SearchNode::setValue(QString value)
{
	m_value = value;
}

int SearchNode::tierIndex() const
{
    return m_tier_no;
}

void SearchNode::setTierIndex(int i)
{
    m_tier_no = i-1;
}

QString SearchNode::tierName() const
{
    return m_tier_name;
}

void SearchNode::setTierName(const QString &name)
{
    m_tier_name = name;
    m_search_by_name = true;
}

SearchTierRelation SearchNode::crossTierSpecifier() const
{
    return m_cross_tier_spec;
}

void SearchNode::setCrossTierSpecifier(const QString &value)
{
    if (value == "align") {
        m_cross_tier_spec = AlignmentRelation;
    }
    else if (value == "prec") {
        m_cross_tier_spec = PrecedenceRelation;
    }
    else if (value == "dom") {
        m_cross_tier_spec = DominanceRelation;
    }
}

bool SearchNode::searchByName() const
{
    return m_search_by_name;
}

