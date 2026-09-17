## Raku Native / C implementation hook up

When using the following prompt with Codex and "ChatGPT-5.6-Terra" the "src" directory had the code
of the C package ["C-TriesWithFrequencies"](https://github.com/antononcube/C-TriesWithFrequencies).

```text
Carefully analyze the C code in src of the "TriesWithFrequencies" data structure and functions and create :
- Corresponding Raku native implementation in the file "./lib/ML/TriesWithFrequencies/Native.rakumod"
- Tests in the directory "t"
- Suitable example script in the directory "examples"
Note that: 
- This Raku package, "ML::TriesWithFrequencies::Native", has its functions / subs with the prefix "native-trie-".
- The Raku package "ML::TriesWithFrequencies::Native" has its functions / subs with the prefix "trie-".

For visualization purposes the native tries are converted to "ML::TriesWithFrequencies" tries using the code like:
`trie-from-map-format(native-trie-to-map-format($nativeTrie))` .

So, implement, in Raku, the sub `native-trie-to-map` that converts the native Trie structure to a Raku hashmap. 
``` 