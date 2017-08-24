# Event handling

This page documents the `signal` module, which is responsible for event handling in Dolmen. 

## General concepts

Dolmen provides an event handling mechanism known as signal/slot. A signal corresponds to a unique identifier which can
be triggered when an event occurs, for instance when a button is clicked. A signal can be associated with any number of 
functions called *slots*, which may or may not return a value. Whenever a signal is *emitted*, all the slots which are
connected to it are executed (in an unspecified order). 

This mechanism is used throughout Dolmen, as it provides hooks which plugins can use to react to events triggered by the
program. For example, a signal is emitted whenever a file is loaded, which can be used to add custom metadata to each file, 
among other things.

## Application Programming Interface

---

<a id="func_new" class="apidef"></a>
### new()

Create and return a new signal identifier (id). Each id is guaranteed to be unique, such that two different calls to `new` will never yield the same id.

If you need to store an id for subsequent use, store it in a (preferably local) variable.

```lua
local my_event = signal.new()
-- Do something with my_event...
```

---

<a id="func_connect" class="apidef"></a>
### connect(id, slot)

Connect signal `id` to function `slot`. The slot can take any number of arguments, and can return a value.

```lua
local e = signal.new()

local f = function(name)
	print("Hold the door, " .. name)
end

signal.connect(e, f)

-- Print "Hold the door, Hodor" to the standard output
signal.emit(e, "Hodor")
```

See also: [disconnect](#func_disconnect), [emit](#func_emit)

---

<a id="func_disconnect" class="apidef"></a>
### disconnect(id, slot)

Disconnect signal `id` from function `slot`. If `id` and `slot` are not connected, this function does nothing.


```lua
local e = signal.new()

local f = function(name)
	print("Hold the door, " .. name)
end

signal.connect(evt, f)

-- Print "Hold the door, Hodor" to the standard output
signal.emit(e, "Hodor")

signal.disconnect(e, f)

-- Do nothing since e and f are no longer connected
signal.emit(e, "Hodor")

```

See also: [connect](#func_connect), [emit](#func_emit)

---

<a id="func_emit" class="apidef"></a>
### emit(id, ...)

Emit signal `id`, followed by any number of arguments. The arguments are forwarded to all the slots which are connected to this signal (if any). 
Following Lua's function call conventions, if the slot receives less arguments than it expects, missing arguments have the value `nil`; if it receives more arguments than expected, additional arguments are ignored.

This function collects all the return values from the slots it called into a table, which it returns to the caller. (Keep in mind that if a slot doesn't explicitly return a value, its
return value is `nil`.) 

```lua
local e = signal.new()

local f1 = function(arg1)
	print("f1 received a " .. type(arg1))
end

local f2 = function(arg1, arg2)
	print("f2 received a " .. type(arg1) " and a " .. type(arg2))
end

signal.connect(e, f1)
signal.connect(e, f2)

-- Print "f1 received a number" and "f2 received a number and a string"
signal.emit(e, 3.14, "pi")
```

Note: the order in which slots are called is unspecified. In general, it will correspond to the order in which they were
registered, but this should not be relied upon.  


See also: [connect](#func_connect), [disconnect](#func_disconnect)

