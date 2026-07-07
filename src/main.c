#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {

setbuf(stdout, NULL);
  
  while(1){

    printf("$ ");
    char command[1024];
    fgets(command, sizeof(command), stdin);
    command[strcspn(command, "\n")] = '\0';
    char *argv[100];
    int argc = 0;
    char *token = strtok(command, " ");

    while (token != NULL) {
      argv[argc] = token;
      argc++;
      token = strtok(NULL, " ");
    }
    argv[argc] = NULL;

    if (argc == 0) {
      continue;
    }

    if (strcmp(argv[0], "echo") == 0) {
      if (argc >=1) {
        for (int i = 1; i < argc; i++) {
          printf("%s ", argv[i]);
        }
        printf("\n");
      }
    }

    else if (strcmp(argv[0], "exit") == 0) {
      return 0;
    }

    else if (strcmp(argv[0], "type") == 0) {
      if (argc < 2) {
        printf("type: missing argument\n");
      } 
      else if (strcmp(argv[1], "echo") == 0 || strcmp(argv[1], "exit") == 0 || strcmp(argv[1], "type") == 0) {
        printf("%s is a shell builtin\n", argv[1]);
      }
      else {
        char *path = getenv("PATH");

        char path_copy[1024];
        strcpy(path_copy, path);

        char *dir = strtok(path_copy, ":");

        char full_path[2046];
        
        while (dir != NULL) {
         
          snprintf(full_path, sizeof(full_path), "%s/%s", dir, argv[1]);
          if (access(full_path, X_OK) == 0) {
            printf("%s is %s\n", argv[1], full_path);
            break;
          }
          dir = strtok(NULL, ":");
        }
        if (dir == NULL) {
          printf("%s: not found\n", argv[1]);
        }
      } 
    }
    else {
      char *path = getenv("PATH");

      char path_copy[1024];
      strcpy(path_copy, path);

      char *dir = strtok(path_copy, ":");

      char full_path[2046];
        
      while (dir != NULL) {
         
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, argv[0]);
        if (access(full_path, X_OK) == 0) {
          pid_t pid = fork();
          if (pid == 0) {
            execv(full_path, argv);
            perror("execv");
            exit(1);
          }
          else {
            waitpid(pid, NULL, 0);
          }
          break;
        }
        dir = strtok(NULL, ":");
        }
      if (dir == NULL) {
        printf("%s: not found\n", argv[0]);
      }
    }
  }
}
