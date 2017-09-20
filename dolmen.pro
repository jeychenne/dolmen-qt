#-------------------------------------------------
#
# Project created by QtCreator 2011-02-13T13:35:41
#
#-------------------------------------------------

QT += core gui sql svg widgets # webkitwidgets

#QMAKE_CXXFLAGS += -std=c++14
CONFIG += c++14

TEMPLATE = app
DEFINES += USE_QT5

# Enable C++ mode and safety checks for Lua in sol. The C++ mode ensures that Lua errors are propagated as C++ exceptions.
DEFINES += SOL_CHECK_ARGUMENTS SOL_SAFE_USERTYPE SOL_SAFE_FUNCTIONS SOL_USING_CXX_LUA

INCLUDEPATH += extra/onigmo extra/onigmo/enc/unicode extra/lua

macx {
    #Qt 5 stuff
    QMAKE_MACOSX_DEPLOYMENT_TARGET=10.7
    QMAKE_MAC_SDK=macosx10.9

	TARGET = Dolmen

	#NOTE: /System/Library/CoreServices/CoreTypes.bundle/Contents/Resources (system icons) on Leopard+
	ICON = icons/dolmen.icns

	LIBS += -framework CoreAudio -framework CoreServices

	INCLUDEPATH += /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/AE.framework/Versions/A/Headers \
		/System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/CarbonCore.framework/Versions/A/Headers

        INCLUDEPATH += /usr/local/include /opt/local/include /usr/local/include/qwt
	LIBS += -L/opt/local/lib #-lsndfile

        DEFINES += __MACOSX_CORE__ macintosh useCarbon=1
	# DEFINES += QWT_DLL
	# static link
        LIBS += /opt/local/lib/libspeexdsp.a /opt/local/lib/libogg.a /opt/local/lib/libvorbisenc.a /opt/local/lib/libvorbis.a /opt/local/lib/libsndfile.a
        LIBS += -F/usr/local/qwt-6.1.0/lib -framework qwt /opt/local/lib/libz.a -L/usr/local/lib -lFLAC
	QTPLUGIN += qsqlite

} else {
    TARGET = dolmen
#LIBS += -lspeexdsp
}


win32-g++|win32-msvc2017 {

#LIBS += -LC:/Qt/Qt5.6.0/5.6/mingw49_32/plugins/sqldrivers -lqsqlite

CONFIG += exceptions
INCLUDEPATH += C:/Lib/include C:/Lib/libsndfile/include C:/Lib/rtaudio/include C:/Lib/libspeex/include
LIBS += -lws2_32 -lwinmm -lole32 -LC:/Lib/libsndfile/lib -llibsndfile-1 -ldsound -LC:/Lib/libspeex/lib -llibspeexdsp -LC:/Lib/qwt -lqwt
LIBS += -LC:/Lib/zlib/lib -lz.dll
DEFINES += __WINDOWS_DS__ WINDOWS
#DEFINES += QWT_DLL
}

win32-msvc2010 {
LIBS += -luser32 -lgdi32
}

win32-g++ {
LIBS += -lgomp
}



linux-g++|linux-g++-64 {

ICON = icons/sound_wave.png

DEFINES += __LINUX_ALSA__ UNIX

INCLUDEPATH += /usr/include/gtk-2.0 /usr/include/gio-unix-2.0/

LIBS += -lpthread -lasound -lsndfile -lspeexdsp -lz -lqwt-qt5

# needed for sendpraat
CONFIG += link_pkgconfig

PKGCONFIG = gtk+-2.0
#--cflags --libs
#DEFINES += HAVE_GETTIMEOFDAY -D__LINUX_ALSA__=1
LIBS += -lgomp
}


linux-g++-64 {

LIBS += -L/usr/lib/x86_64-linux-gnu/qt5/plugins/sqldrivers -lqsqlite

}



