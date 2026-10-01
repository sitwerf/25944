#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/resource.h>
#include <ulimit.h>

extern char **environ;

int main(int argc, char *argv[])
{
    int opt, n = 0;
    int options[argc];
    char *args[argc];

    opterr = 0; //отключаем стандартные сообщения getopt

    if (argc == 1) {
    printf("Usage: %s [-i] [-s] [-p] [-u] [-Unew_ulimit] "
           "[-c] [-Csize] [-d] [-v] [-Vname=value]\n", argv[0]);
    return 0;
}

    //считываем и сохраняем все опции
    while ((opt = getopt(argc, argv, ":ispuU:cC:dvV:")) != -1) {

        if (opt == '?') {
            printf("No option\n");
            return 1;
        }

        if (opt == ':') {
            printf("Option -%c needs an argument\n", optopt);
            return 1;
        }

        options[n] = opt;
        args[n] = optarg;
        n++;
    }

    if (optind < argc) {
        printf("No option\n");
        return 1;
    }

    //выполняем опции справа налево
    for (int i = n - 1; i >= 0; i--) {

        switch (options[i]) {

        case 'i':
            // Реальные и эффективные UID/GID
            printf("UID:  %d\n", (int)getuid());
            printf("EUID: %d\n", (int)geteuid());
            printf("GID:  %d\n", (int)getgid());
            printf("EGID: %d\n", (int)getegid());
            break;

        case 's':               //делаем процесс лидером группы
            if (setpgid(0, 0) == -1)
                perror("setpgid");
            break;

        case 'p':
            // PID, PPID и ID группы процессов
            printf("PID:  %d\n", (int)getpid());
            printf("PPID: %d\n", (int)getppid());
            printf("PGID: %d\n", (int)getpgrp());
            break;

        case 'u': {
            long max_args = sysconf(_SC_ARG_MAX);

            if (max_args == -1)
                perror("sysconf");
            else
                printf("Maximum arguments size: %ld bytes\n", max_args);

            break;
        }

        case 'U': {
            char *end;

            long value = strtol(args[i], &end, 10);      //переводим аргумент из строки в число

            if (*args[i] == '\0' || *end != '\0' || value < 0) {
                printf("Bad value for -U: %s\n", args[i]);
                break;
            }

            if (ulimit(UL_SETFSIZE, value) == -1)
                perror("ulimit");

            break;
        }

        case 'c': {
            struct rlimit r;

            if (getrlimit(RLIMIT_CORE, &r) == -1)   //получаем ограничение размера core-файла
                perror("getrlimit");
            else if (r.rlim_cur == RLIM_INFINITY)
                printf("Core size: unlimited\n");
            else
                printf("Core size: %llu bytes\n",
                       (unsigned long long)r.rlim_cur);

            break;
        }

        case 'C': {
            struct rlimit r;
            char *end;
            unsigned long long size;

            if (args[i][0] == '-') {
                printf("Bad value for -C: %s\n", args[i]);
                break;
            }

            size = strtoull(args[i], &end, 10);

            if (*args[i] == '\0' || *end != '\0') {
                printf("Bad value for -C: %s\n", args[i]);
                break;
            }

            if (getrlimit(RLIMIT_CORE, &r) == -1) {
                perror("getrlimit");
                break;
            }

            r.rlim_cur = (rlim_t)size; // новый размер core-файла

            if (setrlimit(RLIMIT_CORE, &r) == -1)
                perror("setrlimit");

            break;
        }

        case 'd': {
            char dir[1024];

            // Получаем текущую директорию
            if (getcwd(dir, sizeof(dir)) != NULL)
                printf("%s\n", dir);
            else
                perror("getcwd");

            break;
        }

        case 'v':
            // Выводим все переменные окружения
            for (char **env = environ; *env != NULL; env++)
                printf("%s\n", *env);
            break;

        case 'V': {
            char *eq = strchr(args[i], '=');

            if (eq == NULL || eq == args[i]) {
                printf("Use -Vname=value\n");
                break;
            }

            // Делим строку name=value на имя и значение
            *eq = '\0';
            setenv(args[i], eq + 1, 1);
            *eq = '=';

            break;
        }
        }
    }

    return 0;
}