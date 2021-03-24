/**
 * "Virtual" CPU that also maintains track of system time.
 */

#include <stdio.h>

#include "cpu.h"

// run this task for the specified time slice
void run(Task *task, int slice) {
    printf("Running task = [%s] [%d] [%d] [%d] for %d units.\n",task->name, task->tid,task->priority, task->burst, slice);
}
