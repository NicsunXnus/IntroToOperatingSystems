#include "packer.h"
#include <semaphore.h>

// You can declare global variables here
sem_t r_mutex; //as a write lock (to prevent race conditions when updating values)
sem_t r_blockShip; //A semaphore to block balls from being shipped off before the box is filled.
sem_t r_blockFill; //A semaphore to block balls from entering a filled box.
int r_curr_boxSize = 0;
int r_max_boxSize = 2;
int r_ids[2] = {};

sem_t g_mutex; //as a write lock (to prevent race conditions when updating values)
sem_t g_blockShip; //A semaphore to block balls from being shipped off before the box is filled.
sem_t g_blockFill; //A semaphore to block balls from entering a filled box.
int g_curr_boxSize = 0;
int g_max_boxSize = 2;
int g_ids[2] = {};

sem_t b_mutex; //as a write lock (to prevent race conditions when updating values)
sem_t b_blockShip; //A semaphore to block balls from being shipped off before the box is filled.
sem_t b_blockFill; //A semaphore to block balls from entering a filled box.
int b_curr_boxSize = 0;
int b_max_boxSize = 2;
int b_ids[2] = {};

void packer_init(void) {
    // initialise semaphores
    sem_init(&r_mutex, 0, 1);
    sem_init(&r_blockShip, 0, 0);
    sem_init(&r_blockFill, 0, 2);
    
    sem_init(&g_mutex, 0, 1);
    sem_init(&g_blockShip, 0, 0);
    sem_init(&g_blockFill, 0, 2);

    sem_init(&b_mutex, 0, 1);
    sem_init(&b_blockShip, 0, 0);
    sem_init(&b_blockFill, 0, 2);
}

void packer_destroy(void) {
    // Write deinitialization code here (called once at the end of the program).
    sem_destroy(&r_mutex);
    sem_destroy(&r_blockShip);
    sem_destroy(&r_blockFill);

    sem_destroy(&g_mutex);
    sem_destroy(&g_blockShip);
    sem_destroy(&g_blockFill);

    sem_destroy(&b_mutex);
    sem_destroy(&b_blockShip);
    sem_destroy(&b_blockFill);
}

int pack_ball(int colour, int id) {
    if (colour == 1) {
        sem_wait(&r_blockFill);

        sem_wait(&r_mutex);
        r_ids[r_curr_boxSize] = id;
        r_curr_boxSize = r_curr_boxSize + 1;
        sem_post(&r_mutex);


        if (r_curr_boxSize != r_max_boxSize) {
            sem_wait(&r_blockShip);
        }
        else {
            sem_post(&r_blockShip);
        }
        sem_wait(&r_mutex);
        for (int i = 0; i < r_max_boxSize; i++) {
            if (r_ids[i] != id) {
                int temp = r_ids[i];
                r_curr_boxSize = r_curr_boxSize - 1;
                if (r_curr_boxSize == 0) {
                    for (int j = 0; j < r_max_boxSize; j++) {
                        sem_post(&r_blockFill);
                    }                   
                }
                sem_post(&r_mutex);
                return temp;
            }
        }
        sem_post(&r_mutex);
    }

    if (colour == 2) {
        sem_wait(&g_blockFill);

        sem_wait(&g_mutex);
        g_ids[g_curr_boxSize] = id;
        g_curr_boxSize = g_curr_boxSize + 1;
        sem_post(&g_mutex);


        if (g_curr_boxSize != g_max_boxSize) {
            sem_wait(&g_blockShip);
        }
        else {
            sem_post(&g_blockShip);
        }
        sem_wait(&g_mutex);
        for (int i = 0; i < g_max_boxSize; i++) {
            if (g_ids[i] != id) {
                int temp = g_ids[i];
                g_curr_boxSize = g_curr_boxSize - 1;
                if (g_curr_boxSize == 0) {
                    for (int j = 0; j < g_max_boxSize; j++) {
                        sem_post(&g_blockFill);
                    }                   
                }
                sem_post(&g_mutex);
                return temp;
            }
        }
        sem_post(&g_mutex);
    }

    if (colour == 3) {
        sem_wait(&b_blockFill);

        sem_wait(&b_mutex);
        b_ids[b_curr_boxSize] = id;
        b_curr_boxSize = b_curr_boxSize + 1;
        sem_post(&b_mutex);


        if (b_curr_boxSize != b_max_boxSize) {
            sem_wait(&b_blockShip);
        }
        else {
            sem_post(&b_blockShip);
        }
        sem_wait(&b_mutex);
        for (int i = 0; i < b_max_boxSize; i++) {
            if (b_ids[i] != id) {
                int temp = b_ids[i];
                b_curr_boxSize = b_curr_boxSize - 1;

                if (b_curr_boxSize == 0) {
                    for (int j = 0; j < b_max_boxSize; j++) {
                        sem_post(&b_blockFill);
                    }                   
                }
                sem_post(&b_mutex);
                return temp;
            }
        }
        sem_post(&b_mutex);
    }

    return 0;
}