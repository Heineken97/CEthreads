#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define SERIAL_PORT "/dev/ttyUSB0" // Cambia esto al puerto de tu Arduino
#define BAUD_RATE B9600
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

}

int main() {
    int fd = open_and_configure_serial_port(SERIAL_PORT);
    if (fd == -1) return 1;

    hardware_update(fd); // Actualizar los datos en el hardware

    close(fd);
    return 0;
}
