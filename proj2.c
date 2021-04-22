/**
 * FIT VUT 2020/2021
 * IOS - Projekt 2
 * 
 * xsvobo1x
 */

#include "proj2.h"
//#include "prints.h"

int main (int argc, char *argv[])
{
    // Opening the file for printing output log
    FILE *f = fopen("proj2.out", "w");
    if (f == NULL) {
        error_message(f, ERR_FOPEN);
    }

    // Loading arguments into variable arguments
    args_t arguments = {0, 0, 0, 0};
    if (argument_parser(argc, argv, &arguments)){
        error_message(f, ERR_ARGS);
    }

    printf("NE (num of elves) is: %u\n", arguments.ne);
    printf("NR (num of reindeers) is: %u\n", arguments.nr);
    printf("TE is: %u\n", arguments.te);
    printf("TR is: %u\n", arguments.tr);

    // Initialization of semaphores and shared memory
    init_semaphores(f);
    init_memory(f);
    // Setting values to shared variables
    sh_mem->print_count = 0;
    sh_mem->active = 0;
    sh_mem->elves_cnt = 0;
    sh_mem->reindeers_cnt = 0;
    sh_mem->workshop_closed = false;
    sh_mem->elves_helped = 0;
    sh_mem->santa_helping = false;

    // Creating processes (number of processes = number of elves + number of reindeers + Santa)
    for(unsigned i = 0; i < arguments.ne + arguments.nr + 1; i++ ) {
        switch(fork()) {
            case 0:
                if(i == 0) {
                    santa_process(f, arguments);
                    //exit(0);
                } else if (i <= arguments.ne) {
                    elf_process(f, i-1, arguments);
                    //printf("Elf s id %u is working.\n", i);
                    switch(fork()) {
                        case 0:
                            break;
                        case -1:
                            error_message(f, ERR_FORK);
                            break;
                        default:
                            break;
                    }
                    exit(0);
                } else {
                    reindeer_process(f, i-arguments.ne, arguments);
                    switch(fork()) {
                        case 0:
                            break;
                        case -1:
                            error_message(f, ERR_FORK);
                            break;
                        default:
                            break;
                    }
                    exit(0);
                }
                break;
            case -1:
                error_message(f, ERR_FORK);
                break;
            default:
                break;
        }
    }

    // main process waits for its children to exit
    while(wait(NULL)) {
        if (errno == ECHILD) {
           break;
        }
    }

    cleanup_memory(f);
    cleanup_semaphores(f);
    fclose(f);
    exit(0);
}

/**
 * @brief Function maps shared memory. When error occurs it calls error_message() function.
 * 
 * @param f file that must be closed in case error occurs
 * @return void
 */
