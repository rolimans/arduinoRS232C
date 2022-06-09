#define PINO_TX 13
#define PINO_RTS 11
#define PINO_CTS 12
#define BAUD_RATE 1
#define HALF_BAUD_IN_MS 1000 / (2 * BAUD_RATE)
#define START_BIT 0
#define END_BIT 1

#include "Temporizador.h"

byte counter;
byte currentData;

// Calcula bit de paridade - Par ou impar
bool bitParidade(char dado) {
    bool parityBit = 0;
    for (byte i = 0; i < 8; i++) {
        parityBit xor_eq bitRead(dado, i);
    }
    return !parityBit;
}

// Rotina de interrupcao do timer1
// O que fazer toda vez que 1s passou?
ISR(TIMER1_COMPA_vect) {
    //>>>> Codigo Aqui <<<<
    if (counter == 0) {
        digitalWrite(PINO_TX, START_BIT);
    } else if (counter == 9) {
        digitalWrite(PINO_TX, bitParidade(currentData));
    } else if (counter == 10) {
        digitalWrite(PINO_TX, END_BIT);
    } else if (counter == 11) {
        paraTemporizador();
        digitalWrite(PINO_RTS, LOW);
        counter = 0;
        return;
    } else {
        digitalWrite(PINO_TX, bitRead(currentData, counter - 1));
    }
    counter++;
}

// Executada uma vez quando o Arduino reseta
void setup() {
    // desabilita interrupcoes
    noInterrupts();
    // Configura porta serial (Serial Monitor - Ctrl + Shift + M)
    Serial.begin(9600);
    // Inicializa TX ou RX
    pinMode(PINO_TX, OUTPUT);
    pinMode(PINO_RTS, OUTPUT);
    pinMode(PINO_CTS, INPUT);
    digitalWrite(PINO_RTS, LOW);
    counter = 0;
    // Configura timer
    configuraTemporizador(BAUD_RATE);
    // habilita interrupcoes
    interrupts();
}

// O loop() eh executado continuamente (como um while(true))
void loop() {

    // Não faz nada caso esteja ocorrendo transmissão de dados ou não exista
    // dados no buffer Serial
    if (digitalRead(PINO_RTS) == HIGH || Serial.peek() == -1) {
        return;
    }

    currentData = Serial.read();

    digitalWrite(PINO_RTS, HIGH);

    while (digitalRead(PINO_CTS) == LOW) {
        delay(HALF_BAUD_IN_MS);
    }

    iniciaTemporizador();
}
