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
## Soal 4

Dikerjakan oleh Adlya Isriena Aftarisya (5027231066)
