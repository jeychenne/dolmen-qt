## Changelog for Dolmen


### Version 1.9.2 (XX/9/2017)

* Updated RTAudio to version 5.0.0
* The last query is now remembered
* Small performance improvements in queries
* Fixed regression for search grammars introduced in 1.9.1
* Renamed `Conc` menu to `Search`

### Version 1.9.1 (14/9/2017)

* Complex queries now recognizes 3 relations: alignment, precedence and dominance
* Updated documentation for concordancing
* Better warning when a query doesn't return any match

### Version 1.9.0 (25/8/2017)

* Small changes in the user interface
* lua scripting engine
* New regular expression engine
* New documentation
* Removed obsolete UNIX pattern search syntax
* Fixed freeze with open/save dialogs on Linux (Qt5)
* Added Conc menu
* Auto-detection of Unicode encoding (defaults to UTF-8 if no BOM is present)
* Metadata panel can now be resized
* Removed greedy search check box (use quantifiers in regular expressions instead)
* Bookmarks can now be opened in Praat
* Fixed bug which caused the content of an annotation to be loaded multiple times when there were bookmarks in a project
* Fixed crash which occurred when switching between projects
* Fixed minor memory leak (finalization of grammars)
* License is now GPL version 3, with an exception for plugins
* New icon theme (courtesy [icons8.com](https://icons8.com))
* Replaced output tab with a (hideable) status bar

### Version 1.3.3 (24/5/2016)
* fixed encoding when exporting search results to CSV
* minor bug fixes


### Version 1.3.2 (8/5/2016)
* minor bug fixes


### Version 1.3.1 (28/04/2016)
* query results are now sorted


### Version 1.3 (12/9/2014)
* fixed bug in sound playing on Windows and Linux
* import metadata from CSV file
* fixed a bug introduced in version 1.2 that made Dolmen crash when loading a project that contains bookmarks
* now built on top of Qt5
* better compatibility with recent version of Mac OS X (now requires 10.7 or later)
* plugin tabs are no longer displayed in random order
* updated RtAudio to version 4.1.1
* minor fixes and improvements


### Version 1.2 (1/9/2013)
* improvements to sound and annotation views
* sound can be browsed with keyboard arrows in sound and annotation views
* faster loading of projects (annotations are now opened lazily)
* updated RtAudio to version 4.0.12
* RtAudio and libsndfile versions are now displayed in Help > Sound information
* the source code can now be compiled with Qt4 or Qt5
* search by tier name now uses regular expressions instead of string comparison
* search grammars can now define a "tier selecting" field to search a pattern across different tiers
* fixed compiler warnings
* minor fixes and enhancements


### Version 1.1 (1/7/2013)
* updated code to use Qwt 6.1 to prepare the transition to Qt 5 (Qwt is no longer included and must be provided externally)
* sound can now be played/paused with the space bar and stopped with Esc in sound and annotation views
* plots are now antialiased by default (this can be turned off in the preference editor)
* minor code clean-up's
* new icon


### Version 1.0.4 (21/02/2013)
* Windows only: added msvcp100.dll, for systems that don’t have it.


### Version 1.0.3 (16/02/2013)
* prevent Dolmen from crashing when displaying a short sound file in a view


### Version 1.0.2 (15/02/2013)
* Fixes audio playback of stereo files on Windows and Linux


### Version 1.0.1 (16/12/2012)
* fixed a bug that happened when opening intervals shorter than 100 ms in Praat
* the tier name fied is now properly hidden in the search window when the files are Documents


### Version 1.0 (15/12/2012)
* annotator comparison for search grammars (requires an "Annotator" property)
* Search by tier name instead of tier number
* code modified so that it compiles with MSVC2010
* updated libsndfile to 1.0.25 on Windows


### Version 0.9.9 (21/10/2012)

* Qwt is no longer required as an external dependency (v5.2.2 has been included in Dolmen's source code)
* updated Qt to v 4.8.3 on Windows 


### Version 0.9.8 (18/10/2012)

* improved sound playback on Mac OS X and Linux (updated RTAudio to v 4.0.11)
* fixed selection rectangle in waveforms on Linux
* updated Qt to v 4.8.3 on Mac OS X
* refactoring of the output console


### Version 0.9.7.2 (04/05/2012)

* fixed a deployment bug on Windows (SQL plugin)
* error message if the database fails to open


### Version 0.9.7 (23/04/2012)

* data tables (DMT files). Data tables can be created from a query view (right click on a query match > Create table view)
* option for greedy search in the main search window
* minor improvements and bug fixes


### Version 0.9.6 (14/04/2012)

* support for user-defined plugins (written in JavaScript/JSON)
* cleaner check list boxes (mostly used in search window and gauge tool)
* disabled internal debug messages


### Version 0.9.5 (08/04/2012)

This version brings a significant number of changes and improvements:
* metadata is now displayed in a permanent widget on the right-hand side.
* metadata is now stored in an SQLite database instead of DMM files
* tags have been renamed to "properties".
* redesigned property editor which makes editing properties much simpler
* on Mac, the application data folder has been moved from “~/Library” to “~/Application Support”
* resampling now uses speex instead of libsamplerate
* support for Document files (plain text files) along with Annotations
* bug fixes and usability improvements


### Version 0.9.4 (03/03/2012)

* prevent a crash when using the arrow keys in a query view
* tags are now properly refreshed in the metadata editor


### Version 0.9.3 (02/03/2012)

* fixes bugs in regular expression and plain text search
* prevent Dolmen from crashing when playing a new search match while the previous one hasn’t finished playing
* search results can now be browsed using keyboard arrows


### Version 0.9.2 (28/01/2012)

* new version (mac only): fixes a crash when playing a sound 
* updated RTAudio to 4.0.10
* the Mac version is now linked against Qt 4.8


### Version 0.9.1 (21/08/2011)
		
* fixed a bug that made Dolmen crash when loading a new project if the previous one had bookmarks
* added icons for surveys and speakers in the file browser
* items can now be edited from query views (right click, then "Edit item text...")
* the Linux version is now distributed as Debian packages (Debian stable)
* minor bug fixes and improvements


### Version 0.9.0 (19/08/2011)
				
* implementation of tiers in annotation views (can read and edit text of Praat tiers) 
* much smoother scrolling of waveform
* waveforms can now be scrolled with the mouse wheel
* new metadata editor, available from annotation and sound views, as well as from the file browser (right click on an item)
* new shortcuts to hide the left sidebar (ctrl+shift+L) and the bottom tabs (ctrl+shift+b)
* updated support for Praat point tiers (if you have old files with points, it is best to open and resave them in Praat)
* minor improvements to the side bar on Mac OS X


### Version 0.8.3 (08/08/2011)
			
* Sound views now indicate the current position while a file is playing
* Removed Webkit dependency
* The Mac version is now built statically and is much smaller as a result


### Version 0.8.2 (06/08/2011)

* Sound can now be paused in sound views
* Improved audio output of mono files on Mac OS X
			
		
### Version 0.8.1 (23/07/2011)		
		
* Fixed a bug that caused Dolmen to crash on Mac and Linux when playing a sound file containing non-ASCII characters in its path
* Fixed a bug that caused Dolmen to freeze on Windows when interrupting playing
		
		
### Version 0.8.0	(16/07/2011)
		
* the sound stack has been rewritten and now uses libsndfile and RTAudio instead of libsox. 
* initial support for waveforms in sound and annotation views (mono files)
* new tool "Compare speakers or subjects..."
* export to CSV (spreadsheet format)
* bookmarks
* open files in Praat from the file browser
* new command "save project as..." 
* the system tab "metadata" has been removed (metadata is now shown in tool tips in the file browser or in the viewer when opening a file)
* simplified search window
		
			
### Version 0.7.2 (20/03/2011)
		
* fixed display of results in query view when hovering the mouse cursor over items
* better Linux support

	
### Version 0.7.1 (05/03/2011)
                   
* read WaveSurfer label files (*.lab)
* export tiers of DMF files to TextGrid
* TextGrid files that contain text fields spanning over several lines are now properly parsed 
            
   
### Version 0.7.0 (04/03/2011)
            
This version brings many improvements across the board:

* sendpraat and sox have been included in the source code and are no longer neeeded as external dependencies
* cross-tier search
* the preference editor has been simplified and reorganized
* HTML rendering now uses the WebKit engine
* the documentation has been overhauled and is now available in HTML from within Dolmen, instead of a PDF file. 
* many more sound file formats are supported out of the box (still no MP3 nor OGG though)
* the most recent project can now be opened with ctrl+shift+o
* minor bug fixes


### Version 0.6.5 (19/02/2011)
            
Features:           
* added project versioning (Edit > Project Properties...)
* can now open TextGrid without sound in Praat
* added search variables %LINE, %WORD, and symbols &apos;#&apos; (word boundary), &apos;#*&apos; (prefix) and &apos;*#&apos; (suffix).
* documentation for regular expressions

Bug fixes/improvements:
* removed "Open selection in Praat" from Tools menu (which has been disabled for now)
* parentheses can now be used in search pattern (they still need to be escaped in regular expressions)
* fixed identification of the tier when opening in Praat


### Version 0.6.4 (12/02/2011)          
* The description field is now searchable in the search window.
* Tags can be hidden in the search window
* PFC/PAC goodies (see documentation)	           
            

### Version 0.6.3 (09/02/2011)
* First public version.


Development of the C++ version started in early August 2010. A python proof-of-concept was sketched out in April/May 2010.
Dolmen is a complete redesign of the PFC platform (2006/2008), a concordancer implemented in Python and specifically written for the PFC project (www.projet-pfc.net). 
