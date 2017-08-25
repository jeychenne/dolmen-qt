/*
 * DolmenApp.cpp
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

#include "DolmenApp.h"
#include "audio/RtAudio.h"

/*
#include "sql/sqlite3.h"

void sql_test()
{
    sqlite3 *database;
    sqlite3_open("Database.dmdb", &database);
    sqlite3_stmt *statement;

    if (sqlite3_prepare_v2(database, "CREATE TABLE a (b INTEGER, c INTEGER);", -1, &statement, 0) == SQLITE_OK)
    {
        int cols = sqlite3_column_count(statement);
        int result = 0;
        while (true)
        {
            result = sqlite3_step(statement);

            if (result == SQLITE_ROW)
            {
                for (int col = 0; col < cols; col++)
                {
                    string s = (char*)sqlite3_column_text(statement, col);
                    //do something with it
                }
            }
            else
            {
                break;
            }
        }

        sqlite3_finalize(statement);
    }
    sqlite3_close(database);
}
*/


void setSupportedFormats()
{
	SF_FORMAT_INFO	formatInfo;
	int             k, count;

	sf_command(NULL, SFC_GET_SIMPLE_FORMAT_COUNT, &count, sizeof(int));

	for (k = 0; k < count; k++)
	{
		formatInfo.format = k;
		sf_command(NULL, SFC_GET_SIMPLE_FORMAT, &formatInfo, sizeof(formatInfo));
		Global::AudioFormats << formatInfo.extension;
	}

	// it seems that "oga" is the default extension for OGG files in libsndfile
	if (Global::AudioFormats.contains(QString("oga")) && ! Global::AudioFormats.contains(QString("ogg")))
		Global::AudioFormats << "ogg";
	Global::CommonAudioFormats << "wav" << "flac" << "aiff";
}


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DolmenApp::DolmenApp(sol::state_view lua, QString version, QWidget *parent, const QString projectPath) :
    QMainWindow(parent), m_lua(lua)
{
    m_version = version;
    m_toolbar = NULL;
    m_query_parser = NULL;
    m_db = NULL;
	readSettings();
	setProject();
    setMainDatabase();
	checkPraat();
	setupUi();
    setStatusBar(new QStatusBar);
    Global::Output = statusBar();
    statusBar()->setVisible(Global::EnableStatusBar);
    m_plugin_handler = new PluginHandler(this, lua);
    m_tabs_bottom->setHandler(m_plugin_handler);

    Global::Praat = new PraatInstance;
	setConnections();	
	initializeSound();
    initializePlugins();
    initializeGrammars();
    postInitialize();
    setWindowTitle("Dolmen");


	if (projectPath != "")
        current_project->open(projectPath);

    m_tabs_bottom->setCurrentIndex(DM_SHELL_TAB);
}

void DolmenApp::readSettings()
{
	QSettings settings("Dolmen", "Dolmen");

	// create settings file
	if (! settings.contains("AppDataDir"))
	{
		QString defaultPath(Global::AppDataDir);
		QDir appdir(defaultPath);

		if (appdir.exists() || appdir.mkpath(appdir.path()))
		{
			settings.setValue("AppDataDir", defaultPath);
		}
		else
		{
			settings.setValue("AppDataDir", dm_home());
		}

		settings.sync();
	}

#ifdef Q_OS_MAC
    if (settings.contains("Maximized"))
        Global::Maximize = settings.value("Maximized").toBool();
#endif

	if (settings.contains("Geometry"))
		restoreGeometry(settings.value("Geometry").toByteArray());


	if (settings.contains("RecentProjects"))
	{
		QStringList projects = settings.value("RecentProjects").toStringList();

		foreach (QString proj, projects)
		{
			QFileInfo info(proj);
			if (info.exists())
				Global::RecentProjects << proj;
		}
	}

    if (settings.contains("Default/MatchContextWindow")) {
		Global::MatchCotextLength = settings.value("Default/MatchContextWindow").toInt();
    }
    else {
		settings.setValue("Default/MatchContextWindow", MATCH_COTEXT_LENGTH);
    }

    if (settings.contains("Default/SearchStyle")) {
		Global::DefaultSearchStyle = settings.value("Default/SearchStyle").toString();
    }
    else {
		settings.setValue("Default/SearchStyle", Global::DefaultSearchStyle);
    }

    if (settings.contains("Default/UseAntialiasingInPlots")) {
        Global::UseAntialiasingInPlots = settings.value("Default/UseAntialiasingInPlots").toBool();
    }
    else {
        settings.setValue("Default/UseAntialiasingInPlots", Global::UseAntialiasingInPlots);
    }

    if (settings.contains("Default/EnableStatusBar")) {
        Global::EnableStatusBar = settings.value("Default/EnableStatusBar").toBool();
    }

    if (settings.contains("Praat/PraatPath")) {
		Global::PraatPath = settings.value("Praat/PraatPath").toString();
    }
    else {
		settings.setValue("Praat/PraatPath", Global::PraatPath);
    }

	// Application mode
    if (settings.contains("Advanced/ApplicationMode")) {
		Global::ApplicationMode = settings.value("Advanced/ApplicationMode").toString();
    }

    if (settings.contains("Advanced/ResourcesPath")) {
        Global::ResourcesPath = settings.value("Advanced/ResourcesPath").toString();
    }
    else {
        Global::ResourcesPath = getDefaultResourcesDir();
    }


	settings.sync();
}

