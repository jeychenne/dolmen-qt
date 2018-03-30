## Getting started

### Main window

The left panel is the *file manager*: by default, it displays the hierarchical structure of the project, but it can also display bookmarks 
associated with the current project. The right panel is the *information panel*, which is used to display and edit metadata about the file(s) 
currently selected. The bottom panel is the *console*, which can be used to type commands using Dolmen's scripting engine. Finally, 
the central part of the user interface is the *viewer*, which displays views such as the result of a query. Each view is displayed as a 
tab, in a similar fashion to web pages in a modern browser. The default view, the start view, displays a few buttons for the most common 
operations a user may want to perform. 

### Corpus management

Several functions from the `File` menu let the user important files into a project, either individually or  by importing a folder recursively.
The logical structure of a project is independent from the physical organization of the files on the user's computer: once files have been 
added to a project, they can be moved around, merged into new folders or removed without affecting the files on disk.
Dolmen supports several annotation formats, including TextGrid (Praat) and LAB (WaveSurfer). It also supports a number of audio formats, 
including WAV, AIFF and FLAC (the exact number of supported formats depends on the platform). By default, Dolmen will try to automatically 
bind an annotation and a sound file if they have the same base name but a different extension. If the names differ, it is possible to bind 
them manually, by right-clicking on them and choosing the corresponding option in the context menu, or semi-automatically using the 
`Import metadata...` feature from the file menu or using the scripting engine.

The hierarchical organization of a project is a matter of pure convenience to the user and is irrelevant for Dolmen. Instead, the program 
relies on metadata to keep files organized internally and to perform queries. File names represent the most basic type of metadata and 
for small projects (containing a dozen of files or so) this may be all that is needed. When one needs to sort and organize a larger 
collection of files, Dolmen offers a flexible mechanism called *properties*. A property is a typed key/value pair. Each file can be tagged 
with an arbitrary number of such properties: the key represents a category, which is always a text string, and the value may be either 
Boolean, textual or numeric. Typical examples of properties would be _Speaker_ (where each unique speaker identifier represents a distinct 
value, for example _11ajp1_) and _Gender_ (with the values _Male_ and _Female_). Properties can be managed via the property editor, 
available from the information panel when a file selection is active. 

In addition to properties, each file can be annotated with a description, a free-form string which can used to store any kind of information,
and which is also exposed to the search engine to filter files. 
