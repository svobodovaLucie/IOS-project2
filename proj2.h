/**
 * FIT VUT 2020/2021
 * IOS - Projekt 2
 * Santa Claus problem
 * Author: Lucie Svobodová, xsvobo1x
 * Date: 25.04.2021
 * proj2.h
 **/

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
#define SANTA_SEM "/xsvobo1x-santa_sem"
#define ELVES_SEM "/xsvobo1x-elves_sem"
#define CHRISTMAS_WAIT "/xsvobo1x-christmas_wait"
#define REINDEERS_SEM "/xsvobo1x-reindeers_sem"
#define MUTEX_SEM "/xsvobo1x-mutex"
#define PRINTING_SEM "/xsvobo1x-printing"
#define SANTA_HELP_SEM "/xsvobo1x-santa-help-sem"
/**
 * Structure used for shared memory
 */
typedef struct shared {
    unsigned print_count;
    unsigned active;
    unsigned reindeers_cnt;
    unsigned elves_cnt;
    bool workshop_closed;
    unsigned elves_helped;
    bool santa_helping;
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

sem_t *santa_sem, *elves_sem, *christmas_wait, *reindeers_sem, *mutex, *printing, *santa_help;

shared_t *sh_mem = NULL;

/**************** PRINTS.H BEGIN ***********************/
enum {SANTA_SLEEP, SANTA_HELP, SANTA_CLOSE, SANTA_CHRISTMAS,
      ELF_START, ELF_NEED, ELF_GET, ELF_HOLIDAYS,
      RD_START, RD_RETURN, RD_HITCHED,
      ERR_FOPEN, ERR_ARGS, ERR_SEM_INIT, ERR_SEM_OPEN, ERR_MEM_INIT, ERR_FORK, ERR_SEM_DESTROY, ERR_MEM_UNMAP, DEBUG};

void santa_message(FILE *f, int status);
void elf_message(FILE *f, int status, unsigned elfID);
void rd_message(FILE *f, int status, unsigned rdID);
void error_message(FILE *f, int status);
/**************** PRINTS.H END ************************/

/**************** PROCESSES.H BEGIN *******************/
void santa_process(FILE *f, args_t args);
void elf_process(FILE *f, unsigned elfID, args_t args);
void reindeer_process(FILE *f, unsigned rdID, args_t args);
/**************** PROCESSES.H END *********************/

/**************** SETUP.H BEGIN ***********************/
void init_memory(FILE *f, args_t args);
void init_semaphores(FILE *f);
void cleanup_memory(FILE *f);
void cleanup_semaphores(FILE *f);
int argument_parser(int argc, char *argv[], args_t *args);
/**************** SETUP.H END *************************/

#endif