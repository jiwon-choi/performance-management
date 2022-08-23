#include "parse.h"

static
int get_process_size() {
  DIR* proc;
  struct dirent* ent;
  int size = 0;

  proc = opendir("/Users/jiwon/proc/");
  while ((ent = readdir(proc)) != NULL) {
    if (ent->d_type == DT_DIR && atoi(ent->d_name) > 0)
      size++;
  }
  closedir(proc);
  return (size);
}

static
int get_net_size() {
  FILE* fp;
  size_t strlen = 1;
  int size = 0;
  char* str = malloc(sizeof(char) * strlen);

  fp = fopen("/Users/jiwon/proc/net/dev", "r");
  while (getdelim(&str, &strlen, '\n', fp) != EOF) {
    if (!strchr(str, ':'))
      continue;
    if (strcmp(strtok(str, " :"), "lo") == 0)
      continue;
    size++;
  }
  free(str);
  fclose(fp);
  return (size);
}

static
void parse_process_loginuid(struct s_process* process, char* filename) {
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
void parse_process_cmdline(struct s_process* process, char* filename) {
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
void parse_process_stat(struct s_process* process, char* filename) {
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
void parse_process(void** process, int size) {
  DIR* proc;
  struct dirent* ent;

  proc = opendir("/Users/jiwon/proc/");
  *process = malloc(sizeof(struct s_process) * size);
  int idx = 0;
  while ((ent = readdir(proc)) != NULL) {
    int pid;
    if (ent->d_type == DT_DIR && (pid = atoi(ent->d_name)) > 0) {
      char filename[MAX];
      struct s_process* new = *process + sizeof(struct s_process) * idx;
      new->pid = pid;
      sprintf(filename, "/Users/jiwon/proc/%s/stat", ent->d_name);
      parse_process_stat(new, filename);
      sprintf(filename, "/Users/jiwon/proc/%s/cmdline", ent->d_name);
      parse_process_cmdline(new, filename);
      sprintf(filename, "/Users/jiwon/proc/%s/loginuid", ent->d_name);
      parse_process_loginuid(new, filename);
      idx++;
    }
  }
  closedir(proc);
}

static
void parse_net(void** net, int size) {
  FILE* fp;
  size_t strlen = 1;
  char* str = malloc(sizeof(char) * strlen);

  fp = fopen("/Users/jiwon/proc/net/dev", "r");
  *net = malloc(sizeof(struct s_net) * size);
  int idx = 0;
  while (getdelim(&str, &strlen, '\n', fp) != EOF) {
    if (!strchr(str, ':'))
      continue;

    char* word = strtok(str, " :");
    if (strcmp(word, "lo") == 0) continue;

    struct s_net* new = *net + sizeof(struct s_net) * idx;

    strcpy(new->interface, word);
    word = strtok(NULL, " ");
    new->receive_bytes = atoi(word);
    word = strtok(NULL, " ");
    new->receive_packets = atoi(word);
    for (int i = 0; i < 7; i++)
      word = strtok(NULL, " ");
    new->transmit_bytes = atoi(word);
    word = strtok(NULL, " ");
    new->transmit_packets = atoi(word);
    idx++;
  }
  free(str);
  fclose(fp);
}

static
void parse_mem(struct s_mem* mem) {
  FILE* fp;
  size_t strlen = 1;
  char* str = malloc(sizeof(char) * strlen);

  fp = fopen("/Users/jiwon/proc/meminfo", "r");
  while (getdelim(&str, &strlen, '\n', fp) != EOF) {
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
void parse_stat(struct s_stat* stat) {
  FILE* fp;
  size_t strlen = 1;
  char* str = malloc(sizeof(char) * strlen);

  fp = fopen("/Users/jiwon/proc/stat", "r");
  getdelim(&str, &strlen, '\n', fp);

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

void parse_data(struct s_packet* packet) {
  packet->header.net_size = get_net_size();
  packet->header.process_size = get_process_size();

  parse_stat(&(packet->body.stat));
  parse_mem(&(packet->body.mem));
  parse_net(&(packet->body.net), packet->header.net_size);
  parse_process(&(packet->body.process), packet->header.process_size);
}
