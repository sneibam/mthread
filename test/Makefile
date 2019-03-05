#gcc -g -I  lib/ test/sem_test_try_wait.c lib/libmthread.a -o sem_test_try_wait -lpthread
CC=gcc
CFLAGS=-Wall -g -I
DEPS=../lib/libmthread.a
LIBDIR=../lib/

mutex/mutex_test:
	$(CC) $(CFLAGS) $(LIBDIR) $@.c $(DEPS) -o $@.exe -lpthread
	./$@.exe
cond/cond_test_ping_pong:
	$(CC) $(CFLAGS) $(LIBDIR) $@.c $(DEPS) -o $@.exe -lpthread
	./$@.exe
	
cond/cond_test_prod_conso:
	$(CC) $(CFLAGS) $(LIBDIR) $@.c $(DEPS) -o $@.exe -lpthread
	./$@.exe
sem/sem_test_init:
	$(CC) $(CFLAGS) $(LIBDIR) $@.c $(DEPS) -o $@.exe -lpthread
	./$@.exe
clean:
	rm cond/*.exe
	rm mutex/*.exe
	rm sem/*.exe