void DolmenApp::writeSettings()
{
    QSettings settings("Dolmen", "Dolmen");

	settings.setValue("AppDataDir", Global::AppDataDir);
    settings.setValue("Geometry", saveGeometry());

#ifdef Q_OS_MAC
    if (this->isMaximized())
        settings.setValue("Maximized", "true");
    else
        settings.setValue("Maximized", "false");
#endif

	settings.setValue("RecentProjects", Global::RecentProjects);
	settings.setValue("Default/MatchContextWindow", Global::MatchCotextLength);
	settings.setValue("Default/SearchStyle", Global::DefaultSearchStyle);
    settings.setValue("Default/UseAntialiasingInPlots", Global::UseAntialiasingInPlots);
    settings.setValue("Default/EnableStatusBar", Global::EnableStatusBar);

	// Praat
	settings.setValue("Praat/PraatPath", Global::PraatPath);

	// Application mode
	settings.setValue("Advanced/ApplicationMode", Global::ApplicationMode);

    settings.setValue("Advanced/ResourcesPath", Global::ResourcesPath);
}

void DolmenApp::setProject()
{
    current_project = new Project(m_lua);
}

void DolmenApp::setMainDatabase()
{
    QString path = getMainDatabase();
    //qDebug() << "DB" << path;
    QFileInfo info(path);
    bool exists = info.exists();
    m_db = new MainDatabase(this, path);

    if (! exists)
    {
        if (!m_db->status())
            QMessageBox::warning(this, tr("Database problem"), tr("The database %1 could not be created:\n%2").arg(path, m_db->error()));
        m_db->createMainTable();
    }
    else if (!m_db->status())
    {
        QMessageBox::warning(this, tr("Database problem"), tr("The database %1 could not be opened:\n%2").arg(path, m_db->error()));
    }
}


void DolmenApp::installUserPlugin()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Install plugin..."), QDir::homePath(), tr("Zip files (*.zip)"));

    if (! fileName.isEmpty() && fileName.toLower().endsWith(".zip"))
    {
        ZipArchive zip(fileName, getUserPluginDir());

        if (zip.unzip())
            QMessageBox::information(this, tr("Plugin"), tr("The plugin was successfully installed!\nYou need to restart Dolmen to be able to use it."));
        else
            QMessageBox::critical(this, tr("Error"), tr("The plugin could not be installed!"));

    }
}

void DolmenApp::importMetadata(const QString &path)
{
    QList<QStringList> csv = parseCsv(path);
    QStringList header = csv.takeFirst();
    QString file, cat, val;

    if (header.at(0) != "File")
    {
        error(tr("The first column in the CSV file must be named \"File\" and the file must have a header row"));
        return;
    }

    foreach (QStringList fields, csv)
    {
        file = fields.at(0);

        for (int i = 1; i < fields.size(); ++i)
        {
            cat = header.at(i);
            val = fields.at(i);

            if (! val.isEmpty())
            {
                current_project->addMetadataToFile(file, cat, val);
            }
        }
    }
}

void DolmenApp::onExtendConcMenu()
{
    viewer->displayHelp("/scripting/plugins");
}

void DolmenApp::onGoToWebsite()
{
    QDesktopServices::openUrl(QUrl("http://www.dolmenling.com"));
}

void DolmenApp::openRecentProject() // [SLOT]
{
	QAction *action = qobject_cast<QAction*>(sender());
	if (action)
        current_project->open(action->toolTip());
}

void DolmenApp::openLastProject()
{
    current_project->open(Global::RecentProjects.at(0));
}

void DolmenApp::closeEvent(QCloseEvent *event)
{
    if (current_project->hasUnsavedChanges())
	{
		QMessageBox box(this);
		box.setText(tr("The current project has unsaved changes\n"));
		box.setInformativeText(tr("Do you really want to exit?"));
		box.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		box.setDefaultButton(QMessageBox::Save);
		int result = box.exec();

		if (result == QMessageBox::Save)
		{
            action_save_project->execute();
			closeApplication();
			event->accept();
		}
		else if (result == QMessageBox::Discard)
		{
			closeApplication();
			event->accept();
		}
		else
			event->ignore();
	}
	else
	{
		closeApplication();
		event->accept();
	}
}

void DolmenApp::closeApplication()
{
	viewer->close();
	writeSettings();
    delete m_db;
    foreach (Plugin *p, m_plugin_handler->plugins())
        delete p;
}

void DolmenApp::initializeSound()
{
//	if (Global::Audio.getDeviceCount() < 1)
//		error(tr("No audio device found. Sound won't be available."));
//	else
		setSupportedFormats();
}

