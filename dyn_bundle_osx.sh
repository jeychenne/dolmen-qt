#!/bin/bash

cd ../dolmen-build-desktop
macdeployqt Dolmen.app
cp ../dolmen/icons/dolmen.icns Dolmen.app/Contents/Resources/
cp ../dolmen/info.plist Dolmen.app/Contents/
cp /usr/local/qwt-5.2.1/lib/*.dylib Dolmen.app/Contents/MacOS/
cp /opt/local/lib/lib*.dylib Dolmen.app/Contents/MacOS/

echo "done"
