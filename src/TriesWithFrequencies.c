#include "TriesWithFrequencies.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static char *twf_strdup(const char *s) {
  size_t n;
  char *r;
  if (!s)
    s = "";
  n = strlen(s) + 1;
  r = malloc(n);
  if (r)
    memcpy(r, s, n);
  return r;
}

TWFNode *twf_new(const char *key, double value) {
  TWFNode *n = calloc(1, sizeof(*n));
  if (!n)
    return NULL;
  n->key = twf_strdup(key);
  if (!n->key) {
    free(n);
    return NULL;
  }
  n->value = value;
  return n;
}

void twf_free(TWFNode *n) {
  if (!n)
    return;
  twf_free(n->children);
  twf_free(n->next);
  free(n->key);
  free(n);
}

static TWFNode *child(const TWFNode *n, const char *key) {
  TWFNode *p;
  for (p = n ? n->children : NULL; p; p = p->next)
    if (strcmp(p->key, key) == 0)
      return p;
  return NULL;
}
static double child_sum(const TWFNode *n) {
  const TWFNode *p;
  double sum = 0;
  for (p = n ? n->children : NULL; p; p = p->next)
    sum += p->value;
  return sum;
}
static bool append_child(TWFNode *parent, TWFNode *n) {
  TWFNode **p = &parent->children;
  while (*p)
    p = &(*p)->next;
  *p = n;
  return true;
}

TWFNode *twf_clone(const TWFNode *n) {
  TWFNode *r, **out;
  if (!n)
    return NULL;
  r = twf_new(n->key, n->value);
  if (!r)
    return NULL;
  out = &r->children;
  for (n = n->children; n; n = n->next) {
    *out = twf_clone(n);
    if (!*out) {
      twf_free(r);
      return NULL;
    }
    out = &(*out)->next;
  }
  return r;
}

bool twf_equal(const TWFNode *a, const TWFNode *b) {
  const TWFNode *p;
  if (!a || !b)
    return a == b;
  if (a->value != b->value || strcmp(a->key, b->key))
    return false;
  for (p = a->children; p; p = p->next)
    if (!twf_equal(p, child(b, p->key)))
      return false;
  for (p = b->children; p; p = p->next)
    if (!child(a, p->key))
      return false;
  return true;
}

bool twf_insert(TWFNode *root, const char *const *word, size_t length,
                double value, double bottom_value) {
  size_t i;
  TWFNode *n = root, *c;
  if (!root || !word || !length)
    return false;
  n->value += value;
  for (i = 0; i < length; ++i) {
    c = child(n, word[i]);
    if (!c) {
      c = twf_new(word[i], i + 1 == length ? bottom_value : value);
      if (!c)
        return false;
      append_child(n, c);
    } else
      c->value += (i + 1 == length ? bottom_value : value);
    n = c;
  }
  return true;
}

TWFNode *twf_create(const TWFWord *words, size_t count) {
  size_t i;
  TWFNode *r;
  if (!words || !count)
    return NULL;
  r = twf_new("", 0);
  if (!r)
    return NULL;
  for (i = 0; i < count; ++i)
    if (!twf_insert(r, words[i].tokens, words[i].length, 1, 1)) {
      twf_free(r);
      return NULL;
    }
  return r;
}

static TWFNode *merge_node(const TWFNode *a, const TWFNode *b) {
  TWFNode *r, *q;
  const TWFNode *p;
  if (!a)
    return twf_clone(b);
  if (!b)
    return twf_clone(a);
  if (strcmp(a->key, b->key))
    return NULL;
  r = twf_new(a->key, a->value + b->value);
  if (!r)
    return NULL;
  for (p = a->children; p; p = p->next) {
    q = merge_node(p, child(b, p->key));
    if (!q) {
      twf_free(r);
      return NULL;
    }
    append_child(r, q);
  }
  for (p = b->children; p; p = p->next)
    if (!child(a, p->key)) {
      q = twf_clone(p);
      if (!q) {
        twf_free(r);
        return NULL;
      }
      append_child(r, q);
    }
  return r;
}

TWFNode *twf_merge(const TWFNode *a, const TWFNode *b) {
  return merge_node(a, b);
}

