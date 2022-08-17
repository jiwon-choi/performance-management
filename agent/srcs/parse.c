#include "parse.h"

static
void parse_process_loginuid(struct process* process, char* filename) {
  FILE* fp;
  struct passwd* pwd;
  size_t size = 1;
  char* str = malloc(sizeof(char) * size);

  fp = fopen(filename, "r");
  getdelim(&str, &size, '\n', fp);
  process->loginuid = atoi(str);
  pwd = getpwuid(process->loginuid);
  strcpy(process->username, pwd->pw_name);
  free(str);
  fclose(fp);
}

static
void parse_process_cmdline(struct process* process, char* filename) {
  FILE* fp;
  size_t size = 1;
  char* str = malloc(sizeof(char) * size);

  fp = fopen(filename, "r");
  int i = 0;
  while (getdelim(&str, &size, 0, fp) != EOF) {
    strcpy(process->cmdline[i++], str);
  }
  free(str);
  fclose(fp);
}

static
void parse_process_stat(struct process* process, char* filename) {
  FILE* fp;
  size_t size = 1;
  char* str = malloc(sizeof(char) * size);

  fp = fopen(filename, "r");
  getdelim(&str, &size, '\n', fp);

  char* word = strtok(str, " ");
  for (int idx = 2; idx < 18; idx++) {
    word = strtok(NULL, "( )");
    switch (idx) {
    case 2:
      strcpy(process->comm, word);
      break;
    case 4:
      process->ppid = atoi(word);
      break;
    case 14:
      process->utime = strtoul(word, NULL, 10);
      break;
    case 15:
      process->stime = strtoul(word, NULL, 10);
      break;
    case 16:
      process->cutime = strtoul(word, NULL, 10);
      break;
    case 17:
      process->cstime = strtoul(word, NULL, 10);
      break;
    }
  }
  free(str);
  fclose(fp);
}

static
void parse_process(struct process* process) {
  DIR* proc;
  struct dirent* ent;

  proc = opendir("/Users/jiwon/proc/");
  while ((ent = readdir(proc)) != NULL) {
    if (ent->d_type == DT_DIR && (process->pid = atoi(ent->d_name)) > 0) {
      char filename[MAX];
      sprintf(filename, "/Users/jiwon/proc/%s/stat", ent->d_name);
      parse_process_stat(process, filename);
      sprintf(filename, "/Users/jiwon/proc/%s/cmdline", ent->d_name);
      parse_process_cmdline(process, filename);
      sprintf(filename, "/Users/jiwon/proc/%s/loginuid", ent->d_name);
      parse_process_loginuid(process, filename);
    }
  }
  closedir(proc);
}

static
void parse_net(struct net* net) {
  FILE* fp;
  size_t size = 1;
  char* str = malloc(sizeof(char) * size);

  fp = fopen("/Users/jiwon/proc/net/dev", "r");
  while (getdelim(&str, &size, '\n', fp) != EOF) {
    if (!strchr(str, ':'))
      continue;

    char* word = strtok(str, " :");
    if (strcmp(word, "lo") == 0) continue;

    strcpy(net->interface, word);
    word = strtok(NULL, " ");
    net->receive_bytes = atoi(word);
    word = strtok(NULL, " ");
    net->receive_packets = atoi(word);
    for (int i = 0; i < 7; i++)
      word = strtok(NULL, " ");
    net->transmit_bytes = atoi(word);
    word = strtok(NULL, " ");
    net->transmit_packets = atoi(word);
  }
  free(str);
  fclose(fp);
}

static
void parse_mem(struct mem* mem) {
  FILE* fp;
  size_t size = 1;
  char* str = malloc(sizeof(char) * size);

  fp = fopen("/Users/jiwon/proc/meminfo", "r");
  while (getdelim(&str, &size, '\n', fp) != EOF) {
    char* word = strtok(str, " :");
    if (strcmp(word, "MemTotal") == 0) {
      mem->mem_total = atoi(strtok(NULL, " "));
    } else if (strcmp(word, "MemFree") == 0) {
      mem->mem_free = atoi(strtok(NULL, " "));
    } else if (strcmp(word, "SwapTotal") == 0) {
      mem->swap_total = atoi(strtok(NULL, " "));
    } else if (strcmp(word, "SwapFree") == 0) {
      mem->swap_free = atoi(strtok(NULL, " "));
    }
  }
  free(str);
  fclose(fp);
}

static
void parse_stat(struct stat* stat) {
  FILE* fp;
  size_t size = 1;
  char* str = malloc(sizeof(char) * size);

  fp = fopen("/Users/jiwon/proc/stat", "r");
  getdelim(&str, &size, '\n', fp);

  char* word = strtok(str, " ");
  word = strtok(NULL, " ");
  stat->user = atoi(word);
  word = strtok(NULL, " ");
  word = strtok(NULL, " ");
  stat->sys = atoi(word);
  word = strtok(NULL, " ");
  stat->idle = atoi(word);
  word = strtok(NULL, " ");
  stat->iowait = atoi(word);
  free(str);
  fclose(fp);
}

void parse_data(struct data* data) {
  parse_stat(&(data->stat));
  parse_mem(&(data->mem));
  parse_net(&(data->net));
  parse_process(&(data->process));
}
