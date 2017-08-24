/*
 * DMDialogApi.h
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
 * Purpose: dialog scripting API
 * Created: 10/03/2012
 */


#ifndef DMDIALOGAPI_H
#define DMDIALOGAPI_H

#include <QObject>
#include <QDialog>
#include <QMessageBox>
#include <QFileDialog>

class DMDialogApi : public QObject
{
    Q_OBJECT
public:
    explicit DMDialogApi(QObject *parent = 0, QWidget *widget = 0);
    Q_INVOKABLE QString openFile();
    Q_INVOKABLE QString saveFile();

signals:
    
public slots:
    void error(const QString &title, const QString &text);
    void info(const QString &title, const QString &text);



private:
    QWidget *m_widget;
};

#endif // DMDIALOGAPI_H
