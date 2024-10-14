#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define SERIAL_PORT "/dev/ttyUSB0" // Cambia esto al puerto de tu Arduino
#define BAUD_RATE B9600
#define HANDSHAKE_MESSAGE "HELLO"
#define MAX_ATTEMPTS 3
#define TIMEOUT 0 // Tiempo de espera en segundos

typedef struct {
    int type;  // Cambiado a entero
    int id;
} Ship;

void send_all_ships(int fd, Ship ships[], int count) {
    char message[1024] = ""; // Buffer para almacenar el mensaje completo

    // Preparar el mensaje para enviar todos los barcos
    for (int i = 0; i < count; i++) {
        char temp[50];
        snprintf(temp, sizeof(temp), "%d,%d;", ships[i].type, ships[i].id); // Formato "tipo,id;"
        strcat(message, temp); // Concatenar al mensaje principal
    }

    // Enviar el mensaje completo
    write(fd, message, strlen(message));
    write(fd, "\n", 1); // Enviar carácter de nueva línea

    // Esperar la respuesta del Arduino
    usleep(TIMEOUT * 1000000); // Dormir por la duración del tiempo de espera
    char buffer[256];
    int bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    buffer[bytes_read] = '\0'; // Null-terminar la cadena
    printf("Respuesta del Arduino: %s\n", buffer); // Imprimir la respuesta
}

void configure_serial_port(int fd) {
    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, BAUD_RATE);
    cfsetospeed(&options, BAUD_RATE);
    options.c_cflag |= (CLOCAL | CREAD);    // Ignorar líneas de control de módem
    options.c_cflag &= ~PARENB;              // Sin paridad
    options.c_cflag &= ~CSTOPB;              // 1 bit de parada
    options.c_cflag &= ~CSIZE;               // Limpiar la máscara de tamaño actual
    options.c_cflag |= CS8;                   // 8 bits de datos
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Entrada sin procesar
    tcsetattr(fd, TCSANOW, &options);
}

int open_and_configure_serial_port(const char *port) {
    int fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        perror("Error al abrir el puerto serie");
        return -1;
    }
    // Configura el puerto serie aquí (puedes definir la función `configure_serial_port`)
    configure_serial_port(fd);
    return fd;
}


void change_ship_types(Ship ships[], int count, int new_type) {
    for (int i = 0; i < count; i++) {
        ships[i].type = new_type; // Cambiar tipo al nuevo tipo
    }
}

int get_type_from_string(const char *type_str) {
    if (strcmp(type_str, "Patrulla") == 0) {
        return 2;
    } else if (strcmp(type_str, "Pesquero") == 0) {
        return 1;
    } else if (strcmp(type_str, "Normal") == 0) {
        return 0;
    } else {
        return -1; // Tipo no válido
    }
}
void hardware_update(int fd) {
    Ship ships[] = {
        {2, 0}, {0, 0}, {1, 0},
        {2, 0}, {0, 0}, {1, 0},
        {2, 0}, {0, 0}, {1, 0},
        {2, 0}, {0, 0}, {1, 0},
        {2, 0}, {0, 0}, {1, 0},
        {2, 0}, {0, 0}, {1, 0},
        {2, 0}, {0, 0}
    };

    for (int i = 0; i < sizeof(ships) / sizeof(ships[0]); i++) {
        ships[i].id = i + 1; // Asignar IDs dinámicamente
    }

    int count = sizeof(ships) / sizeof(ships[0]);
    send_all_ships(fd, ships, count); // Enviar todos los barcos en un mensaje

    char new_type_str[20];
    while (1) {
        printf("Introduce el nuevo tipo de barco ('Patrulla', 'Normal', 'Pesquero', etc.) o 'salir' para terminar:\n");
        fgets(new_type_str, sizeof(new_type_str), stdin); // Leer la entrada del usuario
        new_type_str[strcspn(new_type_str, "\n")] = 0; // Eliminar el salto de línea

        if (strcmp(new_type_str, "salir") == 0) {
            break; // Salir del bucle si el usuario escribe 'salir'
        }

        int new_type = get_type_from_string(new_type_str);
        if (new_type != -1) {
            change_ship_types(ships, count, new_type); // Cambiar tipos al ingresado
            send_all_ships(fd, ships, count); // Enviar datos actualizados
            printf("Todos los tipos de barcos cambiados a '%s' y datos enviados.\n", new_type_str);
        } else {
            printf("Tipo de barco no válido.\n");
        }
    }
}

int main() {
    int fd = open_and_configure_serial_port(SERIAL_PORT);
    if (fd == -1) return 1;

    hardware_update(fd); // Actualizar los datos en el hardware

    close(fd);
    return 0;
}
