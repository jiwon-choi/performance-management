#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>

#define MAX 100000

struct pair {
  char*             key;
  int               value;
  struct pair*  next;
};

struct net_data {
  char*             name;
  int               receive_bytes;
  int               receive_packets;
  int               transmit_bytes;
  int               transmit_packets;
  struct net_data*  next;
};

void parse_process() {
  DIR* proc;
  struct dirent* ent;
  char* files[] = {"stat", "comm", "cmdline", "loginuid"};

  proc = opendir("/Users/jiwon/proc/");
  while ((ent = readdir(proc)) != NULL) {
    int pid;
    if (ent->d_type == DT_DIR && (pid = atoi(ent->d_name)) > 0) {
      char dir[MAX];
      sprintf(dir, "/Users/jiwon/proc/%s/", ent->d_name);
      for (int i = 0; i < 4; i++) {
        char file[MAX];
        sprintf(file, "%s%s", dir, files[i]);
        printf("%s\n", file);
      }
    }
  }
}

void parse_meminfo(struct pair** mem) {
  FILE* fp;
  char str[MAX];
  char* keys[] = {"MemTotal", "MemFree", "SwapTotal", "SwapFree"};
  int keys_size = 4;

  fp = fopen("/Users/jiwon/proc/meminfo", "r");
  while (fgets(str, MAX, fp)) {
    char* word = strtok(str, " :");
    for (int i = 0; keys_size > 0 && i < 4; i++) {
      if (keys[i] && !strcmp(word, keys[i])) {
        struct pair* new = malloc(sizeof(struct pair));
        new->key = strdup(word);
        word = strtok(NULL, " :");
        new->value = atoi(word);
        keys[i] = NULL;

        new->next = *mem;
        *mem = new;
        break;
      }
    }
  }
  fclose(fp);
}

void parse_net(struct net_data** net) {
  FILE* fp;
  char str[MAX];
  
  fp = fopen("/Users/jiwon/proc/net/dev", "r");
  while (fgets(str, MAX, fp)) {
    if (!strchr(str, ':'))
      continue;

    char* word = strtok(str, " :");
    if (!strcmp(word, "lo")) continue;

    struct net_data* new = malloc(sizeof(struct net_data));
    new->name = strdup(word);
    word = strtok(NULL, " ");
    new->receive_bytes = atoi(word);
    word = strtok(NULL, " ");
    new->receive_packets = atoi(word);
    for (int i = 0; i < 7; i++)
      word = strtok(NULL, " ");
    new->transmit_bytes = atoi(word);
    word = strtok(NULL, " ");
    new->transmit_packets = atoi(word);

    new->next = *net;
    *net = new;
  }
  fclose(fp);
}

void parse_stat(void) {
  FILE* fp;
  char str[MAX];

  fp = fopen("/Users/jiwon/proc/stat", "r");
  fgets(str, MAX, fp);

  char* word = strtok(str, " ");
  int stat[4];
  int i = 0;
  for (int idx = 1; idx < 6; idx++) {
    word = strtok(NULL, " ");
    if (idx == 2) continue;
    stat[i++] = atoi(word);
  }
  fclose(fp);
  for (int i = 0; i < 4; i++)
    printf("%d ", stat[i]);
}

int main(int argc, char* argv[]) {
  struct net_data* net = NULL;
  struct pair* mem = NULL;

  parse_stat();
  printf("\n--------------------------------\n");

  parse_net(&net);
  while (net) {
    printf("%s, %d, %d, %d, %d\n", net->name, net->receive_bytes, net->receive_packets, net->transmit_bytes, net->transmit_packets);
    net = net->next;
  }
  printf("--------------------------------\n");

  parse_meminfo(&mem);
  while (mem) {
    printf("%s, %d\n", mem->key, mem->value);
    mem = mem->next;
  }
  printf("--------------------------------\n");

  parse_process();
  // TODO net free
  // TODO mem free
  return (0);
}
