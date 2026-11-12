
#include <stdio.h>
#include <unistd.h>
#include <termios.h>

int main() {
    struct termios old, new;
    char c;

    if (tcgetattr(0, &old) == -1) {
        perror("tcgetattr");
        return 1;
    }

    new = old;
    new.c_lflag &= ~(ICANON | ECHO);
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;

    if (tcsetattr(0, TCSANOW, &new) == -1) {
        perror("tcsetattr");
        return 1;
    }

    printf("Нравится Mac? (y/n): ");
    fflush(stdout);

    if (read(0, &c, 1) != 1) {
        tcsetattr(0, TCSANOW, &old);
        perror("read");
        return 1;
    }

    if (tcsetattr(0, TCSANOW, &old) == -1) {
        perror("tcsetattr");
        return 1;
    }

    printf("%c\n", c);

    if (c == 'y') {
        printf("Ответ: Да\n");
    }
    else if (c == 'n') {
        printf("Ответ: Нет\n");
    }
    else {
        printf("Нет такого варианта ответа!\n");
    }

    return 0;
}
