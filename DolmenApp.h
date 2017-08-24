/*
 * DolmenApp.h
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
 * Purpose: Main app
 * Created: September 2010
 */

#ifndef DOLMENAPP_H
#define DOLMENAPP_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QSplitter>
#include <QSizePolicy>
#include <QMessageBox>
#include <QSpacerItem>
#include <QPushButton>
#include <QSet>
#include <QString>
#include <QToolBar>
#include <QTime>
#include <QProgressDialog>
#include <QProcess>

#include "lib/Project.h"
#include "lib/Plugin.h"
#include "lib/SqlDatabase.h"
#include "lib/ZipArchive.h"
#include "utils/routines.h"
#include "utils/Global.h"
#include "search/QueryParser.h"
#include "gui/FileSidebar.h"
#include "gui/MetadataSidebar.h"
#include "gui/CorpusBrowser.h"
#include "gui/BookmarkBrowser.h"
#include "gui/Viewer.h"
#include "gui/BottomPane.h"
#include "gui/MainMenu.h"
#include "gui/actions.h"
#include "gui/QueryView.h"
#include "gui/SearchWindow.h"
#include "gui/splitters.h"
#include "gui/PreferenceEditor.h"
#include "gui/GaugeDialog.h"
#include "gui/GaugeView.h"
#include "gui/AnnotationView.h"
#include "runtime/PluginHandler.h"




class DolmenApp : public QMainWindow
{
    Q_OBJECT

public:
    DolmenApp(sol::state_view lua, QString m_version, QWidget *parent = 0, const QString project = QString());
	~DolmenApp();

public slots:
    void onMainSearchClicked();
    void onMainSearchClicked(Grammar *grammar);
	void warnUser(QString &msg);
	void displayQuery(Query *);
	void closeEvent(QCloseEvent *);
	void askPath(DFile *);

	void displayImportProgressBar(int);
	void updateImportProgressBar(int);
	void loadQuery(const QString &);
	void forwardQuery(int);

    // Since Dolmen 2.0, output and error messages are redirected to the status bar
	void output(const QString &text);
	void error(const QString &text);

	void about();
	void license();
	void acknowledgements();

    void editPreferences();
    void openDocumentation();
    void onOpenProject();
    void onAddFiles();

signals:
	void app_saveFileWithPath(DFile *, const QString &);
	void app_openProject();

private slots:
	void onFileBrowserDoubleClicked();
	void closeCurrentView();
	void openRecentProject();

	void editProject();
	void updatePreferences();
	void enableProjectActions(); // enable actions such as "Save project...", "Open query..."
	void openLastProject();
	void setGlobalProjectPath(QString);
	void clearRecentProjects();
	void cleanUpProcess();
	void displaySupportedAudioFormats();
	void toggleFileBrowser();
	void toggleBottomTabs();
    void toggleMetadata();
    void toggleStatusBar();
	void compareSpeakers();
	void openGaugeView(const QString &task, int tier_no);
	void openAnnotationMatch(Annotation *a, int tier, double tstart, double tend);
    void installUserPlugin();
    void importMetadata(const QString &path);
    void onExtendConcMenu();
    void onGoToWebsite();

private:
    QWidget             *root_widget, *wgt_browser_bar;
    QVBoxLayout         *vlayout_browser_bar;
    Splitter            *hsplitter, *vsplitter, *view_splitter;
    QToolBar            *m_toolbar;
    QMenu               *projects_submenu;
    QProgressDialog     *import_progress;
    QString              m_version;
    FileSidebar         *m_sidebar;
    CorpusBrowser       *corpus_browser;
    BookmarkBrowser     *bookmark_browser;
    Viewer              *viewer;
    BottomPane          *m_tabs_bottom;
    PreferenceEditor    *prefs_editor;
    MetadataSideBar     *metadata_widget;
    MainMenu            *menu;
    QMenu               *file_menu, *conc_menu, *edit_menu, *window_menu, *tools_menu, *help_menu;
    Project             *current_project;
    QueryParser         *m_query_parser;

    ActionOpenProject   *action_open_project;
    ActionAddFile       *action_add_file;
    ActionOpenQuery     *action_open_query;
    ActionAddFolder     *action_add_folder;
    ActionSaveProject   *action_save_project;
    ActionSaveProjectAs *action_save_project_as;
    ActionImportMetaCsv *action_import_meta;
    QAction             *action_close_view, *action_show_browser, *action_show_bottom_tabs, *action_show_status,
                        *action_show_metadata, *action_compare_speakers, *action_compare_annotators;


    QProcess            *process; // process to display doc
    QList<Query*>        queries; // keep track of all previous queries, in order
    MainDatabase        *m_db;
    PluginHandler       *m_plugin_handler;
    sol::state_view      m_lua;

	// initialize GUI
	void setProject();
	void setupUi();
	void setMainMenu();
	void setFileMenu();
    void setConcMenu();
	void setEditMenu();
	void setWindowMenu();
	void setToolsMenu();
	void setHelpMenu();
    void setBrowserBar();
    void setMetadataBar();
	void setSplitters();
	void setViewer();
	void setBottomTabs();
	void setConnections();
	void checkPraat();
	void resetQueryParser();
	void readSettings();
	void writeSettings();
    void setQueryViewConnections(QueryView *view);
	void initializeSound();
    void initializePlugins();
    void initializeGrammars();
    void postInitialize();
    void testPlugins(QDir root, QStringList &plugins);
    void setMainPlugin(Plugin *plugin);
	void closeApplication();
    void setMainDatabase();
};

#endif // DOLMENAPP_H
