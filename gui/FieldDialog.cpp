/*
 * FieldDialog.cpp
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
 * Purpose: field selector in a search grammar
 * Created: 18/11/2012
 *
 * The development of this module has been supported by the Japanese
 * Society for the Promotion of Science (JSPS), Grant-in-Aid for Scientific
 * Research (B) n°23320121 (2011-2014).
 * Project title: A corpus-based longitudinal study of the interphonological
 * features of Japanese learners of French.
 * Project leader: Sylvain DETEY (Waseda University, Tokyo, Japan).
 */

#include "FieldDialog.h"

FieldDialog::FieldDialog(QWidget *parent, QStringList fields) :
    QDialog(parent)
{
    setWindowTitle(tr("Select fields..."));
    if (objectName().isEmpty())
        setObjectName(QString::fromUtf8("FieldDialog"));
    resize(280, 260);
    this->setModal(true);

    boxes = QList<QCheckBox*>();

    verticalLayoutWidget = new QWidget(this);
    vlayout = new QVBoxLayout(verticalLayoutWidget);
    vlayout->addWidget(new QLabel(tr("Select active fields:")));

    foreach (QString field, fields)
    {
        QCheckBox *box = new QCheckBox(field);
        vlayout->addWidget(box);
        box->setChecked(true);
        boxes << box;
    }

    vlayout->addSpacing(20);
    QHBoxLayout *hl = new QHBoxLayout;

    vlayout->addLayout(hl);
    vlayout->addStretch();

    buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    QHBoxLayout *btn_layout = new QHBoxLayout;
    btn_layout->addStretch();
    btn_layout->addWidget(buttonBox);
    vlayout->addLayout(btn_layout);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}


void FieldDialog::accept()
{
    QList<int> fields;

    for (int i = 0; i < boxes.size(); ++i)
    {
        if (boxes[i]->isChecked())
            fields << i+1;
    }

    emit activeFields(fields);
    QDialog::accept();
}
