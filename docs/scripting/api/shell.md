# Shell

This page documents the `shell` module.

## General concepts

The shell represents Dolmen's user interface. The following functions let you use user interface elements
 (such as dialogs) in order to facilitate interaction with users of your scripts. 

## Application Programming Interface

---

<a id="func_warning" class="apidef"></a>
### warning(message)

Displays a warning dialog.

See also: [alert](#func_alert)

---

<a id="func_alert" class="apidef"></a>
### alert(message)

Displays an error dialog. This can be used for critical errors.

See also: [warning](#func_warning)

---

<a id="func_open_file_dialog" class="apidef"></a>
### open_file_dialog(message)

Displays a dialog that lets the user select a file.

See also: [save_file_dialog](#func_save_file_dialog), [open_directory_dialog](#func_open_directory_dialog)

---

<a id="func_save_file_dialog" class="apidef"></a>
### save_file_dialog(message)

Displays a dialog that lets the user choose a path to save a file.

See also: [open_file_dialog](#func_open_file_dialog), [open_directory_dialog](#func_open_directory_dialog)

---

<a id="func_open_directory_dialog" class="apidef"></a>
### open_directory_dialog(message)

Displays a dialog that lets the user select a directory.

See also: [save_file_dialog](#func_save_file_dialog), [open_file_dialog](#func_open_file_dialog)


---

<a id="func_status" class="apidef"></a>
### status(message, timeout)

Displays `message` in the status bar for `timeout` seconds. If `timeout` is `0`, the message is displayed until the next one appears.
