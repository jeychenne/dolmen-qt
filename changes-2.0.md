
Changes
=======

* Minor changes in the user interface
* New regular expression engine based on PCRE
* Removed obsolete UNIX pattern search syntax
* Fixed freeze with open/save dialogs on Linux (Qt5)
* Added Conc menu
* Removed `find` button (use the Conc menu instead)
* Auto-detection of Unicode encoding (defaults to UTF-8 if no BOM is present)
* Metadata panel can now be resized
* Removed greedy search check box (use quantifiers in regular expressions instead)
* Bookmarks can now be opened in Praat
* Fixed bug which caused the content of an annotation to be loaded multiple times when there were bookmarks in a project
* Fixed crash which occurred when switching between projects
* lua scripting engine

TODO for 2.0
============

- remember settings in search window/improve query serialization
- native annotation based on annotation graphs
- linear search
- plugin manager
- Table object and table view 
- clean up cruft
- better doc
- project appears multiple times in recent?
- integrate Weenink's measurement method

LATER
=====

- descriptive and basic statistics (t-test, chi square)
