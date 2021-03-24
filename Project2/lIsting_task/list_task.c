/**
 * simple.c
 *
 * A simple kernel module. 
 * 
 * To compile, run makefile by entering "make"
 *
 * Operating System Concepts - 10th Edition
 * Copyright John Wiley & Sons - 2018
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init_task.h>

/* This function is called when the module is loaded. */
int list_init(void)
{
       struct task_struct *task = NULL;
       task = &init_task;
       printk(KERN_INFO "Task module loaded");
       printk(KERN_INFO "UID\tPID\tPPID\tNAME\n");
       for_each_process(task){
              printk(KERN_INFO "%d\t%d\t%d\t%s\n",task->loginuid,task->pid,task->parent->pid,task->comm);
       }
       return 0;
}

/* This function is called when the module is removed. */
void list_exit(void) {
	printk(KERN_INFO "Task module removed.\n");
}

/* Macros for registering module entry and exit points. */
module_init( list_init );
module_exit( list_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Listing Tasks");
MODULE_AUTHOR("SGG");