SOURCES += main.cpp\
    DolmenApp.cpp \
    gui/Viewer.cpp \
    gui/View.cpp \
    gui/PropertyEditor.cpp \
    gui/StartView.cpp \
    gui/splitters.cpp \
    gui/SoundView.cpp \
    gui/SearchWindow.cpp \
    gui/QueryView.cpp \
    gui/ProjectView.cpp \
    gui/NumericValueEdit.cpp \
    gui/MainMenu.cpp \
    gui/HtmlItemDelegate.cpp \
	gui/DMLineEdit.cpp \
    gui/ContextMenu.cpp \
    gui/CheckList.cpp \
    gui/BottomPane.cpp \
    gui/AnnotationView.cpp \
    gui/actions.cpp \
    gui/blocks/ChangelogBlock.cpp \
    lib/VirtualFolder.cpp \
    lib/VirtualFile.cpp \
    lib/VFolder.cpp \
    lib/VFileNode.cpp \
    lib/VFile.cpp \
    lib/Tier.cpp \
    lib/Span.cpp \
    lib/Sound.cpp \
    lib/Project.cpp \
    lib/praat.cpp \
    lib/Point.cpp \
    lib/metadata.cpp \
    lib/Item.cpp \
    lib/DFile.cpp \
    lib/Annotation.cpp \
    search/SearchNode.cpp \
    search/QueryParser.cpp \
    search/SearchMatch.cpp \
    search/Query.cpp \
    utils/routines.cpp \
	utils/helpers.cpp \
    gui/VersionEditor.cpp \
    gui/AuthorEditor.cpp \
    lib/Graph.cpp \
    gui/PreferenceEditor.cpp \
    gui/blocks/SignalBlock.cpp \
    gui/CurveData.cpp \
    gui/GaugeView.cpp \
    gui/GaugeDialog.cpp \
    lib/Bookmark.cpp \
    gui/BookmarkBrowser.cpp \
    gui/BookmarkEditor.cpp \
	audio/RtAudio.cpp \
    audio/soundio.cpp \
    gui/SignalPlot.cpp \
    gui/blocks/TierBlock.cpp \
	lib/DMSelection.cpp \
	gui/DMRubberBand.cpp \
    gui/MetadataEditor.cpp \
    audio/SoundPlayer.cpp \
    gui/TierFrame.cpp \
    gui/SoundScrollBar.cpp \
    gui/ItemTextEdit.cpp \
    gui/ItemBoundary.cpp \
    gui/ItemView.cpp \
    gui/TierView.cpp \
    gui/PointView.cpp \
    gui/SpanView.cpp \
	extra/json.cpp \
    lib/JsonObject.cpp \
	lib/Plugin.cpp \
    lib/Document.cpp \
    lib/SqlDatabase.cpp \
    gui/MetadataSidebar.cpp \
    gui/FileSidebar.cpp \
    gui/PropertyTable.cpp \
    lib/Grammar.cpp \
    gui/FieldBox.cpp \
	extra/qxtcheckcombobox.cpp \
	gui/SearchGrammarTab.cpp \
	extra/quazip/zip.c \
	extra/quazip/unzip.c \
	extra/quazip/quazipnewinfo.cpp \
	extra/quazip/quazipfile.cpp \
	extra/quazip/quazip.cpp \
	extra/quazip/quacrc32.cpp \
	extra/quazip/quaadler32.cpp \
	extra/quazip/qioapi.cpp \
	extra/quazip/JlCompress.cpp \
	extra/qconsole/qconsole.cpp \
	extra/lua/lapi.cpp \
	extra/lua/lauxlib.cpp \
	extra/lua/lbaselib.cpp \
	extra/lua/lbitlib.cpp \
	extra/lua/lcode.cpp \
	extra/lua/lcorolib.cpp \
	extra/lua/lctype.cpp \
	extra/lua/ldblib.cpp \
	extra/lua/ldebug.cpp \
	extra/lua/ldo.cpp \
	extra/lua/ldump.cpp \
	extra/lua/lfunc.cpp \
	extra/lua/lgc.cpp \
	extra/lua/linit.cpp \
	extra/lua/liolib.cpp \
	extra/lua/llex.cpp \
	extra/lua/lmathlib.cpp \
	extra/lua/lmem.cpp \
	extra/lua/loadlib.cpp \
	extra/lua/lobject.cpp \
	extra/lua/lopcodes.cpp \
	extra/lua/loslib.cpp \
	extra/lua/lparser.cpp \
	extra/lua/lstate.cpp \
	extra/lua/lstring.cpp \
	extra/lua/lstrlib.cpp \
	extra/lua/ltable.cpp \
	extra/lua/ltablib.cpp \
	extra/lua/ltm.cpp \
	extra/lua/lua.cpp \
	extra/lua/lundump.cpp \
	extra/lua/lutf8lib.cpp \
	extra/lua/lvm.cpp \
	extra/lua/lzio.cpp \
	extra/sendpraat.c\
	extra/string_view.hpp \
	extra/boyer-moore-horspool/horspool.cpp \
	extra/utf8proc/utf8proc.c \
	extra/onigmo/regparse.c \
	extra/onigmo/reggnu.c \
	extra/onigmo/regposerr.c \
	extra/onigmo/regexec.c \
	extra/onigmo/regcomp.c \
	extra/onigmo/regext.c \
	extra/onigmo/regsyntax.c \
	extra/onigmo/regerror.c \
	extra/onigmo/regtrav.c \
	extra/onigmo/st.c \
	extra/onigmo/enc/iso_8859_16.c \
	extra/onigmo/enc/euc_jp.c \
	extra/onigmo/enc/unicode.c \
	extra/onigmo/enc/euc_kr.c \
	extra/onigmo/enc/iso_8859_5.c \
	extra/onigmo/enc/utf_16le.c \
	extra/onigmo/enc/iso_8859_2.c \
	extra/onigmo/enc/windows_1254.c \
	extra/onigmo/enc/gb18030.c \
	extra/onigmo/enc/utf_32be.c \
	extra/onigmo/enc/utf_16be.c \
	extra/onigmo/enc/cp949.c \
	extra/onigmo/enc/windows_1253.c \
	extra/onigmo/enc/iso_8859_9.c \
	extra/onigmo/enc/iso_8859_6.c \
	extra/onigmo/enc/windows_1252.c \
	extra/onigmo/enc/windows_1257.c \
	extra/onigmo/enc/iso_8859_11.c \
	extra/onigmo/enc/gbk.c \
	extra/onigmo/enc/koi8_r.c \
	extra/onigmo/enc/iso_8859_4.c \
	extra/onigmo/enc/iso_8859_15.c \
	extra/onigmo/enc/big5.c \
	extra/onigmo/enc/ascii.c \
	extra/onigmo/enc/iso_8859_3.c \
	extra/onigmo/enc/utf_32le.c \
	extra/onigmo/enc/iso_8859_8.c \
	extra/onigmo/enc/emacs_mule.c \
	extra/onigmo/enc/windows_31j.c \
	extra/onigmo/enc/utf_8.c \
	extra/onigmo/enc/us_ascii.c \
	extra/onigmo/enc/iso_8859_1.c \
	extra/onigmo/enc/iso_8859_10.c \
	extra/onigmo/enc/windows_1251.c \
	extra/onigmo/enc/iso_8859_13.c \
	extra/onigmo/enc/iso_8859_7.c \
	extra/onigmo/enc/koi8_u.c \
	extra/onigmo/enc/iso_8859_14.c \
	extra/onigmo/enc/euc_tw.c \
	extra/onigmo/enc/windows_1250.c \
	extra/onigmo/enc/shift_jis.c \
	extra/onigmo/regposix.c \
	extra/onigmo/regversion.c \
	extra/onigmo/regenc.c \
    lib/ZipArchive.cpp \
    lib/TableModel.cpp \
    lib/DataTable.cpp \
    lib/TableColumn.cpp \
    gui/TableWidget.cpp \
    gui/TableView.cpp \
    gui/FieldDialog.cpp \
    gui/AnnotationCreator.cpp \
    gui/CorpusBrowser.cpp \
    gui/LuaConsole.cpp \
	utils/print.cpp \
	runtime/scripting.cpp \
	runtime/string.cpp \
	runtime/unicode_rt.cpp \
	runtime/regex.cpp \
    gui/ElementBrowser.cpp \
    lib/BrowserElement.cpp \
	extra/utf8proc/utf8proc_data.c \
	runtime/PluginHandler.cpp \
    lib/Script.cpp \
    gui/ScriptBrowser.cpp \
    gui/ScriptView.cpp


