
#define DATA_DIR_MAX 256

typedef struct {
  char dataDir[DATA_DIR_MAX];
  int numWorkers;
} Args;

int parseArgs(int argc, char **argv);

const char * getDataDir();
int getNumWorkers();
