#include "packer.h"
#include <semaphore.h>

// You can declare global variables here
sem_t rg_mutex; //as a write lock (to prevent race conditions when updating values)
sem_t rg_blockShip; //A semaphore to block balls from being shipped off before the box is filled.
sem_t r_blockFill; //A semaphore to block balls from entering a filled box.
sem_t g_blockFill;
int rg_curr_boxSize = 0;
int rg_max_boxSize = 2;
int rg_ids[2] = {};

//.......................................................//

sem_t bbl_mutex; //as a write lock (to prevent race conditions when updating values)
sem_t bbl_blockShip; //A semaphore to block balls from being shipped off before the box is filled.
sem_t b_blockFill; //A semaphore to block balls from entering a filled box.
sem_t bl_blockFill;
int bbl_curr_boxSize = 0;
int bbl_max_boxSize = 2;
int bbl_ids[2] = {};

void packer_init(void) {
    // Write initialization code here (called once at the start of the program).
    sem_init(&rg_mutex, 0, 1);
    sem_init(&rg_blockShip, 0, 0);
    sem_init(&r_blockFill, 0, 1);
    sem_init(&g_blockFill, 0, 1);

    sem_init(&bbl_mutex, 0, 1);
    sem_init(&bbl_blockShip, 0, 0);
    sem_init(&b_blockFill, 0, 1);
    sem_init(&bl_blockFill, 0, 1);
}

void packer_destroy(void) {
    // Write deinitialization code here (called once at the end of the program).
    sem_destroy(&rg_mutex);
    sem_destroy(&rg_blockShip);
    sem_destroy(&r_blockFill);
    sem_destroy(&g_blockFill);

    sem_destroy(&bbl_mutex);
    sem_destroy(&bbl_blockShip);
    sem_destroy(&b_blockFill);
    sem_destroy(&bl_blockFill);
}

int pack_ball(int colour, int id) {
    // Write your code here.
    if (colour == 1 || colour == 2) {
        if (colour == 1) {
            sem_wait(&r_blockFill);
        }
        if (colour == 2) {
            sem_wait(&g_blockFill);
        }
        sem_wait(&rg_mutex);
        rg_ids[rg_curr_boxSize] = id;
        rg_curr_boxSize = rg_curr_boxSize + 1;
        sem_post(&rg_mutex);

        if (rg_curr_boxSize != rg_max_boxSize) {
            sem_wait(&rg_blockShip);
        }
        else {
            sem_post(&rg_blockShip);
        }
        sem_wait(&rg_mutex);
        int temp;
        for (int i = 0; i < rg_max_boxSize; i++) {
            if (rg_ids[i] != id) {
                temp = rg_ids[i];
            }
        }
        rg_curr_boxSize = rg_curr_boxSize - 1;
        if (rg_curr_boxSize == 0) {
            sem_post(&r_blockFill);
            sem_post(&g_blockFill);
        }
        sem_post(&rg_mutex);
        return temp;
    }

    if (colour == 3 || colour == 4) {
        if (colour == 3) {
            sem_wait(&b_blockFill);
        }
        else {
            sem_wait(&bl_blockFill);
        }
        sem_wait(&bbl_mutex);
        bbl_ids[bbl_curr_boxSize] = id;
        bbl_curr_boxSize = bbl_curr_boxSize + 1;
        sem_post(&bbl_mutex);


        if (bbl_curr_boxSize != bbl_max_boxSize) {
            sem_wait(&bbl_blockShip);
        }
        else {
            sem_post(&bbl_blockShip);
        }
        sem_wait(&bbl_mutex);
        int temp;
        for (int i = 0; i < bbl_max_boxSize; i++) {
            if (bbl_ids[i] != id) {
                temp = bbl_ids[i];
            }
        }
        bbl_curr_boxSize = bbl_curr_boxSize - 1;
        if (bbl_curr_boxSize == 0) {
            sem_post(&b_blockFill);
            sem_post(&bl_blockFill);
        }
        sem_post(&bbl_mutex);
        return temp;
    }
    return 0;
}