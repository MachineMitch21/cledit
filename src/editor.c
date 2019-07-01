#include "editor.h"

#include <termios.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include "cdefs.h"

struct termios orig_termios;

struct Editor {
	char cursor_col;
	char cursor_row;
};

/* Internal functions */
char editorReadKey(struct Editor* editor) {
	int nread;
	char c;
	while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
		if (nread == -1 && errno != EAGAIN) die("read");
	}
	return c;
}

void editorCursorPosEscSeq(char cursor_col, char cursor_row) {
	char buffer[32];
	snprintf(buffer, 32, "\x1b[%d;%dH", cursor_col, cursor_row);
	write(STDOUT_FILENO, buffer, strlen(buffer));
}

void editorDrawRows() {
	int y;
	for (y = 0; y < 24; y++) {
		write(STDOUT_FILENO, "~\r\n", 3);
	}
}

/* API functions */
struct Editor* editorCreate(char cursor_col_pos, char cursor_row_pos) {
	struct Editor* e = malloc(sizeof(struct Editor));
	editorSetCursorPos(e, cursor_col_pos, cursor_row_pos);
	return e;
}

void editorFree(struct Editor* editor) {
	free(editor);
}

void editorSetCursorPos(struct Editor* editor, char cursor_col, char cursor_row) {
	editor->cursor_col = (cursor_col < 1 ? 1 : cursor_col);
	editor->cursor_row = (cursor_row < 1 ? 1 : cursor_row);
	editorCursorPosEscSeq(cursor_col, cursor_row);
}

int editorGetCursorPos(struct Editor* editor) {
	int cursor_pos = editor->cursor_col;
	cursor_pos << 8;
	cursor_pos |= editor->cursor_row;
	return cursor_pos;
}

void die(const char* s) {
	editorClearScreen();
  perror(s);
  exit(1);
}

void enableRawMode(void) {
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
  atexit(disableRawMode);
  struct termios raw = orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag &= ~(CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

void disableRawMode(void) {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) die("tcsetattr");
}

void editorProcessKeypress(struct Editor* editor) {
	char c = editorReadKey(editor);

	switch (c) {
		case CTRL_KEY('q'):
			editorClearScreen();
			exit(0);
			break;
	}
}

void editorRefreshScreen(struct Editor* editor) {
	write(STDOUT_FILENO, "\x1b[2J", 4);
	char col = (editor != NULL ? editor->cursor_col : 1);
	char row = (editor != NULL ? editor->cursor_row : 1);
	editorCursorPosEscSeq(col, row);
	editorDrawRows();
	editorCursorPosEscSeq(col, row);
}

void editorClearScreen() {
	write(STDOUT_FILENO, "\x1b[2J", 4);
	editorCursorPosEscSeq(1, 1);
}