const TWFNode *twf_retrieve(const TWFNode *n, const char *const *word,
                            size_t length) {
  size_t i;
  const TWFNode *p;
  if (!n)
    return NULL;
  for (i = 0; i < length; ++i) {
    p = child(n, word[i]);
    if (!p)
      break;
    n = p;
  }
  return n;
}

size_t twf_position(const TWFNode *n, const char *const *word, size_t length) {
  size_t i = 0;
  if (!n || !word)
    return 0;
  while (i < length && (n = child(n, word[i])))
    ++i;
  return i;
}

bool twf_is_key(const TWFNode *n, const char *const *w, size_t l) {
  return l && twf_position(n, w, l) == l;
}

bool twf_has_complete_match(const TWFNode *n, const char *const *w, size_t l) {
  n = twf_retrieve(n, w, l);
  return n && (!n->children || child_sum(n) < n->value);
}

bool twf_contains(const TWFNode *n, const char *const *w, size_t l) {
  return twf_is_key(n, w, l) && twf_has_complete_match(n, w, l);
}

static TWFNode *prob_node(const TWFNode *n, double divisor) {
  TWFNode *r, *q;
  const TWFNode *p;
  double sum;
  if (!n)
    return NULL;
  r = twf_new(n->key, divisor ? n->value / divisor : n->value);
  if (!r)
    return NULL;
  sum = n->value ? n->value : child_sum(n);
  for (p = n->children; p; p = p->next) {
    q = prob_node(p, sum);
    if (!q) {
      twf_free(r);
      return NULL;
    }
    append_child(r, q);
  }
  return r;
}

TWFNode *twf_node_probabilities(const TWFNode *n) {
  TWFNode *r = prob_node(n, 1);
  if (r)
    r->value = 1;
  return r;
}

static TWFNode *prune_node(const TWFNode *n, int max, int level) {
  TWFNode *r, *q;
  const TWFNode *p;
  if (!n)
    return NULL;
  r = twf_new(n->key, n->value);
  if (!r)
    return NULL;
  if (max >= 0 && level >= max)
    return r;
  for (p = n->children; p; p = p->next) {
    q = prune_node(p, max, level + 1);
    if (!q) {
      twf_free(r);
      return NULL;
    }
    append_child(r, q);
  }
  return r;
}

TWFNode *twf_prune(const TWFNode *n, int max_level) {
  return prune_node(n, max_level, 0);
}

static TWFNode *shrink_node(const TWFNode *n, const char *d, double th,
                            bool internal, int level) {
  TWFNode *r, *q;
  const TWFNode *only;
  size_t len;
  if (!n)
    return NULL;
  if (!n->children)
    return twf_clone(n);
  only = n->children;
  if (strcmp(n->key, "") && !only->next && (!internal || only->children) &&
      ((th < 0 &&
        ((n->value >= 1 && only->value >= 1 && n->value == only->value) ||
         (n->value < 1 && only->value == 1))) ||
       (th >= 0 && only->value >= th))) {
    q = shrink_node(only, d, th, internal, level + 1);
    if (!q)
      return NULL;
    len = strlen(n->key) + strlen(d) + strlen(q->key) + 1;
    r = twf_new("", n->value);
    if (!r) {
      twf_free(q);
      return NULL;
    }
    free(r->key);
    r->key = malloc(len);
    if (!r->key) {
      twf_free(r);
      twf_free(q);
      return NULL;
    }
    strcpy(r->key, n->key);
    strcat(r->key, d);
    strcat(r->key, q->key);
    r->children = q->children;
    q->children = NULL;
    twf_free(q);
    return r;
  }
  r = twf_new(n->key, n->value);
  if (!r)
    return NULL;
  for (only = n->children; only; only = only->next) {
    q = shrink_node(only, d, th, internal, level + 1);
    if (!q) {
      twf_free(r);
      return NULL;
    }
    append_child(r, q);
  }
  return r;
}

TWFNode *twf_shrink(const TWFNode *n, const char *d, double th, bool internal) {
  return shrink_node(n, d ? d : "", th, internal, 0);
}

