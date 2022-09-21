#include "save.h"

pthread_mutex_t g_db_mutex;

void save_udp(struct s_udp_begin* begin, struct s_udp_end* end, MYSQL* db) {
  write_log("Save udp");

  time_t raw_time = time(&raw_time);
  struct tm s_time;
  localtime_r(&raw_time, &s_time);

  char save_time[25];
  strncpy(save_time, ctime(&raw_time), 25);
  save_time[24] = 0;

	char str[1000];
	sprintf(str, "insert into udp values (\"%s\", \"%s\", %u, %d, %f, %f, %f);", save_time, begin->agent_name, begin->pid, begin->pkt_no, begin->begin_time, end->end_time, end->elapse_time);
//	write_log(str);
	pthread_mutex_lock(&g_db_mutex);
	mysql_query(db, str);
//	write_log(mysql_error(db));
	pthread_mutex_unlock(&g_db_mutex);
}

void save_stat(struct s_packet* packet, MYSQL* db) {
  write_log("Save stat");

  time_t raw_time = time(&raw_time);
  struct tm s_time;
  localtime_r(&raw_time, &s_time);

  struct s_header* header = packet->data;
  struct s_stat* body = packet->data + sizeof(struct s_header);

  char save_time[25];
  strncpy(save_time, ctime(&raw_time), 25);
  save_time[24] = 0;

	char str[10000];
	sprintf(str, "insert into stat values (\"%s\", \"%s\", \"%s\", %d, %d, %d, %d);", header->time, save_time, header->agent_name, body->user, body->sys, body->idle, body->iowait);
//	write_log(str);
	pthread_mutex_lock(&g_db_mutex);
	mysql_query(db, str);
//	write_log(mysql_error(db));
	pthread_mutex_unlock(&g_db_mutex);
}

void save_mem(struct s_packet* packet, MYSQL* db) {
  write_log("Save mem");

  time_t raw_time = time(&raw_time);
  struct tm s_time;
  localtime_r(&raw_time, &s_time);

  struct s_header* header = packet->data;
  struct s_mem* body = packet->data + sizeof(struct s_header);

  char save_time[25];
  strncpy(save_time, ctime(&raw_time), 25);
  save_time[24] = 0;

	char str[1000];
	sprintf(str, "insert into mem values (\"%s\", \"%s\", \"%s\", %d, %d, %d, %d);", header->time, save_time, header->agent_name, body->mem_total, body->mem_free, body->swap_total, body->swap_free);
//	write_log(str);
	pthread_mutex_lock(&g_db_mutex);
	mysql_query(db, str);
//	write_log(mysql_error(db));
	pthread_mutex_unlock(&g_db_mutex);
}

void save_net(struct s_packet* packet, MYSQL* db) {
  write_log("Save net");

  time_t raw_time = time(&raw_time);
  struct tm s_time;
  localtime_r(&raw_time, &s_time);

  struct s_header* header = packet->data;

  char save_time[25];
  strncpy(save_time, ctime(&raw_time), 25);
  save_time[24] = 0;

	char str[4096];
  for (int idx = 0; idx < header->number_of_body; idx++) {
    struct s_net* chunk = packet->data + sizeof(struct s_header) + sizeof(struct s_net) * idx;
		sprintf(str, "insert into net values (\"%s\", \"%s\", \"%s\", \"%s\", %d, %d, %d, %d);", header->time, save_time, header->agent_name, chunk->interface, chunk->receive_bytes, chunk->receive_packets, chunk->transmit_bytes, chunk->transmit_packets);
//		write_log(str);
	pthread_mutex_lock(&g_db_mutex);
		mysql_query(db, str);
//		write_log(mysql_error(db));
	pthread_mutex_unlock(&g_db_mutex);
  }
}

void save_process(struct s_packet* packet, MYSQL* db) {
  write_log("Save process");

  time_t raw_time = time(&raw_time);
  struct tm s_time;
  localtime_r(&raw_time, &s_time);

  struct s_header* header = packet->data;

  char save_time[25];
  strncpy(save_time, ctime(&raw_time), 25);
  save_time[24] = 0;

	char str[4096];
  for (int idx = 0; idx < header->number_of_body; idx++) {
    struct s_process* chunk = packet->data + sizeof(struct s_header) + sizeof(struct s_process) * idx;
		sprintf(str, "insert into process values (\"%s\", \"%s\", \"%s\", %u, %u, %u, %ld, %ld, %lu, %lu, \"%s\", \"%s\", \"%s\");",
				header->time, save_time, header->agent_name, chunk->pid, chunk->ppid, chunk->loginuid, chunk->cutime, chunk->cstime, chunk->utime, chunk->stime, chunk->comm, chunk->username, chunk->cmdline);
//		write_log(str);
	pthread_mutex_lock(&g_db_mutex);
		mysql_query(db, str);
//		write_log(mysql_error(db));
	pthread_mutex_unlock(&g_db_mutex);
  }
}
