#include <stdio.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "terminal_control.h"



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
 
 //struct containing data of processes
typedef struct PidListItem{
	int pid;
	char dname[32];
	char state;
	unsigned long vm;
	long int rss;
	float mempercentage;
	float cpu;
	}PidListItem;


//function to print processes data in order
void PidList_print(PidListItem*proc_list,int size,int rows){
	
	for(int index = 0 ; index<rows-8 && index<size;index++){
		
		gotoxy(0, 10+index);
		//%-12s%-12d%-12d\n
		printf("%-12d %-12c %-12lu %-12ld %-12.2f  %-12.2f  %-12s", proc_list[index].pid,proc_list[index].state, proc_list[index].vm,proc_list[index].rss,proc_list[index].mempercentage,proc_list[index].cpu,proc_list[index].dname);
	}
	
}

int compare(const void * a, const void * b){
	PidListItem p = *(PidListItem*)a;
	PidListItem q = *(PidListItem*)b;
	return q.vm - p.vm;
}

//batto
int myRead(int rows){
	DIR *procdir;
	FILE *meminfo;
    FILE *fp;
    FILE *up;
    struct dirent *entry;
    char path[32 + 5 + 5]; // d_name 256 + /proc + /stat
    int pid;
    char dname[32];
	char state;
	unsigned long vm; //virtual memory
	long int rss;	//resident set size: number of pages the process has in real memory
	int no;
    long int uptime; //system's uptime
	unsigned long long utime; //amount of time the process has been running in user mode
	unsigned long stime; //amount of time the process has been running in kernel mode
	unsigned long long int starttime; //process's start time measured since system booted
	float cpu;
	float CLK_TCK = (float) sysconf(_SC_CLK_TCK); //getting number of clock ticks in order to convert in seconds
	float mempercentage; //top measures %MEM as RES compared to memory without swap
	
	

    //open /proc directory.
    procdir = opendir("/proc");
    if (!procdir) {
        perror("opendir failed");
        return 1;
    }

	
	
	int n_proc = 0;
	
	while ((entry = readdir(procdir))) {
       //skip anything that is not a PID directory.
       if (is_pid_dir(entry))
           n_proc += 1;
	}
	
	closedir(procdir);
	procdir = opendir("/proc");
	
	int index = 0;
	PidListItem proc_list[n_proc];
    //iterate through all files and directories of /proc.
    while (index < n_proc) {
		if(!(entry = readdir(procdir))){
			break;
		}
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
        fscanf(fp, "%d %s %c",&pid, &dname, &state); // 1st value process's ID, 2nd value filename, 3rd value process's state
        for(int i = 0; i<10;i++){
			fscanf(fp,"%d",&no);
		}
		fscanf(fp,"%lu",&utime); // 14th value
		fscanf(fp,"%ld",&stime); // 15th value
		for(int i = 0; i<6;i++){
			fscanf(fp,"%d",&no);
		}
		fscanf(fp,"%llu",&starttime); //22nd value
		fscanf(fp,"%lu",&vm); //23rd value
		fscanf(fp,"%ld",&rss); // 24th value
		
		
		//getting system's uptime
		up = fopen("/proc/uptime","r");
		if(!up){
			perror("fopen failed");
			return 1;
		}
		fscanf(up,"%llu",&uptime);
		fclose(up);
		meminfo = fopen("/proc/meminfo","r"); // first field of meminfo is total memory without swap 
		if(!meminfo){
			perror("fopen failed");
			return 1;
		}
		char line[80];  //first line is  "MemTotal:        xxxxx kB", need to extract xxxx

		fgets(line,sizeof(line),meminfo);
		char * p = line;
		long memtotal;
		while(*p){
			if(isdigit(*p)){
			memtotal = strtol(p,&p,10);//converts initial part of the string in digit with base 10
			}
			p++;
		}
		fclose(meminfo);
		
		starttime = starttime/CLK_TCK; //converting clock ticks into seconds
	
		utime = utime/CLK_TCK; //converting clock ticks into seconds
	
		stime = stime/CLK_TCK; //converting clock ticks into seconds
		
		long unsigned elapsed = uptime-starttime; //calculating process's elapsed time
		
		//calculating cpu percentege dividing sum of user and kernel time with elapsed time
		cpu = ((float)(utime+stime)/(float)(elapsed))*100;
		
		
		
		//creating element with process information
		proc_list[index].pid = pid;
		strncpy(proc_list[index].dname,dname,32);
		proc_list[index].state = state;
		//top measures in kb
		vm = vm/1000;
		proc_list[index].vm = vm;
		proc_list[index].rss = rss;
		proc_list[index].cpu = cpu;
		mempercentage = ((float)rss/(float)memtotal)*100;
		proc_list[index].mempercentage = mempercentage;
	

		
		fclose(fp);
		index++;
		
    }
	
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
	printf(RED"PID          STATE        VM           RES          MEM           CPU           NAME"RESET);
	qsort(proc_list,n_proc,sizeof(PidListItem),compare);
	PidList_print(proc_list,n_proc,rows);       
    closedir(procdir);
 
     return 0;
}



	