static TWFNode *threshold_node(const TWFNode *n, double th, bool keep,
                               const char *replacement) {
  TWFNode *r, *q;
  const TWFNode *p;
  double removed = 0;
  if (!n)
    return NULL;
  r = twf_new(n->key, n->value);
  if (!r)
    return NULL;
  for (p = n->children; p; p = p->next)
    if ((p->value >= th) == keep) {
      q = threshold_node(p, th, keep, replacement);
      if (!q) {
        twf_free(r);
        return NULL;
      }
      append_child(r, q);
    } else
      removed += p->value;
  if (replacement && removed) {
    q = twf_new(replacement, removed);
    if (!q) {
      twf_free(r);
      return NULL;
    }
    append_child(r, q);
  }
  return r;
}

TWFNode *twf_remove_by_threshold(const TWFNode *n, double th, bool keep,
                                 const char *replacement) {
  return threshold_node(n, th, keep, replacement);
}

static int cmp_value(const void *a, const void *b) {
  const TWFNode *x = *(const TWFNode **)a, *y = *(const TWFNode **)b;
  return x->value < y->value ? 1 : x->value > y->value ? -1 : 0;
}

static TWFNode *pareto_node(const TWFNode *n, double f, bool top,
                            const char *replacement) {
  const TWFNode *p;
  TWFNode **a, *r, *q;
  size_t cnt = 0, i;
  double sum = 0, cum = 0, removed = 0;
  if (!n)
    return NULL;
  for (p = n->children; p; p = p->next)
    ++cnt;
  if (!cnt)
    return twf_clone(n);
  a = malloc(cnt * sizeof(*a));
  if (!a)
    return NULL;
  for (p = n->children, i = 0; p; p = p->next)
    a[i++] = (TWFNode *)p;
  qsort(a, cnt, sizeof(*a), cmp_value);
  for (i = 0; i < cnt; ++i)
    sum += a[i]->value;
  r = twf_new(n->key, n->value);
  if (!r) {
    free(a);
    return NULL;
  }
  for (i = 0; i < cnt; ++i) {
    bool take = top ? cum <= f * sum : cum > f * sum;
    if (take) {
      q = pareto_node(a[i], f, top, replacement);
      if (!q) {
        free(a);
        twf_free(r);
        return NULL;
      }
      append_child(r, q);
    } else
      removed += a[i]->value;
    cum += a[i]->value;
  }
  free(a);
  if (replacement && removed) {
    q = twf_new(replacement, removed);
    if (!q) {
      twf_free(r);
      return NULL;
    }
    append_child(r, q);
  }
  return r;
}

TWFNode *twf_remove_by_pareto_fraction(const TWFNode *n, double f, bool top,
                                       const char *replacement) {
  return pareto_node(n, f, top, replacement);
}

static int visit_node(const TWFNode *n, const char ***path, size_t *length,
                      size_t *capacity, TWFPathVisitor fn, void *ctx) {
  const TWFNode *p;
  int rc;
  bool terminal;
  if (strcmp(n->key, "")) {
    if (*length == *capacity) {
      size_t cap = *capacity ? *capacity * 2 : 8;
      const char **q = realloc((void *)*path, cap * sizeof(**path));
      if (!q)
        return -1;
      *path = q;
      *capacity = cap;
    }
    (*path)[(*length)++] = n->key;
  }
  terminal = !n->children || child_sum(n) < n->value;
  if (terminal && (rc = fn(*path, *length, n->value - child_sum(n), ctx)))
    return rc;
  for (p = n->children; p; p = p->next)
    if ((rc = visit_node(p, path, length, capacity, fn, ctx)))
      return rc;
  if (strcmp(n->key, ""))
    --*length;
  return 0;
}

int twf_visit_words(const TWFNode *n, TWFPathVisitor fn, void *ctx) {
  const char **path = NULL;
  size_t length = 0, cap = 0;
  int rc;
  if (!n || !fn)
    return -1;
  rc = visit_node(n, &path, &length, &cap, fn, ctx);
  free((void *)path);
  return rc;
}

