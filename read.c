#include "stdio.h"
#include "dirent.h"
#include "read.h"


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
    
    
//batto
int read(){
	DIR *procdir;
    FILE *fp;
    struct dirent *entry;
    char path[256 + 5 + 5]; // d_name 256 + /proc + /stat
    int pid;


    //open /proc directory.
    procdir = opendir("/proc");
    if (!procdir) {
        perror("opendir failed");
        return 1;
    }

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

        //get PID, process name.
        fscanf(fp, "%d %s",&pid, &path);
        printf("%d, %s\n", pid, path);
        fclose(fp);
    }
        
     closedir(procdir);
     return 0;
}
	
	
	
