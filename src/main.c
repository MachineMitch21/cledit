#include "editor.h"
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>

int main() {
  enableRawMode();
  while (1) {
    char c = '\0';
    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
    if (iscntrl(c)) {
      printf("%d\r\n", c);
    } else {
      printf("%d ('%c')\r\n", c, c);
    }
    if (c == 'q') break;
  }
  disableRawMode();
  return 0;
}