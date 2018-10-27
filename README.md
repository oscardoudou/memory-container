# CSC 501 Project 2: Memory Container

## Overview
In the previous project, you have experienced the power of adding a new abstraction, resource containers, in the system and use that as another way to schedule/allocate computing resource for tasks (e.g., processes, threads). In addition to processors, memory is another resource that tasks can share and resource containers can potentially help to manage.

In this project, we are going to extend the concept of resource containers to memory. A resource container can hold a few physical memory locations. Tasks within the same resource container can share these pool of memory locations upon requests. On the other hand, tasks do not belong to the same container cannot share memory locations, even though they are spawned from the same process.

To implement this abstraction, we plan to use a kernel module, as what you have done in the previous project. This kernel module supports a few ioctl commands that allow user-space library/applications to initialize a device, allocate memory locations, or assign a task to a resource container. The library interacts with both the application and the kernel module to translate the requests and responses between them.

You will be given the prototype of the kernel module with a `core.c` and `ioctl.c` file in its source directory that only contains empty functions. We also provide a user-space library that allows an application to interact with this kernel module through ioctl interfaces as well as a sample benchmark application that you may extend to test if your kernel module functions correctly.

You are strongly encouraged to work in a group of 2. Groups do the same project as individuals. Both members will receive the same grade. Note that working in groups may or may not make the project easier, depending on how the group interactions work out. If collaboration issues arise, contact your instructor as soon as possible: flexibility in dealing with such issues decreases as the deadline approaches. 

## Objective
* Learning UNIX/Linux kernel programming as well as the constraints
* Learning UNIX/Linux system memory management
* Learning UNIX/Linux kernel modules
* Learning multithreading programming

## Step
### Reserve a VCL machine or Install Virtual Machine
To begin, you need to first form a group and setup the environment for developing your project. You should set up a machine or a VMWare virtual machine (CS students should have free license for that https://www.csc.ncsu.edu/vmap/) with clean Ubuntu 16.04 installation. You also may use the VCL service maintained by NCSU through https://vcl.ncsu.edu/. You may reserve one virtual machine and connect to this machine remotely by selecting reservations. We will use the "Ubuntu 16.04 Base" to test your kernel module. However, the VCL service will reset once your reservation timeout. 

### Clone from the NCSU Github
Then, you need to clone the code from https://github.ncsu.edu/htseng3/CSC501_Container_Memory and make your own private repository. Please do not fork for the given repository, otherwise you will be the public repository.

### Kernel Compilation
```shell
cd kernel_module
sudo make clean
sudo make
sudo make install
cd ..
```

### User Space Library Compilation
```shell
cd library
sudo make clean
sudo make
sudo make install
cd ..
```

### Benchmark Compilation
```shell
cd benchmark
make clean
make
cd ..
```

### Run
```shell
./test.sh <num of objects> <max size of objects> <num of tasks> <num of containers>

# example
# growing on the number of processes 
./test.sh 128 4096 1 1
./test.sh 128 4096 2 1
./test.sh 128 4096 4 1

# growing on the number of objects
./test.sh 128 4096 1 1
./test.sh 512 4096 1 1
./test.sh 1024 4096 1 1
./test.sh 4096 4096 1 1

# growing on the object size
./test.sh 128 4096 1 1
./test.sh 128 8192 1 1

# growing on the number of containers
./test.sh 128 4096 2 2
./test.sh 128 4096 8 8
./test.sh 128 4096 64 64

# combination
./test.sh 256 8192 8 4
```
## Tasks
1. Implementing the process_container kernel module: it needs the following features:

    - __create__: you will need to support create operation that creates a container if the corresponding cid hasn't been assigned yet, and assign the task to the container. These create requests are invoked by the user-space library using ioctl interface. The ioctl system call will be redirected to `memory_container_ioctl` function located in `src/ioctl.c`

    - __delete__: you will need to support delete operation that removes tasks from the container. If there is no task in the container, the container should be destroyed as well. These delete requests are invoked by the user-space library using ioctl interface. The ioctl system call will be redirected to `memory_container_ioctl` function located in `src/ioctl.c`

    - __mmap__: you will need to support mmap, the interface that user-space library uses to request the mapping of kernel space memory into the user-space memory. The kernel module takes an offset from the user-space library and allocate the requested size associated with that offset. You may consider that offset as an object id. If an object associated with an offset was already created/requested since the kernel module is loaded, the mmap request should assign the address of the previously allocated object to the mmap request. The kernel module interface will call `memory_container_mmap()` in `src/core.c` to request an mmap operation. One of the parameters for the `memory_container_mmap()` is `struct vm_area_struct *vma`. This data structure contains page offset, starting virtual address, and etc, those you will need to allocate memory space.

    - __lock/unlock__: you will need to support locking and unlocking that guarantees only one process can access an object at the same time. These lock/unlock functions are invoked by the user-space library using ioctl interface. The ioctl system call will be redirected to `memory_container_ioctl` function located in `src/ioctl.c`

    - __free__: you will need to support delete operation that removes an object from memory_container. These delete requests are invoked by the user-space library using ioctl interface. The ioctl system call will be redirected to `memory_container_ioctl` function located in `src/ioctl.c`

2. Test the developed module: It's your responsibility to test the developed kernel module thoroughly. Our benchmark is just a starting point of your testing. The TA/grader will generate a different test sequence to test your program when grading. Your module should support an infinite number of containers and different numbers of tasks with each container.


## Useful Kernel Functions/Variables
### Mutex
```c
mutex_init(struct mutex *lock);
mutex_lock(struct mutex *lock);
mutex_unlock(struct mutex *lock);
```

### Misc.
```c
// function
unsigned long copy_from_user(void *to, 
    const void __user *from, 
    unsigned long n
);

// variable
struct task_struct *current;
```

### Memory Allocation
```c
// functions
kmalloc(size_t size, gfp_t flags);
kcalloc(size_t n, size_t size, gfp_t flags);
kfree(const void * objp);

// variables
gfp_t GFP_KERNEL;
```

### Page/Address Alignment
```c
// functions
phys_addr_t virt_to_phys(volatile void * address);
int remap_pfn_range(struct vm_area_struct *vma,
    unsigned long addr,
    unsigned long pfn,
    unsigned long size,
    pgprot_t prot
);


// variable
struct vm_area_struct *vma;
```

### Debug Message
```c
// functions
printk(const char *fmt, ...);
```

## Turn Ins
You only need to (or say you can only) turn in the `core.c` and the `ioctl.c` file in the `./kernel_module/src` directory as a tarball. All your modifications should be limited within these two files. Exactly 1 member of each group should submit the source code. All group members' names and Unity IDs should be easily found in a single line comment at the beginning of the code in the following format:

```c
// Project 2: 1st member's name, 1st member's Unity ID; 2nd member's name, 2nd member's Unity ID 
```

You need to name the tarball as `{1st_members_unityid}_{2nd_members_unityid}_mcontainer.tar.gz`

## References and Hints
1. You should try to figure out the interaction between user-space applications (e.g. benchmark, validate) and the library, the library and the kernel module. You should especial understand how mmap is used in npheap_alloc function from the library. Here is the explanation of mmap function http://man7.org/linux/man-pages/man2/mmap.2.html

2. You should try to figure out the interactions between user-space applications (e.g. benchmark) and the user-space library, the user-space library and the kernel module. 

3. You may need to reference the [Linux kernel programming guide](http://www.tldp.org/LDP/lkmpg/2.6/lkmpg.pdf) and [Linux Device Drivers, 3rd Edition](https://lwn.net/Kernel/LDD3/) since user-space libraries will not be available for kernel code.