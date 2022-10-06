#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/limits.h>
#include <errno.h>
#include <fcntl.h>
#define MIN_ALLOC_SIZE 5

size_t cwdLen = PATH_MAX;
char *cwd = NULL;

void execute(char ** tokenArr, char ** path){
  char *concatCommand;
  size_t pathLen;
  size_t cmdLen;
  size_t i=0;
  if (memcmp(tokenArr[0],"/",1)==0 || memcmp(tokenArr[0],".",1)==0) {

      if (execv(tokenArr[0],tokenArr)==-1) {
        perror("");
      }
      exit(0);

  }else{

        /* iterate all path in the array */
        while (path[i]!=NULL){
          pathLen = strlen(path[i]);
          cmdLen = strlen(tokenArr[0]);
          concatCommand = malloc(pathLen+cmdLen+2);
          memmove(concatCommand, path[i], pathLen);
          memmove(&concatCommand[pathLen], "/",1);
          memmove(&concatCommand[pathLen+1],tokenArr[0],cmdLen);
          /* execute, if not success, keep trying by loop */
          if (execv(concatCommand,tokenArr)==-1) {
            /* print error message and exit process
            if it's the last path */

              if (path[i+1]==NULL) {
                perror("");
                exit(0);
              }
            }
            i++;

        }
  }

}


void update_cwd(char **prompt, size_t *promptSize) {
  char *res = NULL;
  size_t newPromptLen = 0;
  memset(cwd, 0, cwdLen * sizeof(char));

  do {
    res = getcwd(cwd, cwdLen);
    if (res == NULL) {
      if (errno == ERANGE) {
        cwdLen *= 2;
        cwd = realloc(cwd, cwdLen * sizeof(char));
        memset(cwd, 0, cwdLen * sizeof(char));
      } else {
        printf("Error: Could not determine cwd!\n");
        break;
      }
    }
  } while (res == NULL);

  /* Update prompt */
  /* 5 extra chars: one for null terminator, 4 for prompt decorations */
  newPromptLen = strlen(cwd) + 5;
  memset(*prompt, 0, *promptSize);
  if (newPromptLen > *promptSize) {
    *promptSize = *promptSize * 2;
    *prompt = realloc(*prompt, *promptSize * sizeof(char));
    memset(*prompt, 0, *promptSize);
  }
  memcpy(*prompt+1, cwd, strlen(cwd));
  (*prompt)[0] = '[';
  memcpy(&(*prompt)[strlen(cwd)+1], "] >", strlen("] >"));
  (*prompt)[*promptSize - 1] = '\0';
}

void get_line(char **command, size_t *bufSize) {
  size_t idx = 0;

  memset(*command, 0, *bufSize);

  while (1) {
    char ch = (char)getchar();

    if (idx + 1 >= *bufSize) {
      *bufSize = *bufSize * 2;
      *command = realloc(*command, *bufSize);
    }

    if (ch == '\n') {
      (*command)[idx] = '\0';
      idx += 1;
      break;
    }

    (*command)[idx] = ch;
    idx += 1;
  }
  *bufSize = idx + 1;
}


/* Basic skeleton program for the shell.
   Right now all it does it store the stdin into
   memory, and then echo the command right back out to
   the user.
*/
int main() {
    char *command = NULL;
    size_t maxTokenCount = MIN_ALLOC_SIZE;
    char **tokenArr = malloc(maxTokenCount * sizeof(char*));
    size_t cmdSize = MIN_ALLOC_SIZE * sizeof(char);
    size_t tokenIndex = 0;
    char *path[] = {"/bin","/usr/local/bin","/student/binfo/bin",
    "/usr/local/sbin", "/usr/bin","/usr/sbin",NULL};

    size_t j = 0;
    int pipeCount = 0;
    size_t pipeIndex;
    pid_t id;

    char *prompt = NULL;
    size_t promptLen = cwdLen;

    command = malloc(cmdSize);
    cwd = malloc(cwdLen * sizeof(char));
    prompt = malloc(cwdLen * sizeof(char));

    while (1) {
        char *strtokRes = NULL;

        /* Resetting some vars in prep for the next cotokenArrmmand */
        tokenIndex = 0;
        j = 0;
        pipeCount = 0;
        update_cwd(&prompt, &promptLen);
        printf("%s ", prompt);
        /* Grab the next command from stdin */
        get_line(&command, &cmdSize);

        tokenIndex = 0;
        /* This should never happen, but just to be safe...*/
        if (!command) { break; }
        /* Main tokenizer code. */
        tokenArr[0] = strtok(command, " ");
        if (!tokenArr[0]) {
            /* There were no tokens, skip. */
            continue;
        }
        while ((strtokRes = strtok(NULL, " "))) {
            tokenIndex += 1;
            if (tokenIndex + 1 > maxTokenCount) {
                maxTokenCount *= 2;
                tokenArr = realloc(tokenArr, maxTokenCount * sizeof(char*));
            }
            tokenArr[tokenIndex] = strtokRes;
        }
        /* Our token array must be null-terminated, according to man exec. */
        tokenIndex += 1;
        if (tokenIndex + 1 > maxTokenCount) {
            maxTokenCount += 1;
            tokenArr = realloc(tokenArr, maxTokenCount * sizeof(char*));
        }
        tokenArr[tokenIndex] = NULL;

        /* Test if we were given a builtin command */
        if (strcmp(tokenArr[0], "exit") == 0) {
          exit(0);
        } else if (strcmp(tokenArr[0], "cd") == 0) {
          int err = 0;
          if (tokenIndex != 2) {
            printf("Error: Provided incorrect arg count to cd!\n");
          }
          err = chdir(tokenArr[1]);
          if (err) {
            printf("Failed to change directory!\n");
          }
        } else {
          /* Handle command execing */
          while(tokenArr[j] != NULL){
            if (memcmp(tokenArr[j],"|",1) == 0 && strlen(tokenArr[j]) == 1) {
              pipeCount++;
              pipeIndex = j;
            }
            j++;
          }

          if (pipeCount>1) {
            fprintf(stderr,"Number of pipe character is greater than 1, "
            "can not handle\n" );
          }else if (pipeCount==1) {
            /* two file descriptors points to same file.
            one for write , one for read */
            int fdwrite = open("./temp.txt", O_CREAT|O_WRONLY, 0777);
            int fdread = open("./temp.txt", O_RDONLY, 0x777);
            /* split the list in to two */
            char **tokenArrIn = malloc((pipeIndex+1) * sizeof(char*));
            tokenArr[pipeIndex] = NULL;
            memmove(tokenArrIn,tokenArr,(pipeIndex+1)*sizeof(char*));
            id = fork();
            /*send the split list to two processes respectivly */
            if (id==0) {
              dup2(fdwrite,STDOUT_FILENO);
              execute(tokenArrIn,path);
            }

            wait(NULL);

            id = fork();
            if (id == 0) {
              dup2(fdread,STDIN_FILENO);
              execute(&tokenArr[pipeIndex+1],path);
            }
            wait(NULL);
            remove("./temp.txt");
          }else{
            id = fork();
            if (id==0) {
              execute(tokenArr,path);
            }
          }
          wait(NULL);
        }
    }

    /* Shouldn't matter, but might as well do it. */
    free(tokenArr);
    return 0;
}
