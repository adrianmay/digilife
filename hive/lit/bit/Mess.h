typedef struct MessPayload {
  // Empty
} MessPayload;

typedef struct Mess {
  char type;
  int serial;
  MessPayload payload;
} Mess;


