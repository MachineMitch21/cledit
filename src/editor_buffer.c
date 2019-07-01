#include <string.h>
#include <stdlib.h>
#include "editor_buffer.h"

void ebAppend(struct editor_buffer* eb, const char* s, int len) {
  char* buf = realloc(eb->buf, eb->len + len);

  if (buf == NULL) return;
  memcpy(&buf[eb->len], s, len);
  eb->buf = buf;
  eb->len += len;
}

void ebFree(struct editor_buffer* eb) {
  free(eb->buf);
}