void DolmenApp::initializePlugins()
{
    // first load system plugins, then user plugins
    QDir systemPluginRoot(getSystemPluginDir());
    QDir userPluginRoot(getUserPluginDir());
    QStringList plugins;

    QFileInfo sysinfo(systemPluginRoot.path());

    if (sysinfo.exists())
    {
        plugins = systemPluginRoot.entryList();
        testPlugins(systemPluginRoot, plugins);
    }

    plugins = userPluginRoot.entryList();
    testPlugins(userPluginRoot, plugins);
}

void DolmenApp::initializeGrammars()
{
    if (!m_plugin_handler->plugins().isEmpty()) {
        conc_menu->addSeparator();
    }

    foreach (Plugin *p, m_plugin_handler->plugins())
    {
        auto menu = conc_menu->addMenu(p->name());

        foreach (auto g, p->getGrammars())
        {
            auto action = menu->addAction(g->name());

            connect(action, &QAction::triggered, [this,g]() {
                onMainSearchClicked(g);
            });
        }
    }
}

void DolmenApp::postInitialize()
{
    conc_menu->addSeparator();
    auto extend_conc = new QAction(tr("How to extend this menu"));
    conc_menu->addAction(extend_conc);
    connect(extend_conc, SIGNAL(triggered()), this, SLOT(onExtendConcMenu()));
}

void DolmenApp::testPlugins(QDir root, QStringList &plugins)
{
    plugins.sort();

    foreach(QString file, plugins)
    {
        if (file.startsWith("."))
            continue;

        QString path = root.absolutePath() + QDir::separator() + file;

        if (QFileInfo(path).isDir())
        {
            QDir pluginDir(path);
            Plugin *plugin = new Plugin(pluginDir);

            if (plugin->isValid())
            {
                if (plugin->initialize())
                {
                    if (plugin->isMain())
                        this->setMainPlugin(plugin);

                    connect(plugin, SIGNAL(output(QString)), this, SLOT(output(QString)));
                    plugin->postInitialize();

                    if (plugin->hasMenu())
                    {
                        QMenu *pluginMenu = plugin->menu();

                        // append actions to Tools menu
                        if (pluginMenu->title() == tr("Tools"))
                            tools_menu->addActions(pluginMenu->actions());
                        // insert plugin menu before the Window menu
                        else
                            menu->insertMenu(window_menu->menuAction(), pluginMenu);
                    }

                    m_plugin_handler->addPlugin(plugin);
                }
            }
            else delete plugin;
        }
    }
}

void DolmenApp::setMainPlugin(Plugin *plugin)
{
    // if several plugins are declared as MainApplication, the last plugin that is loaded
    // overrides the previous ones.
    this->setWindowTitle(plugin->name());
    m_plugin_handler->setMainPlugin(plugin);
}


void DolmenApp::setupUi()
{
	this->resize(QSize(800, 550));
    root_widget = new QWidget(this);

    setMainMenu();
	setSplitters();

	// layout parts of the interface
    wgt_browser_bar = new QWidget(hsplitter);

#ifdef Q_OS_MAC // blend with the color of the file browser
    wgt_browser_bar->setAutoFillBackground(true);
    QPalette p(wgt_browser_bar->palette());
	p.setColor(QPalette::Background, "#dce4eb");
    wgt_browser_bar->setPalette(p);
#endif
    vlayout_browser_bar = new QVBoxLayout(wgt_browser_bar);
    wgt_browser_bar->setLayout(vlayout_browser_bar);
    vlayout_browser_bar->setSpacing(0);
#ifdef Q_OS_MAC
    vlayout_browser_bar->setContentsMargins(0,0,0,0);
#else
    vlayout_browser_bar->setContentsMargins(2,0,0,0);
#endif

    setBrowserBar();
    vsplitter->addWidget(view_splitter);
    setViewer();
    setMetadataBar();

    setBottomTabs();

    hsplitter->addWidget(wgt_browser_bar);

	hsplitter->addWidget(vsplitter);

	// set size of the different parts of the UI
    hsplitter->setStretchFactor(0,2); // side bar
    hsplitter->setStretchFactor(1,4); // views + tabs
	hsplitter->setCollapsible(1, false);

	vsplitter->setStretchFactor(0,5); // views
    vsplitter->setStretchFactor(1,2); // bottom tabs
	vsplitter->setCollapsible(0, false);

	this->setCentralWidget(hsplitter);

#ifdef Q_OS_MAC
    if (Global::Maximize)
        this->showMaximized();
#endif
}

void DolmenApp::setSplitters()
{
    // top-level vertical splitter (split left-side browser side bar from the rest)
	hsplitter = new Splitter(Qt::Horizontal, this);
	QSizePolicy hsizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    hsplitter->setSizePolicy(hsizePolicy);

	// vertical splitter containing views and tabs
	vsplitter = new Splitter(Qt::Vertical);

    QSizePolicy vsizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    vsizePolicy.setHorizontalStretch(0);
    vsizePolicy.setVerticalStretch(7);
	vsizePolicy.setHeightForWidth(vsplitter->sizePolicy().hasHeightForWidth());
    vsplitter->setSizePolicy(vsizePolicy);

    // split views from the right-side metadata side bar
    view_splitter = new Splitter(Qt::Horizontal);
    QSizePolicy hsizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view_splitter->setSizePolicy(hsizePolicy2);
}

