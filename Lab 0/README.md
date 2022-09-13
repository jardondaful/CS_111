# A Kernel Seedling

This kernel module shows the current number of running processes.

## Building

First, run 'make' to build the module.
Then, run 'sudo insmod proc_count.ko' to insert the model into the kernel.

## Running

Run 'cat /proc/count' to view the contents of the /proc/count file.
The expected output will be an integer indicating the current number
of running processes.

## Cleaning Up

Run 'sudo rmmod proc_count' to remove the model from the kernel. Then run 'make clean' to get rid of the object and executable files.

## Testing

I tested the module on the kernel release version 5.14.8-arch1-1.

