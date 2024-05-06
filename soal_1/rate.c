#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {
    int baseKey = 12345678; 
    int shmid;
    char *shared_memory;

    for (int i = 0; ; i++) {
        int key = baseKey + i; 
        shmid = shmget(key, 1024, 0666);
        if (shmid == -1) {
            break;
        }
        shared_memory = shmat(shmid, NULL, 0);
        if (shared_memory == (char *) -1) {
            perror("shmat");
            exit(1);
        }

        printf("Contents of shared memory: %s\n", shared_memory);
      
        char *filename = shared_memory;
        const char *type;
        if (strstr(filename,"trashcan.csv")){
            type = "Trash can";
        } else {
            type = "Parking lot";
        }
        char fullpath[1024];
        strcpy(fullpath, "/Users/tarisa/smt-2/sisop/shift3/1/new-data/"); 
        strcat(fullpath, filename); 
        printf("Attempting to open file at: %s\n", fullpath);
        FILE *file = fopen(fullpath, "r");
        if (!file) {
            perror("fopen");
            exit(1);
        }

        char line[1024];
        float maxRating = 0.0;
        char bestName[256] = {0};

        while (fgets(line, sizeof(line), file)) {
            char *name = strtok(line, ",");
            char *rating_str = strtok(NULL, ",");
            float rating = atof(rating_str);

            if (rating > maxRating) {
                maxRating = rating;
                strcpy(bestName, name);
            } else if (rating == maxRating) {
                strcat(bestName, ", "); 
                strcat(bestName, name);
            }
        }

        fclose(file);

        printf("Type: %s\n", type);
        printf("Filename: %s\n", filename);
        printf("------------\n");
        printf("Name: %s\n", bestName);
        printf("Rating: %.1f\n\n", maxRating);

        shmdt(shared_memory);
    }

    return 0;
}
