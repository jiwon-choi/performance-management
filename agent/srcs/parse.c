#include "parse.h"
#include "queue.h"

static
int get_process_size() {
  DIR* proc;
  struct dirent* ent;
  int size = 0;

  proc = opendir(PROC_LOCATION);
  while ((ent = readdir(proc)) != NULL) {
    if (ent->d_type == DT_DIR && atoi(ent->d_name) > 0)
      size++;
  }
  closedir(proc);
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

// TODO 좀비 프로세스의 경우 cmdline이 비어있는 건지 cmdline 파일 자체가 없는 건지 확인
static
void parse_process_cmdline(struct s_process* process, char* filename) {
  int fd = open(filename, O_RDONLY);
  int read_size = read(fd, process->cmdline, CMDLINE_MAX);
  
  process->cmdline[read_size] = '\0';
  for (int i = 0; i < read_size - 1; i++) {
    if (process->cmdline[i] == '\0')
      process->cmdline[i] = ' ';
  }
  close(fd);
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

void* parse_process(void* queue) {
  while (1) {
    DIR* proc;
    struct dirent* ent;

    proc = opendir(PROC_LOCATION);

    int process_size = get_process_size();
    struct s_packet* packet = malloc(sizeof(struct s_packet));
    packet->size = sizeof(struct s_header) + sizeof(struct s_process) * process_size;
    packet->data = malloc(packet->size);
    packet->next = NULL;

    struct s_header* header = packet->data;
    header->type_of_body = PROCESS;
    header->number_of_body = process_size;

    int chunk_idx = 0;
    while ((ent = readdir(proc)) != NULL) {
      int pid;
      if (ent->d_type == DT_DIR && (pid = atoi(ent->d_name)) > 0) {
        char filename[MAX];

        struct s_process* chunk = packet->data + sizeof(struct s_header) + sizeof(struct s_process) * chunk_idx;
        chunk->pid = pid;
        sprintf(filename, "%s%s/stat", PROC_LOCATION, ent->d_name);
        parse_process_stat(chunk, filename);
        sprintf(filename, "%s%s/cmdline", PROC_LOCATION, ent->d_name);
        parse_process_cmdline(chunk, filename);
        sprintf(filename, "%s%s/loginuid", PROC_LOCATION, ent->d_name);
        parse_process_loginuid(chunk, filename);
        chunk_idx++;
      }
    }
    insert_queue(queue, packet);

    closedir(proc);

    sleep(4);
  }
  return (0);
}

static
int get_net_size() {
  FILE* fp;
  size_t strlen = 1;
  int size = 0;
  char* str = malloc(sizeof(char) * strlen);

  fp = fopen(NET_LOCATION, "r");
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

void* parse_net(void* queue) {
  while (1) {
    FILE* fp;
    size_t strlen = 1;
    char* str = malloc(sizeof(char) * strlen);

    fp = fopen(NET_LOCATION, "r");

    int net_size = get_net_size();
    struct s_packet* packet = malloc(sizeof(struct s_packet));
    packet->size = sizeof(struct s_header) + sizeof(struct s_net) * net_size;
    packet->data = malloc(packet->size);
    packet->next = NULL;

    struct s_header* header = packet->data;
    header->type_of_body = NET;
    header->number_of_body = net_size;

    int chunk_idx = 0;
    while (getdelim(&str, &strlen, '\n', fp) != EOF) {
      if (!strchr(str, ':'))
        continue;

      char* word = strtok(str, " :");
      if (strcmp(word, "lo") == 0) continue;

      struct s_net* chunk = packet->data + sizeof(struct s_header) + sizeof(struct s_net) * chunk_idx;

      strcpy(chunk->interface, word);
      word = strtok(NULL, " ");
      chunk->receive_bytes = atoi(word);
      word = strtok(NULL, " ");
      chunk->receive_packets = atoi(word);
      for (int i = 0; i < 7; i++)
        word = strtok(NULL, " ");
      chunk->transmit_bytes = atoi(word);
      word = strtok(NULL, " ");
      chunk->transmit_packets = atoi(word);
      chunk_idx++;
    }
    insert_queue(queue, packet);

    free(str);
    fclose(fp);

    sleep(3);
  }
}

void* parse_mem(void* queue) {
  while (1) {
    FILE* fp;
    size_t strlen = 1;
    char* str = malloc(sizeof(char) * strlen);

    fp = fopen(MEM_LOCATION, "r");

    struct s_packet* packet = malloc(sizeof(struct s_packet));
    packet->size = sizeof(struct s_header) + sizeof(struct s_mem);
    packet->data = malloc(packet->size);
    packet->next = NULL;

    struct s_header*  header = packet->data;
    header->type_of_body = MEM;
    header->number_of_body = 1;

    struct s_mem*     body = packet->data + sizeof(struct s_header);

    while (getdelim(&str, &strlen, '\n', fp) != EOF) {
      char* word = strtok(str, " :");
      if (strcmp(word, "MemTotal") == 0) {
        body->mem_total = atoi(strtok(NULL, " "));
      } else if (strcmp(word, "MemFree") == 0) {
        body->mem_free = atoi(strtok(NULL, " "));
      } else if (strcmp(word, "SwapTotal") == 0) {
        body->swap_total = atoi(strtok(NULL, " "));
      } else if (strcmp(word, "SwapFree") == 0) {
        body->swap_free = atoi(strtok(NULL, " "));
      }
    }
    insert_queue(queue, packet);

    free(str);
    fclose(fp);

    sleep(2);
  }
}

void* parse_stat(void* queue) {
  while (1) {
    FILE* fp;
    size_t strlen = 1;
    char* str = malloc(sizeof(char) * strlen);

    fp = fopen(STAT_LOCATION, "r");

    struct s_packet* packet = malloc(sizeof(struct s_packet));
    packet->size = sizeof(struct s_header) + sizeof(struct s_stat);
    packet->data = malloc(packet->size);
    packet->next = NULL;

    struct s_header*  header = packet->data;
    header->type_of_body = STAT;
    header->number_of_body = 1;

    struct s_stat*    body = packet->data + sizeof(struct s_header);
    getdelim(&str, &strlen, '\n', fp);

    char* word = strtok(str, " ");
    word = strtok(NULL, " ");
    body->user = atoi(word);
    word = strtok(NULL, " ");
    word = strtok(NULL, " ");
    body->sys = atoi(word);
    word = strtok(NULL, " ");
    body->idle = atoi(word);
    word = strtok(NULL, " ");
    body->iowait = atoi(word);
    insert_queue(queue, packet);

    free(str);
    fclose(fp);

    sleep(1);
  }
  return (0);
}
