//Bibliotheken einbinden
#include <avr/io.h>
#include <stdio.h>
#include <string.h>

//Makrodefinitionen
#define F_CPU 16000000UL          //CPU-Frequenz 
#define BAUD 9600                 //Baudrate für serielle Kommunikation
#define MYUBRR F_CPU/16/BAUD-1    //USART Baudrate Register


//Funktion zur Konfiguration der seriellen Schnittstelle
void serial_port_configure(unsigned int baud_rate) {
    UBRR0H = (unsigned char)(baud_rate>>8);                  
    UBRR0L = (unsigned char)baud_rate;                      
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);                     
    UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);                   
}

//Funktion zum Empfangen einzelner Zeichen
unsigned char serial_port_receive(void) {
    while (!(UCSR0A & (1<<RXC0)));
    return UDR0;
}

//Funktion zum Senden einzelner Zeichen
void serial_port_transmit(unsigned char char_to_send) {
    while (!(UCSR0A & (1<<UDRE0)));
    UDR0 = char_to_send;
}

//Funktion zum Senden von Strings
void serial_port_send_string(const char *string_pointer) {
    while (*string_pointer  != '\0') {
        serial_port_transmit(*string_pointer );
        string_pointer ++;
    }
}

int main(void) {

    //Variablen deklarieren
    int term1;                      //Erster Term Benutzereingabe
    int term2;                      //Zweiter Term Benutzereingabe
    int result;                     //Ergebnis der Kalkulation
    int index;                      //Index Eingabezeichenfolge
    int check_input_user;           //Ueberpruefung des Eingabeformats
    char input_character;           //Aktuell empfangenes Zeichen
    char input_user[64];            //Benutzereingabe
    char arithmetic_operator;       //Mathematischer Operator
    char result_string[64];         //Ergebnisstring

    //Aufrufen der konfigurierten seriellen Schnittstelle
    serial_port_configure(MYUBRR);

    while (1) {

        //Loeschen alter Datensaetze des gesamten Speicherbereichs
        index = 0;
        memset(input_user, 0, sizeof(input_user));

        //Einlesen Benutzereingabe
        do {
            input_character = serial_port_receive();              
            input_user[index++] = input_character;          
        } while (input_character != '\n' && index < 64);

        //Benutzereingabe parsen
        check_input_user = sscanf(input_user, "%d %c %d", &term1, &arithmetic_operator, &term2);
        if (check_input_user != 3) {
            serial_port_send_string("Ungueltige Eingabe\n");
            continue;
        }

        //Kalkulation durchfuehren
        switch (arithmetic_operator) {
            case '+':
                result = term1 + term2;
                break;
            case '-':
                result = term1 - term2;
                break;
            case '*':
                result = term1 * term2;
                break;
            case '/':
                if (term2 != 0) {
                    result = term1 / term2;
                } else {
                    serial_port_send_string("Division durch Null nicht moeglich\n");
                    continue;
                }
                break;
            default:
                serial_port_send_string("Ungueltige Eingabe\n");
                continue;
        }

        //Ergebnis konvertiern und zuruecksenden
        snprintf(result_string, 64, "Das Ergebnis lautet: %d\n", result);
        serial_port_send_string(result_string);
    }

    return 0;
}
