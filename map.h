#ifndef __MAP_H
#define __MAP_H

#include <stdbool.h>

/**
 * Hash map implementation for C.
 * 
 * This hash map uses strings as keys, and allows association of any arbitrary
 * value type through the use of `void *` pointers. Additionally, this
 * implementation leaves memory management of stored values to the client (for 
 * example, destroying a map using `map_destroy` will free the memory for the
 * map itself, but it will not free memory that was used to store its values).
 */
typedef struct map *map;

/**
 * Create a new, empty map.
 * 
 * The returned map has dynamically allocated memory associated with it, and
 * this memory must be reclaimed after use with `map_destroy`.
 */
map map_create();

/**
 * Free the memory used for a map after use.
 * 
 * Note that this routine does not free any memory that was allocated for the
 * values stored in the map. That memory must be freed by the client as
 * appropriate.
 */
void map_destroy(map m);

/**
 * Get the size of a map.
 */
int map_size(const map m);

/**
 * Determine whether a map contains a given key.
 * 
 * Keys are case-sensitive.
 */
bool map_contains(const map m, const char *key);

/**
 * Set the value for a given key within a map.
 * 
 * This will add a new key if it does not exist. If the key already exists, the
 * new value will replace the old one.
 */
void map_set(map m, const char *key, void *value);

/**
 * Retrieve the value for a given key in a map.
 * 
 * Crashes if the map does not contain the given key.
 */
void *map_get(const map m, const char *key);

/**
 * Remove a key and return its value from a map.
 * 
 * Crashes if the map does not already contain the key.
 */
void *map_remove(map m, const char *key);

/**
 * Iterate over a map's keys.
 * 
 * Usage:
 * 
 * for (char *key = map_first(m); key != NULL; key = map_next(m, key)) {
 *   ...
 * }
 * 
 * Note that the `key` passed to `map_next` must have come from a previous call
 * to `map_first` or `map_next`. Passing strings from other sources produces
 * undefined behavior.
 */
const char *map_first(map m);
const char *map_next(map m, const char *key);

#endif
