#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern char *tzname[];

int main()
{
    time_t now;
    struct tm *sp;

    setenv("TZ", "PST8", 1);      // PST = UTC - 8 часов
    tzset();                       // установить часовой пояс

    time(&now);                    // текущее время
    sp = localtime(&now);          // получить время в PST

    printf("%02d/%02d/%d %02d:%02d %s\n",
           sp->tm_mon + 1,
           sp->tm_mday,
           sp->tm_year + 1900,
           sp->tm_hour,
           sp->tm_min,
           tzname[sp->tm_isdst]);

    return 0;
}