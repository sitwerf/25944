#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 1024

struct Node {
    char *str;
    struct Node *next;
};

int main()
{
    char buffer[MAX_LEN];

    struct Node *head = NULL;
    struct Node *tail = NULL;

    printf("Enter strings:\n");

    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {

        if (buffer[0] == '.')           //ввод заканчивается, если первый символ строки - точка
            break;

        size_t len = strlen(buffer);

        struct Node *new_node = malloc(sizeof(struct Node));          //выделяем память под новый узел

        if (new_node == NULL) {
            perror("malloc");
            return 1;
        }

        new_node->str = malloc(len + 1);            // +1 для завершающего символа '\0'

        if (new_node->str == NULL) {
            perror("malloc");
            free(new_node);
            return 1;
        }

        // Копируем строку из buffer в динамическую память
        strcpy(new_node->str, buffer);

        //новый элемент пока последний в списке
        new_node->next = NULL;

        if (head == NULL) {             //если список пустой
            head = new_node;
            tail = new_node;
        }
        else {                  //добавляем новый элемент в конец списка
            tail->next = new_node;
            tail = new_node;
        }
    }

    printf("\nStrings:\n");

    //выводим все строки из списка
    struct Node *current = head;

    while (current != NULL) {
        printf("%s", current->str);
        current = current->next;
    }

    //освобождаем выделенную память
    current = head;

    while (current != NULL) {
        struct Node *next = current->next;

        free(current->str);
        free(current);

        current = next;
    }

    return 0;
}