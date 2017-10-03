#include "map.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct cell {
  struct cell *next;
  void *value;
  char key[];
} cell;

struct map {
  struct cell **elems;
  int capacity;
  int size;
};

// Internal helper functions. Implemented at the bottom of this file.
static unsigned int hash(const char *key);
static void extend_if_necessary(map m);

/**
 * Create a new, empty map.
 * 
 * The returned map has dynamically allocated memory associated with it, and
 * this memory must be reclaimed after use with `map_destroy`.
 */
map map_create() {

  // Allocate space for the map's primary data structure. More space will be 
  // allocated in the future when values are added to the map.
  map m = malloc(sizeof (map));
  assert(m != NULL);
  m->elems = calloc(1, sizeof (struct cell *));
  assert(m->elems != NULL);

  // Initialize metadata. The map starts with capacity for one entry.
  m->capacity = 1;
  m->size = 0;

  return m;
}

/**
 * Free the memory used for a map after use.
 * 
 * Note that this routine does not free any memory that was allocated for the
 * values stored in the map. That memory must be freed by the client as
 * appropriate.
 */
void map_destroy(map m) {

  // Loop over each cell in the map and free it.
  for (int i = 0; i < m->capacity; i += 1) {
    struct cell *curr = m->elems[i];
    while (curr != NULL) {
      struct cell *next = curr->next;
      free(curr);
      curr = next;
    }
  }

  free(m->elems);
  free(m);
}

/**
 * Get the size of a map.
 */
int map_size(const map m) {
  return m->size;
}

/**
 * Determine whether a map contains a given key.
 * 
 * Keys are case-sensitive.
 */
bool map_contains(const map m, const char *key) {
  int b = hash(key) % m->capacity;

  // Search linearly for a matching key through the appropriate linked list.
  for (struct cell *curr = m->elems[b]; curr != NULL; curr = curr->next) {
    if (strcmp(curr->key, key) == 0) return true;
  }
  return false;
}

/**
 * Set the value for a given key within a map.
 * 
 * This will add a new key if it does not exist. If the key already exists, the
 * new value will replace the old one.
 */
void map_set(map m, const char *key, void *value) {
  int b = hash(key) % m->capacity;

  // First, look for an existing entry with the given key in the map. If it
  // exists, simply update its value.
  for (struct cell *curr = m->elems[b]; curr != NULL; curr = curr->next) {
    if (strcmp(curr->key, key) == 0) {
      curr->value = value;
      return;
    }
  }

  extend_if_necessary(m);
  b = hash(key) % m->capacity;

  // No existing key was found, so insert it as a new entry at the head of the
  // list.
  struct cell *new = malloc(sizeof (struct cell) + strlen(key) + 1);
  new->next = m->elems[b];
  new->value = value;
  strcpy(new->key, key);
  m->elems[b] = new;
  m->size += 1;
}

/**
 * Retrieve the value for a given key in a map.
 * 
 * Crashes if the map does not contain the given key.
 */
void *map_get(const map m, const char *key) {
  int b = hash(key) % m->capacity;

  // Search linearly for a matching key through the appropriate linked list.
  for (struct cell *curr = m->elems[b]; curr != NULL; curr = curr->next) {
    if (strcmp(curr->key, key) == 0) return curr->value;
  }

  // Key not found.
  bool key_found = false;
  assert(key_found);
  exit(1);
}

/**
 * Remove a key and its value from a map.
 * 
 * Crashes if the map does not already contain the key.
 */
void *map_remove(map m, const char *key) {
  int b = hash(key) % m->capacity;

  // Here, use a double pointer to make removal easier.
  struct cell **curr;
  for (curr = &m->elems[b]; *curr != NULL; curr = &(*curr)->next) {
    if (strcmp((*curr)->key, key) == 0) {

      // Bridge the linked list accross the removed element.
      struct cell *found = *curr;
      *curr = (*curr)->next;

      void *value = found->value;
      free(found);
      m->size -= 1;
      return value;
    }
  }

  // Key not found.
  bool key_found = false;
  assert(key_found);
  exit(1);
}

/**
 * Get the "first" key (arbitrarily ordered) in a map. If the map is empty,
 * returns NULL.
 */
const char *map_first(map m) {

  // Find and return the first cell in the first non-empty bucket.
  for (int i = 0; i < m->capacity; i += 1) {
    if (m->elems[i] != NULL) {
      return m->elems[i]->key;
    }
  }

  return NULL;
}

/**
 * Get the next key after a given key within a map.
 * 
 * Used for iteration. Returns NULL if there are no more keys. Note that the
 * provided `key` must have been returned from a previous call to `map_first`
 * or `map_next`. Passing other strings produces undefined behavior.
 */
const char *map_next(map m, const char *key) {

  // First, get a reference to the current cell and check its successor.
  struct cell *curr = (void *) (key - sizeof (struct cell));
  if (curr->next != NULL) {
    return curr->next->key;
  }

  // If no immediate successor exists, begin searching the rest of the buckets.
  int b = hash(key) % m->capacity;
  for (int i = b + 1; i < m->capacity; i += 1) {
    if (m->elems[i] != NULL) {
      return m->elems[i]->key;
    }
  }
  
  // No more keys.
  return NULL;
}

/**
 * Internal helper; hash a string key.
 */
static unsigned int hash(const char *key) {
  unsigned int hash = -1;
  while (*key) {
    hash *= 31;
    hash ^= (unsigned char) *key;
    key += 1;
  }
  return hash;
}

/*
 * Grow the capacity of the hash map by a factor of two, only when the map's 
 * load becomes greater than one.
 */
static void extend_if_necessary(map m) {
  if (m->size == m->capacity) {

    // Save old values first, since all map entries will need to be copied over.
    int capacity = m->capacity;
    struct cell **elems = m->elems;

    // Doubling the capacity when necessary allows for an amortized constant 
    // runtime for extension.
    m->capacity *= 2;
    m->elems = calloc(m->capacity, sizeof (struct cell *));

    for (int i = 0; i < capacity; i += 1) {
      struct cell *curr = elems[i];
      while (curr != NULL) {
        struct cell *next = curr->next;

        // Move the entry from the old data structure to the new.
        int b = hash(curr->key) % m->capacity;
        curr->next = m->elems[b];
        m->elems[b] = curr;

        curr = next;
      }
    }

    free(elems);
  }
}
