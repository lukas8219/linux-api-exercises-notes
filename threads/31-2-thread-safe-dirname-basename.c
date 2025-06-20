#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

static pthread_once_t once = PTHREAD_ONCE_INIT;
static pthread_key_t data_key;

void release_data(){
  //IMPL me
}

void create_key(){
  pthread_key_create(&data_key, release_data);
}

char* dirname_ts(char* path){
  //handle error
  int s = pthread_once(&once, create_key);
  char* buf;

  buf = pthread_getspecific(data_key);
  if (buf == NULL) {
    buf = malloc(PATH_MAX * sizeof(char));
    pthread_setspecific(data_key, buf);
  }

  int index = strlen(path);
  if (path[index - 1] == '/'){
    printf("Deleting since last character is /\n");
    index -= 1;
  }

  int found_forward_slash = -1;
  for (int j=index; j > 0; j--) {
    printf("[%d] %c\n", j, path[j]);
    if (path[j] == '/'){
      printf("Found / at %d\n", j);
      found_forward_slash = j;
      break;
    }
  }

  printf("Copying from index 0 to %d\n", found_forward_slash);
  strncpy(buf, path, found_forward_slash);

  return buf;
}

int main(){
  char* dir = "/usr/lib";
  //TODO: implement test cases with both dirname and dirrname_ts
  printf("%s\n", dirname_ts(dir));
}
