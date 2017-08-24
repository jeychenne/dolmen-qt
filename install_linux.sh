#!/bin/bash

# last modified: 18/02/2011
# Author: Julien Eychenne

## Location where Dolmen will be installed
BINDIR="/usr/local/bin"

## Directory where the documentation is installed
## Currently hard-coded, don't touch
DOCDIR="/usr/share/doc/dolmen"

## You shouldn't touch those variables
DOCFILE="dolmen_documentation.pdf"
DOLMEN="dolmen"
SENDPRAAT="sendpraat"


# This script must be run as root
if [ "`whoami`" != "root" ]; then
	echo	
	echo "This script must be run as root". 
	echo "On Ubuntu, use the command \"sudo ./install_linux.sh\""
	echo "Otherwise, login as root with the command \"su\""
	echo "and rerun the script"
	echo	
	exit 2
fi

echo ""
if [ ! -d "$DOCDIR" ]; then
	echo "creating $DOCDIR..."
	mkdir $DOCDIR
fi

echo "copying documentation to $DOCDIR..."
cp $DOCFILE $DOCDIR/$DOCFILE

echo "installing dolmen into $BINDIR..."
cp $DOLMEN $BINDIR/$DOLMEN

echo "installing sendpraat into $BINDIR..."
cp $SENDPRAAT $BINDIR/$SENDPRAAT

# try to locate Praat
if [ "`which praat`" == "" ]; then 
	echo "WARNING: Praat is not installed or is not in your PATH." 
fi

echo "done."
echo


