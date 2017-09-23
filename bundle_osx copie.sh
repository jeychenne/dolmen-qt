#!/bin/bash

# first run macdeployqt to copy the frameworks, then run this script to fix the paths

SRCDIR="$HOME/dev/build-Release"
#LIBDIR="$HOME/Devel/dmdynlibs"
OUTDIR="$HOME/dev/build-Release"
RESOURCES="$OUTDIR/Dolmen.app/Contents/Resources"
FRAMEWORKS="$OUTDIR/Dolmen.app/Contents/Frameworks"

cd $OUTDIR


    

#cp -rf $SRCDIR/qt_menu.nib $RESOURCES/
#cp -r $LIBDIR $FRAMEWORKS

# otool -L to check the dependencies of the executable

OLDLIB="/Users/julien/Qt/5.3/clang_64/lib"
NEWLIB="@executable_path/../Frameworks"
BIN="Dolmen.app/Contents/MacOS/Dolmen"

QTWIDGETS="QtWidgets.framework/Versions/5/QtWidgets"
QTCORE="QtCore.framework/Versions/5/QtCore"
QTSVG="QtSvg.framework/Versions/5/QtSvg"
QTGUI="QtGui.framework/Versions/5/QtGui"
QTSQL="QtSql.framework/Versions/5/QtSql"
QTSCRIPT="QtScript.framework/Versions/5/QtScript"
QTPRINT="QtPrintSupport.framework/Versions/5/QtPrintSupport"
QTXML="QtXml.framework/Versions/5/QtXml"
QWT="qwt.framework/Versions/6/qwt"
QTOPENGL="QtOpenGL.framework/Versions/5/QtOpenGL"
QTCONCURRENT="QtConcurrent.framework/Versions/5/QtConcurrent"

OPT="/opt/local/lib"
FLAC="libFLAC.8.dylib"
OGG="libogg.0.dylib"

# $1 : framework
# $2 : binary
function change_name {
	install_name_tool -change $OLDLIB/$1 $NEWLIB/$1 $2
}

# $1 : framework
function change_lib {
	install_name_tool -id $NEWLIB/$1 $FRAMEWORKS/$1
}


# change framework paths
for F in $QTWIDGETS $QTCORE $QTSVG $QTGUI $QTSQL $QTSCRIPT $QWT
do
	change_name $F $BIN
done

# FLAC
cp  $OPT/$FLAC $FRAMEWORKS/
install_name_tool -change $OPT/$FLAC $NEWLIB/$FLAC $BIN

cp $OPT/$OGG $FRAMEWORKS/
install_name_tool -change $OPT/$OGG $NEWLIB/$OGG $FRAMEWORKS/$FLAC
change_lib $OGG

	
# change paths inside frameworks
change_name $QTCORE $FRAMEWORKS/$QTGUI

change_name $QTWIDGETS $FRAMEWORKS/$QTPRINT
change_name $QTGUI $FRAMEWORKS/$QTPRINT
change_name $QTCORE $FRAMEWORKS/$QTPRINT

change_name $QTCORE $FRAMEWORKS/$QTSCRIPT

change_name $QTCORE $FRAMEWORKS/$QTSQL

change_name $QTCORE $FRAMEWORKS/$QTSVG
change_name $QTGUI $FRAMEWORKS/$QTSVG
change_name $QTWIDGETS $FRAMEWORKS/$QTSVG

change_name $QTCORE $FRAMEWORKS/$QTSCRIPT

change_name $QTCORE $FRAMEWORKS/$QTWIDGETS
change_name $QTGUI $FRAMEWORKS/$QTWIDGETS

change_name $QTCORE $FRAMEWORKS/$QTXML

# QWT
install_name_tool -change $QWT $NEWLIB/$QWT $BIN

change_name $QTOPENGL $FRAMEWORKS/$QWT
change_name $QTWIDGETS $FRAMEWORKS/$QWT

change_name $QTGUI $FRAMEWORKS/$QWT
change_name $QTCORE $FRAMEWORKS/$QWT
change_name $QTSVG $FRAMEWORKS/$QWT
change_name $QTPRINT $FRAMEWORKS/$QWT
change_name $QTCONCURRENT $FRAMEWORKS/$QWT

# QTOPENGL
change_lib $QTOPENGL
change_name $QTWIDGETS $FRAMEWORKS/$QTOPENGL
change_name $QTGUI $FRAMEWORKS/$QTOPENGL
change_name $QTCORE $FRAMEWORKS/$QTOPENGL

# QTCONCURRENT
change_lib $QTCONCURRENT $FRAMEWORKS/$QTCONCURRENT
change_name $QTCORE $FRAMEWORKS/$QTCONCURRENT


	
	

echo "done"; echo
