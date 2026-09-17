use v6.d;
use lib 'lib';
use ML::TriesWithFrequencies::Native;

my $trie = native-trie-create-by-split(<bar bark bars balm cert cell>);
my %map = native-trie-to-map($trie);
say %map.raku;
say "Root frequency: " ~ %map<TRIEROOT><TRIEVALUE>;
say "Frequency for 'bar': " ~ %map<TRIEROOT><b><a><r><TRIEVALUE>;
# With ML::TriesWithFrequencies installed:
# trie-say(trie-from-map-format(%map));
native-trie-free($trie);
