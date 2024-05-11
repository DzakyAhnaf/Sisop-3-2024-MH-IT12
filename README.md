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

### Deskripsi Soal

Pada zaman dahulu pada galaksi yang jauh-jauh sekali, hiduplah seorang Stelle. Stelle adalah seseorang yang sangat tertarik dengan Tempat Sampah dan Parkiran Luar Angkasa. Stelle memulai untuk mencari Tempat Sampah dan Parkiran yang terbaik di angkasa. Dia memerlukan program untuk bisa secara otomatis mengetahui Tempat Sampah dan Parkiran dengan rating terbaik di angkasa.

### Catatan

- Struktur folder awal:
    
    ```yaml
    ├── auth.c
    ├── microservices
    │   ├── database
    │   │   └── db.log
    │   ├── db.c
    │   └── rate.c
    └── new-data
        ├── belobog_trashcan.csv
        ├── ikn.csv
        └── osaka_parkinglot.csv
    
    ```
    
- Struktur folder akhir:
    
    ```yaml
    ├── auth.c
    ├── microservices
    │   ├── database
    │   │   ├── belobog_trashcan.csv
    │   │   ├── db.log
    │   │   └── osaka_parkinglot.csv
    │   ├── db.c
    │   └── rate.c
    └── new-data
    ```
    
- Library:
    
    ```yaml
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/ipc.h>
    #include <sys/shm.h>
    #include <dirent.h>
    ```
    
    - stdio.h: menyediakan fungsi-fungsi standar input-output
    - string.h: menyediakan fungsi-fungsi yang berkaitan dengan manipulasi string seperti `strstr`, `strcpy`, dan fungsi-fungsi lainnya.
    - stdlib.h: berisi fungsi-fungsi standar yang digunakan dalam pemrograman C
    - sys/ipc.h dan sys/shm.h: Digunakan untuk interprocess communication (komunikasi antar proses) dengan shared memory.
    - dirent.h: Berisi fungsi-fungsi untuk membaca direktori dan mengolah informasi file.
- contoh isi file csv:
    
    ```yaml
    name, rating
    Dotonbori, 9.7
    Kiseki, 9.7
    Osaka Castle, 8.5
    ```
    

## Pengerjaan

### auth.c

```yaml
int main() {
    int key = 12345678;
    DIR *dir = opendir("new-data");
    if (dir == NULL) {
        perror("opendir");
        exit(1);
    }
    
    struct dirent *entry;
    int shm_key = 12345678;
    ...
```

- `int key = 12345678;` dan `int shm_key = 12345678;` deklarasi variabel key yang berguna untuk mengakses shared memory
- `DIR *dir = opendir("new-data");` Membuka direktori dengan nama "new-data" dan menyimpan hasilnya ke dalam pointer `dir`
- `if (dir == NULL) {` Jika `dir` bernilai `NULL`, artinya gagal membuka direktori "new-data".
- `struct dirent *entry;` Mendefinisikan pointer `entry` yang bertipe `struct ****dirent`. Struktur `dirent` digunakan untuk mewakili entri (file atau direktori) dalam sebuah direktori.

```yaml
....
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

            // Write to shared memory
            strcpy(shared_memory, entry->d_name);

            // Detach shared memory
            shmdt(shared_memory);

            // Increment the key for the next shared memory segment
            shm_key++;
        } else {
            char filePath[1024];
            snprintf(filePath, sizeof(filePath), "new-data/%s", entry->d_name);
            remove(filePath);
        }
    }

    // Close directory
    closedir(dir);

    return 0;
}
```

Pada bagian ini while loop untuk membaca file atau entry-entry dalam direktori “new-data” dan akan mengembalikan nilai `NULL` jika telah mencapai akhir dari direktori. Dalam loop, setiap entri yang dibaca dicek apakah nama file-nya mengandung "trashcan.csv" atau "parkinglot.csv" menggunakan fungsi `strstr()`. Jika ya, maka akan dilakukan operasi shared memory:

