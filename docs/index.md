# Dolmen: a platform for the analysis of annotated speech


The source code is available on [GitHub](https://github.com/jeychenne/dolmen).

## Overview

Dolmen is a free, open-source software toolbox for the analysis of annotated speech. It offers a user-friendly interface to manage, annotate and query language corpora. It is particularly well suited for dealing with time-aligned data. The main features it offers are: 

* Project management: organize files into projects and manage versions. 
* Extensible metadata: files can be annotated with properties, which allow you to sort and organize your data. 
* Interaction with Praat: Dolmen can read TextGrid files and open files directly in Praat. 
* Powerful search engine: build and save complex queries; search patterns across tiers. 
* Standard-based: Dolmen files are encoded in XML and Unicode. 
* Scripting engine: Dolmen can be extended with plugins written in Lua and JSON.

Dolmen runs on all major platforms (Windows, Mac OS X and GNU/Linux) and is freely available under the terms of the GNU General Public License (GPL). 
The latest version of Dolmen can be downloaded from [http://dolmen.rtfd.io](http://dolmen.rtfd.io). If you encounter any problem or bug, 
please write to jeychenne@gmail.com. 

## Download 

### Dolmen 2 (current version)

Download version 2.0.0 (30/03/2018):

This version is recommended for all users.

- Windows 7 and later: [dolmen_setup.exe](http://julieneychenne.info/files/v2/dolmen_setup.exe)
- MacOS 10.7 and later: [dolmen.dmg](http://julieneychenne.info/files/v2/dolmen.dmg)
- Linux (Ubuntu 16.04 64-bit): [dolmen-2.0.0.tar.bz2](http://julieneychenne.info/files/v2/dolmen-2.0.0-linux.tar.bz2)
- source code: available on [GitHub](https://github.com/jeychenne/dolmen/releases)


### Dolmen 1.3 (legacy version)

Dolmen 1.3 can be downloaded from [here](http://www.julieneychenne.info/dolmen/#download).

## Installation


### Windows
On Windows, Dolmen is provided as a self-contained installer file. Simply double-click on ‘dolmen_setup.exe’ and follow the instructions.

The procedure will install Dolmen in your `Program Files` directory and will create a shortcut in the start menu (and optionally on the desktop).

If you wish to be able to open files in Praat from Dolmen, you will need to install Praat in Dolmen’s installation directory, which should be either 
`C:\Program Files (x86)\Dolmen2\Tools` or `C:\Program Files\Dolmen2\Tools`, depending on your system. Alternatively, you can modify Praat’s default 
path with the preference editor.</p>

### Mac OS

On Mac OS, Dolmen is provided as a standard DMG image disk. Mount the image by double-clicking on it and drag the application `Dolmen` into your 
`Applications` folder. If you want Dolmen to be able to interact with Praat, you will need to install it in the `Applications` folder too.

Currently, only Mac OS 10.7 (Snow Leopard) and later are “officially” supported. It does not work on earlier versions.


### Linux (Debian/Ubuntu)

The official executable that is provided on the website is built on Debian 9 and is available for 64-bit architectures. 

Since the program is available as a dynamically-linked executable, first make sure that the needed dependencies are installed (asound, libsndfile, speexdsp, Qt 5 and GTK 2). Most of these packages should already be installed, but you can issue the following command in a terminal to make sure they are:

```
sudo apt-get install libasound2 libsndfile1 libspeexdsp1 libgtk2.0-0 qt5-default libqt5sql5-sqlite
```

Next, assuming that you downloaded the archive in your `Downloads` directory, type the following commands in a terminal (replacing `XX` by the appropriate version number):

```
cd opt
sudo tar xvjpf ~/Downloads/dolmen-XX-linux.tar.bz2
sudo ln -s /opt/dolmen/bin/dolmen /usr/local/bin/
```


You can now run Dolmen by simply typing `dolmen &` from a terminal window.

If you get an error about a missing SQL plugin, try to add the following line to your `.bashrc` configuration file:

```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib/x86_64-linux-gnu/qt5/plugins/sqldrivers
```

### Compiling from source

You need to install the development packages for QT 5.3 or greater (including the sqlite plugin), GTK 2, ALSA (libasound2), libspeexdsp and libsndfile. 
You also need to manually build Qwt 6.1.0 (or later). Then, assuming that you have downloaded the source for version 1.3 in your `Downloads` directory, 
you can compile it by typing the following commands in the terminal:

```
unzip dolmen-2.0.zip
cd dolmen
qmake dolmen.pro; make
```

This will create an executable file called `dolmen` that you can put anywhere. To put it in `/usr/local/bin`, do:

```
sudo mv dolmen /usr/local/bin/
```

Assuming that `sudo` is installed and properly configured on your system. You can then run Dolmen by simply typing `dolmen` in the terminal. 

In order to be able to read the documentation, you will also need to put the `html` directory somewhere on your disk, and adjust the `resources` path. To do this, go to `Edit > Preferences...` and in the `General` tab, adjust the path for the `Resources folder` to match your installation.

## Known issues

On Mac OS, clicking on the sound scrollbar buttons after an item is selected in a tier results in the scollbar moving until an edge is reached.
