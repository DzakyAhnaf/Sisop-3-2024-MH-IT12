#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>

#define PORT 8081
#define MAX_CLIENTS 5
#define BUFFER_SIZE 256


void downloadFile(const char *url, const char *filename);
void handle_client(int client_socket);
void processcmd(char *command, int client_socket);
void Log(const char *type, const char *logfile, const char *msg);

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    downloadFile("https://drive.google.com/uc?export=download&id=10p_kzuOgaFY3WT6FVPJIXFbkej2s9f50", "/Users/tarisa/smt-2/sisop/shift3/4/myanimelist.csv");

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Waiting for connection...\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        
        handle_client(new_socket);
        break;
    }

    return 0;
}

void downloadFile(const char *url, const char *filename){
    char command[256];
    snprintf(command, sizeof(command), "wget '%s' -O %s", url, filename);
    if(system(command)==0){
        printf("File downloaded successfully.\n");
    } else {
        printf("Error downloading file.\n");
    }

    FILE *file = fopen(filename, "r+");
    if (file == NULL) {
        perror("Error opening file");
        printf("gagal buka :(\n)");
        exit(1);
    }
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    int bytes_read;

    while ((bytes_read = read(client_socket, buffer, BUFFER_SIZE)) > 0) {
        processcmd(buffer, client_socket);
        memset(buffer, 0, sizeof(buffer));
        if (strcmp(buffer, "exit\n") == 0) {
            break; // Exit the loop
        }
    }
    if (bytes_read == 0) {
        printf("Client closed the connection.\n");
    } else {
        perror("read failed");
    }

    close(client_socket); // Tutup koneksi dengan klien
}


