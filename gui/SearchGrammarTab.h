/*
 * SearchGrammarTab.h
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
 * Purpose: implements additional search tabs in the main search window
 * Created: 13/04/2012
 *
 * The development of this module has been supported by the Japanese
 * Society for the Promotion of Science (JSPS), Grant-in-Aid for Scientific
 * Research (B) n°23320121 (2011-2014).
 * Project title: A corpus-based longitudinal study of the interphonological
 * features of Japanese learners of French.
 * Project leader: Sylvain DETEY (Waseda University, Tokyo, Japan).
 */

#ifndef SEARCHGRAMMARTAB_H
#define SEARCHGRAMMARTAB_H

#include <QWidget>
#include <QList>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>

#include "lib/DFile.h"
#include "lib/Grammar.h"
#include "gui/FieldBox.h"

class SearchGrammarTab : public QWidget
{
    Q_OBJECT
public:
    SearchGrammarTab(Grammar *grammar, QWidget *parent = NULL);

    QString regex() const;

    int tier() const;

    QString tierName() const;

    QString filetype() const;

    Grammar * grammar() const;

    QString referenceAnnotator() const;

    bool caseSensitive() const;

    bool compareAnnotators() const;

    bool hasTierSelectingField() const { return m_grammar->hasTierSelectingField(); }

    QString tierNamePattern() const;


private slots:

    void selectFields();

    void setActiveFieldsInGrammar(QList<int> fields);

private:

    QList<FieldBox*>  m_fields;

    Grammar          *m_grammar;

    QCheckBox        *annotator_box;

    QLineEdit        *ref_annotator_edit;

    QPushButton      *btn_selectFields;





};


#endif // SEARCHGRAMMARTAB_H
