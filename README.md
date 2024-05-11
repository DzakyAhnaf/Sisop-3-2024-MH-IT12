# Laporan Resmi Praktikum Sistem Operasi 2024 Modul-3
## Anggota Kelompok IT 12 :

- Muhammad Dzaky Ahnaf (5027231039)
- Adlya Isriena Aftarisya (5027231066)
- Nisrina Atiqah Dwiputri Ridzki (5027231075)

## Daftar Isi

- [Soal 1](#soal-1)
- [Soal 2](#soal-2)
- [Soal 3](#soal-3)
- [Soal 4](#soal-4)

## Soal 1

Dikerjakan oleh Adlya Isriena Aftarisya (5027231066)

## Soal 2

Dikerjakan oleh Nisrina Atiqah Dwiputri Ridzki (5027231075)

### Deskripsi Soal
Max Verstappen 🏎️ seorang pembalap F1 dan programer memiliki seorang adik bernama Min Verstappen (masih SD) sedang menghadapi tahap paling kelam dalam kehidupan yaitu perkalian matematika, Min meminta bantuan Max untuk membuat kalkulator perkalian sederhana (satu sampai sembilan). Sembari Max nguli dia menyuruh Min untuk belajar perkalian dari web (referensi) agar tidak bergantung pada kalkulator.
(Wajib menerapkan konsep pipes dan fork seperti yang dijelaskan di modul Sisop. Gunakan 2 pipes dengan diagram seperti di modul 3).

a. Sesuai request dari adiknya Max ingin nama programnya dudududu.c. Sebelum program parent process dan child process, ada input dari user berupa 2 string. Contoh input: tiga tujuh. 

b. Pada parent process, program akan mengubah input menjadi angka dan melakukan perkalian dari angka yang telah diubah. Contoh: tiga tujuh menjadi 21. 

c. Pada child process, program akan mengubah hasil angka yang telah diperoleh dari parent process menjadi kalimat. Contoh: `21` menjadi “dua puluh satu”.

d. Max ingin membuat program kalkulator dapat melakukan penjumlahan, pengurangan, dan pembagian, maka pada program buatlah argumen untuk menjalankan program : 
  - perkalian	: ./kalkulator -kali
  - penjumlahan	: ./kalkulator -tambah
  - pengurangan	: ./kalkulator -kurang
  - pembagian	: ./kalkulator -bagi
  Beberapa hari kemudian karena Max terpaksa keluar dari Australian Grand Prix 2024 membuat Max tidak bersemangat untuk melanjutkan programnya sehingga kalkulator yang dibuatnya cuma menampilkan hasil positif jika bernilai negatif maka program akan print “ERROR” serta cuma menampilkan bilangan bulat jika ada bilangan desimal maka dibulatkan ke bawah.

e. Setelah diberi semangat, Max pun melanjutkan programnya dia ingin (pada child process) kalimat akan di print dengan contoh format : 
  - perkalian	: “hasil perkalian tiga dan tujuh adalah dua puluh satu.”
  - penjumlahan	: “hasil penjumlahan tiga dan tujuh adalah sepuluh.”
  - pengurangan	: “hasil pengurangan tujuh dan tiga adalah empat.”
  - pembagian	: “hasil pembagian tujuh dan tiga adalah dua.”

f. Max ingin hasil dari setiap perhitungan dicatat dalam sebuah log yang diberi nama histori.log. Pada parent process, lakukan pembuatan file log berdasarkan data yang dikirim dari child process. 
  - Format: [date] [type] [message]
  - Type: KALI, TAMBAH, KURANG, BAGI
  - Ex:
    [10/03/24 00:29:47] [KALI] tujuh kali enam sama dengan empat puluh dua.
    
    [10/03/24 00:30:00] [TAMBAH] sembilan tambah sepuluh sama dengan sembilan belas.
    
    [10/03/24 00:30:12] [KURANG] ERROR pada pengurangan.


### Kode Penyelesaian

```c
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

```
---

### Penjelesan

---

1. Fungsi ```string_to_number```:
	  ```c
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
	
	```
	Fungsi ```string_to_number``` digunakan untuk mengonversi string yang berisi angka dalam bentuk kata-kata menjadi nilai integer. Misalnya, "satu" akan dikonversi menjadi 1, "dua" menjadi 2, dan seterusnya. Fungsi menggunakan ```strcmp``` untuk membandingkan string input dengan daftar kata-kata angka yang mungkin. Jika string tidak sesuai dengan kata-kata angka yang valid, fungsi akan mengembalikan nilai -1 sebagai penanda kesalahan.

2. Fungsi ```number_to_words```:
	  ```c
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
	```
	Fungsi ```number_to_words``` melakukan kebalikan dari string_to_number. Ini mengonversi nilai integer menjadi kata-kata angka. Fungsi ini mengambil angka integer dan menyimpan representasinya dalam bentuk string di parameter result. Untuk angka-angka tertentu seperti 11 hingga 19, angka tersebut akan diubah menjadi kata-kata khusus seperti "sebelas", "dua belas", dan seterusnya.

3. Fungsi ```main```:
	  ```c
	void processFile(const char *filePath) {
	    char buffer[MAX_BUFFER_SIZE];
	    FILE *file = fopen(filePath, "r+");
	    if (file == NULL) {
	        syslog(LOG_ERR, "Error opening file: %s", filePath);
	        return;
	    }
	
	    char *suspiciousStrings[] = {"m4LwAr3", "5pYw4R3", "R4nS0mWaR3"};
	    char *replacementStrings[] = {"[MALWARE]", "[SPYWARE]", "[RANSOMWARE]"};
	
	    while (fgets(buffer, sizeof(buffer), file)) {
	        for (int i = 0; i < sizeof(suspiciousStrings) / sizeof(suspiciousStrings[0]); ++i) {
	            replaceSubstring(buffer, suspiciousStrings[i], replacementStrings[i]);
	            if (strstr(buffer, replacementStrings[i]) != NULL) {
	                writeLogEntry(filePath);
	            }
	        }
	        fseek(file, -strlen(buffer), SEEK_CUR);
	        fputs(buffer, file);
	    }
	    fclose(file);
	}
	```
	
	Fungsi ```main``` adalah fungsi utama dari program ini. Fungsi ini terdiri dari beberapa bagian yaitu Child Process, Parent Process, dan Logging. Fungsi ```main``` memeriksa jumlah argumen yang diberikan pada baris perintah. Jika tidak sesuai dengan yang diharapkan, program akan memberikan pesan tentang cara menggunakan program dan kemudian keluar.

4. Child Process 
	  ```c
	if (pid == 0) { 
	    close(pipefd[1]); 
	    int result;
	    read(pipefd[0], &result, sizeof(result));
	
	    char result_words[50];
	    number_to_words(result, result_words);
	
	    char operation[20];
	    char input1[10], input2[10];
	    if (strcmp(argv[1], "-kali") == 0) {
	        // Code for multiplication operation
	    } else if (strcmp(argv[1], "-tambah") == 0) {
	        // Code for addition operation
	    } else if (strcmp(argv[1], "-kurang") == 0) {
	        // Code for subtraction operation
	    } else if (strcmp(argv[1], "-bagi") == 0) {
	        // Code for division operation
	    }
	}
	
	```
	Dalam child process setelah pemanggilan fork(), program menciptakan sebuah duplikat dari proses induk, memungkinkannya untuk memiliki salinan semua sumber daya, termasuk kode program dan ruang alamat. Child process bertanggung jawab untuk melakukan operasi matematika berdasarkan argumen baris perintah. Setelah menutup ujung tulis pipa, child process membaca hasil operasi matematika dari proses induk melalui pipa, mengonversi hasilnya menjadi kata-kata, dan membaca input angka dari pipa. Sesuai dengan jenis operasi yang diberikan, proses anak mencetak hasil operasi matematika dengan pesan yang sesuai, seperti "hasil perkalian [input1] dan [input2] adalah [hasil]", dan akhirnya mencatat operasi ke dalam file log "histori.log" bersama dengan timestamp.

5. Logging
   ```c
	// Logging
	time_t rawtime;
	struct tm *timeinfo;
	char log_message[200];
	char operation_str[20];
	char time_str[20];
	
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(time_str, sizeof(time_str), "%d/%m/%y %H:%M:%S", timeinfo);
	
	// Membuat pesan log berdasarkan jenis operasi
	if (strcmp(argv[1], "-kali") == 0) {
	    strcpy(operation_str, "KALI");
	    snprintf(log_message, sizeof(log_message), "[%s] [%s] %s kali %s sama dengan %s", time_str, operation_str, input1, input2, result_words);
	} else if (strcmp(argv[1], "-tambah") == 0) {
	    // Code for addition operation
	} else if (strcmp(argv[1], "-kurang") == 0) {
	    // Code for subtraction operation
	} else if (strcmp(argv[1], "-bagi") == 0) {
	    // Code for division operation
	}
	
	// Membuka file log
	FILE *log_file = fopen("histori.log", "a");
	if (log_file == NULL) {
	    perror("fopen");
	    return 1;
	}
	
	// Menulis pesan log ke file
	fprintf(log_file, "%s\n", log_message);
	
	// Menutup file log
	fclose(log_file);
	
	
	  ```
	Proses logging dalam program bertujuan untuk mencatat setiap operasi matematika ke dalam file log "histori.log". Sebelumnya, program membuat pesan log yang mencakup waktu operasi, jenis operasi, serta input dan outputnya. Pesan tersebut kemudian ditambahkan ke file log, dan setelah selesai, file log ditutup. Proses ini memungkinkan program untuk melacak aktivitasnya dan melakukan analisis lebih lanjut jika diperlukan.

6. Parent Process 
	  ```c
	else { // Parent process
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
        // Code for division operation
    }

    write(pipefd[1], &result, sizeof(result));
    write(pipefd[1], input1, sizeof(input1)); // Send input1
    write(pipefd[1], input2, sizeof(input2)); // Send input2

    close(pipefd[1]);
    wait(NULL); // Wait for child process to complete
   }
   ```

	Proses induk bertanggung jawab untuk meminta pengguna memasukkan dua angka dalam bahasa, yang kemudian dikonversi menjadi angka menggunakan fungsi string_to_number. Proses ini juga menangani kondisi error seperti input yang tidak valid, mencetak pesan kesalahan jika diperlukan, dan menulis angka-angka tersebut ke pipa untuk proses anak menggunakan fungsi write. Setelah itu, proses induk menunggu proses anak menyelesaikan tugasnya menggunakan wait(), memastikan bahwa proses induk menunggu hingga proses anak selesai sebelum melanjutkan eksekusi. Setelah menerima hasil dari proses anak, proses induk menutup ujung tulis pipa karena tidak akan lagi mengirim data ke proses anak, dan kemudian menutup pipa setelah selesai digunakan.

### Revisi

- Tambahkan fungsi string ke integer pada parent process
  ```c
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

        // Integer to string conversion
        char num1_str[10], num2_str[10];
        sprintf(num1_str, "%d", num1);
        sprintf(num2_str, "%d", num2);

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

        // Output in numeric form
        printf("Hasil dalam bentuk angka: %d\n", result);

        write(pipefd[1], &result, sizeof(result));
        write(pipefd[1], num1_str, sizeof(num1_str)); // Send num1_str
        write(pipefd[1], num2_str, sizeof(num2_str)); // Send num2_str

        close(pipefd[1]);
        wait(NULL); // Wait for child process to complete
    }

    return 0;
  }
  
  ```
  
### Dokumentasi
- Hasil ouput di terminal
  ![WhatsApp Image 2024-05-11 at 23 47 15_e470ac58](https://github.com/DzakyAhnaf/Sisop-3-2024-MH-IT12/assets/151147728/4002d411-b296-48c9-84f9-bf6212e8a33f)

- Histori.log
![WhatsApp Image 2024-05-11 at 23 47 15_83d2a51a](https://github.com/DzakyAhnaf/Sisop-3-2024-MH-IT12/assets/151147728/43a3ea49-f2b8-4d22-980d-3bdf2bb4ab55)




## Soal 3

Dikerjakan oleh Muhammad Dzaky Ahnaf (5027231039)
### Deskripsi Soal

a. Programnya berketentuan sebagai berikut:
Pada program actions.c, program akan berisi function function yang bisa di call oleh paddock.c

b. Action berisikan sebagai berikut:
- Gap [Jarak dengan driver di depan (float)]: Jika Jarak antara Driver dengan Musuh di depan adalah < 3.5s maka return Gogogo, jika jarak > 3.5s dan 10s return Push, dan jika jarak > 10s maka return Stay out of trouble.
- Fuel [Sisa Bensin% (string/int/float)]: Jika bensin lebih dari 80% maka return Push Push Push, jika bensin di antara 50% dan 80% maka return You can go, dan jika bensin kurang dari 50% return Conserve Fuel.
- Tire [Sisa Ban (int)]: Jika pemakaian ban lebih dari 80 maka return Go Push Go Push, jika pemakaian ban diantara 50 dan 80 return Good Tire Wear, jika pemakaian di antara 30 dan 50 return Conserve Your Tire, dan jika pemakaian ban kurang dari 30 maka return Box Box Box.
- Tire Change [Tipe ban saat ini (string)]: Jika tipe ban adalah Soft return Mediums Ready, dan jika tipe ban Medium return Box for Softs.

Contoh:
		[Driver] : [Fuel] [55%]
		[Paddock]: [You can go]

c. Pada paddock.c program berjalan secara daemon di background, bisa terhubung dengan driver.c melalui socket RPC.

d. Program paddock.c dapat call function yang berada di dalam actions.c.

e. Program paddock.c tidak keluar/terminate saat terjadi error dan akan log semua percakapan antara paddock.c dan driver.c di dalam file race.log

Format log:

[Source] [DD/MM/YY hh:mm:ss]: [Command] [Additional-info]

ex :

[Driver] [07/04/2024 08:34:50]: [Fuel] [55%]

[Paddock] [07/04/2024 08:34:51]: [Fuel] [You can go]

f. Program driver.c bisa terhubung dengan paddock.c dan bisa mengirimkan pesan dan menerima pesan serta menampilan pesan tersebut dari paddock.c sesuai dengan perintah atau function call yang diberikan.

g. Jika bisa digunakan antar device/os (non local) akan diberi nilai tambahan.

h. Untuk mengaktifkan RPC call dari driver.c, bisa digunakan in-program CLI atau Argv (bebas) yang penting bisa send command seperti poin B dan menampilkan balasan dari paddock.c
		
ex:
- Argv: 
./driver -c Fuel -i 55% 
- in-program CLI: Command: Fuel | Info: 55%

### Kode Penyelesaian
- actions.c
```c
#include <stdio.h>
#include <string.h>

const char* Gap(float distance) {
    if (distance < 3.5) {
        return "Gogogo";
    } else if (distance <= 10.0) {
        return "Push";
    } else {
        return "Stay out of trouble";
    }
}

const char* Fuel(float percentage) {
    if (percentage > 80) {
        return "Push Push Push";
    } else if (percentage >= 50) {
        return "You can go";
    } else {
        return "Conserve Fuel";
    }
}

const char* Tire(int wear) {
    if (wear > 80) {
        return "Go Push Go Push";
    } else if (wear >= 50) {
        return "Good Tire Wear";
    } else if (wear >= 30) {
        return "Conserve Your Tire";
    } else {
        return "Box Box Box";
    }
}

const char* TireChange(const char* type) {
    if (strcmp(type, "Soft") == 0) {
        return "Mediums Ready";
    } else if (strcmp(type, "Medium") == 0) {
        return "Box for Softs";
    }
    return "Invalid tire type";
}

```
- paddock.c
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include "actions.c"

#define PORT 12345
#define LOG_FILE "race.log"

void log_communication(const char* source, const char* command, const char* additional_info) {
    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file == NULL) {
        perror("Failed to open log file");
        return;
    }

    time_t now = time(NULL);
    struct tm *local_time = localtime(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%d/%m/%Y %H:%M:%S", local_time);

    fprintf(log_file, "[%s] [%s]: [%s] [%s]\n", source, time_str, command, additional_info);
    fclose(log_file);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[256];

    // Membuat socket server
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Failed to create server socket");
        exit(EXIT_FAILURE);
    }

    // Menetapkan pengaturan alamat server
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Mengikat socket server ke alamat dan port
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Failed to bind server socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Mulai mendengarkan koneksi klien
    if (listen(server_socket, 5) == -1) {
        perror("Failed to listen on server socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Paddock server is running on port %d...\n", PORT);

    // Terus menerima koneksi klien
    while (1) {
        // Menerima koneksi klien
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("Failed to accept client connection");
            continue;
        }

        // Membaca permintaan dari klien
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
        if (bytes_read == -1) {
            perror("Failed to read client request");
            close(client_socket);
            continue;
        }

        buffer[bytes_read] = '\0';

        // Log permintaan klien
        log_communication("Driver", buffer, "");

        // Parsing permintaan dan panggilan fungsi dari actions.c
        char command[50];
        char info[50];
        const char* response = "Invalid command";

        sscanf(buffer, "%s %s", command, info);

        if (strcmp(command, "Gap") == 0) {
            float gap = atof(info);
            response = Gap(gap);
        } else if (strcmp(command, "Fuel") == 0) {
            float fuel = atof(info);
            response = Fuel(fuel);
        } else if (strcmp(command, "Tire") == 0) {
            int tire = atoi(info);
            response = Tire(tire);
        } else if (strcmp(command, "TireChange") == 0) {
            response = TireChange(info);
        }

        // Mengirim respons ke klien
        write(client_socket, response, strlen(response));
        write(client_socket, "\n", 1);

        // Log tanggapan paddock
        log_communication("Paddock", command, response);

        // Menutup koneksi klien
        close(client_socket);
    }

    // Menutup socket server
    close(server_socket);
    return 0;
}

```
- driver.c
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 12345
#define BUFFER_SIZE 256

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <command> <info>\n", argv[0]);
        printf("Commands: Gap, Fuel, Tire, TireChange\n");
        return 1;
    }

    // Mendapatkan command dan info dari argumen CLI
    const char* command = argv[1];
    const char* info = argv[2];

    // Membuat socket klien
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Failed to create client socket");
        exit(EXIT_FAILURE);
    }

    // Menetapkan pengaturan alamat server
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Menghubungkan ke server paddock
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Failed to connect to paddock server");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Mengirim permintaan ke paddock
    char request[BUFFER_SIZE];
    snprintf(request, sizeof(request), "%s %s\n", command, info);
    write(client_socket, request, strlen(request));

    // Membaca tanggapan dari paddock
    char response[BUFFER_SIZE];
    ssize_t bytes_read = read(client_socket, response, sizeof(response) - 1);
    if (bytes_read == -1) {
        perror("Failed to read response from paddock");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    response[bytes_read] = '\0';

    // Menampilkan tanggapan dari paddock
    printf("[%s] [%s]\n", command, response);

    // Menutup koneksi
    close(client_socket);
    return 0;
}

```

### Penjelasan

Pada soal ini, kita membuat sebuah sistem client dan server. Client hanya terdiri dari program ```driver.c``` sedangkan pada sisi Server terdapat ```actions.c```, ```paddock.c```. Nah, disini kita menggunakan socket untuk menghubungkan antara client dan server agar dapat berkomunkasi. Lalu ```actions.c``` disini hanya berisi function syarat-syarat yang menentukan output yang akan dicatat pada ```race.log``` dan bisa di call oleh ```paddock.c```

- driver.c
1. Mendeklarasi library dan konstanta
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#define PORT 12345
#define BUFFER_SIZE 256
```
Program ini menggunakan beberapa header standar dalam C, seperti ```stdio.h```, ```stdlib.h```, ```string.h```, ```sys/socket.h```, ```netinet/in.h```, dan ```unistd.h``` yang digunakan untuk operasi input/output, alokasi memori, manipulasi string, operasi socket, dan operasi sistem lainnya.
Konstanta ```PORT``` digunakan untuk menentukan nomor port yang akan digunakan untuk komunikasi dengan server paddock, dalam kasus ini adalah ```12345```. Konstanta ```BUFFER_SIZE``` digunakan untuk menentukan ukuran buffer yang digunakan dalam komunikasi socket.

2. Fungsi Main
```c
int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <command> <info>\n", argv[0]);
        printf("Commands: Gap, Fuel, Tire, TireChange\n");
        return 1;
    }
```
Fungsi ```main``` adalah fungsi utama yang dijalankan ketika program dieksekusi. Program ini memeriksa jumlah argumen yang diberikan ketika program dijalankan. Jika jumlah argumen kurang dari 3 (program itu sendiri dihitung sebagai argumen pertama), maka program akan menampilkan pesan penggunaan dan keluar.

3. Mendapatkan Command dan Info dari Argumen CLI
```c
const char* command = argv[1];
const char* info = argv[2];
```
Program mengambil argumen kedua dan ketiga dari baris perintah sebagai ```command``` dan ```info``` yang akan dikirimkan ke server paddock. 

4. Membuat Socket Klien
```c
int client_socket = socket(AF_INET, SOCK_STREAM, 0);
if (client_socket == -1) {
    perror("Failed to create client socket");
    exit(EXIT_FAILURE);
}
```
Program membuat sebuah socket klien menggunakan fungsi ```socket```. Argumen yang diberikan adalah ```AF_INET``` (menggunakan protokol internet IPv4), ```SOCK_STREAM``` (menggunakan protokol TCP), dan ```0``` (menggunakan protokol default). Jika pembuatan socket gagal, program akan menampilkan pesan error dan keluar.

5. Menetapkan Pengaturan Alamat Server
```c
struct sockaddr_in server_addr;
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(PORT);
server_addr.sin_addr.s_addr = INADDR_ANY;
```
Program menetapkan struktur ```sockaddr_in``` yang berisi informasi alamat server paddock. ```sin_family``` diatur ke ```AF_INET``` untuk menggunakan protokol internet IPv4. ```sin_port``` diatur ke ```htons(PORT)``` untuk mengonversi nomor port dari host byte order ke network byte order. ```sin_addr.s_addr``` diatur ke ```INADDR_ANY``` yang berarti program akan terhubung ke server yang berjalan pada mesin lokal.

6. Menghubungkan ke Server Paddock
```c
if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    perror("Failed to connect to paddock server");
    close(client_socket);
    exit(EXIT_FAILURE);
}
```
Program mencoba menghubungkan ke server paddock menggunakan fungsi ```connect```. Jika koneksi gagal, program akan menampilkan pesan error, menutup socket klien, dan keluar.

7. Mengirim Permintaan ke Paddock
```c
char request[BUFFER_SIZE];
snprintf(request, sizeof(request), "%s %s\n", command, info);
write(client_socket, request, strlen(request));
```
Program membangun string permintaan dengan format ```"<command> <info>\n"``` menggunakan fungsi ```snprintf```. Kemudian, permintaan dikirimkan ke server paddock menggunakan fungsi ```write``` pada socket klien.

8. Membaca Tanggapan dari Paddock
```c
char response[BUFFER_SIZE];
ssize_t bytes_read = read(client_socket, response, sizeof(response) - 1);
if (bytes_read == -1) {
    perror("Failed to read response from paddock");
    close(client_socket);
    exit(EXIT_FAILURE);
}
response[bytes_read] = '\0';
```
Program membaca tanggapan dari server paddock menggunakan fungsi ```read``` pada socket klien. Tanggapan akan disimpan dalam buffer ```response```. Jika pembacaan tanggapan gagal, program akan menampilkan pesan error, menutup socket klien, dan keluar. Setelah membaca tanggapan, program menambahkan null terminator (```\0```) pada akhir string tanggapan.

9. Menampilkan Tanggapan dari Paddock
```c
printf("[%s] [%s]\n", command, response);
```
Program menampilkan tanggapan dari server paddock dengan format ```"[<command>] [<response>]"```.

10. Menutup Koneksi
```c
close(client_socket);
return 0;
}
``` 
Terakhir, program menutup socket klien menggunakan fungsi ```close``` dan mengembalikan nilai ```0``` untuk mengindikasikan keberhasilan eksekusi program.

Jadi Secara keseluruhan, program ini adalah sebuah klien sederhana yang terhubung dengan server paddock melalui socket TCP/IP. Klien akan mengirimkan perintah dan informasi yang diberikan sebagai argumen baris perintah, dan kemudian menerima tanggapan dari server paddock. Tanggapan tersebut akan ditampilkan pada konsol.

### Dokumentasi Pengerjaan

1. Menjalankan program ```paddock.c``` terlebih dahulu 

![image](https://github.com/DzakyAhnaf/Sisop-3-2024-MH-IT12/assets/110287409/ec5ea5c8-7f6d-431e-8582-dbe8ba8ace53)

2. Memasukkan perintah yang diinginkan pada ```actions.c```

![image](https://github.com/DzakyAhnaf/Sisop-3-2024-MH-IT12/assets/110287409/fbcdc0a2-5d10-42ef-98fd-ec9f70813ad6)

3. Hasil ```race.log``` dari menjalankan perintah-perintah pada ```actions.c```

![image](https://github.com/DzakyAhnaf/Sisop-3-2024-MH-IT12/assets/110287409/0671224a-d0a1-4e2c-8715-62e88c0f47f9)

## Soal 4

Dikerjakan oleh Adlya Isriena Aftarisya (5027231066)
