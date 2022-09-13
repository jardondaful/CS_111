#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>

//show function
static int show_count(struct seq_file *cat_file, void *v)
{
  struct task_struct *p;
  int count = 0;
  for_each_process(p) {
    count++;
  }
  seq_printf(cat_file, "%d\n", count);
  return 0;
}

static int __init proc_count_init(void)
{
  //show function registration
  proc_create_single("count", 0777, NULL, show_count);
  
  pr_info("proc_count: init\n");
  return 0;
}

static void __exit proc_count_exit(void)
{
  //clean up work
  remove_proc_entry("count", NULL);
  
  pr_info("proc_count: exit\n");
}

module_init(proc_count_init);
module_exit(proc_count_exit);

MODULE_AUTHOR("Nikhil Isukapalli");
MODULE_DESCRIPTION("This program shows the number of running processes.");
MODULE_LICENSE("GPL");
