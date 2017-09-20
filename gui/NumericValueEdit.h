/*
 * NumericValueEdit.h
 *
 * Copyright (C) 2010-2017 Julien Eychenne
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
 * Purpose:
 * Created:
 */

#ifndef NUMERICVALUEEDIT_H
#define NUMERICVALUEEDIT_H

#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>

#include "dolmen.h"
#include "search/SearchNode.h"
#include "gui/DMLineEdit.h"

class NumericValueEdit : public QHBoxLayout
{
public:
	NumericValueEdit(QString desc);

	QString opcode() const;
	double value() const; // double rather than int to deal with most use cases
	QString description() const; // store useful info (ex : category for numeric properties)
	void show();
	void hide();

private:
    DMLineEdit *num_edit;
    QComboBox  *op_box; // operator
    QString     desc;
};

#endif // NUMERICVALUEEDIT_H
