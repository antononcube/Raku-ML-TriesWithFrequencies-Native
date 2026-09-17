use v6.d;
unit module ML::TriesWithFrequencies::Native;

use NativeCall;

my constant $library = %?RESOURCES<libraries/TriesWithFrequencies>
    // $*VM.platform-library-name('TriesWithFrequencies'.IO);

class NativeTrieNode is repr('CStruct') is export {
    has Str                     $.key      is rw;
    has num64                   $.value    is rw;
    has Pointer[NativeTrieNode] $.children is rw;
    has Pointer[NativeTrieNode] $.next     is rw;
}

sub c-new(Str, num64 --> NativeTrieNode) is native($library) is symbol('twf_new') { * }
sub c-free(NativeTrieNode) is native($library) is symbol('twf_free') { * }
sub c-clone(NativeTrieNode --> NativeTrieNode) is native($library) is symbol('twf_clone') { * }
sub c-equal(NativeTrieNode, NativeTrieNode --> uint8) is native($library) is symbol('twf_equal') { * }
sub c-insert(NativeTrieNode, CArray[Str], size_t, num64, num64 --> uint8) is native($library) is symbol('twf_insert') { * }
sub c-merge(NativeTrieNode, NativeTrieNode --> NativeTrieNode) is native($library) is symbol('twf_merge') { * }
sub c-retrieve(NativeTrieNode, CArray[Str], size_t --> NativeTrieNode) is native($library) is symbol('twf_retrieve') { * }
sub c-position(NativeTrieNode, CArray[Str], size_t --> size_t) is native($library) is symbol('twf_position') { * }
sub c-is-key(NativeTrieNode, CArray[Str], size_t --> uint8) is native($library) is symbol('twf_is_key') { * }
sub c-contains(NativeTrieNode, CArray[Str], size_t --> uint8) is native($library) is symbol('twf_contains') { * }
sub c-complete(NativeTrieNode, CArray[Str], size_t --> uint8) is native($library) is symbol('twf_has_complete_match') { * }
sub c-probabilities(NativeTrieNode --> NativeTrieNode) is native($library) is symbol('twf_node_probabilities') { * }
sub c-prune(NativeTrieNode, int32 --> NativeTrieNode) is native($library) is symbol('twf_prune') { * }
sub c-shrink(NativeTrieNode, Str, num64, uint8 --> NativeTrieNode) is native($library) is symbol('twf_shrink') { * }
sub c-threshold(NativeTrieNode, num64, uint8, Str --> NativeTrieNode) is native($library) is symbol('twf_remove_by_threshold') { * }
sub c-pareto(NativeTrieNode, num64, uint8, Str --> NativeTrieNode) is native($library) is symbol('twf_remove_by_pareto_fraction') { * }
sub c-node-counts(NativeTrieNode, size_t is rw, size_t is rw, size_t is rw) is native($library) is symbol('twf_node_counts') { * }
sub c-random-choice(NativeTrieNode, uint8, uint32 is rw, Pointer is rw, size_t is rw --> int32) is native($library) is symbol('twf_random_choice') { * }
sub c-free-choice(Pointer) is native($library) is symbol('twf_free_choice') { * }
sub c-choice-token(Pointer, size_t --> Str) is native($library) is symbol('twf_choice_token') { * }

