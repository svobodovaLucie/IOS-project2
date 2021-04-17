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

    // Creating processes (number of processes = number of elves + number of reindeers + Santa)
    for(unsigned i = 0; i < arguments.ne + arguments.nr + 1; i++ ) {
        switch(fork()) {
            case 0:
                if(i == 0) {
                    santa_process(f);
                    exit(0);
                } else if (i <= arguments.ne) {
                    elf_process(f, i-1, arguments.te);
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
                    reindeer_process(f, i-arguments.ne, arguments.tr);
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
    sem_unlink(santa_SEM);
    sem_unlink(elf_SEM);
    sem_unlink(mutex_SEM);
    sem_unlink(printing_SEM);


    bool error = false;
    // map named semaphores
    if((santa = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
        error = true;
    if((elf = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
        error = true;
    if((mutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
        error = true;
    if((printing = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
        error = true;

    // open semaphores
    if ((santa = sem_open(santa_SEM, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
        error = true;
    if ((elf = sem_open(elf_SEM, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
        error = true;
    if ((mutex = sem_open(mutex_SEM, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
        error = true;
    if ((printing = sem_open(printing_SEM, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
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
    if(sem_close(santa))
        error = true;
    if(sem_close(elf))
        error = true;
    if(sem_close(mutex))
        error = true;
    if(sem_close(printing))
        error = true;

    // unlink
    if(sem_unlink(santa_SEM))
        error = true;
    if(sem_unlink(elf_SEM))
        error = true;
    if(sem_unlink(mutex_SEM))
        error = true;
    if(sem_unlink(printing_SEM))
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
    }
}
/*************** PRINTS.C END *******************/

/*************** PROCESSES.C BEGIN **************/
/**
 * @brief Santa process
 * 
 * @param f log will be printed to this file
 * @return void
 */
void santa_process(FILE *f) {
    santa_message(f, SANTA_SLEEP);
    fprintf(f, "Santaaa\n");
    exit(0);
}

/**
 * @brief Elf process
 * 
 * @param f log will be printed to this file
 * @param elfID ID of an elf
 * @param te the maximum time (in ms) for that the elf works independently (0 <= TE <= 1000)
 * @return void
 */
void elf_process(FILE *f, unsigned elfID, unsigned te) {
    elf_message(f, ELF_START, elfID);
    (void)te;
    //fprintf(f, "Elf %u a te je %u\n", elfID, te);
    exit(0);
}

/**
 * @brief Reindeer process
 * 
 * @param f log will be printed to this file
 * @param rdID ID of a reindeer
 * @param tr the maximum time (in ms) after that a reindeer returns home (0 <= TR <= 1000)
 * @return void
 */
void reindeer_process(FILE *f, unsigned rdID, unsigned tr) {
    rd_message(f, RD_START, rdID);
    (void)tr;
    //fprintf(f, "RD %u a te je %u\n", rdID, tr);
    exit(0);
}

/*************** PROCESSES.C END ****************/