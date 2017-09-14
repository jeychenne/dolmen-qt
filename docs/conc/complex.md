## Complex queries

One performing a [simple queries](simple.md) on a set of annotation files, Dolmen attempts to find a set of concordances in one item (point or
interval) at a time. While it is possible for an item to match a given search pattern several times if several substrings match the pattern, 
matches are nevertheless limited to a single item. 

Sometimes, however, we might want to match text in several items _simultaneously_. Such a query is called a *complex query* in Dolmen. There 
are 3 types of relations between items, detailed below: alignment, precedence and dominance. 

### Building a complex query

When you open a search window, two small buttons with a `+` and `-` sign appear below the main the search field. These buttons allow you to
add and remove search items. Any query which has more than one search item is a complex query. 

When you add one or more search items, you will notice that each of them (except the last one) is followed by a selector with 3 possible
values: `is aligned with`, `precedes` and `dominates`. They correspond to the tier item relations `alignment`, `precedence` and 
`dominance`, respectively. 

Contrary to simple queries, complex queries do not use the KWIC model to display results. Instead of displaying a matched string in its context,
it lets the user select a `display tier`, which appears at the top of the search box. The text that is displayed is the concatenation of all the 
items contain within the time interval defined by simultaneous satisfaction of the constraints on each search item. Several examples are given below.

### Alignment relation

Two items are aligned if they are on different tiers and their left and right boundaries coincide. Suppose that you have a word tier (tier 1), where 
each word was segmented, and a part-of-speech (POS) tier (tier 2) which is aligned with the word tier. To extract all the nouns in the corpus,
you could do the following:

* set `NOUN` as the search pattern for tier 1, and choose the `is aligned with` value of the relation selector.
* set `.+` as the search pattern for tier 2  
* set the display tier to tier 2

Dolmen will first look for all items whose text contains "NOUN" on tier 1, and will keep all those items which contain a non-empty label in an
item of tier 2 which is exactly aligned with a NOUN item on tier 1. Dolmen will then return a list of the text labels on tier 2 which match the 
above criteria. 

As another example, suppose you now want to extract all the adverbs that end with _-ly_. You could do the following:

* set `ADV` as the search pattern for tier 1, and choose the `is aligned with` value of the relation selector.
* set `.+ly$` as the search pattern for tier 2  
* set the display tier to tier 2

Assuming that tier 2 contains exactly one word per interval, this will successfully extract all the adverbs on tier 2 that end with _-ly_.

### Precedence relation

Two items are in a precedence relation if they immediately follow each other. You can search for arbitrarily long sequences by chaining 
search items on the same tier. When you specify a sequence, Dolmen will retrieve the text from the display tier that is included within the 
span defined by the sequence. 

Suppose that you have a word tier (tier 1) and a POS tier (tier 2), as in the alignment examples. Instead of searching for a single word, you might be interested 
in looking for word sequences. To find all the `DET+NOUN` sequences, you could do the following:

* set `DET` as the search pattern for the first tier item in tier 1, and choose the `precedes` value of the relation selector.
* set `NOUN` as the search pattern for the second tier item, setting the tier number to 1 to ensure you are looking in the same tier
* set the display tier to tier 2

Dolmen will first look for all `DET` items on tier one, and will keep only those that are followed by a `NOUN` item on the same tier. It will
then display the text that results from the concatenation of all the items on tier 2 within the span determined by the beginning of the `DET` item and 
by the end of `NOUN` item on tier 1. 

### Dominance relation

An item `a` dominates an item `b` if `a` and `b` are on different tier, the left boundary of `b` is greater or equal to that of `a`, and the 
right boundary of `b` is lesser or equal to that of `a`. Dominance relations typically encode hierarchical structures, for instance 
`word > syllable > segment`. 

Suppose you have 3 tiers in your file: the first one contains spans which denote syllables, the second one contains syllabic constituents ("syll") ("Onset", "Nucleus", "Coda") and the last one individual segments ("p", "a", "t"...). 
In order to retrieve all syllables that end in a coda, you could do the following:

* set `syll` as the search pattern for tier 1, and choose the `dominates` value of the relation selector.
* set `Coda` as the search pattern for tier 2 
* set the display tier to tier 3

This query will first get all the items that have a `syll` label on the first tier; then, for each of those, it will look for a label `Coda` 
on tier 2 within the limits of the span on tier 1; for each item which matches both conditions, it will display the concatenated text of 
the items on tier 3 that are dominated by the matching item on tier 1. 

