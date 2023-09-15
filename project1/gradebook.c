#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gradebook.h"

// This is the (somewhat famous) djb2 hash
unsigned hash(const char *str) {
    unsigned hash_val = 5381;
    int i = 0;
    while (str[i] != '\0') {
        hash_val = ((hash_val << 5) + hash_val) + str[i];
        i++;
    }
    return hash_val % NUM_BUCKETS;
}

gradebook_t *create_gradebook(const char *class_name) {
    gradebook_t *new_book = malloc(sizeof(gradebook_t));
    if (new_book == NULL) {
        return NULL;
    }

    strcpy(new_book->class_name, class_name);
    for (int i = 0; i < NUM_BUCKETS; i++) {
        new_book->buckets[i] = NULL;
    }
    new_book->size = 0;

    return new_book;
}

const char *get_gradebook_name(const gradebook_t *book) {
    // TODO Not yet implemented
    return book->class_name;
}

int add_score(gradebook_t *book, const char *name, int score) {
    // TODO Not yet implemented
    if(score < 0) {
        return -1;
    }
    else {
        int hashnum = hash(name);
        if(book->buckets[hashnum] == NULL) {
            book->buckets[hashnum] = malloc(sizeof(node_t));
            strcpy(book->buckets[hashnum]->name, name);
            book->buckets[hashnum]->score = score;
            book->buckets[hashnum]->next = NULL;
            book->size++;
            return 0;
        }
        else {
            node_t *ptr = book->buckets[hashnum];
            while(ptr->next != NULL) {
                if(strcmp(ptr->name, name) == 0) {
                    ptr->score = score;
                    return 0;
                }
                ptr = ptr->next;
            }
            ptr->next = malloc(sizeof(node_t));
            strcpy(ptr->next->name, name);
            ptr->next->score = score;
            ptr->next->next = NULL; 
            book->size++;
            return 0;
        }
    }
}

int find_score(const gradebook_t *book, const char *name) {
    // TODO Not yet implemented
    node_t *ptr = book->buckets[hash(name)];
    if(ptr == NULL) {
        return -1;
    }
    else {
        while(strcmp(ptr->name, name) != 0 && ptr->next != NULL) {
            ptr = ptr->next;
        }
        if(strcmp(ptr->name, name) == 0 ) {
            return ptr->score;
        }
        return -1;
    }
}

void print_gradebook(const gradebook_t *book) {
    // TODO Not yet implemented
    printf("Scores for all students in %s:\n", book->class_name);
    int count = 0;
    for (int i = 0; i < NUM_BUCKETS && count < book->size; i++) {
        node_t *current = book->buckets[i];
        while (current != NULL) {
            printf("%s: %d\n", current->name, current->score);
            count++;
            current = current->next;
        }
    }
}

void free_gradebook(gradebook_t *book) {
    // TODO Not yet implemented
    int count = 0;
    for(int i = 0; i < NUM_BUCKETS && count < book->size; i++) { 
        node_t *head = book->buckets[i];
        if(head == NULL) {
            continue;
        }
        else {
            while(head != NULL) {
                node_t *ptr = head->next;
                free(head);
                count++;
                head = ptr;
            }
        }
    }
    free(book);
}

int write_gradebook_to_text(const gradebook_t *book) {
    char file_name[MAX_NAME_LEN + strlen(".txt")];
    strcpy(file_name, book->class_name);
    strcat(file_name, ".txt");
    FILE *f = fopen(file_name, "w");
    if (f == NULL) {
        return -1;
    }

    fprintf(f, "%u\n", book->size);
    for (int i = 0; i < NUM_BUCKETS; i++) {
        node_t *current = book->buckets[i];
        while (current != NULL) {
            fprintf(f, "%s %d\n", current->name, current->score);
            current = current->next;
        }
    }
    fclose(f);
    return 0;
}

gradebook_t *read_gradebook_from_text(const char *file_name) {
    // TODO NOT yet implemented
    FILE *f = fopen(file_name,"r");
    if (f == NULL) {
        return NULL;
    }

    int size = 0;
    char class_name[128] = {}; 
    strcpy(class_name, file_name);
    class_name[strlen(class_name)-strlen(".txt")] = '\0';

    gradebook_t *book = create_gradebook(class_name);

    fscanf(f, "%d", &size);

    for(int i = 0; i < size; i++) {
        char name[MAX_NAME_LEN];
        int score;
        fscanf(f, "%s %d", name, &score);
        add_score(book, name, score);
    }
    fclose(f);
    return book;
}

int write_gradebook_to_binary(const gradebook_t *book) {
    // TODO Not yet implemented
    char file_name[MAX_NAME_LEN + strlen(".bin")];
    strcpy(file_name, book->class_name);
    strcat(file_name, ".bin");
    FILE *f = fopen(file_name, "w");
    if (f == NULL) {
        return -1;
    }

    fwrite(&book->size, sizeof(int), 1, f);
    for (int i = 0; i < NUM_BUCKETS; i++) {
        node_t *current = book->buckets[i];
        while (current != NULL) {
            int name_length = strlen(current->name);
            fwrite(&name_length, sizeof(int), 1, f);
            fwrite(current->name, sizeof(char), name_length, f);
            fwrite(&current->score, sizeof(int), 1, f);
            current = current->next;
        }
    }
    fclose(f);
    return 0;
}

gradebook_t *read_gradebook_from_binary(const char *file_name) {
    // TODO Not yet implemented
    FILE *f = fopen(file_name,"r");
    if (f == NULL) {
        return NULL;
    }

    int size = 0;
    char class_name[128] = {}; 
    strcpy(class_name, file_name);
    class_name[strlen(class_name)-strlen(".txt")] = '\0';

    gradebook_t *book = create_gradebook(class_name);

    fread(&size, sizeof(unsigned), 1, f);

    for(int i = 0; i < size; i++) {
        int score = 0;
        int name_length = 0;
        fread(&name_length, sizeof(int), 1, f);
        char name[name_length];
        fread(name, sizeof(char), name_length, f);
        fread(&score, sizeof(int), 1, f);
        name[name_length] = '\0';
        add_score(book, name, score);
    }

    fclose(f);
    return book;
}
