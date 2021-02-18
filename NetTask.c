/*
 * NetTask.c
 *
 *  Created on: Feb 18, 2021
 *      Author: root
 */

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <syslog.h>
#include <pthread.h>

#include "DB/sqlite3.h"
#include "include/log.h"


void *Net_thread(void *arg)
{
  int x = 0;
  pthread_detach(pthread_self());
  pthread_setspecific (tls_key_threadnr, "NetThread");

  logger_info("run...");
  while (1)
  {
	  sleep(1);
	  logger_info("NetThread---->%d",x++);
  }
  logger_info("exit...");
  return NULL;
}



void start_net_task(void)
{
  pthread_t pd;
  pthread_create(&pd, NULL, Net_thread, NULL);
}
