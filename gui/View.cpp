/*
 * View.cpp
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
 * Purpose: Implement a view
 * Created: 02/09/2010
 */

#include "View.h"

View::View(QTabWidget *parent, const QString &title) : QWidget(parent)
{
    m_tabWidget = parent;
	parent->addTab(this, title);
    layout = new QVBoxLayout(this);
	layout->setContentsMargins(0,0,0,0);
	setContextMenuPolicy(Qt::CustomContextMenu);
}

void View::focusOutEvent(QFocusEvent *event)
{
    emit focusLost();
    QWidget::focusOutEvent(event);
}

QTabWidget * View::tabWidget() const
{
    return m_tabWidget;
}
