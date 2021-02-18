/*
 ============================================================================
 Name        : ProX.c
 Author      : DKING
 Version     :
 Copyright   : DKING
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <syslog.h>
#include <pthread.h>
#include "DB/sqlite3.h"

#include "include/log.h"

pthread_key_t tls_key_threadnr;

int main(void)
{
	int x = 0;
	int i = 0;

	pthread_key_create(&tls_key_threadnr, NULL);
	pthread_setspecific (tls_key_threadnr, "Main");

	for(i = 0;i<1000;i++)
	{
		sleep(1);
		logger_info("---->%d",x++);
	}
	return EXIT_SUCCESS;
}