- `shmget()` Mencoba untuk mendapatkan id segmen shared memory. `IPC CREAT` digunakan untuk membuat segmen shared memory, jika segmen dengan kunci (`shm_key`) sudah ada, maka akan menggunakan segmen tersebut. Jika tidak, akan membuat segmen shared memory baru dengan ukuran 1024 byte. `0666` memberikan izin read dan write untuk semua: pemilik, group, user lainnya.
- `shmat()` Menautkan segmen shared memory ke dalam ruang alamat proses. Jika sukses, akan mengembalikan pointer ke awal segmen shared memory
- Data nama file (`entry->d_name`) disalin ke dalam segmen shared memory menggunakan `strcpy()`
- `shmdt()` Memutuskan tautan antara segmen shared memory dan ruang alamat proses
- `shm_key++` dilakukan increment untuk digunakan pada segmen shared memory selanjutnya

Jika nama file tidak sesuai dengan “trashcan.csv” atau “parkinglot.csv”, maka file tersebut akan dihapus dengan menggunakan fungsi `remove()` setelah nama file ditambahkan dengan direktori "new-data". Setelah selesai membaca semua entry pada direktory “new-data” direktori yang telah dibuka akan ditutup dengan fungsi `closedir()`, dan fungsi `main()` akan mengembalikan nilai 0 untuk menandakan bahwa program selesai berjalan tanpa kesalahan.

### rate.c

```yaml
int main() {
    int baseKey = 12345678; 
    int shmid;
    char *shared_memory;
...
```

Pada fungsi main variabel `baseKey` digunakan untuk kunci awal untuk segmen shared memory (pastikan key sama dengan auth.c). `shmid` akan digunakan untuk menyimpan id segmen shared memory, dan `shared_memory` akan digunakan sebagai pointer untuk menunjuk ke segmen shared memory. 

```
...
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
...
```

Program ini menggunakan loop `for` untuk mencoba mendapatkan segmen shared memory dengan kunci yang di-generate menggunakan variabel `baseKey` ditambah dengan nilai `i` untuk mengakses segmen yang berbeda setiap loop. Jika segmen shared memory dengan kunci tersebut tidak ada, loop akan berhenti. Setelah mendapatkan id segmen shared memory (`shmid`), program akan menautkan segmen shared memory tersebut ke dalam ruang alamat proses menggunakan `shmat()`. Jika gagal, program akan keluar dengan pesan kesalahan.

```yaml
...
        printf("Contents of shared memory: %s\n", shared_memory);
      
        char *filename = shared_memory;
        const char *type;
        if (strstr(filename,"trashcan.csv")){
            type = "Trash can";
        } else {
            type = "Parking lot";
        }
...
```

Karena isi dari segmen shared memory adalah sebuah string yang merupakan nama file. String ini ditunjuk oleh pointer `filename` . Berrdasarkan nama file, program menentukan tipe file ("Trash can" atau "Parking lot")

```yaml
...
        char fullpath[1024];
        strcpy(fullpath, "/Users/tarisa/smt-2/sisop/shift3/1/new-data/"); 
        strcat(fullpath, filename); 
        printf("Attempting to open file at: %s\n", fullpath);
        FILE *file = fopen(fullpath, "r");
        if (!file) {
            perror("fopen");
            exit(1);
        }
...
```

Kemudian, program mengkonstruksi path lengkap ke file menggunakan `strcpy()` dan `strcat()`, lalu mencoba membuka file tersebut untuk dibaca dengan `fopen()`. Jika gagal, program akan keluar dengan pesan kesalahan. 

```yaml
...
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
```

Pada bagian ini, program membaca setiap baris dari file yang terbuka, memisahkan nama dan peringkat (rating), dan mencari nilai peringkat maksimum beserta nama yang sesuai. Setelah selesai membaca file, program mencetak informasi tentang file dan hasil perhitungan peringkat terbaik. Terakhir, segmen shared memory dilepaskan dari ruang alamat proses menggunakan `shmdt()` sebelum iterasi berikutnya dimulai atau program selesai.

### db.c

```yaml
int main() {
    int key = 12345678;
    int shmid;
    char *shared_memory;
...
```

