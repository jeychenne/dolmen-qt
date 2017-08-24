# Regular expressions

This page documents the `regex` module.

## General concepts

TODO...

## Application Programming Interface

---

<a id="func_new" class="apidef"></a>
### new(pattern)

Create and return a new regular expression (regex) from a string pattern. The regex can be matched against any string.

```lua
local re = regex.new("^(..)")
-- Do something with re...
```

See also: [pattern](#func_pattern)

---

<a id="func_match" class="apidef"></a>
### match(re, subject)

Match regular expression `re` against string `subject`. Returns `true` if there was a match, `false` otherwise.

See also: [count](#func_count), [capture](#func_capture), [has_match](#func_has_match)

---

<a id="func_has_match" class="apidef"></a>
### has_match(re)

Returns `true` if the last call to `match` was sucessful, and `false` if was unsuccessful or if `match` was not called.


See also: [match](#func_match)


---

<a id="func_capture" class="apidef"></a>
### capture(re, nth)

Returns the `nth` captured sub-expression in the last successful call to `match`. If `nth` equals `0`, the whole matched string
is returned, even if no sub-expression was captured. 

**Note:** This function returns an empty string if `nth` is greater than the number returned by the `count` function.

See also: [count](#func_count), [match](#func_match), [first](#func_first), [last](#func_last)

---

<a id="func_count" class="apidef"></a>
### count(re)

Returns the number of captured sub-expressions in the last call to `match`. This function returns 0 if there was no captured
sub-expression, if there was no match or if `match` was not called.


```lua
local re = regex.new("^a(...)(..)(..)")

-- Print "bra", "ca", "da"
if regex.match(re, "abracadabra") then
    for i=1, regex.count(re) do
        local text = regex.capture(re, i)
        print(text)
    end       
end

```

See also: [capture](#func_capture), [match](#func_match)

---

<a id="func_pattern" class="apidef"></a>
### pattern(re)

Returns the pattern (as a `string`) from which the `re` regular expression was constructed. 

See also: [new](#func_new)

---

<a id="func_first" class="apidef"></a>
### first(re, nth)

Returns the index of the first character of the `nth` capture. If `nth` equals `0`, 
it returns the index of the first character in the whole matched string.

See also: [capture](#func_capture), [last](#func_last)

---

<a id="func_last" class="apidef"></a>
### last(re, nth)

Returns the index of the last character of the `nth` capture. If `nth` equals `0`, 
it returns the index of the last character in the whole matched string.

See also: [capture](#func_capture), [first](#func_first)