void processcmd(char *command, int client_socket) {
    char *day = NULL;
    char *genre = NULL;
    char *title = NULL;
    char *status = NULL;

    if (strcmp(command, "all") == 0) {
        FILE *file = fopen("/Users/tarisa/smt-2/sisop/shift3/4/myanimelist.csv", "r");
        if (file == NULL) {
            perror("Error opening file");
            printf("gagal buka file");
            return;
        } else {
            printf("Receive: %s\n\n",command);
        }

        char line[256];
        char response[4096]; 
        memset(response, 0, sizeof(response));

        strcat(response, "\nAnime Titles:\n");
        int i=1;
        while (fgets(line, sizeof(line), file)) {
            char *title;
            title = strtok(line, ","); 
            title = strtok(NULL, ",");
            title = strtok(NULL, ",");

            if (title!= NULL){
                char temp[256];
                sprintf(temp, "%d. %s\n", i, title);
                strcat(response, temp);
                i++;
            }
        }

        fclose(file);

        send(client_socket, response, strlen(response), 0);

    } else if (strncmp(command, "day ", 4) == 0){
        printf("Receive: %s\n\n",command);

        day = strtok(command, " ");
        day = strtok(NULL, " ");

        FILE *file = fopen("/Users/tarisa/smt-2/sisop/shift3/4/myanimelist.csv", "r");
        if (file == NULL) {
            perror("Error opening file");
            printf("gagal buka file");
            return;
        }

        char line[256];
        char response[4096];
        memset(response, 0, sizeof(response));
        strcat(response, "\nAnime Titles on ");
        strcat(response, day);
        strcat(response, ":\n");

        int i = 1;
        while (fgets(line, sizeof(line), file)) {
            line[strcspn(line, "\n")] = 0;
            char *file_day = strtok(line, ","); //hari

            if (file_day != NULL && strcmp(file_day, day) == 0) {
                genre = strtok(NULL, ","); // masih di genre
                title = strtok(NULL, ","); // udah di judul
                char temp[256];
                sprintf(temp, "%d. %s\n", i, title);
                strcat(response, temp);
                i++;
            }
        }

        fclose(file);
        send(client_socket, response, strlen(response), 0);

    } else if (strncmp(command, "genre ", 6) == 0){
        printf("Receive: %s\n\n",command);
        genre = strtok(command, " ");
        genre = strtok(NULL, "\n");

        FILE *file = fopen("/Users/tarisa/smt-2/sisop/shift3/4/myanimelist.csv", "r");
        if (file == NULL) {
            perror("Error opening file");
            printf("gagal buka file");
            return;
        } 

        char line[256];
        char response[4096];
        memset(response, 0, sizeof(response));
        strcat(response, "\nAnime Titles on ");
        strcat(response, genre);
        strcat(response, ":\n");

        int i = 1;
        while (fgets(line, sizeof(line), file)) {
            line[strcspn(line, "\n")] = 0;
            char *file_day = strtok(line, ","); //hari
            char *file_genre = strtok(NULL, ","); //genre

            if (file_genre != NULL && strcmp(file_genre, genre) == 0) {
                title = strtok(NULL, ","); // udah di judul
                char temp[256];
                sprintf(temp, "%d. %s\n", i, title);
                strcat(response, temp);
                i++;
            }
        }

        fclose(file);
        send(client_socket, response, strlen(response), 0);
        
    } else if (strncmp(command, "status ", 7) == 0){ // contoh: status jujustsu kaisen
        printf("Receive: %s\n\n",command);
        title = strtok(command, " "); // status
        title = strtok(NULL, "\n"); // jujutsu kaisen

        FILE *file = fopen("/Users/tarisa/smt-2/sisop/shift3/4/myanimelist.csv", "r");
        if (file == NULL) {
            perror("Error opening file");
            printf("gagal buka file");
            return;
        }

        char line[256];
        char response[4096];
        memset(response, 0, sizeof(response));
        strcat(response, "\nStatus ");
        strcat(response, title);
        strcat(response, ":\n");

        while (fgets(line, sizeof(line), file)) {
            line[strcspn(line, "\n")] = 0;
            char *file_day = strtok(line, ","); //hari
            char *file_genre = strtok(NULL, ","); //genre
            char *file_title = strtok(NULL, ","); //title

            if (file_title != NULL && strcmp(file_title, title) == 0) {
                status = strtok(NULL, ","); // udah di status
                char temp[256];
                sprintf(temp, "%s\n", status);
                strcat(response, temp);
            }
        }

        fclose(file);
        send(client_socket, response, strlen(response), 0);

    } else if (strncmp(command, "add ", 4) == 0){
        printf("Receive: %s\n\n",command);
        day = strtok(command, " "); //add
        day = strtok(NULL, ","); //hari
        genre = strtok(NULL, ","); //genre
        title = strtok(NULL, ","); //judul
        status = strtok(NULL, "\n"); // status

        char response[4096];
        FILE *file = fopen("/Users/tarisa/smt-2/sisop/shift3/4/myanimelist.csv", "a");
        if (file == NULL) {
            perror("Error opening file");
            printf("gagal buka file");
            return;
        } 

        fprintf(file, "\n%s,%s,%s,%s", day, genre, title, status);
        strcat(response, "Anime berhasil ditambahkan\n");
        char msg[256];
        snprintf(msg, sizeof(msg), "%s ditambahkan", title);
        Log("ADD","/Users/tarisa/smt-2/sisop/shift3/4/change.log", msg);
        fclose(file);
        send(client_socket, response, strlen(response), 0);

    } else if (strncmp(command, "delete ", 7) == 0){
        printf("Receive: %s\n\n",command);
        title = strtok(command, " "); //delete
        title = strtok(NULL, "\n"); //title

        FILE *file = fopen("/Users/tarisa/smt-2/sisop/shift3/4/myanimelist.csv", "r");
        if (file == NULL) {
            perror("Error opening file");
            printf("gagal buka file");
            return;
        } 

        FILE *temp_file = fopen("/Users/tarisa/smt-2/sisop/shift3/4/temp.csv", "w");
        if (temp_file == NULL) {
            perror("Error creating temporary file");
            fclose(file);
            exit(1);
        }

        char line[1024];
        char response[1024];
        char *file_day = strtok(line, ","); //hari
        char *file_genre = strtok(NULL, ","); //genre
        char *file_title = strtok(NULL, ","); //title
        char *file_status = strtok(NULL, "\n"); //status
        while (fgets(line, sizeof(line), file)) {
            line[strcspn(line, "\n")] = 0;
            file_day = strtok(line, ","); //hari
            file_genre = strtok(NULL, ","); //genre
            file_title = strtok(NULL, ","); //title
            file_status = strtok(NULL, "\n"); //status

            //yang ga sama kek yang di cmd (yg mw dihapus), dipindah ke temp file
            if (file_title != NULL && strcmp(file_title, title) != 0) {
                fprintf(temp_file, "%s,%s,%s,%s\n", file_day, file_genre, file_title, file_status);
            }
        }

        char msg[256];
        snprintf(msg, sizeof(msg), "%s berhasil dihapus", file_title);
        Log("DEL", "/Users/tarisa/smt-2/sisop/shift3/4/change.log", msg);

        fclose(file);
        fclose(temp_file);
        remove("/Users/tarisa/smt-2/sisop/shift3/4/myanimelist.csv");
        rename("/Users/tarisa/smt-2/sisop/shift3/4/temp.csv", "/Users/tarisa/smt-2/sisop/shift3/4/myanimelist.csv");
        strcat(response,"Anime Berhasil dihapus\n");
        send(client_socket, response, strlen(response), 0);
        
    } else if(strncmp(command, "edit ", 5) == 0){ //contoh: edit Naruto,Selasa,Comedy,Kungfu Panda,completed
        printf("Receive: %s\n\n",command);
        char *titleOri = strtok(command, " "); //edit
        titleOri = strtok(NULL, ","); //title ori
        day = strtok(NULL, ","); //hari
        genre = strtok(NULL, ","); //genre
        title = strtok(NULL, ","); //title
        status = strtok(NULL, "\n"); //status

        FILE *file = fopen("/Users/tarisa/smt-2/sisop/shift3/4/myanimelist.csv", "r");
        if (file == NULL) {
            perror("Error opening file");
            printf("gagal buka file");
            return;
        } 

        FILE *temp_file = fopen("/Users/tarisa/smt-2/sisop/shift3/4/temp.csv", "w");
        if (temp_file == NULL) {
            perror("Error creating temporary file");
            fclose(file);
            exit(1);
        }

        char line[1024];
        char response[1024];
        char *file_day; 
        char *file_genre; 
        char *file_title; 
        char *file_status; 
        while (fgets(line, sizeof(line), file)) {
            line[strcspn(line, "\n")] = 0;
            file_day = strtok(line, ","); //hari
            file_genre = strtok(NULL, ","); //genre
            file_title = strtok(NULL, ","); //title
            file_status = strtok(NULL, "\n"); //status

            //yang ga sama kek yang di cmd (yg mw dihapus), dipindah ke temp file
            if (file_title != NULL && strcmp(file_title, titleOri) == 0) {
            // Write the edited information to the temporary file
                fprintf(temp_file, "%s,%s,%s,%s\n", day, genre, title, status);
            } else {
                // Write the unchanged information to the temporary file
                fprintf(temp_file, "%s,%s,%s,%s\n", file_day, file_genre, file_title, file_status);
            }
        }

        char msg[256];
        snprintf(msg, sizeof(msg), "%s,%s,%s,%s diubah menjadi %s,%s,%s,%s", file_day, file_genre, file_title, file_status,day,genre,title,status);
        Log("EDIT", "/Users/tarisa/smt-2/sisop/shift3/4/change.log", msg);

        fclose(file);
        fclose(temp_file);
        remove("/Users/tarisa/smt-2/sisop/shift3/4/myanimelist.csv");
        rename("/Users/tarisa/smt-2/sisop/shift3/4/temp.csv", "/Users/tarisa/smt-2/sisop/shift3/4/myanimelist.csv");
        strcat(response,"Anime Berhasil diedit\n");
        send(client_socket, response, strlen(response), 0);

    } else if (strcmp(command,"exit")==0){
        printf("Exit\n");
        return;
    } 
    else {
        char response[256];
        snprintf(response, sizeof(response), "Invalid command\n");
        send(client_socket, response, strlen(response), 0);
    }
}

void Log(const char *type, const char *logfile, const char *msg) {
    FILE *file = fopen(logfile, "a");
    if (file == NULL) {
        perror("Error opening log file");
        printf("gagal buka log file");
        return;
    }

    // Dapatkan tanggal dan waktu saat ini
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%d/%m/%y", tm_info);

    // Tulis entri log ke file
    fprintf(file, "[%s] [%s] %s\n", timestamp, type, msg);
    fclose(file);
}