Pada fungsi main variabel `key` digunakan untuk kunci untuk segmen shared memory (pastikan key sama dengan auth.c). `shmid` akan digunakan untuk menyimpan id segmen shared memory, dan `shared_memory` akan digunakan sebagai pointer untuk menunjuk ke segmen shared memory. 

```yaml
...
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
...
```

Program ini menggunakan loop `for` untuk mencoba mendapatkan segmen shared memory dengan kunci yang di-generate menggunakan variabel `baseKey` ditambah dengan nilai `i` untuk mengakses segmen yang berbeda setiap loop. Jika segmen shared memory dengan kunci tersebut tidak ada, loop akan berhenti. Setelah mendapatkan id segmen shared memory (`shmid`), program akan menautkan segmen shared memory tersebut ke dalam ruang alamat proses menggunakan `shmat()`. Jika gagal, program akan keluar dengan pesan kesalahan.

```yaml
...
        char *filename = shared_memory;
        const char *type;
        if (strstr(filename,"trashcan.csv")){
            type = "Trash can";
        } else {
            type = "Parking lot";
        }
...
```

Bagian ini digunakan untuk membaca string pada segmen shared memory lalu menentukan tipe file ("Trash can" atau "Parking lot") berdasarkan nama file.

```yaml
...
        char source_path[256], db_path[256];
        snprintf(source_path, sizeof(source_path), "/Users/tarisa/smt-2/sisop/shift3/1/new-data/%s", filename);
        snprintf(db_path, sizeof(db_path), "database/%s", filename);
        if (rename(source_path, db_path) == -1) {
            perror("rename");
            printf("Failed to move file");
        } else {
            printf("File moved successfully");
        }

...
```

Disini, program memindahkan file dari direktori "new-data" ke direktori "database". Untuk melakukan ini, program menggunakan fungsi `rename()` yang dapat memindahkan file dari satu lokasi ke lokasi lain di sistem file. Jika gagal, program akan mencetak pesan kesalahan.

```yaml
...
        // log ke db.log
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
```

Program akan membuat file log `db.log` dan mencatat file yang masuk ke dalam direktori “database”. Untuk mencatat waktu, program menggunakan fungsi `localtime()` untuk mendapatkan waktu lokal, lalu menggunakan `strftime()` untuk memformat waktu dalam format yang diinginkan. Informasi yang tercatat mencakup waktu, tipe file, dan nama file yang dipindahkan. Setelah selesai menggunakan segmen shared memory, program melepaskan segmen tersebut dari ruang alamat proses menggunakan `shmdt()`. Setelah itu, program kembali ke iterasi berikutnya atau selesai jika tidak ada lagi segmen shared memory yang ditemukan.

## Output

