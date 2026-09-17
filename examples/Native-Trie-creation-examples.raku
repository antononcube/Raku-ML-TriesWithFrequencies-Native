#!/usr/bin/env perl6

use ML::TriesWithFrequencies::Native;
use ML::TriesWithFrequencies;

say '-' x 120;

my $tr = native-trie-create-by-split( <bar barman bask bell best> );
say native-trie-to-map($tr);

say '-' x 120;

my $ptr = native-trie-node-probabilities( $tr );
say native-trie-to-map($ptr);

say '-' x 120;

my $ptr2 = native-trie-node-probabilities(native-trie-create-by-split(<bar barman bask bell best>));
say trie-from-map-format(native-trie-to-map($ptr2)).JSON

