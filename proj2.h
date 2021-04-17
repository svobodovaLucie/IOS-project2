#ifndef __PROJ2_H__
#define __PROJ2_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

// define semafor names
#define santa_SEM "/xsvobo1x-santa"
#define elf_SEM "/xsvobo1x-elf"
#define mutex_SEM "/xsvobo1x-mutex"
#define printing_SEM "/xsvobo1x-printing"
/**
 * Structure used for shared memory
 */
typedef struct shared {
    unsigned print_count;
} shared_t;

/**
 * Structure for storing arguments
 */
typedef struct args {
    unsigned ne;
    unsigned nr;
    unsigned te;
    unsigned tr;
} args_t;

sem_t *santa, *elf, *mutex, *printing;

shared_t *sh_mem = NULL;

/**************** PRINTS.H BEGIN ***********************/
enum {SANTA_SLEEP, SANTA_HELP, SANTA_CLOSE, SANTA_CHRISTMAS};
enum {ELF_START, ELF_NEED, ELF_GET, ELF_HOLIDAYS};
enum {RD_START, RD_RETURN, RD_HITCHED};
enum {ERR_FOPEN, ERR_ARGS, ERR_SEM_INIT, ERR_SEM_OPEN, ERR_MEM_INIT, ERR_FORK, ERR_SEM_DESTROY, ERR_MEM_UNMAP};

void santa_message(FILE *f, int status);
void elf_message(FILE *f, int status, unsigned elfID);
void rd_message(FILE *f, int status, unsigned rdID);
void error_message(FILE *f, int status);
/**************** PRINTS.H END ************************/

/**************** PROCESSES.H BEGIN *******************/
void santa_process(FILE *f);
void elf_process(FILE *f, unsigned elfID, unsigned te);
void reindeer_process(FILE *f, unsigned rdID, unsigned tr);
/**************** PROCESSES.H END *********************/

/**************** SETUP.H BEGIN ***********************/
void init_memory();
void init_semaphores();
void cleanup_memory();
void cleanup_semaphores();
int argument_parser(int argc, char *argv[], args_t *arguments);
/**************** SETUP.H END *************************/

#endif