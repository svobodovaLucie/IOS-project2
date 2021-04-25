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
#include <errno.h>

/**
 * Define semaphores names
 */
#define SANTA_SEM "/xsvobo1x-santa_sem"
#define ELVES_SEM "/xsvobo1x-elves_sem"
#define CHRISTMAS_WAIT "/xsvobo1x-christmas_wait"
#define REINDEERS_SEM "/xsvobo1x-reindeers_sem"
#define MUTEX_SEM "/xsvobo1x-mutex"
#define PRINTING_SEM "/xsvobo1x-printing"
#define SANTA_HELP_SEM "/xsvobo1x-santa-help-sem"

/**
 * Semaphores used in this program
 */
sem_t *santa_sem;       // used when santa is sleeping
sem_t *santa_help;      // used when santa is helping elves
sem_t *elves_sem;       // used when elves are waiting for santa
sem_t *reindeers_sem;   // used when reindeers are waiting to get hitched
sem_t *christmas_wait;  // used when santa is waiting for reindeers
sem_t *printing;        // used for print functions (they use shared memory)
sem_t *mutex;           // used when using shared memory

/**
 * Structure used for shared memory
 */
typedef struct shared {
    unsigned print_count;
    unsigned reindeers_cnt;
    unsigned elves_cnt;
    unsigned elves_helped;
    bool workshop_closed;
} shared_t;
shared_t *sh_mem = NULL;

/**
 * Structure for storing arguments
 */
typedef struct args {
    unsigned ne;    // the number of elves
    unsigned nr;    // the number of reindeers
    unsigned te;    // the maximum time for that the elf works independently
    unsigned tr;    // the maximum time after that a reindeer returns home
} args_t;


/********************** PRINT FUNCTIONS ****************************/
// enumeration used for print statements
enum {SANTA_SLEEP, SANTA_HELP, SANTA_CLOSE, SANTA_CHRISTMAS,
      ELF_START, ELF_NEED, ELF_GET, ELF_HOLIDAYS,
      RD_START, RD_RETURN, RD_HITCHED,
      ERR_FOPEN, ERR_ARGS, ERR_SEM_OPEN, ERR_MEM_INIT, ERR_FORK, ERR_SEM_DESTROY, ERR_MEM_UNMAP};

void santa_message(FILE *f, int status);
void elf_message(FILE *f, int status, unsigned elfID);
void rd_message(FILE *f, int status, unsigned rdID);
void error_message(FILE *f, int status);

/************************ PROCES FUNCTIONS **************************/
void santa_process(FILE *f, args_t args);
void elf_process(FILE *f, unsigned elfID, args_t args);
void reindeer_process(FILE *f, unsigned rdID, args_t args);

/**************** SETUP AND CLEANUP FUNCTIONS ***********************/
void init_memory(FILE *f, args_t args);
void init_semaphores(FILE *f);
void cleanup_memory(FILE *f);
void cleanup_semaphores(FILE *f);
int argument_parser(int argc, char *argv[], args_t *args);

#endif  // __PROJ2_H__

/*** end of file proj2.h ***/
