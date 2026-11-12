#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int my_execvpe(const char *file, char *const argv[], char *const envp[]) {
    if (strchr(file, '/'))
        return execve(file, argv, envp);

    const char *path = "/bin:/usr/bin";
    for (int i = 0; envp[i] != NULL; i++) {
        if (strncmp(envp[i], "PATH=", 5) == 0) {
            path = envp[i] + 5;
            break;
        }
    }

    char *paths = strdup(path);
    if (paths == NULL)
        return -1;

    char *p = paths;
    while (p != NULL) {
        char *end = strchr(p, ':');
        if (end != NULL)
            *end = '\0';

        char full[4096];
        int n = snprintf(full, sizeof(full), "%s%s%s",
                         *p ? p : ".", "/", file);
        if (n < 0 || n >= (int)sizeof(full)) {
            free(paths);
            errno = ENAMETOOLONG;
            return -1;
        }
        execve(full, argv, envp);

        if (errno != ENOENT && errno != ENOTDIR && errno != EACCES) {
            int err = errno;
            free(paths);
            errno = err;
            return -1;
        }
        p = end == NULL ? NULL : end + 1;
    }
    free(paths);
    return -1;
}

int main(void) {
    char *args[] = {"printenv", NULL};
    char *env[] = {"MY_VAR=Hello", "PATH=/usr/bin:/bin", NULL};
    my_execvpe("printenv", args, env);
    perror("my_execvpe");
    return 1;
}
