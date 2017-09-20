/*
 * View.h
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
 * Purpose: implement views
 * Created: 02/09/10
 */

#ifndef VIEW_H
#define VIEW_H

#include <QWidget>
#include <QTabWidget>
#include <QVBoxLayout>

class View : public QWidget
{
	Q_OBJECT

public:
	View(QTabWidget *parent, const QString &title = QString());
    QTabWidget * tabWidget() const;
	virtual void leave() = 0;
	virtual QString id() const = 0;

signals:
	void displayText(QString);
    void focusLost();

protected:
	QVBoxLayout *layout;
    QTabWidget  *m_tabWidget;

    void focusOutEvent(QFocusEvent *event);
};

#endif // VIEW_H
