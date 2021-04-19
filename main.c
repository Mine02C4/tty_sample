#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>

static int tty_fd = -1;

void
sigint_handler(int signum)
{
  putchar('\n');
  exit(0);
}

void
sigwinch_handler(int signum)
{
  struct winsize wsize;
  ioctl(tty_fd, TIOCGWINSZ, &wsize);
  printf("Window row = %d, col = %d\r", wsize.ws_row, wsize.ws_col);
  fflush(stdout);
}

int
main(const int argc, const char* argv[])
{
  int fd;
  FILE *f;
  struct winsize wsize;
  struct sigaction act;

  f = fopen( "/dev/tty", "r" );
  tty_fd = fileno(f);
  ioctl(tty_fd, TIOCGWINSZ, &wsize);
  printf("Window row = %d, col = %d\r", wsize.ws_row, wsize.ws_col);
  fflush(stdout);

  sigemptyset(&act.sa_mask);
  act.sa_handler = sigint_handler;
  if (sigaction(SIGINT, &act, NULL) == -1) {
    fprintf(stderr, "Error sigaction: %s\n", strerror(errno));
    return errno;
  }
  act.sa_handler = sigwinch_handler;
  if (sigaction(SIGWINCH, &act, NULL) == -1) {
    fprintf(stderr, "Error sigaction: %s\n", strerror(errno));
    return errno;
  }
  while (pause() == -1) {}
  return 0;
}

