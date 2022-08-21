#include "stdio.h"
#include "dirent.h"
#include "read.h"
#include "linked_list.c"
#include "stdlib.h"
#include "string.h"


#define tc_enter_alt_screen() puts("\033[?1049h\033[H")
#define tc_exit_alt_screen() puts("\033[?1049l")
#define tc_clear_screen() puts("\x1B[2J")
#define tc_move_cursor(X, Y) printf("\033[%d;%dH", Y, X)
#define RED     "\033[31m"
#define RESET   "\033[0m"


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
	float cpu;
}PidData;
 
typedef struct PidListItem{
	ListItem list;
	PidData * data;
	
}PidListItem;


void PidList_print(ListHead * head){
	ListItem* aux = head->first;
	int i = 0;
	while(i<20){
		PidListItem* element = (PidListItem*) aux;
		printf("%d %c %lu %.1f  %s\n", element->data->pid,element->data->state, element->data->vm,element->data->cpu,element->data->dname);
		aux = aux->next;
		i++;
	}
	printf("\n");
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
int myRead(){
	DIR *procdir;
    FILE *fp;
    FILE *up;
    struct dirent *entry;
    char path[256 + 5 + 5]; // d_name 256 + /proc + /stat
    int pid;
    char dname[256];
	char state;
	unsigned long vm;
	int no;
	float uptime;
	unsigned long utime;
	unsigned long stime;
	unsigned long long int starttime;
	float cpu;
	int CLK_TCK = 100;
	
	
	
	

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
		
		
		//getting system's uptime
		up = fopen("/proc/uptime","r");
		fscanf(up,"%llu",&uptime);
		fclose(up);
		
		//calculating cpu percentege dividing sum of user and kernel time with elapsed time
		cpu = ((utime+stime)/CLK_TCK)/(uptime - (starttime/CLK_TCK));
		
		
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
		element->data->cpu = -(cpu);
		List_insert(&head,head.last,(ListItem*)element);
	

	
        fclose(fp);
    }
	PidList_sort(&head);
	tc_enter_alt_screen();
	printf(RED "PID STATE VM CPU% NAME\n" RESET);
    PidList_print(&head);        
    tc_move_cursor(0,0);
     closedir(procdir);
     return 0;
}
	
	int main(){

	myRead();

	
	return 1;
}
    
    
	