void DolmenApp::setMainMenu()
{
	menu = new MainMenu(this);
	setMenuBar(menu);

	setFileMenu();
	setEditMenu();
    setConcMenu();
    setToolsMenu();
	setWindowMenu();
	setHelpMenu();
}

void DolmenApp::setFileMenu()
{
    file_menu = menu->addMenu(tr("&File"));

    action_open_project = new ActionOpenProject(this, current_project);
    file_menu->addAction(action_open_project);

    action_add_file = new ActionAddFile(this, current_project);
    file_menu->addAction(action_add_file);

    action_add_folder = new ActionAddFolder(this, current_project);
    file_menu->addAction(action_add_folder);
    file_menu->addSeparator();

    projects_submenu = file_menu->addMenu(tr("Recent projects"));
	foreach (QString proj, Global::RecentProjects)
	{
		QString shortName = proj;
	#ifndef Q_OS_WIN
		if (shortName.startsWith(dm_home()))
			shortName.replace(dm_home(), "~");
	#endif
		QAction *action = new QAction(shortName, this);
		action->setToolTip(proj); // store full path
        projects_submenu->addAction(action);
		connect(action, SIGNAL(triggered()), this, SLOT(openRecentProject()));
	}
    if (projects_submenu->isEmpty())
        projects_submenu->setEnabled(false);
	else
	{
        projects_submenu->addSeparator();
		QAction *action = new QAction(tr("Clear recent projects"), this);
        projects_submenu->addAction(action);
		connect(action, SIGNAL(triggered()), this, SLOT(clearRecentProjects()));
	}

	QAction *lastProjAction = new QAction(tr("Open most recent project"), this);
	lastProjAction->setShortcut(QKeySequence("Ctrl+Shift+o"));
    file_menu->addAction(lastProjAction);
		if (Global::RecentProjects.size() == 0)
			lastProjAction->setEnabled(false);
		else
			connect(lastProjAction, SIGNAL(triggered()), this, SLOT(openLastProject()));

    file_menu->addSeparator();

    action_save_project = new ActionSaveProject(this, current_project);
    file_menu->addAction(action_save_project);
    action_save_project->setEnabled(false);
    action_save_project_as = new ActionSaveProjectAs(this, current_project);
    file_menu->addAction(action_save_project_as);
    action_save_project_as->setEnabled(false);

    file_menu->addSeparator();
    action_open_query = new ActionOpenQuery(this, current_project);
    file_menu->addAction(action_open_query);
    connect(action_open_query, SIGNAL(menu_openQuery(QString)), this, SLOT(loadQuery(QString)));
    action_open_query->setEnabled(false);

    QAction *installPluginAction = new QAction(tr("Install plugin..."), this);
    connect(installPluginAction, SIGNAL(triggered()), this, SLOT(installUserPlugin()));
    file_menu->addAction(installPluginAction);

    action_import_meta = new ActionImportMetaCsv(this);
    connect(action_import_meta, SIGNAL(sendFileName(QString)), this, SLOT(importMetadata(QString)));
    file_menu->addAction(action_import_meta);
    action_import_meta->setEnabled(false);

    file_menu->addSeparator();
    action_close_view = new QAction(tr("Close current view"), this);
    action_close_view->setShortcut(QKeySequence("Ctrl+w"));
    connect(action_close_view, SIGNAL(triggered()), this, SLOT(closeCurrentView()));
    file_menu->addAction(action_close_view);

    file_menu->addSeparator();
	QAction *quitDolmen = new QAction(tr("Quit"), this);
	quitDolmen->setShortcut(QKeySequence("Ctrl+q"));
	connect(quitDolmen, SIGNAL(triggered()), this, SLOT(close()));
    file_menu->addAction(quitDolmen);
}

void DolmenApp::setConcMenu()
{
    conc_menu = menu->addMenu(tr("Conc"));
    auto generic_find = new QAction(tr("Find concordances..."));
    generic_find->setShortcut(QKeySequence("Ctrl+f"));
    //generic_find->setToolTip(tr("Find some text in the current project"));
    conc_menu->addAction(generic_find);
    connect(generic_find, SIGNAL(triggered()), this, SLOT(onMainSearchClicked()));

    // The menu will be finalized once plugins have been loaded...
}

void DolmenApp::setEditMenu()
{
    edit_menu = menu->addMenu(tr("&Edit"));

	QAction *editProj = new QAction(tr("Project properties..."), this);
	connect(editProj, SIGNAL(triggered()), this, SLOT(editProject()));
    edit_menu->addAction(editProj);

	QAction *editPrefs = new QAction(tr("Preferences..."), this);
	connect(editPrefs, SIGNAL(triggered()), this, SLOT(editPreferences()));
    edit_menu->addAction(editPrefs);
}

void DolmenApp::setToolsMenu()
{
    tools_menu = menu->addMenu(tr("&Tools"));

    action_compare_speakers = new QAction(tr("Compare speakers or subjects..."), this);
    tools_menu->addAction(action_compare_speakers);
    action_compare_speakers->setEnabled(false);
    connect(action_compare_speakers, SIGNAL(triggered()), this, SLOT(compareSpeakers()));
}

