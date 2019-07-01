#include "editor.h"
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>

int main() {
	struct Editor* editor = editorCreate(1, 1);
  enableRawMode();
  while (1) {
		editorRefreshScreen(editor);
		editorProcessKeypress(editor);
  }
  return 0;
}