### auth.c
![image](https://github.com/DzakyAhnaf/Sisop-3-2024-MH-IT12/assets/164857172/f35938fd-26ab-4929-9b4b-d4dc52ad1b9d)

### rate.c
![image](https://github.com/DzakyAhnaf/Sisop-3-2024-MH-IT12/assets/164857172/7f712004-8722-4f8d-a2cc-4cf906722845)

### db.c

sebelum dijalankan:
![image](https://github.com/DzakyAhnaf/Sisop-3-2024-MH-IT12/assets/164857172/89035646-9660-4d34-b199-73cf7a841f11)

sesudah dijalankan:
![image](https://github.com/DzakyAhnaf/Sisop-3-2024-MH-IT12/assets/164857172/f510d0ec-55bf-48cf-9f86-a7e1a6baf233)

isi db.log:
![image](https://github.com/DzakyAhnaf/Sisop-3-2024-MH-IT12/assets/164857172/e7a0a31d-7f18-4659-997d-0068c152f87a)

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
 ![WhatsApp Image 2024-05-05 at 13 24 14_971fe5b8](https://github.com/DzakyAhnaf/Sisop-3-2024-MH-IT12/assets/151147728/2822541b-2de1-446d-8219-a4bcaf533aa1)



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

### Deskripsi Soal

Lewis Hamilton seorang wibu akut dan sering melewatkan beberapa episode yang karena sibuk menjadi asisten. Maka dari itu dia membuat list anime yang sedang ongoing (biar tidak lupa) dan yang completed (anime lama tapi pengen ditonton aja). Tapi setelah Lewis pikir-pikir malah kepikiran untuk membuat list anime. Jadi dia membuat file (harap diunduh) dan ingin menggunakan socket yang baru saja dipelajarinya untuk melakukan CRUD pada list animenya.

### Catatan

- struktur folder awal
    
    ```yaml
    ├── client/
    │   └── client.c
    └── server/
        └── server.c
    ```
    
- struktur folder akhir
    
    ```yaml
    ├── change.log
    ├── client/
    │   └── client.c
    ├── myanimelist.csv
    └── server/
        └── server.c
    
    ```
    

## Pengerjaan

### server.c

```yaml
#define PORT 8081
#define MAX_CLIENTS 5
#define BUFFER_SIZE 256
```

- `PORT`: nomor port yang akan digunakan oleh server untuk menerima koneksi dari klien. Dalam kasus ini, port yang digunakan adalah 8081
- `MAX_CLIENTS`: jumlah maksimum klien yang diizinkan terhubung secara bersamaan ke server. Dalam kasus ini, jumlah maksimum klien yang diizinkan adalah 5
- `BUFFER_SIZE`: ukuran buffer yang digunakan untuk membaca dan menulis data antara klien dan server. Dalam kasus ini, ukuran buffer adalah 256 byte

```yaml
int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    downloadFile("https://drive.google.com/uc?export=download&id=10p_kzuOgaFY3WT6FVPJIXFbkej2s9f50", "/Users/tarisa/smt-2/sisop/shift3/4/myanimelist.csv");
...
```

Pada awal fungsi main ini digunakan untuk menginisialisasi variabel:

- Variabel `server_fd` dan `new_socket` ****digunakan untuk menyimpan file descriptor untuk socket server dan socket baru yang diterima, berturut-turut.
- `struct ****sockaddr_in ****address` adalah struktur yang digunakan untuk menyimpan informasi tentang alamat server.
- `opt` adalah variabel yang digunakan untuk mengatur opsi socket.
- `addrlen` adalah variabel yang digunakan untuk menyimpan panjang alamat.

Selain itu, bagian ini juga digunakan untuk mendownload file dari link yang telah diberikan pada soal dengan memanggil fungsi `downloadFile` 

```yaml
...
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

...
```

Fungsi `socket()` digunakan untuk membuat soket. Ini mengembalikan file descriptor untuk soket baru yang telah dibuat. Jika gagal, akan dicetak pesan kesalahan.

```yaml
...
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        exit(EXIT_FAILURE);
    }

...
```

Fungsi `**setsockopt()**` digunakan untuk mengatur opsi soket. Di sini, opsi `SO_REUSEADDR` diatur untuk memungkinkan penggunaan kembali alamat oleh soket.

```yaml
...
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
...
```

Pada blok ini, alamat server diinisialisasi dengan menggunakan `INADDR_ANY` untuk mendengarkan dari semua antarmuka jaringan yang tersedia. Fungsi `bind()` digunakan untuk mengikat soket ke alamat yang ditentukan. Jika gagal, akan dicetak pesan kesalahan. Adapula fungsi `listen()` digunakan untuk memulai mendengarkan koneksi masuk pada soket. Jika gagal, akan dicetak pesan kesalahan

```yaml
...
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
```

- Loop utama server berjalan tanpa henti dan menunggu koneksi dari klien.
- Fungsi `accept()` digunakan untuk menerima koneksi dari klien. Ini akan membuat soket baru untuk koneksi yang diterima.
- Setelah koneksi diterima, fungsi `handle_client(**)**` dipanggil untuk menangani koneksi tersebut.
- Loop akan berlanjut untuk menerima koneksi baru setelah menangani koneksi saat ini. Dalam kode yang Anda berikan, loop dihentikan setelah menangani satu koneksi dengan menggunakan `break`.

```yaml
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
```

Fungsi `downloadFile` memiliki berfungsi untuk mendownload file dari URL yang ditentukan. Fungsi ini menggunakan perintah `wget` untuk mengunduh file dan menyimpannya dengan nama file yang ditentukan. Kemudian membaca file tersebut untuk digunakan pada program selanjutnya.

```yaml
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
```

Fungsi `handle_client` bertanggung jawab untuk menangani koneksi dengan klien. pada fungsi ini, progrram akan membaca data yang dikirim oleh klien, memproses command yang diterima, menghentikan koneksi jika klien mengirimkan pesan “exit” dan menangani penutupan koneksi dari klien.

```yaml
void processcmd(char *command, int client_socket) {
    char *day = NULL;
    char *genre = NULL;
    char *title = NULL;
    char *status = NULL;
...
```

Fungsi `processcmd` mengelola perintah yang diterima dari klien. Ini memeriksa perintah yang diterima dan mengeksekusi tindakan yang sesuai berdasarkan perintah tersebut.

```yaml
...
if (strcmp(command, "all") == 0) {
    FILE *file = fopen("/Users/tarisa/smt-2/sisop/shift3/4/myanimelist.csv", "r");
    if (file == NULL) {
        perror("Error opening file");
        printf("gagal buka file");
        return;
    } else {
        printf("Receive: %s\n\n",command);
    }
...
```

Bagian ini memeriksa apakah perintah yang diterima adalah "all”. Jika iya maka program akan membaca file “myanimelist.csv”.  Jika file tidak dapat dibuka maka akan mencetak pesan error, sebaliknya jika file berhasil dibaca maka akan mencetak “Receive: All” pada server.

```yaml
...
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
...
```

Bagian ini akan memproses file.csv dengan membaca setiap baris dalam file CSV dan menambahkan judul anime ke dalam response, lalu mengirimnya ke klien

```yaml
...
else if (strncmp(command, "day ", 4) == 0){
    printf("Receive: %s\n\n",command);

    day = strtok(command, " ");
    day = strtok(NULL, " ");

    FILE *file = fopen("/Users/tarisa/smt-2/sisop/shift3/4/myanimelist.csv", "r");
    if (file == NULL) {
        perror("Error opening file");
        printf("gagal buka file");
        return;
    }
...
```

- Blok ini memeriksa apakah perintah yang diterima dimulai dengan "day ".
- Menggunakan `strtok` untuk memisahkan string command menjadi token. Token pertama diabaikan karena merupakan kata "day" dan token kedua dianggap sebagai hari.
- Membuka file CSV yang berisi daftar judul anime untuk dibaca.

```yaml
...
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
...
```

Blok ini digunakan untuk memproses isi file.csv. Program akan membaca setiap baris dalam file CSV, setiap baris dibagi menjadi token menggunakan `strtok`. Kemudian jika hari pada baris saat ini cocok dengan hari yang diminta, maka judul anime pada baris tersebut ditambahkan ke dalam response dan mengirimnya ke klien.

```yaml
...
else if (strncmp(command, "genre ", 6) == 0){
    printf("Receive: %s\n\n",command);
    genre = strtok(command, " ");
    genre = strtok(NULL, "\n");

    FILE *file = fopen("/Users/tarisa/smt-2/sisop/shift3/4/myanimelist.csv", "r");
    if (file == NULL) {
        perror("Error opening file");
        printf("gagal buka file");
        return;
    }
...
```

Bagian ini memeriksa apakah perintah yang diterima adalah "genre ”. Jika iya maka program akan membaca file “myanimelist.csv” dan menggunakan **`strtok`** untuk memisahkan string command menjadi token. Token pertama diabaikan karena merupakan kata "genre" dan token kedua dianggap sebagai genre anime yang diminta. Jika file tidak dapat dibuka maka akan mencetak pesan error, sebaliknya jika file berhasil dibaca maka akan mencetak command yang dari klien pada server.

```yaml
...
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
...
```

Blok ini digunakan untuk memproses isi file.csv. Program akan membaca setiap baris dalam file CSV, setiap baris dibagi menjadi token menggunakan `strtok`. Kemudian jika genre pada baris saat ini cocok dengan genre yang diminta, maka judul anime pada genre tersebut ditambahkan ke dalam response dan mengirimnya ke klien.

```yaml
...
else if (strncmp(command, "status ", 7) == 0){ // contoh: status jujustsu kaisen
    printf("Receive: %s\n\n",command);
    title = strtok(command, " "); // status
    title = strtok(NULL, "\n"); // jujutsu kaisen

    FILE *file = fopen("/Users/tarisa/smt-2/sisop/shift3/4/myanimelist.csv", "r");
    if (file == NULL) {
        perror("Error opening file");
        printf("gagal buka file");
        return;
    }
...
```

Bagian ini memeriksa apakah perintah yang diterima adalah "status ”. Jika iya maka program akan membaca file “myanimelist.csv” dan menggunakan **`strtok`** untuk memisahkan string command menjadi token. Token pertama diabaikan karena merupakan kata "status" dan token kedua dianggap sebagai judul anime yang diminta. Jika file tidak dapat dibuka maka akan mencetak pesan error, sebaliknya jika file berhasil dibaca maka akan mencetak command yang dari klien pada server.

```yaml
...
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
...
```

Blok ini digunakan untuk memproses isi file.csv. Program akan membaca setiap baris dalam file CSV, setiap baris dibagi menjadi token menggunakan `strtok`. Kemudian jika judul anime pada baris saat ini cocok dengan judul yang diminta, maka status anime tersebut ditambahkan ke dalam response dan mengirimnya ke klien.

```yaml
...
else if (strncmp(command, "add ", 4) == 0){
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
...
```

- add
    - Blok ini menangani perintah "add" yang dimulai dengan "add ".
    - Menggunakan `strtok` untuk memisahkan string command menjadi token. Token-token tersebut digunakan untuk menentukan nilai dari variabel `day`, `genre`, `title`, dan `status`.
    - Membuka file CSV yang berisi daftar judul anime untuk ditambahkan data baru.
    - Menambahkan data baru ke dalam file CSV dengan menggunakan `fprintf`.
    - Mencatat operasi penambahan ke dalam log file dengan fungsi `Log`.
    - Menutup file setelah selesai ditambahkan.
- delete
    - Blok ini menangani perintah "delete" yang dimulai dengan "delete ".
    - Menggunakan `strtok` untuk memisahkan string command menjadi token. Token tersebut adalah judul anime yang akan dihapus.
    - Membuka file CSV yang berisi daftar judul anime untuk dibaca dan membuat file temporary untuk penulisan data yang akan dipertahankan.
    - Membaca setiap baris dari file CSV dan menulis baris-baris tersebut ke file temporary kecuali baris yang memiliki judul yang sama dengan yang diminta untuk dihapus.
    - Mencatat operasi penghapusan ke dalam log file dengan fungsi `Log`.
    - Menutup file yang dibuka, menghapus file CSV lama, dan mengganti nama file temporary menjadi file CSV baru.
- edit
    - Blok ini menangani perintah "edit" yang dimulai dengan "edit ".
    - Menggunakan `strtok` untuk memisahkan string command menjadi token. Token-token tersebut digunakan untuk menentukan nilai dari variabel `titleOri`, `day`, `genre`, `title`, dan `status`.
    - Membuka file CSV yang berisi daftar judul anime untuk dibaca dan membuat file temporary untuk penulisan data yang akan diubah.
    - Membaca setiap baris dari file CSV dan menulis baris-baris tersebut ke file temporary dengan melakukan perubahan yang diminta. Jika judul anime di baris saat ini cocok dengan judul yang diminta untuk diubah, maka data yang akan ditulis adalah data baru yang telah diberikan dalam perintah.
    - Mencatat operasi pengubahan ke dalam log file dengan fungsi `Log`.
    - Menutup file yang dibuka, menghapus file CSV lama, dan mengganti nama file temporary menjadi file CSV baru.
- exit dan invalid command
    - Jika perintah adalah "exit", maka program akan keluar.
    - Jika perintah tidak valid, maka akan mengirimkan pesan "Invalid command" ke klien.

```yaml
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
```

Fungsi `Log` bertanggung jawab untuk mencatat pesan log ke dalam file log. Fungsi `Log` ini dapat dipanggil dari bagian lain dari program untuk mencatat operasi yang dilakukan.

### client.c

```yaml
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main() {
    int clientsock;
    struct sockaddr_in servAddr;
    char buffer[1024];
    char response[1024]; 

    clientsock = socket(AF_INET, SOCK_STREAM, 0);
    if (clientsock < 0) {
        perror("Error creating socket");
        return 1;
    }

    memset(&servAddr, '0', sizeof(servAddr));

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(8081);
    if(inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr) <= 0) {
        printf("\nInvalid address/address not supported \n");
        return 1;
    }

    if (connect(clientsock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        perror("Error connecting to server");
        return 1;
    }

    printf("Connected to server\n");

    while (1) {
        printf("You: ");
        fgets(buffer, 1024, stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; 

        if (strcmp(buffer, "exit") == 0) {
            send(clientsock, buffer, strlen(buffer), 0); 
            break;
        }

        send(clientsock, buffer, strlen(buffer), 0);

        int bytes_received = recv(clientsock, response, 1024, 0);
        if (bytes_received < 0) {
            perror("recv failed");
            break;
        } else if (bytes_received == 0) {
            printf("Server closed the connection.\n");
            break;
        }

        response[bytes_received] = '\0';

        printf("Server response: %s\n", response);
    }

    close(clientsock);
    return 0;
}
```

Pada fungsi `main()` , variabel-variabel lokal seperti `clientsock`, `servAddr`, `buffer`, dan `response` dideklarasikan. Kemudian program akan melakukan proses-proses berikut:

- Socket dibuat menggunakan fungsi `socket**()**` untuk komunikasi dengan server.
- Alamat server diinisialisasi dengan menggunakan `struct ****sockaddr_in`.
- Koneksi ke server dilakukan dengan fungsi `connect**()**`.
- Selanjutnya, program masuk ke dalam loop tak terbatas untuk berinteraksi dengan server.
- Input dari pengguna (`stdin`) diminta dan dikirimkan ke server menggunakan `send**()**`.
- Respons dari server diterima menggunakan `recv**()**` dan ditampilkan kepada pengguna.
- Jika pengguna memasukkan "exit", program akan ****keluar dari loop dan menutup socket sebelum mengakhiri eksekusi.

### error

- new-data nya beda dir. Solusi: full path
![image](https://github.com/DzakyAhnaf/Sisop-3-2024-MH-IT12/assets/164857172/f12e669e-130b-44c5-a269-a450d36c9d73)

- error: output munculnya malah di server, lupa send ke client
  ![image](https://github.com/DzakyAhnaf/Sisop-3-2024-MH-IT12/assets/164857172/b3e97dcd-d93b-4dd9-8947-9f45ba82e0e7)

- error:
  ![image](https://github.com/DzakyAhnaf/Sisop-3-2024-MH-IT12/assets/164857172/c3b82068-d832-40a9-a64d-292eb9f53211)
  solusi: menambahkan setsockopt 
  ![image](https://github.com/DzakyAhnaf/Sisop-3-2024-MH-IT12/assets/164857172/ca763e2c-1478-4451-837a-77bbccd2e503)

- error: tidak bisa mencetak output all jika all sudah pernah dijalankan sebelumnya
  ![image](https://github.com/DzakyAhnaf/Sisop-3-2024-MH-IT12/assets/164857172/f70ffd37-044b-4793-b97b-850a2d3cdee4)
  solusi: menambahkan ini
  ![image](https://github.com/DzakyAhnaf/Sisop-3-2024-MH-IT12/assets/164857172/f372d7a1-7e39-4ece-b6de-31e33f3c7ce8)

- error:
  ![image](https://github.com/DzakyAhnaf/Sisop-3-2024-MH-IT12/assets/164857172/35d5e2c8-4632-4868-a2cb-1503b4550893)
  solusi: ganti “ “ jadi “\n”
  ![image](https://github.com/DzakyAhnaf/Sisop-3-2024-MH-IT12/assets/164857172/2838d568-5709-41b8-ba82-4fe29ee63c05)

- error: yang berhasil exit cuma di client.c
  ![image](https://github.com/DzakyAhnaf/Sisop-3-2024-MH-IT12/assets/164857172/5175bd25-538b-4a2c-b4ce-1e652ca4931b)
  solusi: break while loop pada server.c
  
