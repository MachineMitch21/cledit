#include "editor.h"

#include <termios.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <sys/ioctl.h>

#include "cdefs.h"
#include "editor_buffer.h"

#define CLEDIT_VERSION "0.0.1"

enum editorKey {
  ARROW_LEFT = 1000,
  ARROW_RIGHT,
  ARROW_UP,
  ARROW_DOWN,
  DEL_KEY,
  HOME_KEY,
  END_KEY,
  PAGE_UP,
  PAGE_DOWN
};

struct Editor {
  struct  termios orig_termios;
  struct  editor_buffer eb;
  int     cx, cy;
  int     screen_cols;
  int     screen_rows;
};

/* Internal functions */
int _editorReadKey(struct Editor* editor) {
	int nread;
	char c;
	while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
		if (nread == -1 && errno != EAGAIN) die("read");
	}

  if (c == '\x1b') {
    char seq[3];

    if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
    if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';

    if (seq[0] == '[') {
      if (seq[1] >= '0' && seq[1] <= '9') {
        if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
        if (seq[2] == '~') {
          switch (seq[1]) {
            case '1': return HOME_KEY;
            case '3': return DEL_KEY;
            case '4': return END_KEY;
            case '5': return PAGE_UP;
            case '6': return PAGE_DOWN;
            case '7': return HOME_KEY;
            case '8': return END_KEY;
          }
        }
      } else {
        switch (seq[1]) {
          case 'A': return ARROW_UP;
          case 'B': return ARROW_DOWN;
          case 'C': return ARROW_RIGHT;
          case 'D': return ARROW_LEFT;
          case 'H': return HOME_KEY;
          case 'F': return END_KEY;
        }
      }
    } else if (seq[0] == 'O') {
      switch (seq[1]) {
        case 'H': return HOME_KEY;
        case 'F': return END_KEY;
      }
    }

    return '\x1b';
  } else {
	  return c;
  }
}

void _editorSetCursorPosEscSeqFull(struct Editor* editor, int cursor_col, int cursor_row) {
	char buffer[32];
	snprintf(buffer, 32, "\x1b[%d;%dH", 
    (editor != NULL ? editor->cy : cursor_row) + 1, 
    (editor != NULL ? editor->cx : cursor_col) + 1
  );
  if (editor != NULL) {
	  ebAppend(&editor->eb, buffer, strlen(buffer));
  } else {
    write(STDOUT_FILENO, buffer, strlen(buffer));
  }
}

void _editorSetCursorPosEscSeqNull(int cursor_col, int cursor_row) {
  _editorSetCursorPosEscSeqFull(NULL, cursor_col, cursor_row);
}

void _editorSetCursorPosEscSeqDirect(struct Editor* editor) {
  if (editor == NULL) die("_editorSetCursorPosEscSeqDirect");
  _editorSetCursorPosEscSeqFull(editor, 0, 0);
}

void _editorMoveCursor(struct Editor* editor, int key) {
  switch (key) {
    case ARROW_DOWN: 
      if (editor->cy != editor->screen_rows - 1) 
        editor->cy++;
      break;
    case ARROW_UP: 
      if (editor->cy != 0) 
        editor->cy--;
      break;
    case ARROW_RIGHT:
      if (editor->cx != editor->screen_cols - 1)
        editor->cx++;
      break;
    case ARROW_LEFT: 
      if (editor->cx != 0) 
        editor->cx--;
      break;
  }
}

void _editorDrawRows(struct Editor* editor) {
	int y;
	for (y = 0; y < editor->screen_rows; y++) {
    if (y == editor->screen_rows / 3) {
      char welcome[80];
      int welcomelen = snprintf(welcome, sizeof(welcome),
        "CL Edit -- version %s", CLEDIT_VERSION);
      if (welcomelen > editor->screen_cols) welcomelen = editor->screen_cols;
      int padding = (editor->screen_rows - welcomelen) / 2;
      if (padding) {
        ebAppend(&editor->eb, "~", 1);
        padding--;
      }
      while (padding--) ebAppend(&editor->eb, " ", 1);
      ebAppend(&editor->eb, welcome, welcomelen);
    } else {
      ebAppend(&editor->eb, "~", 1);
    }

    ebAppend(&editor->eb, "\x1b[K", 3);
    if (y < editor->screen_rows - 1) {
      ebAppend(&editor->eb, "\r\n", 2);
    }
	}
}

void _disableRawModeExitCb(int code, struct Editor* editor) {
  disableRawMode(editor);
}

/* API functions */
struct Editor* editorCreate(char cursor_col_pos, char cursor_row_pos) {
	struct Editor* e = malloc(sizeof(struct Editor));
  if (getWindowSize(&e->screen_cols, &e->screen_rows) == -1) die("getWindowSize");
  e->eb = EDITOR_BUFFER_INIT;
  e->cx = 0;
  e->cy = 0;
	return e;
}

void editorFree(struct Editor* editor) {
	free(editor);
}

int getCursorPos(int* cols, int* rows) {
  char buf[32];
  unsigned int i = 0;

  if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;

  while (i < sizeof(buf) - 1) {
    if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
    if (buf[i] == 'R') break;
    i++;
  }
  buf[i] = '\0';
 
  if (buf[0] != '\x1b' || buf[1] != '[') return -1;
  if (sscanf(&buf[2], "%d;%d", rows, cols) != 2) return -1;

  return 0;
}

int getWindowSize(int* cols, int* rows) {
  struct winsize ws;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
    return getCursorPos(cols, rows);
  } else {
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
  }
}

void die(const char* s) {
	editorClearScreen(NULL);
  perror(s);
  exit(1);
}

void enableRawMode(struct Editor* editor) {
  if (tcgetattr(STDIN_FILENO, &editor->orig_termios) == -1) die("tcgetattr");
  on_exit(_disableRawModeExitCb, editor);
  struct termios raw = editor->orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag &= ~(CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

void disableRawMode(struct Editor* editor) {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &editor->orig_termios) == -1) die("tcsetattr");
}

void editorProcessKeypress(struct Editor* editor) {
	int c = _editorReadKey(editor);

	switch (c) {
		case CTRL_KEY('q'):
			editorClearScreen(NULL);
			exit(0);
			break;

    case PAGE_UP:
    case PAGE_DOWN:
    {
      int times = editor->screen_rows;
      while (times--) {
        _editorMoveCursor(editor, c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
      }
    }
    break;

    case HOME_KEY:
      editor->cx = 0;
      break;
    case END_KEY:
      editor->cx = editor->screen_cols - 1;
      break;

    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_LEFT:
    case ARROW_RIGHT:
      _editorMoveCursor(editor, c);
      break;
	}
}

void editorRefreshScreen(struct Editor* editor) {
  editor->eb = EDITOR_BUFFER_INIT;
  ebAppend(&editor->eb, "\x1b[?25l", 6);
  _editorSetCursorPosEscSeqNull(0, 0);
	_editorDrawRows(editor);
	_editorSetCursorPosEscSeqDirect(editor);
  ebAppend(&editor->eb, "\x1b[?25h", 6);
  write(STDOUT_FILENO, editor->eb.buf, editor->eb.len);
  ebFree(&editor->eb);
}

void editorClearScreen(struct Editor* editor) {
  if (editor != NULL) {
	  ebAppend(&editor->eb, "\x1b[2J", 4);
  } else {
    write(STDOUT_FILENO, "\x1b[2J", 4);
  }
	_editorSetCursorPosEscSeqNull(0, 0);
}