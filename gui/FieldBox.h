/*
 * FieldBox.h
 *
 * Copyright (C) 2010-2013 Julien Eychenne
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

#ifndef FIELDBOX_H
#define FIELDBOX_H

#include <QWidget>
#include <QCheckBox>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>

#include "dolmen.h"
#include "lib/Grammar.h"
#include "extra/qxtcheckcombobox.h"

// Helper class for FieldBox
class FieldValueWidget : public QWidget
{
    Q_OBJECT
public:
    FieldValueWidget(SearchValue value, QWidget *parent = NULL);
    bool isChecked() const;
    void setChecked(bool checked);
    QString match() const;
    QString tierName() const { return tier_name; }

public slots:
    void toggle(bool checked);

private:
    QCheckBox             *m_button;
    QxtCheckComboBox      *choice_box;
    QString                matchall, tier_name;
    QHash<QString,QString> choice_hash;
    bool                   hasChoices;
};

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

class FieldBox : public QWidget
{
    Q_OBJECT

public:
    explicit FieldBox(const SearchField &field, QString allValues = tr("All values"), QWidget *parent = 0);
    QString name() const;
    QString regex() const;
    QString tierNamePattern() const;

signals:
    
public slots:

private slots:
    void toggleAll(bool);
private:
    QString matchall, m_name, m_tier_name_pattern;

//    QHash<QString,QxtCheckComboBox*> choice_hash;
    QList<FieldValueWidget*> value_list;

    bool allChecked() const;
    bool noneChecked() const;
};

#endif // FIELDBOX_H
