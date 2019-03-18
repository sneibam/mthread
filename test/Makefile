#gcc -g -I  lib/ test/sem_test_try_wait.c lib/libmthread.a -o sem_test_try_wait -lpthread
CC=gcc
CFLAGS=-Wall -g -I
DEPS=../lib/libmthread.a
LIBDIR=../lib/

# Mutex

mutex/mutex_test:
	$(CC) $(CFLAGS) $(LIBDIR) $@.c $(DEPS) -o $@.exe -lpthread
	./$@.exe

mutex/mutex_test_create:
	$(CC) $(CFLAGS) $(LIBDIR) $@.c $(DEPS) -o $@.exe -lpthread
	./$@.exe
	
mutex/mutex_test_destroy:
	$(CC) $(CFLAGS) $(LIBDIR) $@.c $(DEPS) -o $@.exe -lpthread
	./$@.exe
	
mutex/mutex_test_init:
	$(CC) $(CFLAGS) $(LIBDIR) $@.c $(DEPS) -o $@.exe -lpthread
	./$@.exe

mutex/mutex_test_trylock:
	$(CC) $(CFLAGS) $(LIBDIR) $@.c $(DEPS) -o $@.exe -lpthread
	./$@.exe

mutex/mutex_test_lock:
	$(CC) $(CFLAGS) $(LIBDIR) $@.c $(DEPS) -o $@.exe -lpthread
	./$@.exe

# Condition

cond/cond_test_init:
	$(CC) $(CFLAGS) $(LIBDIR) $@.c $(DEPS) -o $@.exe -lpthread
	./$@.exe
	
cond/cond_test_destroy:
	$(CC) $(CFLAGS) $(LIBDIR) $@.c $(DEPS) -o $@.exe -lpthread
	./$@.exe
	
cond/cond_test_broadcast:
	$(CC) $(CFLAGS) $(LIBDIR) $@.c $(DEPS) -o $@.exe -lpthread
	./$@.exe


cond/cond_test_ping_pong:
	$(CC) $(CFLAGS) $(LIBDIR) $@.c $(DEPS) -o $@.exe -lpthread
	./$@.exe
	
cond/cond_test_prod_conso:
	$(CC) $(CFLAGS) $(LIBDIR) $@.c $(DEPS) -o $@.exe -lpthread
	./$@.exe

# Semaphore

sem/sem_test:
	$(CC) $(CFLAGS) $(LIBDIR) $@.c $(DEPS) -o $@.exe -lpthread
	./$@.exe

sem/sem_test_init:
	$(CC) $(CFLAGS) $(LIBDIR) $@.c $(DEPS) -o $@.exe -lpthread
	./$@.exe

sem/sem_test_destroy:
	$(CC) $(CFLAGS) $(LIBDIR) $@.c $(DEPS) -o $@.exe -lpthread
	./$@.exe

sem/sem_test_wait_post:
	$(CC) $(CFLAGS) $(LIBDIR) $@.c $(DEPS) -o $@.exe -lpthread
	./$@.exe

sem/sem_test_try_wait:
	$(CC) $(CFLAGS) $(LIBDIR) $@.c $(DEPS) -o $@.exe -lpthread
	./$@.exe

sem/sem_test_getvalue:
	$(CC) $(CFLAGS) $(LIBDIR) $@.c $(DEPS) -o $@.exe -lpthread
	./$@.exe
	
# Parallel For
parallel_for/parallel_for_test:
	$(CC) $(CFLAGS) $(LIBDIR) $@.c $(DEPS) -o $@.exe -lpthread
	./$@.exe
.PHONY: clean
clean:
	rm -f mutex/*.exe
	rm -f sem/*.exe
	rm -f cond/*.exe
	rm -f parallel_for/*.exe
