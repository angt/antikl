#include "libhydrogen/hydrogen.c"

#include <errno.h>
#include <stdio.h>
#include <termios.h>

static struct {
    char pass[256];
    struct termios term;
} g = {0};

static void
cleanup(void)
{
    hydro_memzero(g.pass, sizeof(g.pass));
    tcsetattr(0, TCSANOW, &g.term);
}

int
main(int argc, char **argv)
{
    hydro_init();

    char pad[1 * 5 * 19];
    unsigned n = 0, r[5][5][5] = {0};

    for (unsigned i = 0; i < sizeof(pad); i++)
        pad[i] = ' ' + i;

    for (unsigned i = 0; i < sizeof(pad); i++)
        r[(i / 19) % 5][(i * 5 / 19) % 5][i % 5] = i + 1;

    if (tcgetattr(0, &g.term))
        return 1;

    atexit(cleanup);

    struct termios term = g.term;
    term.c_lflag &= ~(ICANON | ECHO);
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;
    tcsetattr(0, TCSAFLUSH, &term);

    while (1) {
        for (unsigned i = sizeof(pad) - 1; i; i--) {
            unsigned j = hydro_random_uniform(i + 1);
            char tmp = pad[i];
            pad[i] = pad[j];
            pad[j] = tmp;
        }
        unsigned x[3] = {0};
        for (unsigned k = 0; k < 3; k++) {
            fprintf(stderr, "\r");
            for (unsigned i = 0; i < sizeof(pad); i++)
                fprintf(stderr, "\033[9%um%s\033[m",
                        (i * (unsigned[]){1, 5, 19}[k] / 19) % 5 + 1,
                        pad[i] == ' ' ? "␣" : (char[2]){pad[i]});
            char c;
            while (read(0, &c, 1) != 1);
            switch (c) {
                case 'm': x[k] = 4; break;
                case 'b': x[k] = 3; break;
                case 'y': x[k] = 2; break;
                case 'g': x[k] = 1; break;
                case 'r': x[k] = 0; break;
                case '\x7f': goto next;
                default:     goto end;
            }
        }
        unsigned i = r[x[0]][x[1]][x[2]];
        if (i)
            g.pass[n++] = pad[i - 1];
        if (n == sizeof(g.pass))
            return 2;
    next:
        ;
    }
end:
    g.pass[n] = 0;
    fprintf(stderr, "\033[2K\r");
    printf("%s\n", g.pass);
}
