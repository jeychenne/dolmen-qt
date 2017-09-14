## Simple queries

Dolmen offers a number of features to find concordances in a corpus. It also allows user to customize its search interface by creating search grammars 
specifically tailored for a project. Concordancing features are available in the `Conc` menu.

To run a new query, click on `Conc > Find concordances...` or use the shortcut `ctrl+F` (or `cmd+f` on macOS). This will open a new search window, which
lets you search through all the *documents* (i.e. plain text files) or *annotations* in your corpus . We 
will focus on annotations here, but things work in a similar way for documents, mutatis mutandis. 

### The Files box

The `Files` box in the top left corner allows you to select the type of files to search in (documents or annotations). You can either select files individually
if you want to restrict your query to a particular set of files, or leave all files unchecked, in which case Dolmen will try to search in all files.


### The Search box

The `Search` box in the top right corner allows you to enter some text or a regular expression to search for. Next to the search field, 
a spin box lets you select the tier you want to search in. The default choice is `Any tier`, which means that Dolmen will try to find 
your pattern in all tiers of the selected files. You can restrict the search to a particular tier by selecting the appropriate tier number.
Alternatively, you can specify a _tier name pattern_ using a regular expression. If you do specify a tier name pattern, Dolmen will ignore
the tier number and will search instead in any tier whose name matches the pattern. 

By default, the text in the search field is interpreted as a regular expression. If you are looking for plain text string instead, you can
select `plain text` instead of `regular expression` in the selector located under the `+` and `-` buttons. Whether you use a plain text string
or a regular expression, the search will be case-insensitive by default, which means that strings like "foo", "Foo" and "FOO" are treated as
identical. To perform a case sensitive search, simply check the `case sensitive` box. 

Concordances in a simple query follow the KWIC model (key word in context), which means that a match is extracted along with its left and right
context. (The length of the context window can be adjusted in the preferences.) When the context window is longer than the context of the match in a given 
item (time point or interval), dolmen will extract additional text in the preceeding/following items until the left/right contexts have the expected
length. If it cannot find enough items, it will pad the text with white space. By default, Dolmen will use one white space character to join the text 
from different intervals. You can specify a different string (including an empty string) in the `Separator` field.


### Metadata

If you have added properties to your project, a set of property boxes will be added below the `Files` and `Search` box. Each property category
is displayed as a group box containing a list of all the labels of this category. You can check or uncheck any label in any category 
(each category also has an `All labels` button to check/uncheck all labels at once). The search engine will filter files based on the 
conditions that you specify in the property box. Within a category, it uses the Boolean `OR` operator to find the subset of files that has either 
label. Across categories, it uses the `AND` operator to find the intersection of all the subsets defined by each category. 

At the bottom of search window, an additional field lets you filter files based on their description. For example, it is possible to extract all the 
files that contain (or do not contain) a specific string.

### Viewing results

Once you hit the `ok` button, the result of your query is presented as a new `query view` in the viewer. You can browse the results with the mouse wheel. 
The information panel on the right-hand side displays information about the selected token. 

If an annotation is bound to a sound file, you can play a match by double-clicking on it or by pressing the space bar (you can also interrupt it by pressing `Esc`). 

Right-clicking on item will display a context menu that allows you to perform a number of actions:

* `Play selection`: this will play the corresponding item if the annotation is bound to a sound file.

* `Open in annotation`: this will open the annotation in a new view, along with its sound file if it is bound to a sound.

* `Open selection in Praat`: if Praat is installed and configured to work with Dolmen, this will open the match in Praat. Dolmen will open 
the TextGrid (and the sound file if the annotation is bound) in Praat and will display the current match. (Note that you need to have Praat already running for this to work.) 

* `Edit item text`: this allows you to modify the text of the item where the match was found. (Note that the query view is currently not updated to reflect this change.)

* `Create table view`: this will convert the concordance set, along with all its metadata, to a table that can be imported into a spreadsheet program.

* `Export results to tab-separated file (CSV)...`: this exports the concordance set, along with all its metadata, to a CSV file that can be imported into a spreadsheet program.

* `Bookmark search result`: this allows you to bookmark a matched item. Bookmarks are displayed in the bookmark panel, which can accessed by
clicking on the star in the bottom left corner of the main window.

