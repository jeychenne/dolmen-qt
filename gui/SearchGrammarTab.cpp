/*
 * SearchGrammarTab.cpp
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

#include "SearchGrammarTab.h"
#include "FieldDialog.h"

SearchGrammarTab::SearchGrammarTab(Grammar *grammar, QWidget *parent) :
    QWidget(parent)
{
    m_grammar = grammar;
    // reinitialize active fields in case they have been modified in a previous query
    grammar->setAllFieldsActive();

    annotator_box = NULL;
    ref_annotator_edit = NULL;
    btn_selectFields = NULL;

    QVBoxLayout *vl = new QVBoxLayout;
    vl->setSpacing(0);
    vl->setContentsMargins(0,0,0,0);
    QGridLayout *layout = new QGridLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    QList<SearchField> fields = grammar->fields();

    int col = 0;
    int row = 0;


    for (int i = 0; i < fields.size(); i++)
    {
        SearchField field = fields.at(i);
        QStringList values;
        foreach (SearchValue v, field.values)
            values << v.text;

        QString label = m_grammar->translate(tr("All values"));
        FieldBox *field_box = new FieldBox(field, label);
        layout->addWidget(field_box, row, col);
        m_fields << field_box;

        if ((i+1) % grammar->fieldsPerRow() == 0)
        {
            row++;
            col = 0;
        }
        else
            col++;
    }

    QFrame* dummy_hline = new QFrame();
    dummy_hline->setFrameShape(QFrame::HLine);
    QHBoxLayout *dummy_line_layout = new QHBoxLayout;
    dummy_line_layout->addSpacing(150);
    dummy_line_layout->addWidget(dummy_hline);
    dummy_line_layout->addSpacing(150);

    // annotator comparison
    QHBoxLayout *hb = new QHBoxLayout;
    annotator_box = new QCheckBox(tr("Compare annotators"));
    hb->addStretch();//Spacing(30);
    hb->addWidget(annotator_box);
    hb->addSpacing(30);
    hb->addWidget(new QLabel(tr("Reference annotator:")));
    hb->addSpacing(20);
    ref_annotator_edit = new QLineEdit;
    hb->addWidget(ref_annotator_edit);
    hb->addSpacing(20);
    btn_selectFields = new QPushButton(tr("Select fields"));
    btn_selectFields->setEnabled(false);
    hb->addWidget(btn_selectFields);
    hb->addStretch();

    vl->addLayout(layout);
    vl->addLayout(dummy_line_layout);
    vl->addSpacing(10);
    vl->addLayout(hb);
    vl->addSpacing(10);
    vl->addStretch();

    this->setLayout(vl);

    ref_annotator_edit->setEnabled(false);
    connect(annotator_box, SIGNAL(toggled(bool)), ref_annotator_edit, SLOT(setEnabled(bool)));
    connect(annotator_box, SIGNAL(toggled(bool)), btn_selectFields, SLOT(setEnabled(bool)));
    connect(btn_selectFields, SIGNAL(clicked()), this, SLOT(selectFields()));
}

bool SearchGrammarTab::caseSensitive() const
{
    return m_grammar->caseSensitive();
}

bool SearchGrammarTab::compareAnnotators() const
{
    return (annotator_box && annotator_box->isChecked());
}

QString SearchGrammarTab::tierNamePattern() const
{
    return m_fields.at(m_grammar->tierSelectingField())->tierNamePattern();
}

QString SearchGrammarTab::regex() const
{
    QStringList values;

    foreach (FieldBox *fb, m_fields)
        values << fb->regex();

    return values.join(m_grammar->separator());
}

int SearchGrammarTab::tier() const
{
    return m_grammar->tier();
}

QString SearchGrammarTab::tierName() const
{
    return m_grammar->tierName();
}

QString SearchGrammarTab::filetype() const
{
    return "%annotation";
}

Grammar * SearchGrammarTab::grammar() const
{
    return m_grammar;
}

QString SearchGrammarTab::referenceAnnotator() const
{
    if (ref_annotator_edit)
        return ref_annotator_edit->text();
    else
        return QString("");
}

void SearchGrammarTab::selectFields()
{
    QStringList labels;

    foreach (SearchField f, m_grammar->fields())
    {
        labels << f.name;
    }

    FieldDialog *dialog = new FieldDialog(this, labels);
    connect(dialog, SIGNAL(activeFields(QList<int>)), m_grammar, SLOT(setActiveFields(QList<int>)));
    dialog->exec();
}

void SearchGrammarTab::setActiveFieldsInGrammar(QList<int> fields)
{
    m_grammar->setActiveFields(fields);
}



