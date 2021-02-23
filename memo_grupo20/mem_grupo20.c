#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>
#include <linux/hugetlb.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/swap.h>
#include <asm/page.h>
#include <linux/mmzone.h>
#include <linux/cpumask.h>
#include <linux/kernel_stat.h>

#define BUFSIZE  150
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Ecribir informacion de la memoria ram.");
MODULE_AUTHOR("Grupo20");
struct sysinfo inf;

static int escribir_archivo(struct seq_file * archivo, void *v){
    si_meminfo(&inf);
    long total_memoria = (inf.totalram *4 );
    long memoria_libre = (inf.freeram * 4);
    long buffer = (inf.bufferram);
    long cached= (global_node_page_state(NR_FILE_PAGES) * 2 )- inf.bufferram ;
    long memoria_utilizada = total_memoria - (memoria_libre+buffer +cached);
    long porcentaje =((memoria_utilizada *100)/total_memoria);

    seq_printf(archivo,"{\n");
    seq_printf(archivo,"\"Total\":\"%lu\",\n", total_memoria /1024); //total memoria ram
    seq_printf(archivo,"\"Uso\":\"%lu\",\n", memoria_utilizada/1024); // total memoria consumida
    seq_printf(archivo,"\"Porcentaje\":\"%li\"\n",porcentaje); //porcentaje de consumo
     seq_printf(archivo,"} \n");
    return 0;
}


static int al_abrir(struct inode *inode, struct file *file){
    return single_open(file,escribir_archivo,NULL);
}

static struct file_operations operaciones = 
{
    .open = al_abrir,
    .read = seq_read
};

int iniciar(void){ //modulo de inicio 
    proc_create("mem_grupo20",0,NULL,&operaciones);
    printk(KERN_INFO "%s","Cargando modulo.\n");
    printk(KERN_INFO "%s","Hola mundo, somos el grupo 20 y este es el monitor de memoria.\n");   
    return 0;
}

void salir(void){ //modulo salida 
    remove_proc_entry("mem_grupo20",NULL);
    printk(KERN_INFO "%s","Hola mundo, somos el grupo 20 y este es el monitor de memoria.\n");
    printk(KERN_INFO "%s","Sayonara mundo, somos el grupo 20 y este fue el monitor de memoria\n");
}

module_init(iniciar);
module_exit(salir);



