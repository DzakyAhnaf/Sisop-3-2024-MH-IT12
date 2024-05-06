#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

int main() {
    int key = 12345678;
    int shmid;
    char *shared_memory;

    for (int i=0; ; i++){
        key += i;
        int shmid = shmget(key, 1024, 0666);
        if (shmid == -1) {
            break;
        }

        shared_memory = shmat(shmid, NULL, 0);
        if (shared_memory == (char *) -1) {
            perror("shmat");
            exit(1);
        }

        // Baca shared memory
        char *filename = shared_memory;
        const char *type;
            if (strstr(filename,"trashcan.csv")){
                type = "Trash can";
            } else {
                type = "Parking lot";
            }

        // Move file ke directory database
        char source_path[256], db_path[256];
        snprintf(source_path, sizeof(source_path), "/Users/tarisa/smt-2/sisop/shift3/1/new-data/%s", filename);
        snprintf(db_path, sizeof(db_path), "database/%s", filename);
        if (rename(source_path, db_path) == -1) {
            perror("rename");
            printf("Failed to move file");
        } else {
            printf("File moved successfully");
        }

        // Log ke db.log
        FILE *log_file = fopen("database/db.log", "a");
        if (log_file == NULL) {
            perror("fopen");
            exit(1);
        }
      
        time_t rawtime;
        struct tm *timeinfo;
        char buffer[80];

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer, sizeof(buffer), "[%d/%m/%y %H:%M:%S]", timeinfo);

        fprintf(log_file, "%s [%s] [%s]\n", buffer, type, filename);
        fclose(log_file);

        shmdt(shared_memory);
    }
    
    return 0;
}
