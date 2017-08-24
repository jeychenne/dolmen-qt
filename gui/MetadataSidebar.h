/*
 * MetadataSideBar.h
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
 * Purpose: Right-hand side metadata widget
 * Created: 24/03/2012
 */

#ifndef METADATASIDEBAR_H
#define METADATASIDEBAR_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>

#include "lib/DFile.h"
#include "lib/Annotation.h"
#include "search/SearchMatch.h"
#include "lib/Bookmark.h"
#include "gui/MetadataEditor.h"



class MetadataSideBar : public QWidget
{
    Q_OBJECT

public:
    MetadataSideBar(QWidget *parent = NULL);

public slots:
    void displaySearchMatchInfo(const QString &fileInfo, const QString &propertiesInfo, const QString &startInfo, const QString &endInfo); // Annotation
    void displaySearchMatchInfo(const QString &fileInfo, const QString &propertiesInfo); // Document
    void displayFileInfo(DFile *df);
    void displayBookmarkInfo(IBrowserElement *elem);

private slots:
    void editProperties();
    void setFileDescription();

private:
    QLabel      *main_label, *file_label, *soundRef_label, *properties_label, *start_label, *end_label;
    QPushButton *properties_btn;
    QTabWidget  *tabs;
    QWidget     *info_tab;
    QTextEdit   *desc_edit;
    DFile       *file;
    Bookmark    *bookmark;
    bool         description_is_editable; // prevent a dfile's description from being modified when it is set in the widget

    void setupUi();
    void setNothingToDisplay();
    void clearWidgets();
    void setWidgets(bool showTimes);


};

#endif // METADATASIDEBAR_H
