#define _GNU_SOURCE
#include <argp.h>
#include <stdlib.h>
#include <string.h>
#include "h.h"

static Args args = {
  .dataDir = "",
  .numWorkers = 24,  
};

static struct argp_option options[] = {
  {"workers", 'w', "NUM", 0, "How many worker threads"},
  {0}
};

const char *argp_program_version = "hive 1.0";
const char *argp_program_bug_address = "<adrian.alexander.may@gmail.com>";

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  Args *a = state->input;

  switch (key) {
    case 'w': //positional argument
      args.numWorkers = atoi(arg);
      break;
    case ARGP_KEY_ARG: //positional argument
      if (*args.dataDir)
        argp_usage(state);  // prints usage + exits
      strncpy(a->dataDir, arg, DATA_DIR_MAX);
      break;
    case ARGP_KEY_END:
      /* REQUIRED OPTION CHECK */
      if (!*args.dataDir) 
        argp_usage(state);  // prints usage + exits
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = {
  options,
  parse_opt,
  "<DATA_DIR>",
  "Digital Life VM"
};

int parseArgs(int argc, char **argv) {
  char c = argp_parse(&argp, argc, argv, 0, 0, &args);
  return c;
}

const char * getDataDir()    { return args.dataDir;    }
int          getNumWorkers() { return args.numWorkers; }
