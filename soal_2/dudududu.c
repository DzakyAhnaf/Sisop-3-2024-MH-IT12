#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>
#include <time.h>

// convert number string to integer
int string_to_number(char *str) {
    if (strcmp(str, "nol") == 0) return 0;
    if (strcmp(str, "satu") == 0) return 1;
    if (strcmp(str, "dua") == 0) return 2;
    if (strcmp(str, "tiga") == 0) return 3;
    if (strcmp(str, "empat") == 0) return 4;
    if (strcmp(str, "lima") == 0) return 5;
    if (strcmp(str, "enam") == 0) return 6;
    if (strcmp(str, "tujuh") == 0) return 7;
    if (strcmp(str, "delapan") == 0) return 8;
    if (strcmp(str, "sembilan") == 0) return 9;
    if (strcmp(str, "sepuluh") == 0) return 10;
    if (strcmp(str, "seratus") == 0) return 100;
    return -1; // If not a valid number string
}

// convert number to words
void number_to_words(int num, char *result) {
    char *ones[] = {"nol", "satu", "dua", "tiga", "empat", "lima", "enam", "tujuh", "delapan", "sembilan", "sepuluh"};
    char *teens[] = {"sepuluh", "sebelas", "dua belas", "tiga belas", "empat belas", "lima belas", "enam belas", "tujuh belas", "delapan belas", "sembilan belas"};
    char *tens[] = {"", "sepuluh", "dua puluh", "tiga puluh", "empat puluh", "lima puluh", "enam puluh", "tujuh puluh", "delapan puluh", "sembilan puluh"};

    if (num == 100) {
        strcpy(result, "seratus");
    } else if (num >= 0 && num <= 10) {
        strcpy(result, ones[num]);
    } else if (num >= 11 && num <= 19) {
        strcpy(result, teens[num - 10]);
    } else if (num >= 20 && num <= 99) {
        if (num % 10 == 0) {
            strcpy(result, tens[num / 10]);
        } else {
            sprintf(result, "%s %s", tens[num / 10], ones[num % 10]);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <operation>\n", argv[0]);
        return 1;
    }

    // Pipe for communication between parent and child process
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) { // Child process
        close(pipefd[1]); // Close unused write end
        int result;
        read(pipefd[0], &result, sizeof(result));

        char result_words[50];
        number_to_words(result, result_words);

        char operation[20];
        char input1[10], input2[10];
        if (strcmp(argv[1], "-kali") == 0) {
            strcpy(operation, "perkalian");
            read(pipefd[0], input1, sizeof(input1));
            read(pipefd[0], input2, sizeof(input2));
            printf("hasil %s %s dan %s adalah %s.\n", operation, input1, input2, result_words);
        } else if (strcmp(argv[1], "-tambah") == 0) {
            strcpy(operation, "penjumlahan");
            read(pipefd[0], input1, sizeof(input1));
            read(pipefd[0], input2, sizeof(input2));
            printf("hasil %s %s dan %s adalah %s.\n", operation, input1, input2, result_words);
        } else if (strcmp(argv[1], "-kurang") == 0) {
            strcpy(operation, "pengurangan");
            read(pipefd[0], input1, sizeof(input1));
            read(pipefd[0], input2, sizeof(input2));
            if (result < 0) {
                printf("ERROR pada pengurangan\n");
            } else if (result == 0) { // Added condition to handle zero result
                printf("hasil pengurangan %s dan %s adalah %s.\n", input1, input2, result_words);
            } else {
                printf("hasil %s %s dan %s adalah %s.\n", operation, input1, input2, result_words);
            }
        } else if (strcmp(argv[1], "-bagi") == 0) {
            strcpy(operation, "pembagian");
            read(pipefd[0], input1, sizeof(input1));
            read(pipefd[0], input2, sizeof(input2));
            if (result == -1) {
                printf("ERROR pada pembagian nol\n");
            } else {
                printf("hasil %s %s dan %s adalah %s.\n", operation, input1, input2, result_words);
            }
        }

        // Logging
        time_t rawtime;
        struct tm *timeinfo;
        char log_message[200];
        char operation_str[20];
        char time_str[20];

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(time_str, sizeof(time_str), "%d/%m/%y %H:%M:%S", timeinfo);

        if (strcmp(argv[1], "-kali") == 0) {
            strcpy(operation_str, "KALI");
            snprintf(log_message, sizeof(log_message), "[%s] [%s] %s kali %s sama dengan %s", time_str, operation_str, input1, input2, result_words);
        } else if (strcmp(argv[1], "-tambah") == 0) {
            strcpy(operation_str, "TAMBAH");
            snprintf(log_message, sizeof(log_message), "[%s] [%s] %s tambah %s sama dengan %s", time_str, operation_str, input1, input2, result_words);
        } else if (strcmp(argv[1], "-kurang") == 0) {
            strcpy(operation_str, "KURANG");
            if (result < 0) {
                snprintf(log_message, sizeof(log_message), "[%s] [%s] ERROR pada pengurangan", time_str, operation_str);
            } else if (result == 0) { // Added condition to handle zero result
                snprintf(log_message, sizeof(log_message), "[%s] [%s] %s kurang %s sama dengan %s", time_str, operation_str, input1, input2, result_words);
            } else {
                snprintf(log_message, sizeof(log_message), "[%s] [%s] %s kurang %s sama dengan %s", time_str, operation_str, input1, input2, result_words);
            }
        } else if (strcmp(argv[1], "-bagi") == 0) {
            strcpy(operation_str, "BAGI");
            if (result == -1) {
                snprintf(log_message, sizeof(log_message), "[%s] [%s] ERROR pada pembagian nol", time_str, operation_str);
	} else {
		snprintf(log_message, sizeof(log_message), "[%s] [%s] %s bagi %s sama dengan %s", time_str, operation_str, input1, input2, result_words);
	}
   }
   
 FILE *log_file = fopen("histori.log", "a");
    if (log_file == NULL) {
        perror("fopen");
        return 1;
    }

    fprintf(log_file, "%s\n", log_message);
    fclose(log_file);

    close(pipefd[0]);
} else { // Parent process
    close(pipefd[0]); // Close unused read end

    char input1[10], input2[10];
    printf("Masukkan dua angka (dalam bahasa): ");
    scanf("%s %s", input1, input2);

    int num1 = string_to_number(input1);
    int num2 = string_to_number(input2);

    if (num1 == -1 || num2 == -1) {
        printf("Input tidak valid.\n");
        close(pipefd[1]);
        return 1;
    }

    int result;
    if (strcmp(argv[1], "-kali") == 0) result = num1 * num2;
    else if (strcmp(argv[1], "-tambah") == 0) result = num1 + num2;
    else if (strcmp(argv[1], "-kurang") == 0) result = num1 - num2;
    else if (strcmp(argv[1], "-bagi") == 0) {
        if (num2 == 0) {
            result = -1; // Flag for division by zero
        } else {
            result = floor(num1 / num2);
        }
    }

    write(pipefd[1], &result, sizeof(result));
    write(pipefd[1], input1, sizeof(input1)); // Send input1
    write(pipefd[1], input2, sizeof(input2)); // Send input2

    close(pipefd[1]);
    wait(NULL); // Wait for child process to complete
}

return 0;
}
