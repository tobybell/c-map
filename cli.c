#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"

#define MAX_LINE 80

/**
 * Read at most `MAX_LINE` characters (plus terminal newline) from `stdin` into
 * the buffer `dest`. If the line is too long, will reprompt automatically.
 * Returns a bool indicating whether `stdin` is still open or not.
 */
bool read_cmd(char *dest) {

  while (true) {

    // Return false when input has been closed.
    if (feof(stdin)) return false;

    printf("> ");

    // Process at most MAX_LINE characters from stdin.
    for (int i = 0; i <= MAX_LINE; i += 1) {
      char ch = getchar();
      if (ch == '\n' || ch == '\0') {
        dest[i] = '\0';
        return true;
      }
      dest[i] = ch;
    }

    // We overran the max line length. Consume the rest of the line and
    // reprompt.
    for (char ch = getchar(); ch != '\n' && ch != '\0'; ch = getchar());
    printf("    error; line too long (> 80)\n");
  }
}

/**
 * Clean up an existing map's memory, freeing both the map itself and its
 * contents.
 */
void do_cleanup(map m) {
  if (m != NULL) {
    // TODO
    map_destroy(m);
  }
}

/**
 * Make sure a map has been initialized before trying to operate on it. If `m`
 * is NULL, will print out an error message suggesting the `init` command and
 * then return false. Otherwise, returns true.
 */
bool ensure_exists(map m) {
  if (m != NULL) return true;
  printf("    error; use `init` first to initialize a new empty map\n");
  return false;
}

/**
 * Parse a line that should not contain any other arguments. Returns
 * `true`/`false` to indicate success.
 */
bool parse(char *line, char *cmd) {
  if (strtok(NULL, " ") != NULL) {
    printf("    error; use format `%s`\n", cmd);
    return false;
  }
  return true;
}

/**
 * Parse a line with a single token argument. The found token string 
 * (dynamically allocated) will be placed into `tok`. Returns `true`/`false` to
 * indicate success.
 */
bool parse_s(char *line, char *cmd, char **tok) {
  char *token = strtok(NULL, " ");
  if (token == NULL || strtok(NULL, " ") != NULL) {
    printf("    error; use format `%s %%[^ ]`\n", cmd);
    return false;
  }
  *tok = strdup(token);
  return true;
}

/**
 * Parse a line with two token arguments. The found token strings (both
 * dynamically allocated) will be placed into `tok1` and `tok2`. Returns
 * `true`/`false` to indicate success.
 */
bool parse_ss(char *line, char *cmd, char **tok1, char **tok2) {
  char *token1 = strtok(NULL, " ");
  char *token2 = strtok(NULL, " ");
  if (token2 == NULL || strtok(NULL, " ") != NULL) {
    printf("    error; use format `%s %%[^ ] %%[^ ]`\n", cmd);
    return false;
  }
  *tok1 = strdup(token1);
  *tok2 = strdup(token2);
  return true;
}

/**
 * Accepts a command string `command` and runs the correct routine.
 */
void run_cmd(char *line) {

  // Stores the map manipulated by the shell.
  static map m = NULL;

  // Extract the command from the string using `strtok`.
  char *cmd = strtok(line, " ");
  if (cmd == NULL) return;

  // Command: `help`. List commands.
  if (strcmp(cmd, "help") == 0) {
    if (!parse(line, cmd)) return;
    printf("    help               List available commands\n");
    printf("    exit/quit/q        Exit map shell\n");
    printf("    init               Initialize new empty map\n");
    printf("    size               Get current map size\n");
    printf("    ls/print/dump      Get all map contents\n");
    printf("    contains <key>     Check if map contains <key>\n");
    printf("    set <key> <value>  Set <value> for <key>\n");
    printf("    get <key>          Get the value for <key>\n");
    printf("    remove <i>         Remove the value for <key>\n");
  }

  // Command: `exit`, `quit`. Closes the shell.
  else if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "quit") == 0 ||
      strcmp(cmd, "q") == 0) {
    if (!parse(line, cmd)) return;
    exit(0);
  }

  // Command: `init`. Creates a new, empty map.
  else if (strcmp(cmd, "init") == 0) {
    if (!parse(line, cmd)) return;
    do_cleanup(m);
    m = map_create();
    printf("    m = {}\n");
  }

  // Command: `size`. Gets the current size of the map.
  else if (strcmp(cmd, "size") == 0) {
    if (!parse(line, cmd)) return;
    if (!ensure_exists(m)) return;
    printf("    |m| = %d\n", map_size(m));
  }

  // Command: `ls`, `dump`, `print`. Prints the full map contents.
  else if (strcmp(cmd, "ls") == 0 || strcmp(cmd, "dump") == 0 || 
      strcmp(cmd, "print") == 0) {
    if (!parse(line, cmd)) return;
    if (!ensure_exists(m)) return;

    printf("    m = {");
    const char *key = map_first(m);
    if (key != NULL) {
      const char *value = map_get(m, key);
      printf("%s:%s", key, value);
      for (key = map_next(m, key); key != NULL; key = map_next(m, key)) {
        value = map_get(m, key);
        printf(", %s:%s", key, value);
      }
    }
    printf("}\n");
  }

  // Command: `contains %[^ ]`. Check if map contains a key.
  else if (strcmp(cmd, "contains") == 0) {
    char *key;
    if (!parse_s(line, cmd, &key)) return;
    if (!ensure_exists(m)) return;

    if (map_contains(m, key)) {
      printf("    true\n");
    } else {
      printf("    false\n");
    }
    free(key);
  }

  // Command: `set %[^ ] %[^ ]`. Set a new value for a key.
  else if (strcmp(cmd, "set") == 0) {
    char *key;
    char *value;
    if (!parse_ss(line, cmd, &key, &value)) return;
    if (!ensure_exists(m)) return;

    map_set(m, key, value);
    printf("    m[%s] = %s\n", key, value);
    free(key);
  }

  // Command: `get %d`. Prints the value for a given key.
  else if (strcmp(cmd, "get") == 0) {
    char *key;
    if (!parse_s(line, cmd, &key)) return;
    if (!ensure_exists(m)) return;

    // Cannot get a nonexistent key.
    if (!map_contains(m, key)) {
      printf("    error; key not found\n");
    } else {
      char *value = map_get(m, key);
      printf("    m[%s] = %s\n", key, value);
    }
    free(key);
  }

  // Command: `remove %[^ ]`. Remove the entry with a given key.
  else if (strcmp(cmd, "remove") == 0) {
    char *key;
    if (!parse_s(line, cmd, &key)) return;
    if (!ensure_exists(m)) return;

    // Cannot remove nonexistent key.
    if (!map_contains(m, key)) {
      printf("    error; key not found\n");
    } else {
      char *value = map_remove(m, key);
      printf("    # m[%s] = %s\n", key, value);
      free(value);
    }
    free(key);
  }

  else {
    printf("    error; unknown command (%s)\n", cmd);
  }
}

int main() {

  printf("Map CLI; use `help` if you are totally lost.\n");

  char command[MAX_LINE + 1];
  while (read_cmd(command)) {
    run_cmd(command);
  }

  return 0;
}
