# C Map

A simple, bucket-based hash map implementation for C that uses string keys. Additionally, includes a basic command line interface for interacting with the map.

## Using It

To clone this repository, run

    $ git clone https://github.com/tobinbell/c-map.git

To build the provided map object and command line interface, run

    $ cd c-map
    $ make

You can then interact with the C Map data structure through the provided command line interface by running

    $ ./map-cli

### Example Usage

#### CLI

    $ ./map-cli
    Map CLI; use `help` if you are totally lost.
    > init
        m = {}
    > set apple pie
        m[apple] = pie
    > set orange juice
        m[orange] = juice
    > print
        m = {orange: juice, apple: pie}
    > q

#### In C

    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include "map.h"
    
    int main() {
    
      // Initialize a new map.
      map m = map_create();
      
      // Read in values from the user.
      printf("Enter some strings. When finished, type \"done\".\n");
      char buf[4096];
      while (true) {
        scanf(" %4095s", buf);
        if (strcmp(buf, "done") == 0) break;

        if (map_contains(m, buf)) {
            *(int *) map_get(m, buf) += 1;
        } else {
            int *n = malloc(sizeof (int));
            *n = 1;
            map_set(m, buf, n);
        }
      }
      
      printf("You entered %d unique strings:\n", map_size(m));
      for (const char *key = map_first(m); key != NULL; key = map_next(m, key)) {
        printf(" %s: %d time(s)\n", key, *(int *) map_get(m, key));
      }
      
      // Free map memory when done, including client values.
      for (const char *key = map_first(m); key != NULL; key = map_next(m, key)) {
        free(map_get(m, key));
      }
      map_destroy(v);
      
      return 0;
    }

## Design and Performance

C Vector uses a dynamically allocated array to store its contents. Dynamic growth is achieved via doubling of this array when necessary. As more elements are added to the vector and extension becomes more expensive, the doubling operation ensures that extensions also become less frequent, producing *O*(1) ammortized append time. Inserting and removing elements from the interior of the vector is made possible by shifting the latter portion of the array up or down accordingly, producing *O*(*n*) runtime for non-posterior insertion and removal operations.

A complete description of this data structure's runtime is given below.

| Operation | Runtime  |
|-----------|----------|
| Size      | *O*(1)   |
| Get       | *O*(1)   |
| Set       | *O*(1)   |
| Push      | *O*(1)   |
| Pop       | *O*(1)   |
| Insert    | *O*(*n*) |
| Remove    | *O*(*n*) |

## Notes

As described in *Design* above, C Vector does not store data values internally, but rather by reference. Thus, operations on a C Vector accept and return `void *` pointers, and no client data is explicitly copied by the vector. This means that client code must take responsibility for storing values, either "dyanimcally" (no pun intended) using `malloc` or in some other data structure.

Clients are also solely responsible for managing the memory holding stored values. Even when a C Vector is destroyed (using `vector_destroy`), only its own memory will be freed, and not any memory referenced by the client pointers it stores. Therefore, clients should take care to explicitly free all value memory as appropriate before destroying a C Vector.
