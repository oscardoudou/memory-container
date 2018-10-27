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
//     Validating the Results of Memory Container
//
////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <time.h>
#include <mcontainer.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>


int main(int argc, char *argv[])
{
    int i = 0, j = 0, error = 0;
    int number_of_objects = 1024, max_size_of_objects = 8192, number_of_containers = 1;
    int child_pid, cid, stat, devfd;
    long size;
    __u64 object_id, current_time;
    char *data, op, *mapped_data;
    char ***containers;
    pid_t *pid;

    // takes arguments from command line interface.
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s number_of_objects max_size_of_objects number_of_containers\n", argv[0]);
        exit(1);
    }

    number_of_objects = atoi(argv[1]);
    max_size_of_objects = atoi(argv[2]);
    number_of_containers = atoi(argv[3]);

    pid = (pid_t *) calloc(number_of_containers - 1, sizeof(pid_t));

    // allocate memory spaces
    data = (char *)malloc(max_size_of_objects * sizeof(char));
    containers = (char ***)calloc(number_of_containers, sizeof(char **));

    for (i = 0; i < number_of_containers; i++)
    {
        containers[i] = (char **)calloc(number_of_objects, sizeof(char *));
        for (j = 0; j < number_of_objects; j++)
        {
            containers[i][j] = (char *)malloc(max_size_of_objects * sizeof(char));
            memset(containers[i][j], 0, max_size_of_objects);
        }
    }

    // Replay the log to validate the results in containers.
    while (scanf("%c %d %d %llu %llu %ld %s", &op, &child_pid, &cid, &current_time, &object_id, &size, &data[0]) != EOF)
    {
        if (op == 'S')
        {
            strcpy(containers[cid][(int)object_id], data);
            memset(data, 0, max_size_of_objects);
        }
        else if (op == 'D')
        {
            memset(containers[cid][(int)object_id], 0, max_size_of_objects);
        }
    }

    // open the container kernel module to check the results.
    devfd = open("/dev/mcontainer", O_RDWR);
    if (devfd < 0)
    {
        fprintf(stderr, "Device open failed");
        exit(1);
    }

    // parent process forks children to validate the results in containers.
    for (i = 0; i < (number_of_containers - 1) && pid != 0; i++)
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

    cid = getpid() % number_of_containers;
    mcontainer_create(devfd, cid);

    for (i = 0; i < number_of_objects; i++)
    {
        mapped_data = (char *)mcontainer_alloc(devfd, i, max_size_of_objects);
        if (strcmp(mapped_data, containers[cid][i]) != 0)
        {
            fprintf(stderr, "Container %d Object %d has a wrong value %s v.s. %s\n", cid, i, mapped_data, containers[cid][i]);
            error++;
        }
    }

    // cleanup
    if (error == 0)
    {
        fprintf(stderr, "Container %d Pass\n", cid);
    }

    mcontainer_delete(devfd);
    
    close(devfd);
    free(data);
    for (i = 0; i < number_of_containers; i++)
    {
        for (j = 0; j < number_of_objects; j++)
        {
            free(containers[i][j]);
        }
        free(containers[i]);
    }
    free(containers);

    if (child_pid != 0)
    {
        for (i = 0; i < (number_of_containers - 1); i++)
        {
            waitpid(pid[i], &stat, 0);  
        }
    }

    free(pid);
    return 0;
}