sub word-array(Positional:D $word --> CArray[Str]) {
    my $result = CArray[Str].new;
    for $word.list.kv -> $i, $token { $result[$i] = $token.Str }
    $result[$word.elems] = Str;
    $result
}
sub native-trie-new(Str() $key = '', Real() $value = 0 --> NativeTrieNode) is export { c-new($key, $value.Num) }
sub native-trie-free(NativeTrieNode:D $trie) is export { c-free($trie) }
sub native-trie-clone(NativeTrieNode:D $trie --> NativeTrieNode) is export { c-clone($trie) }
sub native-trie-equal(NativeTrieNode:D $a, NativeTrieNode:D $b --> Bool) is export { so c-equal($a, $b) }
sub native-trie-create(@words --> NativeTrieNode) is export {
    my $trie = native-trie-new();
    for @words -> $word {
        die 'Every word must be Positional' unless $word ~~ Positional;
        next unless $word.elems;
        die 'Could not insert word' unless native-trie-insert($trie, $word);
    }
    $trie
}
sub native-trie-create-by-split(@words, Str() :$separator = '' --> NativeTrieNode) is export {
    my @tokenized = @words.map({ $separator.chars ?? .split($separator).Array !! .comb.Array });
    native-trie-create(@tokenized)
}
sub native-trie-insert(NativeTrieNode:D $trie, Positional:D $word, Real() :$value = 1, Real() :$bottom-value = $value --> Bool) is export {
    return False unless $word.elems;
    my $array = word-array($word);
    so c-insert($trie, $array, $word.elems, $value.Num, $bottom-value.Num)
}
sub native-trie-merge(NativeTrieNode:D $a, NativeTrieNode:D $b --> NativeTrieNode) is export { c-merge($a, $b) }
sub native-trie-retrieve(NativeTrieNode:D $trie, Positional:D $word --> NativeTrieNode) is export { my $a = word-array($word); c-retrieve($trie, $a, $word.elems) }
sub native-trie-position(NativeTrieNode:D $trie, Positional:D $word --> Int) is export { my $a = word-array($word); c-position($trie, $a, $word.elems).Int }
sub native-trie-is-key(NativeTrieNode:D $trie, Positional:D $word --> Bool) is export { $word.elems && so c-is-key($trie, word-array($word), $word.elems) }
sub native-trie-contains(NativeTrieNode:D $trie, Positional:D $word --> Bool) is export { $word.elems && so c-contains($trie, word-array($word), $word.elems) }
sub native-trie-has-complete-match(NativeTrieNode:D $trie, Positional:D $word --> Bool) is export { so c-complete($trie, word-array($word), $word.elems) }
sub native-trie-node-probabilities(NativeTrieNode:D $trie --> NativeTrieNode) is export { c-probabilities($trie) }
sub native-trie-prune(NativeTrieNode:D $trie, Int() $max-level --> NativeTrieNode) is export { c-prune($trie, $max-level) }
sub native-trie-shrink(NativeTrieNode:D $trie, Str() :$delimiter = '', Real() :$threshold = -1, Bool :$internal-only = False --> NativeTrieNode) is export { c-shrink($trie, $delimiter, $threshold.Num, $internal-only.Int) }
sub native-trie-remove-by-threshold(NativeTrieNode:D $trie, Real() $threshold, Bool :$keep-at-or-above = True, Str :$replacement-key = Str --> NativeTrieNode) is export { c-threshold($trie, $threshold.Num, $keep-at-or-above.Int, $replacement-key) }
sub native-trie-remove-by-pareto-fraction(NativeTrieNode:D $trie, Real() $fraction, Bool :$keep-top = True, Str :$replacement-key = Str --> NativeTrieNode) is export { c-pareto($trie, $fraction.Num, $keep-top.Int, $replacement-key) }

sub native-trie-counts(NativeTrieNode:D $trie --> Hash) is export {
    my size_t ($total, $internal, $leaves) = 0, 0, 0;
    c-node-counts($trie, $total, $internal, $leaves);
    { total => $total.Int, internal => $internal.Int, leaves => $leaves.Int }
}
sub native-trie-node-counts(NativeTrieNode:D $trie --> Hash) is export {
    native-trie-counts($trie)
}

sub native-trie-random-choice(NativeTrieNode:D $trie, Int() $count = 1,
                              Bool :$weighted = True, Int :$seed --> List) is export {
    die 'Count must be non-negative' if $count < 0;
    my uint32 $state = ($seed // (1 +^ 31)).UInt;
    my @choices;
    for ^$count {
        my Pointer $tokens .= new;
        my size_t $length = 0;
        die 'Could not choose a word from native trie'
            if c-random-choice($trie, $weighted.Int, $state, $tokens, $length) != 0;
        my @choice = (^$length).map({ c-choice-token($tokens, $_).Str });
        @choices.push: @choice.Array;
        c-free-choice($tokens);
    }
    @choices.List
}

sub node-to-map(NativeTrieNode:D $node --> Hash) {
    my %node = TRIEVALUE => $node.value;
    my $child = $node.children;
    while $child {
        my $current = nativecast(NativeTrieNode, $child);
        %node{$current.key} = node-to-map($current);
        $child = $current.next;
    }
    %node
}
# This map is accepted by trie-from-map-format(native-trie-to-map($trie)).
sub native-trie-to-map(NativeTrieNode:D $trie --> Hash) is export { { TRIEROOT => node-to-map($trie) } }
# Compatibility alias used by early package documentation.
sub native-trie-to-map-format(NativeTrieNode:D $trie --> Hash) is export { native-trie-to-map($trie) }
