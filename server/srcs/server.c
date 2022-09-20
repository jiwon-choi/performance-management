#include "server.h"

extern pthread_mutex_t g_log_mutex;

void* run_worker(void* vparam) {
//  struct s_queue_wrapper* param = (struct s_queue_wrapper*)vparam;
	struct s_worker_param* param = (struct s_worker_param*)vparam;

  while (1) {
    if (!param->qwrapper->queue) {
      sleep(1);
      continue;
    }
    pthread_mutex_lock(&(param->qwrapper->queue_mutex));
    struct s_packet* pop = dequeue(&(param->qwrapper->queue));
    pthread_mutex_unlock(&(param->qwrapper->queue_mutex));
    if (!pop) continue;
    struct s_header* header = pop->data;
    if (header->type_of_body == STAT) {
      save_stat(pop, param->db);
    } else if (header->type_of_body == MEM) {
      save_mem(pop);
    } else if (header->type_of_body == NETWORK) {
      save_net(pop);
    } else if (header->type_of_body == PROCESS) {
      save_process(pop);
    }
    free(pop->data);
    pop->data = NULL;
    free(pop);
    pop = NULL;
  }
  return (0);
}

int main(void) {
  init_daemon();

  mkdir("files", 0777);
  mkdir("files/data", 0777);
  mkdir("files/logs", 0777);
  pthread_mutex_init(&g_log_mutex, NULL);
  set_signal();

  write_log("Run server");

  int server_fd;
  struct sockaddr_in address;
  tcp_connection(&server_fd, &address);
  int addr_size = sizeof(address);

  struct s_queue_wrapper wrapper;
  wrapper.queue = NULL;
  pthread_mutex_init(&wrapper.queue_mutex, NULL);

  pthread_t tid;
  pthread_create(&tid, NULL, udp_connection, NULL);
  pthread_detach(tid);

	struct s_worker_param worker_data;
	worker_data.db = mysql_init(NULL);
	if (!mysql_real_connect(worker_data.db, "localhost", "root", "root", 0, 0, 0, 0)) {
		write_log("mysql connection failed");
		return (EXIT_FAILURE);
	}
	mysql_query(worker_data.db, "create database exem;");
	mysql_query(worker_data.db, "use exem;");
	mysql_query(worker_data.db, "create table stat ( agentTime varchar(30), saveTime varchar(30), agentName varchar(10), user int, sys int, idle int, iowait int );");

	worker_data.qwrapper = &wrapper;

  for (int i = 0; i < 8; i++) {
    char msg[40];
    pthread_create(&tid, NULL, run_worker, &worker_data);
    pthread_detach(tid);
    sprintf(msg, "Created worker thread %d", i + 1);
    write_log(msg);
  }

  write_log("Waiting TCP accept");
  struct s_recv_param* data;
  while (1) {
    data = malloc(sizeof(struct s_recv_param));
    data->qwrapper = &wrapper;
    if ((data->socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addr_size)) < 0) {
      exit(EXIT_FAILURE);
    }
    write_log("Accepted an agent");
    pthread_create(&tid, NULL, recv_packet, data);
    pthread_detach(tid);
    write_log("Created recv thread");
  }

  return (0);
}
