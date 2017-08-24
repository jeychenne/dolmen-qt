# Plugins

Dolmen can be extended with plugins, which are written in JSON and Lua. When it starts up, Dolmen loads all plugins which are located
in the system plugin directory or in the user plugin directory. Plugins can be redistributed as ZIP files (the `.zip` extension is compulsory).
To install a plugin, go to `File > Install plugin...` and choose the ZIP file. It will be installed in the current user’s plugin directory.

## Structure of a plugin

To be valid, a plugin must adhere to a number of conventions: if they are not respected, Dolmen will silently ignore the plugin. 
The root directory of the plugin must contain the following:

* a description file, named description.json (compulsory)
* a `Scripts` sub-directory, which contains all your scripts (optional).
* a `Grammars` sub-directory, which contains all your
* a `Resources` sub-directory, which may contain anything (optional).

The description file contains all the information necessary to initialize the plugin. All declarative aspects of the plugin are written in the JSON format 
and must bear the extension `.json`. Scripts are written in Lua and must bear the extension `.lua`.


