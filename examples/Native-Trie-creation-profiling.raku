#!/usr/bin/env perl6

use ML::TriesWithFrequencies::Native;

say "Now running Raku compiler: {$*RAKU.compiler.version}!";

my @words = slurp("/usr/share/dict/words".IO).lines;

say '@words.elems :', @words.elems;
say '@words.roll(12) :', @words.roll(12);

# Across a word collections sizes.
say "=" x 60;
say "Across word collections sizes";
say "=" x 60;

my $method = 'insert';
say "Using ", (:$method);

srand(12);
for [1..5].map({ 10 ** $_ }) -> $n {
    say '$n = ', $n;
    my @wordsLocal = $n > @words.elems ?? @words !! @words.roll($n);
    my @word-lists = @wordsLocal>>.comb>>.List;
    my $start = now;
    #my $tr = native-trie-create-by-split( @wordsLocal );
    my $tr = native-trie-create( @word-lists, :$method);
    my $tend = now;
    say 'number of words = ', @wordsLocal.elems, ', creation time:', $tend - $start;
    say "Trie statistics: {native-trie-node-counts($tr).gist}";
    #say $tr.toWLFormat;
}
