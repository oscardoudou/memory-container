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
//     Header File of Memory Container in User Space
//
////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C"
{
#endif

#include <memory_container/memory_container.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

    int mcontainer_delete(int devfd);
    int mcontainer_create(int devfd, int cid);
    void *mcontainer_alloc(int devfd, __u64 offset, __u64 size);
    int mcontainer_lock(int devfd, __u64 offset);
    int mcontainer_unlock(int devfd, __u64 offset);
    int mcontainer_free(int devfd, __u64 offset);

#ifdef __cplusplus
}
#endif