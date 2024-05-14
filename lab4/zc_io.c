#include "zc_io.h"

#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

// The zc_file struct is analogous to the FILE struct that you get from fopen.
struct zc_file {
    // Insert the fields you need here.
    char * v_mem;
    int file_des;
    off_t offset;
    size_t file_size;
	sem_t roomEmpty;
	sem_t mutex;
	int num_of_readers;
};

/**************
 * Exercise 1 *
 **************/

zc_file *zc_open(const char *path) {
    // To implement
    char *addr;
    int fd;
    struct stat sb;
    off_t offset;
    size_t length;   
    fd = open(path, O_CREAT | O_RDWR, 0600);
    offset = 0;
    fstat(fd, &sb);
    length = sb.st_size;
    if (length == 0) {
    	//mapped to a page in memory if empty file created
    	addr = mmap(NULL, sysconf(_SC_PAGE_SIZE), PROT_READ | PROT_WRITE,
                MAP_SHARED, fd, offset);
    } else {
    	addr = mmap(NULL, length, PROT_READ | PROT_WRITE,
                MAP_SHARED, fd, offset);
    }
    if (addr == MAP_FAILED) {
    	printf("The error message is : %s\n", 
                         strerror(errno));
    	perror("Message from perror");;
    	
    	return NULL;
    }
    zc_file * file = malloc(sizeof(zc_file));
    file->v_mem = addr;
    file->file_des = fd;
    file->offset = offset;
    file->file_size = length;
    sem_init(&file->roomEmpty, 0, 1);
    sem_init(&file->mutex,0,1);
    file->num_of_readers = 0;
    
    return file;
}

int zc_close(zc_file *file) {
    // To implement
    int sync_result = msync(file->v_mem, file->file_size, MS_SYNC);
    if (sync_result == -1) {
    	printf("Error occurred during closing when flushing contents into file");
    	exit(1);
    	return -1;
    }
    int result = munmap(file->v_mem, file->file_size);
    if (result == -1) {
    	return -1;
    }
    close(file->file_des);
    sem_destroy(&file->roomEmpty);
    sem_destroy(&file->mutex);
    free(file);
    return 0;
}

const char *zc_read_start(zc_file *file, size_t *size) {	
    sem_wait(&file->mutex);
    file->num_of_readers = file->num_of_readers + 1;
    //When there is at least one reader reading, all writers shall not write
    if (file->num_of_readers == 1) {
    	sem_wait(&file->roomEmpty);
   	}
   	size_t remainder = file->file_size - (size_t) file->offset;
    off_t offset_change = file->offset;
    off_t local_offset = file->offset;
    char * local_v_mem = file->v_mem;
   	sem_post(&file->mutex);
    
    if (remainder < *size) {
    	//"If the file contains less than size bytes remaining,
    	//then the number of bytes available should be written
    	//to size"
    	    	
    	//Shift pointer to end of file
    	offset_change = local_offset + (off_t) remainder;
    	*size = remainder;
    } else {
    	offset_change = local_offset + (off_t) *size;
    }
    sem_wait(&file->mutex);
    file->offset = offset_change;
    sem_post(&file->mutex);
    return local_v_mem + (size_t) local_offset;
}

void zc_read_end(zc_file *file) {    
    sem_wait(&file->mutex);
    file->num_of_readers = file->num_of_readers - 1;
    if (file->num_of_readers == 0) {
    	sem_post(&file->roomEmpty);
    }
    sem_post(&file->mutex);
}

/**************
 * Exercise 2 *
 **************/

char *zc_write_start(zc_file *file, size_t size) {
	//"The function returns the pointer to a buffer of at least
	//size bytes that can be written. Data writtten to this buffer
	//will be written to file later."
	//
    sem_wait(&file->roomEmpty);
    size_t new_offset = ((size_t)file->offset) + size;
    if (new_offset > file->file_size) {
    	//"If file is not mapped to virtual address yet or size of mapped memory is less than size,
    	//then increase size of file and update mapping in virtual memory"
    	int truncate_result = ftruncate(file->file_des, new_offset);
    	if (truncate_result == -1) {
    		printf("Error occurred while truncating");
    		exit(1);
    	}
    	struct stat sb;
    	size_t new_size;
    	fstat(file->file_des, &sb);
    	new_size = sb.st_size;
    	void* new_mapping = mremap(file->v_mem, file->file_size, new_size, MREMAP_MAYMOVE);
    	if (new_mapping == (void *)-1) {
    		printf("Error occurred during remapping");
    		exit(1);
    	}
    	file->file_size = new_size;
    	file->v_mem = new_mapping;
 	}
    //"Update offset"
    off_t local_offset = file->offset;
    file->offset = (off_t) new_offset;
    return file->v_mem + (size_t) local_offset;
}

void zc_write_end(zc_file *file) {
	//"The function is called when a writing operation on file ends and pushes to
	//the file on disk any changes that might have been done in the buffer between zc_write_start
	//and zc_write_end"
    int sync_result = msync(file->v_mem, file->file_size, MS_SYNC);
    if (sync_result == -1) {
    	printf("Error flushing contents into file during write");
    	exit(1);
    }
    sem_post(&file->roomEmpty);
}

/**************
 * Exercise 3 *
 **************/

off_t zc_lseek(zc_file *file, long offset, int whence) {
    //Returns the resulting offset location as measured in bytes from the beginning of the file or (off_t) -1 if an error occurs
    sem_wait(&file->roomEmpty);
    off_t res_off;
    if (whence == 0) {
    //: offset is relative to the start of the file
    	file->offset = offset;
    } else if (whence == 1) {
    //offset is relative to the current position indicator
    	file->offset = file->offset + offset;   	
    } else if (whence == 2) {
    //: offset is relative to the end-of-file
    	file->offset = file->file_size + offset;
    } else {
    	return -1;
    }
    res_off = file->offset;
    sem_post(&file->roomEmpty);
    return res_off;
}

/**************
 * Exercise 4 *
 **************/

int zc_copyfile(const char *source, const char *dest) {    
    //first open source
    zc_file * src_f =  zc_open(source);
    //read the source
    size_t * file_size = &(src_f->file_size);
    zc_read_start(src_f, file_size);
    zc_read_end(src_f);
    //write the source into dest
    zc_file * dest_f = zc_open(dest);
    zc_write_start(dest_f, src_f->file_size);
    memcpy(dest_f->v_mem, src_f->v_mem, src_f->file_size);
    zc_write_end(dest_f);
    //close the source and dest
    int close_src = zc_close(src_f);
    int close_dest = zc_close(dest_f);
    if (close_src + close_dest < 0) {
    	return -1;
    }
    return 0;
}
