# ML::TriesWithFrequencies::Native

[![Actions Status](https://github.com/antononcube/Raku-ML-TriesWithFrequencies-Native/actions/workflows/linux.yml/badge.svg)](https://github.com/antononcube/Raku-ML-TriesWithFrequencies-Native/actions)
[![Actions Status](https://github.com/antononcube/Raku-ML-TriesWithFrequencies-Native/actions/workflows/macos.yml/badge.svg)](https://github.com/antononcube/Raku-ML-TriesWithFrequencies-Native/actions)

[![License: Artistic-2.0](https://img.shields.io/badge/License-Artistic%202.0-0298c3.svg)](https://opensource.org/licenses/Artistic-2.0)

This Raku package has C-implementations of functions for creation and manipulation of
[Tries (Prefix trees)](https://en.wikipedia.org/wiki/Trie)
with frequencies.

The package provides Machine Learning (ML) functionalities,
not "just" a Trie data structure. 

The package is a faster and compatible version of the Raku package 
["ML::TriesWithFrequencies"](https://github.com/antononcube/Raku-ML-TriesWithFrequencies), [AAp1].

This Raku implementation is based on the code of the C implementation, [AAp2], 
which, in turn, closely follows the Java implementation [AAp5].

The subset of functions with the prefix "native-trie-" follows the one used in the:
- Raku package [AAp1] with prefix "trie-"
- Mathematica package [AAp4].

-------

## Installation

Via zef-ecosystem:

```
zef install ML::TriesWithFrequencies::Native
```

From GitHub:

```
zef install https://github.com/antononcube/Raku-ML-TriesWithFrequencies-Native
```

------

## Usage

Consider a trie (prefix tree) created over a list of words:

```raku
use ML::TriesWithFrequencies::Native;
use ML::TriesWithFrequencies;

my $tr = native-trie-create-by-split( <bar bark bars balm cert cell> );
 
# define visualization function
sub native-trie-say($t) { trie-say(trie-from-map-format(native-trie-to-map($t))) };

native-trie-say($tr);
```


Here we convert the trie with frequencies above into a trie with probabilities:

```raku
my $ptr = native-trie-node-probabilities( $tr );
native-trie-say($ptr);
```


Here we shrink the trie with probabilities above:

```raku
native-trie-say(native-trie-shrink($ptr));
```


Here we retrieve a sub-trie with a key:

```raku
native-trie-say(native-trie-retrieve($ptr, 'bar'.comb))
```


Generate random words using trie, make a new trie, and visualize it:

```raku
my @randomWords = native-trie-random-choice($ptr, 200);
my $ptrRandom = native-trie-node-probabilities(native-trie-create(@randomWords));
native-trie-say($ptrRandom);
```


Compare with the original one:

```raku
native-trie-say($ptr)
```


**Remark:** It is expected with large numbers of generated words to get frequencies
very close to those of the original trie.

------

## Representation

Such trees can be nicely represented as hashmaps. For example:

```raku
my $tr = native-trie-shrink(native-trie-create-by-split(<core cort>));
native-trie-to-map-format($tr);
```

------

## Performance 

This package "ML::TriesWithFrequencies::Native" is approximately 10÷15 times faster than "ML::TriesWithFrequencies"
on "larger" lists of words. 
See the benchmark file ["Native-Trie-creation-profiling.raku"](./examples/Native-Trie-creation-profiling.raku).

------

## Hook up with "ML::TriesWithFrequencies"

*TBD...*

------

## Implementation

The following steps were taken to implement and document this package ("ML::TriesWithFrequencies::Native").
All Large Language Model (LLM) steps were with OpenAI's "GPT-5.6-Terra". 
 
- Reprogram the Java-implementation, [AAp5], to C, [AAp2], using an LLM.
- Making a skeleton package version of "ML::TriesWithFrequencies::Native".
  - Using "Math::DistanceFunctions::Native" as a reference.
- Taking the README of "ML::TriesWithFrequencies", [AAp1], and changing it to reflect the desired native-trie functionalities.
- Add the function `trie-from-map-format` in "ML::TriesWithFrequencies" order to transfer native tries into Raku tries.
  - See the sub `native-trie-say` defined above.
- Implement the method `eq` for the class `ML::TriesWithFrequencies::Trie` of "ML::TriesWithFrequencies".
- First implementation with an LLM using the first prompt in ["prompts.md"](./docs/prompts.md).
- Subsequent LLM-supported implementation changes based on observed behavior.
  - See the rest of the prompts in ["prompts.md"](./docs/prompts.md).
- Profiling and making `native-trie-create` take a method option .

Reading and understanding the Raku code of "ML::TriesWithFrequencies::Native" is pretty straightforward. 

I read the C code to a point -- one of the reasons to make this package is to be able to extensively test that C code.
(Say, by visual- or programmatic comparisons.)

------

## References

### Articles

[AA1] Anton Antonov,
["Tries with frequencies for data mining"](https://mathematicaforprediction.wordpress.com/2013/12/06/tries-with-frequencies-for-data-mining/),
(2013),
[MathematicaForPrediction at WordPress](https://mathematicaforprediction.wordpress.com).

[AA2] Anton Antonov,
["Removal of sub-trees in tries"](https://mathematicaforprediction.wordpress.com/2014/10/12/removal-of-sub-trees-in-tries/),
(2013),
[MathematicaForPrediction at WordPress](https://mathematicaforprediction.wordpress.com).

[AA3] Anton Antonov,
["Tries with frequencies in Java"](https://mathematicaforprediction.wordpress.com/2017/01/31/tries-with-frequencies-in-java/),
(2017),
[MathematicaForPrediction at WordPress](https://mathematicaforprediction.wordpress.com).
[GitHub Markdown](https://github.com/antononcube/MathematicaForPrediction).

[WK1] Wikipedia entry, [Trie](https://en.wikipedia.org/wiki/Trie).

### Packages

[AAp1] Anton Antonov,
[ML::TriesWithFrequencies, Raku package](https://github.com/antononcube/Raku-ML-TriesWithFrequencies),
(2021-2024),
[GitHub/antononcube](https://github.com/antononcube).

[AAp2] Anton Antonov,
[C-TriesWithFrequencies, C package](https://github.com/antononcube/Raku-ML-TriesWithFrequencies-Native),
(2026),
[GitHub/antononcube](https://github.com/antononcube).

[AAp3] Anton Antonov,
[Tries with frequencies, Mathematica Version 9.0 package](https://github.com/antononcube/MathematicaForPrediction/blob/master/TriesWithFrequenciesV9.m),
(2013),
[MathematicaForPrediction at GitHub](https://github.com/antononcube/MathematicaForPrediction).

[AAp4] Anton Antonov,
[Tries with frequencies, Mathematica package](https://github.com/antononcube/MathematicaForPrediction/blob/master/TriesWithFrequencies.m),
(2013-2018),
[MathematicaForPrediction at GitHub](https://github.com/antononcube/MathematicaForPrediction).

[AAp5] Anton Antonov,
[Tries with frequencies in Java](https://github.com/antononcube/MathematicaForPrediction/tree/master/Java/TriesWithFrequencies),
(2017),
[MathematicaForPrediction at GitHub](https://github.com/antononcube/MathematicaForPrediction).

[AAp6] Anton Antonov,
[Java tries with frequencies, Mathematica package](https://github.com/antononcube/MathematicaForPrediction/blob/master/JavaTriesWithFrequencies.m),
(2017),
[MathematicaForPrediction at GitHub](https://github.com/antononcube/MathematicaForPrediction).

[AAp7] Anton Antonov,
[Java tries with frequencies Mathematica unit tests](https://github.com/antononcube/MathematicaForPrediction/blob/master/UnitTests/JavaTriesWithFrequencies-Unit-Tests.wlt),
(2017),
[MathematicaForPrediction at GitHub](https://github.com/antononcube/MathematicaForPrediction).


### Videos

[AAv1] Anton Antonov,
["Prefix Trees with Frequencies for Data Analysis and Machine Learning"](https://www.youtube.com/watch?v=MdVp7t8xQbQ),
(2017),
Wolfram Technology Conference 2017,
[Wolfram channel at YouTube](https://www.youtube.com/channel/UCJekgf6k62CQHdENWf2NgAQ).
