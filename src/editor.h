#ifndef EDITOR_H
#define EDITOR_H

#include <unistd.h>

#define CTRL_KEY(k) ((k) & 0x1f)

struct Editor;

void die(const char* s);
void enableRawMode(struct Editor* editor);
void disableRawMode(struct Editor* editor);

struct Editor* editorCreate(char cursor_col_pos, char cursor_row_pos);
void editorFree(struct Editor* editor);

void editorProcessKeypress(struct Editor* editor);
void editorRefreshScreen(struct Editor* editor);
void editorClearScreen();

int getCursorPos(int* cols, int* rows);
int getWindowSize(int* cols, int* rows);

#endif 