void DolmenApp::setWindowMenu()
{
    window_menu = menu->addMenu(tr("&Window"));

    action_show_browser = new QAction(tr("Show file browser"), this);
    action_show_browser->setShortcut(QKeySequence("Ctrl+Shift+l"));
    action_show_browser->setCheckable(true);
    action_show_browser->setChecked(true);

    action_show_metadata = new QAction(tr("Show metadata"), this);
    action_show_metadata->setShortcut(QKeySequence("Ctrl+Shift+m"));
    action_show_metadata->setCheckable(true);
    action_show_metadata->setChecked(true);

    action_show_bottom_tabs = new QAction(tr("Show system tabs"), this);
    action_show_bottom_tabs->setShortcut(QKeySequence("Ctrl+Shift+b"));
    action_show_bottom_tabs->setCheckable(true);
    action_show_bottom_tabs->setChecked(true);

    action_show_status = new QAction(tr("Show status bar"), this);
    action_show_status->setCheckable(true);
    action_show_status->setChecked(Global::EnableStatusBar);
    action_show_status->setEnabled(Global::EnableStatusBar);

    connect(action_show_browser, SIGNAL(triggered()), this, SLOT(toggleFileBrowser()));
    connect(action_show_bottom_tabs, SIGNAL(triggered()), this, SLOT(toggleBottomTabs()));
    connect(action_show_metadata, SIGNAL(triggered()), this, SLOT(toggleMetadata()));
    connect(action_show_status, SIGNAL(triggered()), this, SLOT(toggleStatusBar()));

    window_menu->addAction(action_show_browser);
    window_menu->addAction(action_show_metadata);
    window_menu->addAction(action_show_bottom_tabs);
    window_menu->addSeparator();
    window_menu->addAction(action_show_status);
}


void DolmenApp::setHelpMenu()
{
    help_menu = menu->addMenu(tr("&Help"));

	QAction *docAction = new QAction(tr("Documentation"), this);
	connect(docAction, SIGNAL(triggered()), this, SLOT(openDocumentation()));
    help_menu->addAction(docAction);
    help_menu->addSeparator();

    QAction *webAction = new QAction(tr("Go to website"), this);
    connect(webAction, SIGNAL(triggered()), this, SLOT(onGoToWebsite()));
    help_menu->addAction(webAction);

	QAction *licenseAction = new QAction(tr("License"), this);
	connect(licenseAction, SIGNAL(triggered()), this, SLOT(license()));
    help_menu->addAction(licenseAction);

	QAction *acknowAction = new QAction(tr("Acknowledgements"), this);
	connect(acknowAction, SIGNAL(triggered()), this, SLOT(acknowledgements()));
    help_menu->addAction(acknowAction);
    help_menu->addSeparator();

    QAction *formatsAction = new QAction(tr("Sound information"), this);
	connect(formatsAction, SIGNAL(triggered()), this, SLOT(displaySupportedAudioFormats()));
    help_menu->addAction(formatsAction);
    help_menu->addSeparator();

	QAction *aboutAction = new QAction(tr("About Dolmen"), this);
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
    help_menu->addAction(aboutAction);
}

void DolmenApp::checkPraat()
{
	QFileInfo praat(Global::PraatPath);

	if (praat.exists())
		Global::PraatIsInstalled = true;
}

void DolmenApp::clearRecentProjects()
{
    projects_submenu->clear();
	Global::RecentProjects = QStringList();
    projects_submenu->setEnabled(false);
}

void DolmenApp::setBrowserBar()
{
    m_sidebar = new FileSidebar(m_toolbar);

    corpus_browser = new CorpusBrowser(current_project, Global::Praat, m_sidebar->label());
    bookmark_browser = new BookmarkBrowser;

    m_sidebar->setCorpus(corpus_browser);
    m_sidebar->setBookmarks(bookmark_browser);
    //TODO: add scripts
    m_sidebar->showCorpus();
    vlayout_browser_bar->addWidget(m_sidebar);

    connect(m_sidebar, SIGNAL(updateUi()), corpus_browser, SLOT(updateTitle()));
    connect(current_project, SIGNAL(project_modified(VFolder*)), m_sidebar, SLOT(showCorpus()));
}

void DolmenApp::setMetadataBar()
{
    metadata_widget = new MetadataSideBar;
    view_splitter->addWidget(metadata_widget);

    view_splitter->setStretchFactor(0, 3);
    view_splitter->setStretchFactor(1, 2);
    view_splitter->setCollapsible(0, false);
}

void DolmenApp::setViewer()
{
    viewer = new Viewer(view_splitter, this);
    connect(viewer, SIGNAL(displayText(QString)), this, SLOT(output(QString)));
	viewer->setStartView();
}

void DolmenApp::setBottomTabs()
{
    m_tabs_bottom = new BottomPane(vsplitter, m_lua);
}

void DolmenApp::onMainSearchClicked()
{
    onMainSearchClicked(nullptr);
}


