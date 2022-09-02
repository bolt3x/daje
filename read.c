#include "stdio.h"
#include "dirent.h"
#include "read.h"
#include "linked_list.c"
#include "stdlib.h"
#include "string.h"
#include "terminal_control.h"
#include <sys/ioctl.h>
#include <unistd.h>



//batto
//helper function to check if a struct dirent from /proc is a PID directory.
int is_pid_dir(const struct dirent *entry) {
    const char *p;

    for (p = entry->d_name; *p; p++) {
        if (!isdigit(*p))
            return 0;
    }

    return 1;
 }
typedef struct PidData{
	int pid;
	char dname[256];
	char state;
	unsigned long vm;
	long int rss;
	float mempercentage;
	float cpu;
}PidData;
 
typedef struct PidListItem{
	ListItem list;
	PidData * data;
	
}PidListItem;


void PidList_print(ListHead * head,int rows){
	ListItem* aux = head->first;
	int l = rows-8;
	
	for(int k = 0 ; k<l;k++){
		PidListItem* element = (PidListItem*) aux;
		gotoxy(0, 10+k);
		//%-12s%-12d%-12d\n
		printf("%-12d %-12c %-12lu %-12ld %-12.2f %-12.2f% -12s", element->data->pid,element->data->state, element->data->vm,element->data->rss,element->data->mempercentage,element->data->cpu,element->data->dname);
		aux = aux->next;
	}
	
}
/* function to swap data of two nodes a and b*/
void swap(ListItem *a, ListItem *b) 
{ 
	PidData * temp =(PidData*) malloc(sizeof(PidData));
	PidListItem* as = (PidListItem*)a;
	PidListItem* bs = (PidListItem*)b;
	memcpy(temp,as->data,sizeof(PidData));
	memcpy(as->data,bs->data,sizeof(PidData));
	memcpy(bs->data,temp,sizeof(PidData));
	
	
} 

void bubbleSort(ListItem * start) 
{ 
    int swapped, i; 
    ListItem *ptr1; 
	ListItem *lptr = NULL; 
	PidListItem * current;
	PidListItem * next;
    /* Checking for empty list */
    if (start == NULL) 
        return; 
  
    do
    { 
        swapped = 0; 
        ptr1 = start; 
  
        while (ptr1->next != lptr) 
        { 
			current = (PidListItem*)ptr1;
			next = (PidListItem*)ptr1->next;
            if (current->data->vm < next->data->vm) 
            { 
                swap(ptr1, ptr1->next); 
                swapped = 1; 
            } 
            ptr1 = ptr1->next; 
        } 
        lptr = ptr1; 
    } 
    while (swapped); 
} 
  



void PidList_sort(ListHead* head){
	bubbleSort(head->first);	
}






//batto
int myRead(int rows){
	DIR *procdir;
	FILE *meminfo;
    FILE *fp;
    FILE *up;
    struct dirent *entry;
    char path[256 + 5 + 5]; // d_name 256 + /proc + /stat
    int pid;
    char dname[256];
	char state;
	unsigned long vm;
	long int rss;
	int no;
    long int uptime;
	unsigned long long utime;
	unsigned long stime;
	unsigned long long int starttime;
	float cpu;
	float CLK_TCK = 100;
	float mempercentage;
	
	

    //open /proc directory.
    procdir = opendir("/proc");
    if (!procdir) {
        perror("opendir failed");
        return 1;
    }
    ListHead head;
	List_init(&head);
	
	
	
	
	
    //iterate through all files and directories of /proc.
    while ((entry = readdir(procdir))) {
        //skip anything that is not a PID directory.
        if (!is_pid_dir(entry))
            continue;

        //try to open /proc/<PID>/stat.
        snprintf(path, sizeof(path), "/proc/%s/stat", entry->d_name);
        fp = fopen(path, "r");

        if (!fp) {
            perror(path);
            continue;
        }
        
	
        //get PID, process name, state, virtual memory.
        fscanf(fp, "%d %s %c",&pid, &dname, &state);
        for(int i = 0; i<10;i++){
			fscanf(fp,"%d",&no);
		}
		fscanf(fp,"%lu",&utime);
		fscanf(fp,"%ld",&stime);
		for(int i = 0; i<6;i++){
			fscanf(fp,"%d",&no);
		}
		fscanf(fp,"%llu",&starttime);
		fscanf(fp,"%lu",&vm);
		fscanf(fp,"%ld",&rss);
		
		
		//getting system's uptime
		up = fopen("/proc/uptime","r");
		if(!up){
			perror("fopen failed");
			return 1;
		}
		fscanf(up,"%llu",&uptime);
		fclose(up);
		meminfo = fopen("/proc/meminfo","r");
		if(!meminfo){
			perror("fopen failed");
			return 1;
		}
		char line[80];
		fgets(line,sizeof(line),meminfo);
		char * p = line;
		long memtotal;
		while(*p){
			if(isdigit(*p)){
			memtotal = strtol(p,&p,10);
			}
			p++;
		}
		fclose(meminfo);
		
		starttime = starttime/CLK_TCK;
	
		utime = utime/CLK_TCK;
	
		stime = stime/CLK_TCK;
		
		long unsigned elapsed = uptime-starttime;
		
		//calculating cpu percentege dividing sum of user and kernel time with elapsed time
		cpu = ((float)(utime+stime)/(float)(elapsed))*100;
		
		
		//creating linkedlist element with process information
		PidListItem* element =(PidListItem*)malloc(sizeof(PidListItem));
		element->data = (PidData*)malloc(sizeof(PidData));
		element->list.prev = 0;
		element->list.next = 0;
		element->data->pid = pid;
		strncpy(element->data->dname,dname,256);
		element->data->state = state;
		//top misura in kb
		vm = vm/1000;
		element->data->vm = vm;
		element->data->rss = rss;
		element->data->cpu = cpu;
		mempercentage = ((float)rss/(float)memtotal)*100;
		element->data->mempercentage = mempercentage;
		List_insert(&head,head.last,(ListItem*)element);
	

		
        fclose(fp);
    }
	PidList_sort(&head);
	gotoxy(0,2);
	
	
	

	
	printf(RED R"EOF(
______       ___            __   _______     ______       ___            __   _______ 
|       \     /   \          |  | |   ____|  |       \     /   \          |  | |   ____|
|  .--.  |   /  ^  \         |  | |  |__     |  .--.  |   /  ^  \         |  | |  |__   
|  |  |  |  /  /_\  \  .--.  |  | |   __|    |  |  |  |  /  /_\  \  .--.  |  | |   __|  
|  '--'  | /  _____  \ |  `--'  | |  |____   |  '--'  | /  _____  \ |  `--'  | |  |____
|_______/ /__/     \__\ \______/  |_______|  |_______/ /__/     \__\ \______/  |_______|                    
)EOF"RESET);


	gotoxy(0,9);
	printf(RED"PID          STATE        VM           RES          MEM          CPU         NAME"RESET);
	PidList_print(&head,rows);        
    closedir(procdir);
     return 0;
}



	


