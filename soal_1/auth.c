#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <dirent.h>

int main() {
    int key = 12345678;
    DIR *dir = opendir("new-data");
    if (dir == NULL) {
        perror("opendir");
        exit(1);
    }

    struct dirent *entry;
    int shm_key = 12345678;
    while ((entry = readdir(dir)) != NULL) {
        if ((strstr(entry->d_name, "trashcan.csv") || strstr(entry->d_name, "parkinglot.csv"))) {
            int shmid = shmget(shm_key, 1024, IPC_CREAT | 0666);
            if (shmid == -1) {
                perror("shmget");
                exit(1);
            }

            char *shared_memory = shmat(shmid, NULL, 0);
            if (shared_memory == (char *) -1) {
                perror("shmat");
                exit(1);
            }

            strcpy(shared_memory, entry->d_name);

            shmdt(shared_memory);

            // Increment key buat shared memory segment selanjutnya
            shm_key++;
        } else {
            char filePath[1024];
            snprintf(filePath, sizeof(filePath), "new-data/%s", entry->d_name);
            remove(filePath);
        }
    }

    closedir(dir);

    return 0;
}