void DolmenApp::onMainSearchClicked(Grammar *grammar)
{
    if (current_project->isEmpty())
    {
        QMessageBox msg(QMessageBox::Warning, tr("error"), tr("The current project is empty!"));
        msg.exec();

        return;
    }

	this->resetQueryParser();
    SearchWindow *window = new SearchWindow(m_query_parser, this, grammar);
    connect(window, SIGNAL(sendQuery(Query*)), this, SLOT(displayQuery(Query*)));
	window->show();
}

void DolmenApp::resetQueryParser()
{
	// first regenerate search engine to make sure it uses the current project
    if (m_query_parser)
	{
        disconnect(m_query_parser, SIGNAL(error_search(QString&)), this, SLOT(warnUser(QString&)));
        delete m_query_parser;
	}

    m_query_parser = new QueryParser(current_project);
    connect(m_query_parser, SIGNAL(error_search(QString&)), this, SLOT(warnUser(QString&)));
}

void DolmenApp::onFileBrowserDoubleClicked()
{
    if (current_project->isEmpty())
		emit app_openProject();
}


void DolmenApp::loadQuery(const QString &queryString)
{
	this->resetQueryParser();
    Query *query = m_query_parser->parse(queryString);//TODO: set the query's grammar
	this->displayQuery(query);
}

void DolmenApp::forwardQuery(int index)
{
	if (index >= queries.size())
		return;

	Query *query = queries[index];

	for (int i = 0; i < viewer->count(); ++i)
	{
		QWidget *w = viewer->widget(i);
		if (isInstance(w, QueryView))
		{
			QueryView *qw = qobject_cast<QueryView*>(w);
			if (qw->isQuery(query))
			{
				viewer->setCurrentIndex(i);
				return;
			}
		}
	}

	// redisplay query if it wasn't found
    QueryView *resultsView = new QueryView(viewer, metadata_widget, Global::Praat, query, tr("Query %1").arg(++index));
    this->setQueryViewConnections(resultsView);
	viewer->setCurrentWidget(resultsView);
}

void DolmenApp::displayQuery(Query *query)
{
//	QTime timer;
//	timer.start();
    query->run();

//	qDebug() << "Time to run query: " << (float)timer.elapsed()/1000.0;

	foreach (QString warning, query->warnings())
		this->error(warning);

	if (query->results().size() == 0)
	{
		QString msg(tr("No match found."));
		error(msg);
	}
	else
	{
//		timer.restart();
        QueryView *resultsView = new QueryView(viewer, metadata_widget, Global::Praat, query);
        this->setQueryViewConnections(resultsView);
		viewer->setCurrentWidget(resultsView);
        m_tabs_bottom->appendQuery(query);

		queries << query;
	}
//	qDebug() << "Time to display query: " << (float)timer.elapsed()/1000.0;
}

void DolmenApp::setQueryViewConnections(QueryView *view)
{
    connect(view, SIGNAL(itemMatchMetadata(QString,QString,QString,QString)), metadata_widget, SLOT(displaySearchMatchInfo(QString,QString,QString,QString)));
    connect(view, SIGNAL(textMatchMetadata(QString,QString)), metadata_widget, SLOT(displaySearchMatchInfo(QString,QString)));
    connect(view, SIGNAL(output(QString)), this, SLOT(output(QString)));
    connect(view, SIGNAL(forwardBookmark(Bookmark*)), current_project, SLOT(addBookmark(Bookmark*)));
    connect(view, SIGNAL(openMatch(Annotation*,int,double,double)), this, SLOT(openAnnotationMatch(Annotation*,int,double,double)));
    connect(view, SIGNAL(forwardDataTable(DataTable*)), current_project, SLOT(addDataTable(DataTable*)));
}

void DolmenApp::editPreferences()
{
    prefs_editor = new PreferenceEditor(this);
    prefs_editor->setAttribute(Qt::WA_DeleteOnClose);
    connect(prefs_editor, SIGNAL(accepted()), this, SLOT(updatePreferences()));
    prefs_editor->show();


}

void DolmenApp::editProject()
{
    if (current_project->isEmpty())
		error(tr("The current project is empty"));
	else
        viewer->openProjectView(current_project);
}

void DolmenApp::updatePreferences()
{
    // Update status bar
    action_show_status->setEnabled(Global::EnableStatusBar);
    action_show_status->setChecked(Global::EnableStatusBar);
    bool visible = action_show_status->isEnabled() && action_show_status->isChecked();
    statusBar()->setVisible(visible);

	this->writeSettings();
    disconnect(prefs_editor, SIGNAL(accepted()), this, SLOT(updatePreferences()));
    prefs_editor->close();
}


void DolmenApp::setGlobalProjectPath(QString path)
{
	QFileInfo dir(path);
	Global::CurrentProjectDir = QDir::toNativeSeparators(dir.absolutePath());
	Global::CurrentProjectPath = path;
}

void DolmenApp::toggleFileBrowser()
{
    if (wgt_browser_bar->isHidden())
	{
        wgt_browser_bar->show();
        action_show_browser->setChecked(true);
	}
	else
	{
        wgt_browser_bar->hide();
        action_show_browser->setChecked(false);
	}
}

void DolmenApp::toggleBottomTabs()
{
    if (m_tabs_bottom->isHidden())
	{
        m_tabs_bottom->show();
	}
	else
	{
        m_tabs_bottom->hide();
	}
}

