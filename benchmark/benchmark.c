//////////////////////////////////////////////////////////////////////
//                      North Carolina State University
//
//
//
//                             Copyright 2016
//
////////////////////////////////////////////////////////////////////////
//
// This program is free software; you can redistribute it and/or modify it
// under the terms and conditions of the GNU General Public License,
// version 2, as published by the Free Software Foundation.
//
// This program is distributed in the hope it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
//
////////////////////////////////////////////////////////////////////////
//
//   Author:  Hung-Wei Tseng, Yu-Chia Liu
//
//   Description:
//     Running Applications on Memory Container
//
////////////////////////////////////////////////////////////////////////

#include <mcontainer.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/syscall.h>

int main(int argc, char *argv[])
{
    // variable initialization
    int i = 0; 
    int number_of_processes = 1, number_of_objects = 1024, max_size_of_objects = 8192, number_of_containers = 1;
    int a, j, cid, size, stat, child_pid, devfd, max_size_of_objects_with_buffer;
    char filename[256];
    char *mapped_data, *data;
    unsigned long long msec_time;
    FILE *fp;
    struct timeval current_time;
    pid_t *pid; 

    // takes arguments from command line interface.
    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s number_of_objects max_size_of_objects number_of_processes number_of_containers\n", argv[0]);
        exit(1);
    }

    number_of_objects = atoi(argv[1]);
    max_size_of_objects = atoi(argv[2]);
    number_of_processes = atoi(argv[3]);
    number_of_containers = atoi(argv[4]);

    max_size_of_objects_with_buffer = max_size_of_objects + 100;
    pid = (pid_t *) calloc(number_of_processes - 1, sizeof(pid_t));

    // open the kernel module to use it
    devfd = open("/dev/mcontainer", O_RDWR);
    if (devfd < 0)
    {
        fprintf(stderr, "Device open failed");
        exit(1);
    }

    // parent process forks children
    for (i = 0; i < (number_of_processes - 1); i++)
    {
        child_pid = fork();
        if (child_pid == 0)
        {
            break;
        }
        else
        {
            pid[i] = child_pid;
        }
    }

    data = (char *) malloc(max_size_of_objects_with_buffer * sizeof(char));

    // create the log file
    srand((int)time(NULL) + (int)getpid());
    sprintf(filename, "mcontainer.%d.log", (int)getpid());
    fp = fopen(filename, "w");

    // create/link this process to a container.
    cid = getpid() % number_of_containers;
    mcontainer_create(devfd, cid);

    // Writing to objects
    for (i = 0; i < number_of_objects; i++)
    {
        mcontainer_lock(devfd, i);
        mapped_data = (char *)mcontainer_alloc(devfd, i, max_size_of_objects);

        // error handling
        if (!mapped_data)
        {
            fprintf(stderr, "Failed in mcontainer_alloc()\n");
            exit(1);
        }

        // generate a random number to write into the object.
        a = rand() + 1;

        // starts to write the data to that address.
        gettimeofday(&current_time, NULL);
        for (j = 0; j < max_size_of_objects_with_buffer - 10; j = strlen(data))
        {
            sprintf(data, "%s%d", data, a);
        }
        strncpy(mapped_data, data, max_size_of_objects-1);
        mapped_data[max_size_of_objects-1] = '\0';
        
        // prints out the result into the log
        fprintf(fp, "S\t%d\t%d\t%ld\t%d\t%d\t%s\n", getpid(), cid, current_time.tv_sec * 1000000 + current_time.tv_usec, i, max_size_of_objects, mapped_data);
        mcontainer_unlock(devfd, i);
        memset(data, 0, max_size_of_objects_with_buffer);
    }

    // try delete something
    i = rand() % number_of_objects;
    mcontainer_lock(devfd, i);
    gettimeofday(&current_time, NULL);
    mcontainer_free(devfd, i);
    fprintf(fp, "D\t%d\t%d\t%ld\t%d\t%d\t%s\n", getpid(), cid, current_time.tv_sec * 1000000 + current_time.tv_usec, i, max_size_of_objects, "delete_an_object");
    mcontainer_unlock(devfd, i);
    
    
    // done with works, cleanup and wait for other processes.
    mcontainer_delete(devfd);
    close(devfd);
    if (child_pid != 0)
    {
        for (i = 0; i < (number_of_processes - 1); i++)
        {
            waitpid(pid[i], &stat, 0);  
        }
    }
    free(pid);
    free(data);
    return 0;
}
