#include "../../agent/incs/packet.h"
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

void save_stat(struct s_packet* packet) {
  time_t raw_time = time(&raw_time);
  char filename[50];
  struct tm s_time;
  localtime_r(&raw_time, &s_time);
  sprintf(filename, "files/data/stat_%d-%02d-%02d", s_time.tm_year + 1900, s_time.tm_mon + 1, s_time.tm_mday);

  FILE* fp = fopen(filename, "a+");

  struct s_header* header = packet->data;
  struct s_stat* body = packet->data + sizeof(struct s_header);

  char save_time[25];
  strncpy(save_time, ctime(&raw_time), 25);
  save_time[24] = 0;

  fprintf(fp, "%s | %s | %-8s | ", header->time, save_time, header->agent_name);
  fprintf(fp, "user %-10d | sys %-10d | idle %-10d | iowait %-10d",
          body->user, body->sys, body->idle, body->iowait);
  fprintf(fp, "\n");
  fclose(fp);
}

void save_mem(struct s_packet* packet) {
  time_t raw_time = time(&raw_time);
  char filename[50];
  struct tm s_time;
  localtime_r(&raw_time, &s_time);
  sprintf(filename, "files/data/mem_%d-%02d-%02d", s_time.tm_year + 1900, s_time.tm_mon + 1, s_time.tm_mday);

  FILE* fp = fopen(filename, "a+");

  struct s_header* header = packet->data;
  struct s_mem* body = packet->data + sizeof(struct s_header);

  char save_time[25];
  strncpy(save_time, ctime(&raw_time), 25);
  save_time[24] = 0;

  fprintf(fp, "%s | %s | %-8s | ", header->time, save_time, header->agent_name);
  fprintf(fp, "memtotal %-10d | memfree %-10d | swaptotal %-10d | swapfree %-10d",
          body->mem_total, body->mem_free, body->swap_total, body->swap_free);
  fprintf(fp, "\n");
  fclose(fp);
}

void save_net(struct s_packet* packet) {
  time_t raw_time = time(&raw_time);
  char filename[50];
  struct tm s_time;
  localtime_r(&raw_time, &s_time);
  sprintf(filename, "files/data/net_%d-%02d-%02d", s_time.tm_year + 1900, s_time.tm_mon + 1, s_time.tm_mday);

  FILE* fp = fopen(filename, "a+");

  struct s_header* header = packet->data;

  char save_time[25];
  strncpy(save_time, ctime(&raw_time), 25);
  save_time[24] = 0;

  for (int idx = 0; idx < header->number_of_body; idx++) {
    struct s_net* chunk = packet->data + sizeof(struct s_header) + sizeof(struct s_net) * idx;
    fprintf(fp, "%s | %s | %-8s | ", header->time, save_time, header->agent_name);
    fprintf(fp, "interface %-15s | Rbytes %-10d | Rpackets %-10d | Tbytes %-10d | Tpackets %-10d",
            chunk->interface, chunk->receive_bytes, chunk->receive_packets, chunk->transmit_bytes, chunk->transmit_packets);
    fprintf(fp, "\n");
  }
  fclose(fp);
}

void save_process(struct s_packet* packet) {
  time_t raw_time = time(&raw_time);
  char filename[50];
  struct tm s_time;
  localtime_r(&raw_time, &s_time);
  sprintf(filename, "files/data/process_%d-%02d-%02d", s_time.tm_year + 1900, s_time.tm_mon + 1, s_time.tm_mday);

  FILE* fp = fopen(filename, "a+");

  struct s_header* header = packet->data;

  char save_time[25];
  strncpy(save_time, ctime(&raw_time), 25);
  save_time[24] = 0;

  for (int idx = 0; idx < header->number_of_body; idx++) {
    struct s_process* chunk = packet->data + sizeof(struct s_header) + sizeof(struct s_process) * idx;
    fprintf(fp, "%s | %s | %-8s | ", header->time, save_time, header->agent_name);
    fprintf(fp, "pid %-6u | ppid %-6u | loginuid %-6u | ", chunk->pid, chunk->ppid, chunk->loginuid);
    // fprintf(fp, "cutime %10ld | cstime %10ld | utime %10lu | stime %10lu\n", chunk->cutime, chunk->cstime, chunk->utime, chunk->stime);
    fprintf(fp, "username %-8s | comm %-10s | cmdline : %s", chunk->username, chunk->comm, chunk->cmdline);
    fprintf(fp, "\n");
  }
  fclose(fp);
}