void DolmenApp::toggleMetadata()
{
    if (metadata_widget->isHidden())
        metadata_widget->show();
    else
        metadata_widget->hide();
}

void DolmenApp::toggleStatusBar()
{
    if (statusBar()->isHidden()) {
        statusBar()->show();
    }
    else {
        statusBar()->hide();
    }
}

void DolmenApp::setConnections()
{
	/*** FROM PROJECT ***/

	// redraw tree in file browser
    connect(current_project, SIGNAL(project_modified(VFolder*)), corpus_browser, SLOT(updateTree(VFolder*)));
    // clear tree in file browser
    connect(current_project, SIGNAL(project_cleared()), corpus_browser, SLOT(clearProject()));
	// enable open query
    connect(current_project, SIGNAL(project_modified(VFolder*)), this, SLOT(enableProjectActions()));
	// reset global project path
    connect(current_project, SIGNAL(project_pathSet(QString)), this, SLOT(setGlobalProjectPath(QString)));
	// read/write error
    connect(current_project, SIGNAL(file_error(QString&)), this, SLOT(warnUser(QString&)));
	// update browser header
    connect(current_project, SIGNAL(project_saved()), corpus_browser, SLOT(resetLabels()));
	// update browser labels
    connect(current_project, SIGNAL(project_filesModified()), corpus_browser, SLOT(setFileColor()));
	// ask path for unsaved files
    connect(current_project->fileSystemRoot(), SIGNAL(file_needsPath(DFile*)), this, SLOT(askPath(DFile*)));
	// standard output
    connect(current_project, SIGNAL(information(QString)), this, SLOT(output(QString)));
	// standard error
    connect(current_project, SIGNAL(error(QString)), this, SLOT(error(QString)));
	// progress bar
    connect(current_project, SIGNAL(project_startImportingFolder(int)), this, SLOT(displayImportProgressBar(int)));
    connect(current_project, SIGNAL(project_fileImported(int)), this, SLOT(updateImportProgressBar(int)));
	// bookmarks
    connect(current_project, SIGNAL(project_bookmarksModified(QList<Bookmark*>)),
            bookmark_browser, SLOT(redraw(QList<Bookmark*>)));
    // add file to database
    connect(current_project, SIGNAL(project_newFileAdded(DFile*)), m_db, SLOT(addFile(DFile*)));
    connect(current_project, SIGNAL(saveFileMetadata(DFile*)), m_db, SLOT(saveFileMetadata(DFile*)));
    connect(current_project, SIGNAL(setFileMetadata(DFile*)), m_db, SLOT(setFileMetadata(DFile*)));

	/*** TO PROJECT ***/

	// move files to folder
    connect(corpus_browser, SIGNAL(tree_mergeIntoFolder(QList<VFileNode*>,QString&)),
            current_project, SLOT(mergeFilesIntoFolder(QList<VFileNode*>,QString&)));
	// add property to file
    connect(corpus_browser, SIGNAL(tree_tagFile(Property*,VFile*)), current_project, SLOT(tagFile(Property*,VFile*)));
	// remove property from file
    connect(corpus_browser, SIGNAL(tree_untagFile(Property*,VFile*)), current_project, SLOT(untagFile(Property*,VFile*)));
	// delete property from project
    connect(corpus_browser, SIGNAL(tree_deleteProperty(Property*)), current_project, SLOT(deleteProperty(Property*)));
	// project has been modified
    connect(corpus_browser, SIGNAL(tree_modified()), current_project, SLOT(setChanged()));
	// save file with path
    connect(this, SIGNAL(app_saveFileWithPath(DFile*,QString)), current_project,
			SLOT(saveFileWithPath(DFile*,QString)));
	// remove bookmarks
    connect(bookmark_browser, SIGNAL(removeElements(QList<IBrowserElement*>)),
            current_project, SLOT(removeBookmarks(QList<IBrowserElement*>)));

    connect(bookmark_browser, SIGNAL(elementClicked(IBrowserElement*)), metadata_widget, SLOT(displayBookmarkInfo(IBrowserElement*)));


	/*** FROM FILE BROWSER ***/
    connect(corpus_browser, SIGNAL(tree_fileSelected(DFile*)), metadata_widget, SLOT(displayFileInfo(DFile*)));
	//connect(buttonMainSearch, SIGNAL(clicked()), this, SLOT(onMainSearchClicked()));
	//connect(fileBrowser, SIGNAL(), this, SLOT(onFileBrowserDoubleClicked()));

	/*** FROM VIEWER ***/

	/*** TO VIEWER ***/
    connect(corpus_browser, SIGNAL(tree_openFile(DFile*)), viewer, SLOT(openFile(DFile*)));
    connect(bookmark_browser, SIGNAL(openDFile(DFile*,double,double)),
			viewer, SLOT(openFile(DFile*,double,double)));

	/*** FROM MAIN WINDOW ***/
    connect(this, SIGNAL(app_openProject()), action_open_project, SLOT(execute()));
    connect(Global::Praat, SIGNAL(praat_error(QString)), this, SLOT(error(QString)));

	/*** FROM TABS ***/
    connect(m_tabs_bottom, SIGNAL(reopenQuery(int)), this, SLOT(forwardQuery(int)));

}

