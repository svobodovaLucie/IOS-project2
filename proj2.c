/**
 * HEADER
 * FIT VUT - IOS 2020/2021
 * Projekt 2
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <time.h>



/**
 * Argument parser
 * 
 * @param argv command line arguments
 * @param ne the number of elves (0 < NE < 1000)
 * @param nr the number of reindeers (0 < NR < 20)
 * @param te the maximum time (in ms) for that the elf works independently
 * @param tr the maximum time (in ms) after that a reindeer returns home
 * @return
 */
int argument_parser(char *argv[], unsigned *ne, unsigned *nr, unsigned *te, unsigned *tr) {
    long int tmp;
    char *endptr;

    // NE - number of elves (0 < NE < 1000)
    tmp = strtol(argv[1], &endptr, 10);
    if (!(tmp > 0 && tmp < 1000 && endptr[0] == '\0'))
        return -1;
    *ne = tmp;
    
    // NR - number of reindeers (0 < NR < 20)
    tmp = strtol(argv[2], &endptr, 10);
    if (!(tmp > 0 && tmp < 20 && endptr[0] == '\0'))
        return -1;
    *nr = tmp;

    // TE - the maximum time (in ms) for that the elf works independently
    tmp = strtol(argv[3], &endptr, 10);
    if (!(tmp >= 0 && tmp <= 1000 && endptr[0] == '\0'))
        return -1;
    *te = tmp;

    // TR - the maximum time (in ms) after that a reindeer returns home
    tmp = strtol(argv[4], &endptr, 10);
    if (!(tmp >= 0 && tmp <= 1000 && endptr[0] == '\0'))
        return -1;
    *tr = tmp;

    return 0;
}

/**
 * @brief Funkce inicializuje 
 * 
 * @param
 * @return
 */
int initialize() {
    // otevreni souboru
    // inicializace semaforu
    // inicializace sdilene pameti
    return 0;
}

int cleanup(FILE *f_out) {
    fclose(f_out);
    // unmap sdilene pameti
    // close semaforu
    return 0;
}

void santa_process(FILE *f) {
    //printf("[santa] pid %d from [parent] pid %d\n", getpid(),getppid());
    fprintf(f, "A: Santa: going to sleep\n");    // TODO change A to the number of action (shared mem)
    fflush(f);
    exit(0);
}

void elf_process(FILE *f, unsigned elfID, unsigned te) {
    //printf("[elf number %u] pid %d from [parent] pid %d\n", elfID, getpid(),getppid());
    fprintf(f, "A: Elf %u: started\n", elfID+1);    // TODO change A to the number of action (shared mem)
    fflush(f);

    // generate random time for that an elf is working independently
    srand(time(NULL) * getpid() + elfID);
    if (usleep((rand() % (te + 1)))) {
        // error
    }
    exit(0);
}

void reindeer_process(FILE *f, unsigned rdID, unsigned tr) {
    //printf("[reindeer number %u] pid %d from [parent] pid %d\n", rdID, getpid(), getppid());
    fprintf(f, "A: RD %u: rstarted\n", rdID+1);    // TODO change A to the number of action (shared mem)
    fflush(f);

    // generate random time for that a reindeer is on holiday
    srand(time(NULL) * getpid() + rdID);
    if (usleep((rand() % (tr - tr/2 + 1) + tr/2))) {
        // error
    }
    exit(0);
}

/********************************************************************************************/
int main(int argc, char *argv[]) {

    unsigned ne, nr, te, tr;

    if (argc != 5) {
        printf("The number of arguments is %d\n", argc);
        return 1;
    }
    FILE *f_out = fopen("proj2.out", "w");
    if (f_out == NULL) {
        fprintf(stderr, "Error when opening file proj2.out\n");
        return 1;
    }

    if (argument_parser(argv, &ne, &nr, &te, &tr)) {
        cleanup(f_out);
        // help vypis
        printf("Invalid arguments.\n");
        return 1;
    }
    
    if (initialize()) {
        return 1;
    }

    srand(time(NULL));

    


    //printf("NE (num of elves) is: %u\n", ne);
    //printf("NR (num of reindeers) is: %u\n", nr);
    //printf("TE is: %u\n", te);
    //printf("TR is: %u\n", tr);

    // creating processes
    /* Hlavní proces vytváří ihned po spuštění jeden proces Santa, NE procesů skřítků a NR procesů sobů. */
    // parent process will be the Santa process
    // child process will generate elves and reindeers
    pid_t process_ID = fork();

    if (process_ID == 0) {
        // santa process
        santa_process(f_out);
    } else if (process_ID < 0) {    // error
        cleanup(f_out);
        fprintf(stderr, "PID is <0.\n");
    }
    // zde bezi main process
    process_ID = fork();
    if (process_ID == 0) {
        // branch that generates elves ad reindeers
        process_ID = fork();
        if (process_ID == 0) {
            // branch that generates elves
            for (unsigned i = 0; i < ne; i++) { // nebo i<(ne-1), protoze posledni elf je generovan z pid > 0?
                pid_t elf_ID = fork();
                if (elf_ID == 0) {
                    // new elf generated
                    elf_process(f_out, i, te);
                } else if (elf_ID < 0) {    // error
                    cleanup(f_out);
                    fprintf(stderr, "PID je <0.\n");
                }
                // není tady poslední elf?
            }
        } else if (process_ID > 0) {
            // branch that generates reindeers
            for (unsigned i = 0; i < nr; i++) { // nr - 1, protoze posledni elf je generovan z pid > 0?
                pid_t reindeer_ID = fork();
                if (reindeer_ID == 0) {
                    // new elf generated
                    reindeer_process(f_out, i, tr);
                } else if (reindeer_ID < 0) {   // error
                    cleanup(f_out);
                    fprintf(stderr, "PID je <0.\n");
                }
                // poslední reindeer? 
            }
        } else {    // error
            cleanup(f_out);
            fprintf(stderr, "PID je <0.\n");
        }
    } else if (process_ID < 0) {    // error
        cleanup(f_out);
        fprintf(stderr, "PID je <0.\n");
    }

    // zde bezi main process

    /* Poté čeká na ukončení všech procesů, které aplikace vytváří. Jakmile jsou tyto procesy ukončeny, ukončí se i hlavní proces s kódem (exit code) 0 */
    // main process waits for other processes to end
    for (unsigned i = 0; i < (nr + ne + 1); i++) {  // +1 == +Santa
        wait(NULL);
    }
    
    
    if (cleanup(f_out)) {
        return 1;
    }
    
    return 0;

}
