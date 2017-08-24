# String manipulation

This page documents the `string` module.

## General concepts

A `string` is a sequence of characters enclosed between double quotes, such as `"this"`. Strings in Lua are immutable, which means that you cannot
modify them directly. All function that "modify" a string actually return a new string but leave the original string unchanged. 

All string functions assume that strings are encoded according to the UTF-8 [Unicode](http://www.unicode.org/) standard. A good tutorial about UTF-8 can be 
found at the following address: [http://www.zehnet.de/2005/02/12/unicode-utf-8-tutorial](http://www.zehnet.de/2005/02/12/unicode-utf-8-tutorial/). In the
remainder of this document, the word *character* is used to mean *code point*, unless otherwise noted. 


## Application Programming Interface

---

<a id="func_at" class="apidef"></a>
### at(str, pos)

Get character at position `pos`.

---

<a id="func_len" class="apidef"></a>
### len(str)

Returns the length of the string, in Unicode code points.

```lua
local size = string.len("안녕하세요")
print(size) -- Prints "5"
```

See also: [byte_count](#func_bytecount)

---

<a id="func_bytecount" class="apidef"></a>
### byte_count(str)

Returns the length of the string, in bytes (or Unicode code units). For strings encoded in ASCII (mostly, strings of English text with no "special" character), 
each code unit is represented with 1 byte, such that `bytecount` and `len` return the same result. For most other languages, however, the number of bytes and the 
number of code points will be different.

```lua

local english = "hello"
local korean  = "안녕하세요"

print(string.len(english)) -- prints "5"
print(string.len(korean))  -- prints "5"

print(string.byte_count(english)) -- prints "5"
print(string.byte_count(korean))  -- prints "15"
```

See also: [len](#func_len)

---

<a id="func_trim" class="apidef"></a>
### trim(str)

Returns a copy of the string with whitespace characters removed at both ends of the string. 

```lua
local s = "\t  hello  \n"

s = string.trim(s)
print("$" .. s .. "$") -- prints "$hello$"
```

See also: [ltrim](#func_ltrim), [rtrim](#func_rtrim)

---

<a id="func_ltrim" class="apidef"></a>
### ltrim(str)

Returns a copy of the string with whitespace characters removed at the left end of the string. 

```lua
local s = "  hello  "

s = string.ltrim(s)
print("$" .. s .. "$") -- prints "$hello  $"
```

See also: [trim](#func_trim), [rtrim](#func_rtrim)

---

<a id="func_rtrim" class="apidef"></a>
### rtrim(str)

Returns a copy of the string with whitespace characters removed at the right end of the string. 

```lua
local s = "  hello  "

s = string.rtrim(s)
print("$" .. s .. "$") -- prints "$  hello$"
```

See also: [ltrim](#func_ltrim), [trim](#func_trim)

---

<a id="func_startswith" class="apidef"></a>
### starts_with(str, prefix)

Returns true if `str` starts with `prefix`, and `false` otherwise.

See also: [ends_with](#func_endswith)

---

<a id="func_endswith" class="apidef"></a>
### ends_with(str, suffix)

Returns true if `str` ends with `suffix`, and `false` otherwise.

See also: [starts_with](#func_startswith)

---

<a id="func_contains" class="apidef"></a>
### contains(str, substring)

Returns true if `str` ends with `substring`, and `false` otherwise.


---

<a id="func_count" class="apidef"></a>
### count(str, substring)

Returns the number of times `substring` appears in `str`. 

```lua
local s = "cacococococa"
local count = string.count(s, "coco")

print(count) -- prints "2"
```

Note: matches don't overlap. 

---

<a id="func_ucase" class="apidef"></a>
### ucase(str)

Returns a copy of `str` where each code point has been converted to upper case. 

```lua
local s1 = "c'était ça"
local s2 = string.ucase(s1)

print(s2) -- prints "C'ÉTAIT ÇA"
```

See also: [lcase](#func_lcase)

---

<a id="func_lcase" class="apidef"></a>
### lcase(str)

Returns a copy of `str` where each code point has been converted to lower case. 

```lua
local s1 = "C'ÉTAIT ÇA"
local s2 = string.lcase(s1)

print(s2) -- prints "c'était ça"
```

See also: [ucase](#func_ucase)


---

<a id="func_replace" class="apidef"></a>
### replace(str, old, new)

Returns a copy of `str` where all (non-overlapping) instances of the substring `old` have been replaced by `new`.

See also:  [replace_at](#func_replace_at), [replace_first](#func_replace_first), [replace_last](#func_replace_last)

---

<a id="func_replace_at" class="apidef"></a>
### replace_at(str, at, count, new)

Returns a copy of `str` where `count` code points, starting at position `at`, have been replaced by `new`.

See also: [replace](#func_replace), [replace_first](#func_replace_first), [replace_last](#func_replace_last)

---

<a id="func_replace_first" class="apidef"></a>
### replace_first(str, old, new)

Returns a copy of `str` where the first instance of the substring `old` has been replaced by `new`.

See also:  [replace_at](#func_replace_at), [replace](#func_replace), [replace_last](#func_replace_last) 

---

<a id="func_replace_last" class="apidef"></a>
### replace_last(str, old, new)

Returns a copy of `str` where the last instance of the substring `old` has been replaced by `new`.

See also:  [replace_at](#func_replace_at), [replace](#func_replace), [replace_first](#func_replace_first) 


 ---

<a id="func_concat" class="apidef"></a>
### concat(str1, str2)

Create a new string which is the concatenation of `str1` and `str2`. In general, you should use Lua's built-in concatenation 
operator `..` instead of this function. 


 ---

<a id="func_contains" class="apidef"></a>
### contains(str, substr)

Returns true if `str` contains `substr` and false otherwise. If `substr` is the empty string, the result is true. 


---

<a id="func_remove" class="apidef"></a>
### remove(str, substr)

Returns a copy of `str` where all (non-overlapping) instances of the substring `substr` have been removed.

See also: [remove_at](#func_remove_at), [remove_first](#func_remove_first), [remove_last](#func_remove_last)

---

<a id="func_remove_at" class="apidef"></a>
### remove_at(str, at, count)

Returns a copy of `str` where `count` code points, starting at position `at`, have been removed.

See also: [remove](#func_remove), [remove_first](#func_remove_first), [remove_last](#func_remove_last)

---

<a id="func_remove_first" class="apidef"></a>
### remove_first(str, substr)

Returns a copy of `str` where the first instance of `substr` has been removed.

See also: [remove_at](#func_remove_at), [remove](#func_remove), [remove_last](#func_remove_last) 

---

<a id="func_remove_last" class="apidef"></a>
### remove_last(str, substr)

Returns a copy of `str` where the last instance of `substr` has been removed.

See also: [remove_at](#func_remove_at), [remove](#func_remove), [remove_first](#func_remove_first) 

---

<a id="func_reverse" class="apidef"></a>
### reverse(str)

Returns a new string with all the characters in `str` in reversed order.

---

<a id="func_insert" class="apidef"></a>
### insert(str, pos, other)

Returns a copy of `str` with `other` inserted at position `pos`


---

<a id="func_substr" class="apidef"></a>
### substr(str, from, to)

Returns the substring of `str` starting at index `from` and ending at index `to` (inclusive). If `to` equals `-1`, returns
the substring from `from` until the end of the string.

```lua
local s = "c'était ça"

print(string.substr(s, 3, 7)) -- "était"
print(string.substr(s, 3,-1)) -- "était ça"

```


---

<a id="func_left" class="apidef"></a>
### left(str, n)

Get the substring corresponding to the `n` first characters of the string. 

---

<a id="func_right" class="apidef"></a>
### right(str, n)

Get the substring corresponding to the `n` last characters of the string. 

---

<a id="func_first" class="apidef"></a>
### first(str)

Get the first character of the string.


---

<a id="func_last" class="apidef"></a>
### last(str)

Get the last character of the string.


---

<a id="func_join" class="apidef"></a>
### join(strings, delim)

Returns a new string which is the result of the concatenation of the strings in table `strings`, separated by `delim`. 

---

<a id="func_split" class="apidef"></a>
### split(str, delim)

Returns a table of strings which have been split at each occurrence of the substring `delim`. If `delim` is the empty
string, it returns a list of the characters in the string. 