void DolmenApp::displayImportProgressBar(int count)
{
	QString txt = tr("Loading files...");
    import_progress = new QProgressDialog(txt, QString(), 0, count, this);
    import_progress->setWindowModality(Qt::WindowModal);
}

void DolmenApp::updateImportProgressBar(int count)
{
    import_progress->setValue(count);

    if (count == import_progress->maximum())
	{
        import_progress->close();
        delete import_progress;
        import_progress = NULL;
	}
}

void DolmenApp::warnUser(QString &msg)
{
	QMessageBox::critical(this, tr("Error"), msg, QMessageBox::Ok, QMessageBox::NoButton);
}

void DolmenApp::askPath(DFile *f)
{
    QString path = nat(QFileDialog::getSaveFileName(this, tr("Save file as..."), f->path()));
	QString ext = QFileInfo(path).suffix().toLower();

	if (path != "")
	{
        if (isInstance(f, Annotation))
		{
            Annotation *annot = qobject_cast<Annotation*>(f);

            if (annot->isNative() && ! ext.endsWith(DM_FILE_EXT))
				path += DM_DOT_FILE_EXT;
		}
		else if (isInstance(f, Sound) && ! isValidSoundExt(ext))
            dm_debug("TODO: add file extension for sound files");

		emit app_saveFileWithPath(f, path);
	}
}

void DolmenApp::compareSpeakers()
{
	GaugeDialog *dlg = new GaugeDialog(this);
	connect(dlg, SIGNAL(compareSpeakers(QString,int)), this, SLOT(openGaugeView(QString,int)));
}


void DolmenApp::openGaugeView(const QString &task, int tier_no)
{
    GaugeView *view = new GaugeView(viewer, current_project, task, tier_no);
	viewer->setCurrentWidget(view);
}

void DolmenApp::openAnnotationMatch(Annotation *a, int tier, double tstart, double tend)
{
	Q_UNUSED(tier);
	AnnotationView *view = new AnnotationView(viewer, a, tstart, tend);
	viewer->setCurrentWidget(view);
}

void DolmenApp::enableProjectActions()
{
    action_open_query->setEnabled(true);
    action_save_project->setEnabled(true);
    action_save_project_as->setEnabled(true);
    action_compare_speakers->setEnabled(true);
    action_import_meta->setEnabled(true);
}

void DolmenApp::output(const QString &text)
{
    if (Global::Output) {
        Global::Output->showMessage(text, 3000);
    }
}

void DolmenApp::error(const QString &text)
{
    if (Global::Output) {
        Global::Output->showMessage(text, 6000);
    }
}

void DolmenApp::openDocumentation()
{
    viewer->displayHelp("");
}

void DolmenApp::onOpenProject()
{
    action_open_project->execute();
}

void DolmenApp::onAddFiles()
{
    action_add_file->execute();
}

void DolmenApp::displaySupportedAudioFormats()
{
	SF_FORMAT_INFO	formatInfo;
	int             k, count;
    QStringList     formats, temp;
    QString         text, sf_version;
    char            buffer [128] ;

    sf_command (NULL, SFC_GET_LIB_VERSION, buffer, sizeof(buffer)) ;
	sf_command(NULL, SFC_GET_SIMPLE_FORMAT_COUNT, &count, sizeof(int));

    sf_version = QString(buffer);
    temp = sf_version.split('-');
    sf_version = temp[1];

	for (k = 0; k < count; k++)
	{
		formatInfo.format = k;
		sf_command(NULL, SFC_GET_SIMPLE_FORMAT, &formatInfo, sizeof(formatInfo));
		formats << formatInfo.name;
	}

    text = "Support sound formats on this platform: \n\n" + formats.join(", ");
    text += "\n\nlibsndfile version: " + sf_version;
    text += "\nRtAudio version: " + QString(RtAudio::getVersion().c_str());

	QMessageBox box(this);
    box.setText(tr("Sound information"));
    box.setInformativeText(text);
	box.exec();
}

void DolmenApp::about()
{
	QMessageBox box(this);
    box.setText(tr("<p><strong>Dolmen %1</strong></p>").arg(m_version));
    box.setInformativeText(QString::fromUtf8("<p>Copyright © 2010-2017 Julien Eychenne</p>"
											 "<p>Dolmen is distributed under the GNU General Public License (GPL). "
											 "See \"License\" for details.</p>"
											 "<p>Build date: ") +
						   Global::AppBuildDate.toString("dd.MM.yyyy") + QString(".</p>"));
	box.exec();
}

void DolmenApp::license()
{
    viewer->displayHelp("/about/license");
}

void DolmenApp::acknowledgements()
{
    viewer->displayHelp("/about/acknowledgements");
}

void DolmenApp::closeCurrentView()
{
	viewer->closeTab(viewer->currentIndex());
}

void DolmenApp::cleanUpProcess()
{
	disconnect(process, SIGNAL(finished(int)), this, SLOT(cleanUpProcess()));
	process->close();
}

DolmenApp::~DolmenApp()
{

}

