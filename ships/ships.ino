#include <Arduino.h>

#define MAX_SHIPS 20

struct Ship {
    int type;  // Cambiado a entero
    int id;
};

Ship ships[20]; // Array to hold the ships
int shipCount = 0; // Counter for the number of ships

const int data = 2;      // Pin de datos
const int latch = 3;     // Pin latch
const int clockPin = 4;  // Pin de reloj
boolean leds[60];

// Función para crear la lista de leds basado en la lista de Ships
void crearListaLeds(Ship ships[], int shipCount) {
    int ledIndex = 0;
    for (int i = 0; i < shipCount; i++) {
        if (ledIndex + 3 > 60) {
            break; // Evita exceder el tamaño del array leds
        }

        // Asignar bits basados en el tipo de Ship
        if (ships[i].type == 2) { // Patrulla
            leds[ledIndex] = 0;   // B
            leds[ledIndex + 1] = 0; // G
            leds[ledIndex + 2] = 1; // R
        } else if (ships[i].type == 0) { // Normal
            leds[ledIndex] = 0;   // B
            leds[ledIndex + 1] = 1; // G
            leds[ledIndex + 2] = 0; // R
        } else if (ships[i].type == 1) { // Pesquero
            leds[ledIndex] = 1;   // B
            leds[ledIndex + 1] = 0; // G
            leds[ledIndex + 2] = 0; // R
        }
        ledIndex += 3;
    }
}

// Función para convertir el arreglo a una variable uint8_t de 8 bits.
uint8_t convertir(boolean arreglo[]) {
    uint8_t val = B00000000;  // Inicia la variable con todos los bits en 0.
    for (int i = 0; i < 8; i++) {
        val = val << 1;  // Mueve los bits hacia la izquierda.
        if (arreglo[i] == HIGH) {
            val = val + B1;  // Suma un 1 si el valor es HIGH.
        }
    }
    return val;  // Retorna la variable convertida.
}

// Función para separar los primeros 16 bits en SR1 y SR2
void separarBits(boolean SR1[8], boolean SR2[8]) {
    // Separar primeros 8 bits para SR1
    for (int i = 0; i < 8; i++) {
        SR1[i] = leds[i];
    }

    // Separar los siguientes 8 bits para SR2
    for (int i = 0; i < 8; i++) {
        SR2[i] = leds[i + 8];
    }
}

// Función para activar los pines a partir del pin 5
void activarPins() {
    // Empezar desde el bit 16 en la lista leds
    int pin = 5;
    for (int i = 16; i < 30; i++) {
        digitalWrite(pin, leds[i] ? HIGH : LOW);
        pin++;
    }
}

void setup() {
    Serial.begin(9600);
    while (!Serial) { }

    pinMode(data, OUTPUT);
    pinMode(latch, OUTPUT);
    pinMode(clockPin, OUTPUT);

    // Poner los pines adicionales (D5 a A7) como salida.
    for (int pin = 5; pin <= 13; pin++) {
        pinMode(pin, OUTPUT);  // Pines D5 a D13
    }
    pinMode(A0, OUTPUT);  // Pines analógicos A0 a A7
    pinMode(A1, OUTPUT);
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);
    pinMode(A4, OUTPUT);
}

void loop() {
    if (Serial.available() > 0) {
        char input[256];
        int len = Serial.readBytesUntil('\n', input, sizeof(input) - 1);
        input[len] = '\0'; // Null-terminar la cadena

        // Procesar la entrada recibida
        shipCount = 0;
        char *token = strtok(input, ";");
        while (token != NULL && shipCount < MAX_SHIPS) {
            if (sscanf(token, "%d,%d", &ships[shipCount].type, &ships[shipCount].id) == 2) {
                shipCount++;
            }
            token = strtok(NULL, ";");
        }

        crearListaLeds(ships, shipCount);

        // Arreglos para manejar los 2 registros de desplazamiento (16 bits).
        boolean SR1[8];
        boolean SR2[8];

        separarBits(SR1, SR2);

        digitalWrite(latch, LOW);  // Comienzo el protocolo de comunicación.

        // Enviar los datos a los dos registros de desplazamiento (16 bits en total).
        shiftOut(data, clockPin, LSBFIRST, convertir(SR2));  // Segundo integrado.
        shiftOut(data, clockPin, LSBFIRST, convertir(SR1));  // Primer integrado.

        digitalWrite(latch, HIGH);  // Finaliza la comunicación.

        activarPins();  // Activar los LEDs en los pines directos

        Serial.println("ACK");
    }
}
