/**
 * HEADER
 * FIT VUT - IOS 2020/2021
 * Projekt 2
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>

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
}
/********************************************************************************************/
int main(int argc, char *argv[]) {

    unsigned ne, nr, te, tr;

    if (argc != 5) {
        printf("The number of arguments is %d\n", argc);
        return 1;
    }
    if (argument_parser(argv, &ne, &nr, &te, &tr)) {
        // cleanup();
        // help vypis
        printf("Invalid arguments.\n");
        return 1;
    }

    FILE *f_out = fopen("proj2.out", "w");
    if (f_out == NULL) {
        fprintf(stderr, "Error when opening file proj2.out\n");
        return 1;
    }
    
    if (initialize()) {
        return 1;
    }


    printf("NE is: %u\n", ne);
    printf("NR is: %u\n", nr);
    printf("TE is: %u\n", te);
    printf("TR is: %u\n", tr);








    if (cleanup(f_out)) {
        return 1;
    }
    return 0;

}