HEADERS  += \
    DolmenApp.h \
    gui/Viewer.h \
    gui/View.h \
    gui/PropertyEditor.h \
    gui/StartView.h \
    gui/splitters.h \
    gui/SoundView.h \
    gui/SearchWindow.h \
    gui/QueryView.h \
    gui/ProjectView.h \
    gui/NumericValueEdit.h \
    gui/MainMenu.h \
    gui/HtmlItemDelegate.h \
	gui/DMLineEdit.h \
    gui/ContextMenu.h \
    gui/CheckList.h \
    gui/BottomPane.h \
    gui/AnnotationView.h \
    gui/actions.h \
    gui/blocks/ChangelogBlock.h \
    lib/VirtualFolder.h \
    lib/VirtualFile.h \
    lib/VFolder.h \
    lib/VFileNode.h \
    lib/VFile.h \
    lib/Tier.h \
    lib/Span.h \
    lib/Sound.h \
    lib/Project.h \
    lib/praat.h \
    lib/Point.h \
    lib/metadata.h \
    lib/Item.h \
    lib/DFile.h \
    lib/Annotation.h \
    search/SearchNode.h \
    search/QueryParser.h \
    search/SearchMatch.h \
    search/Query.h \
    utils/routines.h \
    utils/Global.h \
	utils/helpers.h \
    dolmen.h \
    gui/VersionEditor.h \
    gui/AuthorEditor.h \
	extra/sendpraat.h \
    lib/Graph.h \
    gui/PreferenceEditor.h \
    gui/blocks/SignalBlock.h \
    gui/CurveData.h \
    gui/GaugeView.h \
    gui/GaugeDialog.h \
    lib/Bookmark.h \
    gui/BookmarkBrowser.h \
    gui/BookmarkEditor.h \
	audio/RtAudio.h \
    audio/soundio.h \
    gui/SignalPlot.h \
    gui/blocks/TierBlock.h \
	lib/DMSelection.h \
    gui/DMRubberBand.h \
    gui/MetadataEditor.h \
    audio/SoundPlayer.h \
    gui/TierFrame.h \
    gui/SoundScrollBar.h \
    gui/ItemTextEdit.h \
    utils/version.h \
    gui/ItemBoundary.h \
    gui/ItemView.h \
    gui/TierView.h \
    gui/PointView.h \
    gui/SpanView.h \
	extra/json.h \
    lib/JsonObject.h \
    lib/Plugin.h \
    lib/Document.h \
    lib/SqlDatabase.h \
    gui/MetadataSidebar.h \
    gui/FileSidebar.h \
    gui/PropertyTable.h \
    lib/Grammar.h \
    gui/FieldBox.h \
	extra/qxtnamespace.h \
	extra/qxtglobal.h \
	extra/qxtcheckcombobox.h \
	extra/qxtcheckcombobox_p.h \
    gui/SearchGrammarTab.h \
	extra/quazip/zip.h \
	extra/quazip/unzip.h \
	extra/quazip/quazipnewinfo.h \
	extra/quazip/quazipfileinfo.h \
	extra/quazip/quazipfile.h \
	extra/quazip/quazip.h \
	extra/quazip/quazip_global.h \
	extra/quazip/quacrc32.h \
	extra/quazip/quachecksum32.h \
	extra/quazip/quaadler32.h \
	extra/quazip/JlCompress.h \
	extra/quazip/ioapi.h \
	extra/quazip/crypt.h \
	extra/qconsole/qconsole.h \
	extra/lua/lapi.h \
	extra/lua/lauxlib.h \
	extra/lua/lcode.h \
	extra/lua/lctype.h \
	extra/lua/ldebug.h \
	extra/lua/ldo.h \
	extra/lua/lfunc.h \
	extra/lua/lgc.h \
	extra/lua/llex.h \
	extra/lua/llimits.h \
	extra/lua/lmem.h \
	extra/lua/lobject.h \
	extra/lua/lopcodes.h \
	extra/lua/lparser.h \
	extra/lua/lprefix.h \
	extra/lua/lstate.h \
	extra/lua/lstring.h \
	extra/lua/ltable.h \
	extra/lua/ltm.h \
	extra/lua/luaconf.h \
	extra/lua/lua.h \
	extra/lua/lualib.h \
	extra/lua/lundump.h \
	extra/lua/lvm.h \
	extra/lua/lzio.h \
	extra/sol.hpp \
	extra/utf8.hpp \
	extra/string_view.hpp \
	extra/utf8proc/utf8proc.h \
	extra/boyer-moore-horspool/horspool.hpp \
	extra/onigmo/regint.h \
	extra/onigmo/onigmognu.h \
	extra/onigmo/regparse.h \
	extra/onigmo/regenc.h \
	extra/onigmo/config.h \
	extra/onigmo/onigmo.h \
	extra/onigmo/enc/shift_jis.h \
	extra/onigmo/enc/jis/props.h \
	extra/onigmo/enc/unicode/casefold.h \
	extra/onigmo/enc/unicode/name2ctype.h \
	extra/onigmo/enc/iso_8859.h \
	extra/onigmo/st.h \
	extra/onigmo/onigmoposix.h \
    lib/ZipArchive.h \
    lib/TableModel.h \
    lib/DataTable.h \
    lib/TableColumn.h \
    gui/TableWidget.h \
    gui/TableView.h \
    gui/FieldDialog.h \
    gui/AnnotationCreator.h \
    gui/CorpusBrowser.h \
    gui/LuaConsole.h \
	utils/print.hpp \
	utils/ref_count.hpp \
	runtime/scripting.h \
	runtime/string.hpp \
	runtime/unicode.hpp \
	runtime/error.hpp \
	runtime/regex.hpp \
    gui/ElementBrowser.h \
    lib/BrowserElement.h \
    extra/lua/lua.hpp \
    extra/onigmo/win32/config.h \
    extra/onigmo/config_linux.h \
    extra/quazip/zconf.h \
    extra/quazip/zlib.h \
	runtime/PluginHandler.h \
	include/load_plugins_lua.hpp \
	include/signal_lua.hpp \
    lib/Script.h \
    gui/ScriptBrowser.h \
    gui/ScriptView.h

RESOURCES += \
    icons.qrc

RC_FILE = dolmen.rc


