use v6.d;


unit module ML::TriesWithFrequencies::Native;

use NativeCall;
use NativeHelpers::Array;

my constant $library = %?RESOURCES<libraries/TriesWithFrequencies>;

class TrieNodeStruct is repr('CStruct') {
}
