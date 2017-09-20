/*
 * FieldBox.cpp
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
 * Purpose: GUI representation of a search field
 * Created: 13/04/2012
 *
 * The development of this module has been supported by the Japanese
 * Society for the Promotion of Science (JSPS), Grant-in-Aid for Scientific
 * Research (B) n°23320121 (2011-2014).
 * Project title: A corpus-based longitudinal study of the interphonological
 * features of Japanese learners of French.
 * Project leader: Sylvain DETEY (Waseda University, Tokyo, Japan).
 */

#include "FieldBox.h"


FieldValueWidget::FieldValueWidget(SearchValue value, QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *vl = new QVBoxLayout;
    vl->setSpacing(5);
    vl->setContentsMargins(0,0,0,0);
    choice_box = NULL;

    m_button = new QCheckBox(value.text);
    matchall = value.match;
    tier_name = value.tierName;
    vl->addWidget(m_button);

    if (!value.choices.isEmpty())
    {
       choice_box = new QxtCheckComboBox;
        foreach(SearchChoice c, value.choices)
        {
            choice_box->addItem(c.text);
            choice_hash.insert(c.text, c.match);
        }
        vl->addWidget(choice_box);
    }

    this->setLayout(vl);
}


bool FieldValueWidget::isChecked() const
{
    return m_button->isChecked();
}

void FieldValueWidget::setChecked(bool checked)
{
    m_button->setChecked(checked);
}

void FieldValueWidget::toggle(bool checked)
{
    m_button->setChecked(checked);
}

QString FieldValueWidget::match() const
{
    if (choice_box == NULL)
        return matchall;

    else
    {
        QStringList checkedValues = choice_box->checkedItems();

        if (checkedValues.isEmpty())
            return matchall;
        else
        {
            QStringList choices;
            foreach (QString choice, checkedValues)
                choices << choice_hash.value(choice);

            if (choices.count() == 1)
                return choices.at(0);
            else
                return QString("%1%2%3").arg(DM_NONCAPT_LPAR, choices.join("|"), DM_NONCAPT_RPAR);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////


FieldBox::FieldBox(const SearchField &field, QString allValues, QWidget *parent) :
    QWidget(parent)
{
    matchall = field.matchAll;
    m_name = field.name;
    m_tier_name_pattern = field.matchTierLabels;
    QVBoxLayout *dummy = new QVBoxLayout;
    QVBoxLayout *vl = new QVBoxLayout;
    vl->setSpacing(1);
    vl->setContentsMargins(10,10,10,10);

    QGroupBox *group = new QGroupBox(m_name);

    QCheckBox *header_box = new QCheckBox;
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setContentsMargins(0,0,0,0);
    hlayout->addWidget(header_box);
    hlayout->addSpacing(10);
    hlayout->addWidget(new QLabel("<b>" + allValues + "</b>"));
    hlayout->addStretch();
    vl->addLayout(hlayout);
    vl->addSpacing(10);

    foreach(SearchValue v, field.values)
    {
        FieldValueWidget *fieldValue = new FieldValueWidget(v);
        value_list.append(fieldValue);
        vl->addWidget(fieldValue);
    }
    vl->addStretch();
    group->setLayout(vl);
    dummy->addWidget(group);
    this->setLayout(dummy);

    connect(header_box, SIGNAL(toggled(bool)), this, SLOT(toggleAll(bool)));
}

void FieldBox::toggleAll(bool checked)
{
    foreach (FieldValueWidget *value, value_list)
        value->setChecked(checked);
}

QString FieldBox::regex() const
{
    if (allChecked() || noneChecked())
        return this->matchall;

    else
    {
        QStringList values;

        foreach (FieldValueWidget *box, value_list)
        {
            if (box->isChecked())
                values << box->match();
        }

        if (values.count() == 1)
            return values.at(0); // FIXME: why is this non-capturing?
        else
            return QString("%1%2%3").arg(DM_NONCAPT_LPAR, values.join("|"), DM_NONCAPT_RPAR);
    }
}

QString FieldBox::tierNamePattern() const
{
    if (allChecked() || noneChecked())
        return m_tier_name_pattern;

    QStringList names;

    foreach (FieldValueWidget *box, value_list)
    {
        if (box->isChecked())
            names << box->tierName();
    }

    return names.join("|");
}

QString FieldBox::name() const
{
    return m_name;
}

bool FieldBox::allChecked() const
{
    foreach (FieldValueWidget *value, value_list)
    {
        if (!value->isChecked())
            return false;
    }

    // if there's only one value (which may contain a choice list), use this value instead of "matchall"
    if (value_list.size() == 1)
        return false;

    return true;
}

bool FieldBox::noneChecked() const
{
    foreach (FieldValueWidget *value, value_list)
    {
        if (value->isChecked())
            return false;
    }

    return true;
}
