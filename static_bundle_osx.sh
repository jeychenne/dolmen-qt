#!/bin/bash

cd /Users/julien/Devel/build-dolmen-Qt_4_8_3-Release
cp -rf ~/Devel/dolmen/qt_menu.nib Dolmen.app/Contents/Resources/

# otool -L to check the dependencies of the executable
# change dynamic libraries

#install_name_tool -change /opt/local/lib/libz.1.dylib \
#	/usr/lib/libz.1.dylib Dolmen.app/Contents/MacOS/Dolmen



echo "done"
