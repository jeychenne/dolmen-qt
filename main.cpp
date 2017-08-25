#include <iostream>
#include <QApplication>
#include <QSettings>
#include <QLabel>
#include <QPushButton>
#include <QFileInfo>
#include <QAction>
#ifdef Q_OS_MAC
    #include <QProcess>
#endif
#include "DolmenApp.h"
#include "lib/metadata.h"
#include "utils/version.h"
#include "utils/Global.h"
#include "utils/routines.h"

using namespace std;


// Global variables (stored in Global.h)

QHash<QString, metadata::Property*> metadata::Property::properties     = QHash<QString, metadata::Property*>();
QMap<int, VFileNode*>  Global::IdMap                   = QMap<int, VFileNode*>();
QHash<QString, DFile*> Global::FileHash                = QHash<QString, DFile*>();
QString                Global::DefaultSearchStyle      = DM_SEARCH_STYLE_PERL;
QStringList            Global::RecentProjects          = QStringList();
QStringList            Global::AudioFormats            = QStringList();
QStringList            Global::CommonAudioFormats      = QStringList();
int                    Global::MatchCotextLength       = MATCH_COTEXT_LENGTH;
QString                Global::CurrentProjectDir;
QString                Global::CurrentProjectPath;
QString                Global::ApplicationMode         = "Default";
QString                Global::AppVersionString        = DM_APP_VERSION;
QDate                  Global::AppBuildDate            = DM_APP_DATE;
QStatusBar*            Global::Output                  = nullptr;
/* These variables are fully initialized later on */
bool                   Global::PraatIsInstalled        = false;
bool                   Global::UseAntialiasingInPlots  = true;
bool                   Global::EnableStatusBar         = true;
QString                Global::AppDataDir              = dm_home();
QString                Global::AppRootDir;
QString                Global::MacResourcesDir;
QString                Global::PraatPath;
QString                Global::ResourcesPath;
QString                Global::PraatPrefsDir           = dm_home();
const char*            Global::AudioDriver             = "";
PraatInstance*         Global::Praat                   = nullptr;
#ifdef Q_OS_MAC
bool                   Global::Maximize                = false;
#endif


// These paths may be overriden; in this case, their value should normally be stored in the settings.
void setDefaultPaths(char *argv0)
{
	QFileInfo app(argv0);

	if (! Global::AppDataDir.endsWith(QDir::separator()))
	{
		Global::AppDataDir    += QDir::separator();
		Global::PraatPrefsDir += QDir::separator();
	}

#ifdef Q_OS_WIN
	Global::AppRootDir       = QDir::toNativeSeparators(app.dir().path());
	Global::PraatPath        = Global::AppRootDir + "\\Tools\\praat.exe";
	Global::PraatPrefsDir   += "\\Praat";
	Global::AppDataDir      += "Application Data\\Dolmen";
	Global::AudioDriver      = "waveaudio";

#elif defined(Q_OS_MAC)
	// the full path should be "/Application/Dolmen.app/Contents/MacOS/Dolmen"
	// chop subfolders
	QFileInfo bundle(app.dir().path());
	bundle = QFileInfo(bundle.dir().path());

	Global::PraatPrefsDir   += "/Library/Preferences/Praat Prefs";
	Global::MacResourcesDir  = bundle.path() + "/Contents/Resources";
	Global::PraatPath        = "/Applications/Praat.app";
	Global::AppRootDir       = bundle.dir().path(); // chop bundle name
    Global::AppDataDir      += "Library/Application Support/Dolmen";
    Global::AudioDriver      = (char*) "coreaudio";

#else
	Global::AppRootDir       = app.dir().path();
    Global::PraatPath        = "/usr/bin/praat";
	Global::PraatPrefsDir   += "/.praat-dir";
	Global::AppDataDir      += ".config/Dolmen";
    Global::AudioDriver      = "alsa";
#endif
}



int main(int argc, char *argv[])
{
	QString projectPath = "";

	if (argc > 2 && (strcmp(argv[1], "-p") == 0 || strcmp(argv[1], "--project") == 0))
	{
		projectPath = QString(argv[2]);
	}

	if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))
	{
		cout << "Usage: dolmen OPTION [PATH]" << endl;
		cout << "\t-h, --help\t displays this message" << endl;
		cout << "\t-v, --version\t displays version" << endl;
		cout << "\t-p, --project\t loads project PATH" << endl;
		cout << "Examples:" << endl;
		cout << "\tdolmen --version" << endl;
		cout << "\tdolmen -p ~/data/my_first_project.dmpr" << endl;

	}
	else if (argc == 2 && (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0))
	{
		cout << "Dolmen version " DM_APP_VERSION << endl;
	}
	else
	{
		setDefaultPaths(argv[0]);

        // move application data directory from "~/Library" to "~/Application Support" on Mac OS X
#ifdef Q_OS_MAC
        QString oldPrefs = Global::AppDataDir;
        oldPrefs.replace("/Application Support/", "/Preferences/");
        QFileInfo oldInfo(oldPrefs);
        if (oldInfo.exists())
        {
            // check whether new folder already exists
            QFileInfo newInfo(Global::AppDataDir);
            if (newInfo.exists())
            {
                QProcess *proc = new QProcess;
                QStringList args;
                args << "-r" << Global::AppDataDir;
                proc->start("rm", args);
            }

            QStringList arguments;
            arguments << oldPrefs << Global::AppDataDir;
            //qDebug() << "mv" << oldPrefs << Global::AppDataDir;
            QProcess *proc = new QProcess;
            proc->start("mv", arguments);
        }
#endif

        QApplication app(argc, argv);
//#ifdef Q_OS_WIN
//        QString p(argv[0]);
//        p.replace("dolmen.exe", "Resources" + QString(QDir::separator()) + "plugins");
//        app.addLibraryPath(p);
//#endif

//#ifdef Q_OS_UNIX
//                // FIXME: this is the path on debian, I don't know about other distributions
//                app.addLibraryPath("/usr/lib/qt4/plugins/sqldrivers");
//#endif
		app.setApplicationVersion(DM_APP_VERSION);

		app.setApplicationName("Dolmen");
		app.setOrganizationName("Dolmen");

        // Instance of the lua engine
        sol::state lua;
        DolmenApp dolmen(lua, app.applicationVersion(), 0, projectPath);
		dolmen.show();
		return app.exec();
	}
}
