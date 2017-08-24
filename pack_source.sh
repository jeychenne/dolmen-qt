#!/bin/sh

VERSION="$1"

if [ "$1" -eq "" ]; then
  echo "usage: pack_source.sh version"
  exit 2
fi

SRC="$HOME/devel"
TEMP="$HOME/temp"

DIR="$TEMP/dolmen_$VERSION"
BZIP="dolmen_$VERSION.tar.bz2"
ZIP="dolmen_$VERSION.zip"

echo "creating temporary directory..."
cp -r "$SRC/dolmen" "$DIR"

echo "removing files..."
rm -r $DIR/build/*
rm "$DIR/Dolmen/doc/dolmen_documentation.pdf"

cd $TEMP
echo "compressing $BZIP..."
tar cjf "$BZIP" "dolmen_$VERSION"
echo "compressing $ZIP..."
zip -r "$ZIP" "dolmen_$VERSION"

echo "cleaning up..."
rm -rf $DIR
echo "done"
