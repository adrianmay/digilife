#pragma once

typedef struct {
} MessPayload;

typedef struct {
  char type;
  int serial;
  MessPayload payload;
} MessTicket;


void showMessTicket(MessTicket * p);

