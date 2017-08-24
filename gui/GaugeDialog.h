/*
 * GaugeDialog.h
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
 * Purpose:
 * Created: 01/04/2011
 */

#ifndef GAUGEDIALOG_H
#define GAUGEDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QComboBox>
#include <QMessageBox>
#include <QSpinBox>

#include "lib/metadata.h"


using namespace metadata;

class GaugeDialog : public QDialog
{
	Q_OBJECT

public:
	GaugeDialog(QWidget *parent);
    //void show();

signals:
	void compareSpeakers(const QString &task, int tier_no);

public slots:
	void accept();

private:
	QDialogButtonBox *buttonBox;
    QWidget          *verticalLayoutWidget;
    QVBoxLayout      *vlayout;
    QComboBox        *task_box;
    QSpinBox         *tier_spin;
    QSet<QString>     categories;

	void checkProperties();

};
#endif // GAUGEDIALOG_H
