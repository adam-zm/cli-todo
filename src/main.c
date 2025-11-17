#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    char *description;
    bool done;
} Task;

typedef struct {
    char **lines;
    size_t size;
    int count;
} FileBuf;

int fb_push(FileBuf *fb, char *line) {
    int new_line_chars = strlen(line);
    char** new_mem = realloc(fb->lines, fb->count + 1);

    if(new_mem == NULL) return -1;

    fb->lines = new_mem;

    char *linecp = strdup(line);
    if(!linecp) return -1;
    fb->lines[fb->count] = linecp;
    fb->count += 1;
    fb->size += new_line_chars+1;

    return 0;
}

void fb_free(FileBuf *fb) {
    for(int i=0;i<fb->count;i++) {
        free(fb->lines[i]);
    }

    fb->lines = NULL;
    fb->count = 0;
    fb->size = 0;
}

int main(int argc, char **argv) {
    char *command;
    FILE *task_file;
    FileBuf fb = {0};

    task_file = fopen("tasks.txt", "a+");
    if(task_file == NULL) return -1;

    if(argc > 1){
        command = argv[1];
    } else {
        command = "none";
    }

    char buffer[1024];

    fseek(task_file, 0, SEEK_SET);
    while(fgets(buffer, 1024, task_file) != NULL) {
        fb_push(&fb, buffer);
    }

    if(command == NULL) return -1;

    if(strcmp(command, "add") == 0) {
        if(argc <= 2) {
            printf("You need to provide the task description\n");
            fclose(task_file);
            return -1;
        }

        Task task;
        task.description = argv[2];
        task.done = false;

        fprintf(task_file, "[ ] - %s\n", task.description);
    } else if (strcmp(command, "toggle") == 0) {
        if(argc <= 2) {
            printf("You need to specify the task number\n");
            fclose(task_file);
            return -1;
        }

        int task_id = atoi(argv[2]);

        if(task_id < fb.count) {
            fb.lines[task_id][1] = 'X';

            fclose(task_file);
            task_file = fopen("tasks.txt", "w+");

            for(int i=0;i<fb.count;i++) {
                fprintf(task_file, "%s", fb.lines[i]);
            }
        }else{
            printf("Line at index %d not found\n", task_id);
        }
    }else{
        fseek(task_file, 0, SEEK_SET);
        while(!feof(task_file)) {
            char buffer[256];
            if(fgets(buffer, 256, task_file)) {
                printf("%s", buffer);
            }
        }
    }

    fb_free(&fb);
    fclose(task_file);

    return 0;
}
