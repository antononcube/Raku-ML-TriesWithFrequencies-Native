#ifndef TRIES_WITH_FREQUENCIES_H
#define TRIES_WITH_FREQUENCIES_H

/* A token trie with cumulative frequencies.  A node owns key, child and next. */
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef struct TWFNode {
    char *key;
    double value;
    struct TWFNode *children;
    struct TWFNode *next;
} TWFNode;

typedef struct {
    const char *const *tokens;
    size_t length;
} TWFWord;

typedef int (*TWFPathVisitor)(const char *const *tokens, size_t length,
                              double terminal_value, void *context);

/* Construction and ownership.  Every returned node must be released with
   twf_free().  NULL is an allocation failure (or an empty input to create). */
TWFNode *twf_new(const char *key, double value);
void twf_free(TWFNode *trie);
TWFNode *twf_clone(const TWFNode *trie);
bool twf_equal(const TWFNode *a, const TWFNode *b);
TWFNode *twf_create(const TWFWord *words, size_t count);
bool twf_insert(TWFNode *trie, const char *const *word, size_t length,
                double value, double bottom_value);
TWFNode *twf_merge(const TWFNode *a, const TWFNode *b);

/* Lookup. A NULL word/zero length retrieves the root. */
const TWFNode *twf_retrieve(const TWFNode *trie, const char *const *word,
                            size_t length);
size_t twf_position(const TWFNode *trie, const char *const *word,
                    size_t length);
bool twf_is_key(const TWFNode *trie, const char *const *word, size_t length);
bool twf_contains(const TWFNode *trie, const char *const *word, size_t length);
bool twf_has_complete_match(const TWFNode *trie, const char *const *word,
                            size_t length);

/* Transformations; these do not modify their input. */
TWFNode *twf_node_probabilities(const TWFNode *trie);
TWFNode *twf_prune(const TWFNode *trie, int max_level);
TWFNode *twf_shrink(const TWFNode *trie, const char *delimiter,
                    double threshold, bool internal_only);
TWFNode *twf_remove_by_threshold(const TWFNode *trie, double threshold,
                                  bool keep_at_or_above,
                                  const char *replacement_key);
TWFNode *twf_remove_by_pareto_fraction(const TWFNode *trie, double fraction,
                                       bool keep_top, const char *replacement_key);

/* Enumeration calls visitor for every terminal word. The root's empty key is
   omitted from paths. A nonzero visitor result stops enumeration and is returned. */
int twf_visit_words(const TWFNode *trie, TWFPathVisitor visitor, void *context);
void twf_node_counts(const TWFNode *trie, size_t *total, size_t *internal,
                     size_t *leaves);
int twf_random_choice(const TWFNode *trie, bool weighted, unsigned int *seed,
                      const char ***tokens, size_t *length);
void twf_free_choice(const char **tokens);

/* JSON is allocated with malloc; caller frees it.  max_level < 0 means all. */
char *twf_to_json(const TWFNode *trie, int max_level);
void twf_print(const TWFNode *trie, FILE *stream);

#endif
