#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched/signal.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/mm.h>



struct task_struct *task; //estructura definifa en sched.h para tareas /procesos 
struct task_struct *task_child; //estructura necesaria para iterar a través de tareas secundarias
struct list_head *list;  //estructura necesaria para recorrer la lista en cada tarea -> estructura de hijos.
int ejecucion=0;
int suspendidos=0;
int detenidos=0;
int zombies=0;
int totalp=0;


char * get_task_state(long state){
    switch (state)
        {
        case TASK_RUNNING:
        ejecucion=ejecucion+1; 
        totalp=totalp+1;  
            return "Ejecucion";
            
        case TASK_INTERRUPTIBLE:
        suspendidos=suspendidos+1;
        totalp=totalp+1;
            return "Suspendido";
            
        case TASK_UNINTERRUPTIBLE:
        suspendidos=suspendidos+1;
        totalp=totalp+1;
            return "Suspendido";

        case __TASK_STOPPED:
        suspendidos=suspendidos+1;
        totalp=totalp+1;
            return "Suspendido";
            
        case __TASK_TRACED: 
         detenidos=detenidos+1;
         totalp=totalp+1;
            return "Detenido";
            
        case TASK_STOPPED:
         detenidos=detenidos+1;
         totalp=totalp+1;
            return "Detenido";
            
        case EXIT_ZOMBIE:
        zombies=zombies+1;
        totalp=totalp+1;
            return "Zombie";
                
        default:
        totalp=totalp+1;
        return "Desconocido";
          //  sprintf(buffer, "Desconocido %ld", state);
            //return buffer;
        }
}


static int escribir_archivo(struct seq_file * archivo, void *v){
    int contador =0;
    int contadoraux=0;    

    seq_printf(archivo," {\n");
    seq_printf(archivo, "\"ListaProcesos\": [ ");
    #define Convert(x) ((x) << (PAGE_SHIFT - 10))
    for_each_process(task){
        if(contador>0){
            seq_printf(archivo,"   , \n");
        }

        seq_printf(archivo," {\n");
        seq_printf(archivo, "\"PID\": \"%d\",",task->pid);
        seq_printf(archivo, "\n\"PROCESO\": \"%s\",", task->comm);
        seq_printf(archivo, "\n\"ESTADO\": \"%s\",",get_task_state(task->state));
        if(task->mm!=NULL){
            seq_printf(archivo, "\n\"MEMORIA\": \"%ld\",",Convert(get_mm_rss(task->mm)));
        } else {
            seq_printf(archivo,"\n\"MEMORIA\":\"0\",");
        }        
        seq_printf(archivo,"\n\"USUARIO\": \"%d\",", __kuid_val(task->real_cred->uid));
    
        seq_printf(archivo, "\n\"HIJOS\": [ ");
        contadoraux=0;

        list_for_each(list, &task->children){
            if(contadoraux>0){
                seq_printf(archivo,"   , \n");
            }

            task_child = list_entry(list,struct task_struct, sibling);
            seq_printf(archivo,"\n\t{\n");
            seq_printf(archivo,"\t\"PID\": \"%d\",",task_child->pid);
            seq_printf(archivo,"\n\t\"PROCESO\": \"%s\",",task_child->comm);
            seq_printf(archivo,"\n\t\"ESTADO\": \"%s\",",get_task_state(task_child->state));
            if(task_child->mm!=NULL){
                    seq_printf(archivo, "\n\t\"MEMORIA\":\"%ld\",",Convert(get_mm_rss(task_child->mm)));
            } else {
                    seq_printf(archivo,"\n\t\"MEMORIA\":\"0\",");
            }
            seq_printf(archivo,"\n\t\"USUARIO\": \"%d\"", __kuid_val(task_child->real_cred->uid));
            seq_printf(archivo,"\n\t } \n");
            contadoraux++;
        }
        seq_printf(archivo," ] \n");
        contador++;
        seq_printf(archivo,"}  \n");

    }
    seq_printf(archivo, " ] ,\n");
    seq_printf(archivo, "\"General\": [  \n{");
    seq_printf(archivo,"\n\"Ejecucion\": \"%d\",",ejecucion);
    seq_printf(archivo,"\n\"Suspendidos\": \"%d\",",suspendidos);
    seq_printf(archivo,"\n\"Detenidos\": \"%d\",",detenidos);
    seq_printf(archivo,"\n\"Zombie\": \"%d\",",zombies);
    seq_printf(archivo,"\n\"Total\": \"%d\"",totalp);
    seq_printf(archivo,"} \n ]  \n");
    seq_printf(archivo,"}  \n");
    #undef k
    return 0;
}

static int al_abrir(struct inode *inode, struct file *file){
    ejecucion=0;
   suspendidos=0;
   detenidos=0;
   zombies=0;
  totalp=0;
    return single_open(file,escribir_archivo,NULL); 
}

static struct file_operations operaciones = 
{
    .open = al_abrir,
    .read = seq_read
};

int iniciar(void){ //modulo de inicio 
    proc_create("cpu_grupo20",0,NULL,&operaciones);
    printk(KERN_INFO "%s","Cargando modulo.\n");
    printk(KERN_INFO "%s","Hola mundo, somos el grupo 20 y este es el monitor de memoria.\n"); 

    return 0;
}

void salir(void){ //modulo salida 

    remove_proc_entry("cpu_grupo20",NULL);
    printk(KERN_INFO "%s","Removiendo modulo.\n");
    printk(KERN_INFO "%s","Sayonara mundo, somos el grupo 20 y este fue el monitor de cpu\n");

}

module_init(iniciar);
module_exit(salir);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Itera a través de todos los procesos / procesos hijos en el SO.");
MODULE_AUTHOR("Grupo 20");
