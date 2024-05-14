#include <semaphore.h>
#include <stddef.h>

#include "packer.h"

// You can declare global variables here
sem_t r_mutex; //as a write lock (to prevent race conditions when updating values)
sem_t r_blockShip; //A semaphore to block balls from being shipped off before the box is filled.
sem_t r_blockFill; //A semaphore to block balls from entering a filled box.
int r_curr_boxSize = 0; //keeps track of the number of balls in the box
int r_max_boxSize; //a constant describing the maximum box size
int r_ids[64] = {}; //array to store the ids of the balls, initialised to the maximum possible size
int r_count = 0; //used to ensure that all balls in the box have been accounted for,and reaches maximum state of the max box size 
int r_shipCount = 0; //used to ensure that the box have been filled before proceeding to the accounting stage,and reaches maximum state of the max box size - 1
int r_shipDone = -1; //used to ensure that only the last ball in the pack to enter the box is the one waking up the blockShip semaphore, reaches maximum state of 0

sem_t g_mutex; 
sem_t g_blockShip; 
sem_t g_blockFill; 
int g_curr_boxSize = 0;
int g_max_boxSize;
int g_ids[64] = {};
int g_count = 0;
int g_shipCount = 0;
int g_shipDone = -1;

sem_t b_mutex; 
sem_t b_blockShip; 
sem_t b_blockFill; 
int b_curr_boxSize = 0;
int b_max_boxSize;
int b_ids[64] = {};
int b_count = 0;
int b_shipCount = 0;
int b_shipDone = -1;

void packer_init(int balls_per_pack) {
    // initialise semaphores
    sem_init(&r_mutex, 0, 1);
    sem_init(&r_blockShip, 0, 0);
    sem_init(&r_blockFill, 0, balls_per_pack);
    r_max_boxSize = balls_per_pack;

    sem_init(&g_mutex, 0, 1);
    sem_init(&g_blockShip, 0, 0);
    sem_init(&g_blockFill, 0, balls_per_pack);
    g_max_boxSize = balls_per_pack;

    sem_init(&b_mutex, 0, 1);
    sem_init(&b_blockShip, 0, 0);
    sem_init(&b_blockFill, 0, balls_per_pack);
    b_max_boxSize = balls_per_pack;
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

void pack_ball(int colour, int id, int *other_ids) {
    if (colour == 1) {
        sem_wait(&r_blockFill);
        
        sem_wait(&r_mutex);
        r_ids[r_curr_boxSize] = id;
        r_curr_boxSize = r_curr_boxSize + 1;
        sem_post(&r_mutex);

        sem_wait(&r_mutex);
        if (r_curr_boxSize != r_max_boxSize) {
            r_shipCount++;
            sem_post(&r_mutex);

            sem_wait(&r_blockShip);
        }
        else {
            if (r_shipDone != 0) {
                for (int i = 0; i < r_shipCount; i++) {
                    sem_post(&r_blockShip);
                }
                r_shipDone = 0; //ensures that only one thread gets to do the posting
            }
            sem_post(&r_mutex);
        }
        sem_wait(&r_mutex);
        int size_OtherId = 0;
        for (int i = 0; i < r_max_boxSize; i++) {
            if (r_ids[i] != id) {
                other_ids[size_OtherId] = r_ids[i];
                size_OtherId = size_OtherId + 1;
            }
        }
        r_count++;
        if (r_count == r_max_boxSize) {
            r_count = 0;
            r_curr_boxSize = 0;
            r_shipCount = 0;
            r_shipDone = -1;
            for (int j = 0; j < r_max_boxSize; j++) {
                sem_post(&r_blockFill);
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

        sem_wait(&g_mutex);
        if (g_curr_boxSize != g_max_boxSize) {
            g_shipCount++;
            sem_post(&g_mutex);

            sem_wait(&g_blockShip);
        }
        else {
            if (g_shipDone != 0) {
                for (int i = 0; i < g_shipCount; i++) {
                    sem_post(&g_blockShip);
                }
                g_shipDone = 0; 
            }
            sem_post(&g_mutex);
        }

        sem_wait(&g_mutex);
        int size_OtherId = 0;
        for (int i = 0; i < g_max_boxSize; i++) {
            if (g_ids[i] != id) {
                other_ids[size_OtherId] = g_ids[i];
                size_OtherId = size_OtherId + 1;
            }
        }
        
        g_count = g_count + 1;
        if (g_count == g_max_boxSize) {
            g_count = 0;
            g_curr_boxSize = 0;
            g_shipCount = 0;
            g_shipDone = -1;
            for (int j = 0; j < g_max_boxSize; j++) {
                sem_post(&g_blockFill);
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

        sem_wait(&b_mutex);
        if (b_curr_boxSize != b_max_boxSize) {
            b_shipCount++;
            sem_post(&b_mutex);

            sem_wait(&b_blockShip);
        }
        else {           
            if (b_shipDone != 0) {
                for (int i = 0; i < b_shipCount; i++) {
                    sem_post(&b_blockShip);
                }
                b_shipDone = 0;
            }
            sem_post(&b_mutex); 
        }

        sem_wait(&b_mutex);
        int size_OtherId = 0;
        for (int i = 0; i < b_max_boxSize; i++) {
            if (b_ids[i] != id) {               
                other_ids[size_OtherId] = b_ids[i];
                size_OtherId = size_OtherId + 1;
                
            }
        }
        b_count++;
        if (b_count == b_max_boxSize) {
            b_count = 0;
            b_curr_boxSize = 0;
            b_shipCount = 0;
            b_shipDone = -1;
            for (int j = 0; j < b_max_boxSize; j++) {
                sem_post(&b_blockFill);
            }
        }
        sem_post(&b_mutex);
    }
}