void init_memory(FILE *f)
{
    if ((sh_mem = mmap(NULL, sizeof(shared_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
        error_message(f, ERR_MEM_INIT);
}

/**
 * @brief Function maps and opens semaphores. When error occurs it calls error_message() function.
 * 
 * @param f file that must be closed in case error occurs
 * @return void
 */
void init_semaphores(FILE *f)
{
    // just to be sure they don't exist
    sem_unlink(SANTA_SEM);
    sem_unlink(ELVES_SEM);
    sem_unlink(CHRISTMAS_WAIT);
    sem_unlink(REINDEERS_SEM);
    sem_unlink(MUTEX_SEM);
    sem_unlink(PRINTING_SEM);
    sem_unlink(SANTA_HELP_SEM);

    bool error = false;
    /*
    // map named semaphores
    if((santa_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
        error = true;
    if((elves_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
        error = true;
    if((christmas_wait = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
        error = true;
    if((reindeers_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
        error = true;
    if((mutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
        error = true;
    if((printing = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
        error = true;
    */
    // open semaphores
    if ((santa_sem = sem_open(SANTA_SEM, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
        error = true;
    if ((elves_sem = sem_open(ELVES_SEM, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
        error = true;
    if ((christmas_wait = sem_open(CHRISTMAS_WAIT, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
        error = true;
    if ((reindeers_sem = sem_open(REINDEERS_SEM, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
        error = true;
    if ((mutex = sem_open(MUTEX_SEM, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
        error = true;
    if ((printing = sem_open(PRINTING_SEM, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
        error = true;
    if ((santa_help = sem_open(SANTA_HELP_SEM, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
        error = true;

    if(error)
        error_message(f, ERR_SEM_INIT);
}

/**
 * @brief Function unmaps shared memory. When error occurs it calls error_message() function.
 * 
 * @param f file that must be closed in case error occurs
 * @return void
 */
void cleanup_memory(FILE *f)
{
    if (munmap(sh_mem, sizeof(shared_t)))
        error_message(f, ERR_MEM_UNMAP);
}

/**
 * @brief Function closes and unlinks semaphores. When error occurs it calls error_message() function.
 * 
 * @param f file that must be closed in case error occurs
 * @return void
 */
void cleanup_semaphores(FILE *f)
{
    bool error = false;
    // close
    if(sem_close(santa_sem))
        error = true;
    if(sem_close(elves_sem))
        error = true;
    if(sem_close(christmas_wait))
        error = true;
    if(sem_close(reindeers_sem))
        error = true;
    if(sem_close(mutex))
        error = true;
    if(sem_close(printing))
        error = true;
    if(sem_close(santa_help))
        error = true;

    // unlink
    if(sem_unlink(SANTA_SEM))
        error = true;
    if(sem_unlink(CHRISTMAS_WAIT))
        error = true;
    if(sem_unlink(ELVES_SEM))
        error = true;
    if(sem_unlink(REINDEERS_SEM))
        error = true;
    if(sem_unlink(MUTEX_SEM))
        error = true;
    if(sem_unlink(PRINTING_SEM))
        error = true;
    if(sem_unlink(SANTA_HELP_SEM))
        error = true;

    if(error)
        error_message(f, ERR_SEM_DESTROY);
}

/**
 * @brief Argument parser
 * 
 * @param argc the number of command line arguments
 * @param argv command line arguments
 * @param arguments struct where parametres of the programm are stored (ne, nr, te, tr)
 * @return 0 on success, -1 when arguments are invalid
 */
int argument_parser(int argc, char *argv[], args_t *arguments) {
    long int tmp;
    char *endptr;

    // the only valid input os in format: ./proj2 NE NR TE TR
    if (argc != 5) {
        return -1;
    }

    // NE - the number of elves (0 < NE < 1000)
    tmp = strtol(argv[1], &endptr, 10);
    if (!(tmp > 0 && tmp < 1000 && endptr[0] == '\0'))
        return -1;
    arguments->ne = tmp;
    
    // NR - the number of reindeers (0 < NR < 20)
    tmp = strtol(argv[2], &endptr, 10);
    if (!(tmp > 0 && tmp < 20 && endptr[0] == '\0'))
        return -1;
    arguments->nr = tmp;

    // TE - the maximum time (in ms) for that the elf works independently (0 <= TE <= 1000)
    tmp = strtol(argv[3], &endptr, 10);
    if (!(tmp >= 0 && tmp <= 1000 && endptr[0] == '\0'))
        return -1;
    arguments->te = tmp;

    // TR - the maximum time (in ms) after that a reindeer returns home (0 <= TR <= 1000)
    tmp = strtol(argv[4], &endptr, 10);
    if (!(tmp >= 0 && tmp <= 1000 && endptr[0] == '\0'))
        return -1;
    arguments->tr = tmp;

    return 0;
}

/*************** PRINTS.C BEGIN *******************/
/**
 * @brief Function prints santa message to a file.
 * 
 * @param f file that must be closed in case error occurs
 * @param status id of message that should be printed
 * @return void
 */
void santa_message(FILE *f, int status) {
    sem_wait(printing);
    sh_mem->print_count++;
    switch(status) {
        case SANTA_SLEEP:
            fprintf(f, "%d: Santa: going to sleep\n", sh_mem->print_count);
            break;
        case SANTA_HELP:
            fprintf(f, "%d: Santa: helping elves\n", sh_mem->print_count);
            break;
        case SANTA_CLOSE:
            fprintf(f, "%d: Santa: closing workshop\n", sh_mem->print_count);
            break;
        case SANTA_CHRISTMAS:
            fprintf(f, "%d: Santa: Christmas started\n", sh_mem->print_count);
            break;
    }
    fflush(f);
    sem_post(printing);
}

/**
 * @brief Function prints elf message to a file.
 * 
 * @param f file that must be closed in case error occurs
 * @param status id of message that should be printed
 * @param elfID elf ID
 * @return void
 */
void elf_message(FILE *f, int status, unsigned elfID) {
    sem_wait(printing);
    sh_mem->print_count++;
    switch(status) {
        case ELF_START:
            fprintf(f, "%d: Elf %u: started\n", sh_mem->print_count, elfID);
            break;
        case ELF_NEED:
            fprintf(f, "%d: Elf %u: need help\n", sh_mem->print_count, elfID);
            break;
        case ELF_GET:
            fprintf(f, "%d: Elf %u: get help\n", sh_mem->print_count, elfID);
            break;
        case ELF_HOLIDAYS:
            fprintf(f, "%d: Elf %u: taking holidays\n", sh_mem->print_count, elfID);
            break;
    }
    fflush(f);
    sem_post(printing);
}

/**
 * @brief Function prints reindeer message to a file.
 * 
 * @param f file that must be closed in case error occurs
 * @param status id of message that should be printed
 * @param rdID reindeer ID
 * @return
 */
void rd_message(FILE *f, int status, unsigned rdID) {
    sem_wait(printing);
    sh_mem->print_count++;
    switch(status) {
        case RD_START:
            fprintf(f, "%d: RD %u: rstarted\n", sh_mem->print_count, rdID);
            break;
        case RD_RETURN:
            fprintf(f, "%d: RD %u: return home\n", sh_mem->print_count, rdID);
            break;
        case RD_HITCHED:
            fprintf(f, "%d: RD %u: get hitched\n", sh_mem->print_count, rdID);
            break;
    }
    fflush(f);
    sem_post(printing);
}

/**
 * @brief Function prints error message and cleans up.
 * 
 * @param f file that must be closed in case error occurs
 * @param status id of message that should be printed
 * @return void
 */
void error_message(FILE *f, int status) {
    switch(status) {
        case ERR_FOPEN:
            fprintf(stderr, "Error: Can't open the file \"proj2.out\"\n");
            exit(1);
        case ERR_ARGS:
            fprintf(stderr, "Error: Invalid arguments.\n");
            fclose(f);
            exit(2);
        case ERR_FORK:
            fprintf(stderr, "Error: Can't create another process.\n");
            cleanup_memory(f);
            cleanup_semaphores(f);
            fclose(f);
            exit(3);
        case ERR_SEM_INIT:
            fprintf(stderr, "Error: Can't initialize semaphores.\n");
            fclose(f);
            exit(3);
        case ERR_SEM_OPEN:
            fprintf(stderr, "Error\n");
            exit(1);
        case ERR_SEM_DESTROY:
            fprintf(stderr, "Error: Can't destroy semaphores.\n");
            exit(1);
        case DEBUG:
            fprintf(stderr, "Debuggg\n");
    }
}
/*************** PRINTS.C END *******************/

/*************** PROCESSES.C BEGIN **************/
/**
 * @brief Santa process
 * 
 * @param f log will be printed to this file
 * @param args structure in that command line arguments are stored (ne, nr, te, tr)
 * @return void
 */
void santa_process(FILE *f, args_t args) {
    while (1) {
        // Santa sleeps in his shop when process starts
        santa_message(f, SANTA_SLEEP);
        // santa sleeps and he is woken up by either elves or reindeers
        sem_wait(santa_sem);
        printf("Santa after sem_wait\n");
        // santa checks the status
        sem_wait(mutex);
            // critical section
            if (sh_mem->reindeers_cnt == args.nr) {
                //sem_post(mutex);
                // Santa closes his workshop
                santa_message(f, SANTA_CLOSE);
                // reindeers will be hitched
                for(unsigned i = 0; i < args.nr; i++) {
                    sem_post(reindeers_sem);
                }
                //sem_wait(mutex);
                sh_mem->workshop_closed = true;
                while (sh_mem->elves_cnt > 0) { // nebo >=
                    sem_post(elves_sem);
                    sh_mem->elves_cnt--;
                }
                sem_post(mutex);
                break;
            // if there are 3 elves waiting, Santa helps them
            } else if (sh_mem->elves_cnt >= 3) {
                santa_message(f, SANTA_HELP);
                sh_mem->elves_helped=3;
                for (unsigned i = 0; i < 3; i++) {
                    sem_post(elves_sem);
                }
                //sh_mem->santa_helping = true;
            } 
        sem_post(mutex);

        //sem_wait(santa_help);
    }
    // all reindeers are hitched - Christmas can start
    sem_wait(christmas_wait);
    santa_message(f, SANTA_CHRISTMAS);

    exit(0);
}

/**
 * @brief Elf process
 * 
 * @param f log will be printed to this file
 * @param elfID ID of an elf
 * @param args structure in that command line arguments are stored (ne, nr, te, tr)
 * @return void
 */
void elf_process(FILE *f, unsigned elfID, args_t args) {
    // elf works in a loop until holidays start
    // elf started
    elf_message(f, ELF_START, elfID);

    while (1) {
        // elf works independently
        srand(time(NULL) * getpid() + elfID);
        if (usleep(rand() % (args.te*1000 + 1))) {
            // error
        }

        // when elf stops working he needs help from Santa
        elf_message(f, ELF_NEED, elfID);
        
        // when Christmas warning is on the workshop elves take holidays
        sem_wait(mutex);
            // critical section
            if (sh_mem->workshop_closed) {
                sem_post(mutex);
                break;
            }
            // when 3 elves are waiting for Santa they wake up him
            sh_mem->elves_cnt++;
            if (sh_mem->elves_cnt >= 3) {
                sem_post(santa_sem);
            }
        sem_post(mutex);
        
        // he waits in front of workshop
        sem_wait(elves_sem);
        
        // elf got help
        sem_wait(mutex);
            // critical section
            // if holidays start the elves will break
            if (sh_mem->workshop_closed) {
                sem_post(mutex);
                break;
            }
        sem_post(mutex);

        elf_message(f, ELF_GET, elfID);

        /*
        sem_wait(mutex);
            // critical section
            sh_mem->elves_helped--;
            sh_mem->elves_cnt--;
            if (sh_mem->elves_helped == 0) {
                // probudit santu
                sem_post(santa_help);
                //sh_mem->santa_helping = false;
            }
        sem_post(mutex);
        */
       // he goes back to work
    }
    // Christmas started - elves are taking holidays
    elf_message(f, ELF_HOLIDAYS, elfID);

    exit(0);
}

/**
 * @brief Reindeer process
 * 
 * @param f log will be printed to this file
 * @param rdID ID of a reindeer
 * @param args structure in that command line arguments are stored (ne, nr, te, tr)
 * @return void
 */

void reindeer_process(FILE *f, unsigned rdID, args_t args) {
    // reindeer started
    rd_message(f, RD_START, rdID);

    // reindeer in on holidays
    srand(time(NULL) * getpid() + rdID);
    if (usleep(rand() % (args.tr*1000 - (args.tr*1000)/2 + 1) + (args.tr*1000)/2)) {
        // error
    }

    // reindeer returns from holiday
    rd_message(f, RD_RETURN, rdID);

    // reindeer waits for Santa to get hitched
    // when the last reindeer returns he wakes up Santa
    sem_wait(mutex);
        // critical section
        sh_mem->reindeers_cnt++;
        if (sh_mem->reindeers_cnt == args.nr) {
            sem_post(santa_sem);
        }
    sem_post(mutex);

    // waits to get hitched
    sem_wait(reindeers_sem);
    // reindeer got hitched
    rd_message(f, RD_HITCHED, rdID);

    // waiting for all reindeers to get hitched
    sem_wait(mutex);
        // critical section
        sh_mem->reindeers_cnt--;
        if (sh_mem->reindeers_cnt == 0) {
            sem_post(christmas_wait);
        }
    sem_post(mutex);

    exit(0);
}
/*************** PROCESSES.C END ****************/