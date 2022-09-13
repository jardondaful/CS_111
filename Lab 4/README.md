# Hey! I'm Filing Here

This code builds a 1 MiB ext2 file system with 2 directories, 1 regular file, and 1 symbolic link.

## Building

Run 'make' to build the program.

## Running

First, run './ext2-create', which runs the executable and creates cs111-base.img.
Then, run 'mkdir mnt', which creates a directory called 'mnt' to mount the filesystem to.
Running 'sudo mount -o loop cs111-base.mnt' mounts the filesystem and lets you use a file.
Run 'cd mnt' to switch into the mounted directory.
Running 'ls -ain' gives the output:

total 7
     2 drwxr-xr-x 3    0    0 1024 Jun  5 21:16 .
942069 drwxr-xr-x 4 1000 1000 4096 Jun  5 21:17 ..
    13 lrw-r--r-- 1 1000 1000   11 Jun  5 21:16 hello -> hello-world
    12 -rw-r--r-- 1 1000 1000   12 Jun  5 21:16 hello-world
    11 drwxr-xr-x 2    0    0 1024 Jun  5 21:16 lost+found

## Cleaning up

First, switch back to the directory containing 'mnt' and run 'sudo umount mnt' to unmount the filesystem.
Then, run 'rmdir mnt' to delete the directory used for mounting.
Finally, run 'make clean' to get rid of the object and executable files.
