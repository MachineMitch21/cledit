#ifndef EDITOR_BUFFER_H
#define EDITOR_BUFFER_H

struct editor_buffer {
  char* buf;
  int   len;
};

#define EDITOR_BUFFER_INIT (struct editor_buffer) { NULL, 0 }

void ebAppend(struct editor_buffer* eb, const char* s, int len);
void ebFree(struct editor_buffer* eb);

#endif 