void twf_node_counts(const TWFNode *n, size_t *total, size_t *internal,
                     size_t *leaves) {
  const TWFNode *p;
  if (!n)
    return;
  if (total)
    ++*total;
  if (n->children) {
    if (internal)
      ++*internal;
    for (p = n->children; p; p = p->next)
      twf_node_counts(p, total, internal, leaves);
  } else if (leaves)
    ++*leaves;
}

static unsigned int next_random(unsigned int *seed) {
  *seed = *seed * 1103515245u + 12345u;
  return *seed;
}

int twf_random_choice(const TWFNode *n, bool weighted, unsigned int *seed,
                      const char ***tokens, size_t *length) {
  const char **out = NULL;
  size_t len = 0, cap = 0;
  const TWFNode *p, *chosen;
  double sum, r;
  unsigned int local = seed ? *seed : 1;
  if (!n || !tokens || !length)
    return -1;
  while (n->children) {
    sum = 0;
    for (p = n->children; p; p = p->next)
      sum += weighted ? p->value : 1;
    r = ((double)next_random(&local) / (double)UINT32_MAX) * sum;
    chosen = n->children;
    for (p = n->children; p; p = p->next) {
      r -= weighted ? p->value : 1;
      if (r <= 0) {
        chosen = p;
        break;
      }
    }
    if (len == cap) {
      size_t x = cap ? cap * 2 : 8;
      const char **q = realloc((void *)out, x * sizeof(*out));
      if (!q) {
        free((void *)out);
        return -1;
      }
      out = q;
      cap = x;
    }
    out[len++] = chosen->key;
    n = chosen;
  }
  if (seed)
    *seed = local;
  *tokens = out;
  *length = len;
  return 0;
}

void twf_free_choice(const char **tokens) { free((void *)tokens); }
const char *twf_choice_token(const char *const *tokens, size_t index) { return tokens ? tokens[index] : NULL; }

typedef struct {
  char *s;
  size_t length, capacity;
} String;
static int put(String *b, const char *s) {
  size_t n = strlen(s), need = b->length + n + 1, cap;
  char *p;
  if (need > b->capacity) {
    cap = b->capacity ? b->capacity : 64;
    while (cap < need)
      cap *= 2;
    p = realloc(b->s, cap);
    if (!p)
      return -1;
    b->s = p;
    b->capacity = cap;
  }
  memcpy(b->s + b->length, s, n + 1);
  b->length += n;
  return 0;
}

static int put_number(String *b, double x) {
  char num[64];
  snprintf(num, sizeof(num), "%.17g", x);
  return put(b, num);
}

static int put_json_string(String *b, const char *s) {
  const unsigned char *p = (const unsigned char *)s;
  char esc[7];
  if (put(b, "\""))
    return -1;
  for (; *p; ++p) {
    if (*p == '\"' || *p == '\\') {
      esc[0] = '\\';
      esc[1] = (char)*p;
      esc[2] = 0;
      if (put(b, esc))
        return -1;
    } else if (*p < 32) {
      snprintf(esc, sizeof(esc), "\\u%04x", *p);
      if (put(b, esc))
        return -1;
    } else {
      esc[0] = (char)*p;
      esc[1] = 0;
      if (put(b, esc))
        return -1;
    }
  }
  return put(b, "\"");
}

static int json_node(String *b, const TWFNode *n, int max, int level) {
  const TWFNode *p;
  if (put(b, "{\"key\":") || put_json_string(b, n->key) ||
      put(b, ",\"value\":") || put_number(b, n->value) ||
      put(b, ",\"children\":["))
    return -1;
  if (max < 0 || level < max)
    for (p = n->children; p; p = p->next) {
      if (p != n->children && put(b, ","))
        return -1;
      if (json_node(b, p, max, level + 1))
        return -1;
    }
  return put(b, "]}");
}

char *twf_to_json(const TWFNode *n, int max) {
  String b = {0};
  if (!n || json_node(&b, n, max, 0)) {
    free(b.s);
    return NULL;
  }
  return b.s;
}

void twf_print(const TWFNode *n, FILE *stream) {
  char *s = twf_to_json(n, -1);
  if (s) {
    fputs(s, stream ? stream : stdout);
    fputc('\n', stream ? stream : stdout);
    free(s);
  }
}
