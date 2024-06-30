//Einbinden der Bibliotheken
#include <stdio.h>
#include <windows.h>
#include <stdbool.h>
#include <string.h>

//Definieren des COM-Ports für die serielle Kommunikation
#define com_port "\\\\.\\COM3"

//Funktion zur Konfiguration der seriellen Schnittstelle
void configureSerialPort(HANDLE serial_handle) {
    DCB serial_configuration_parameter;             //Variable fuer die Festlegung der Parameter

    //Konfigurieren und anwenden Parameter fuer serielle Schnittstelle
    serial_configuration_parameter.BaudRate = CBR_9600;
    serial_configuration_parameter.ByteSize = 8;
    serial_configuration_parameter.StopBits = ONESTOPBIT;
    serial_configuration_parameter.Parity = NOPARITY;

    SetCommState(serial_handle, &serial_configuration_parameter);

    //Konfigurieren und anwenden der Timeout Einstellungen
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 100;
    timeouts.ReadTotalTimeoutConstant = 100;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 100;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    SetCommTimeouts(serial_handle, &timeouts);
}

int main() {

    //Variablen deklarieren
    char input_user[64];            //Benutzereingabe
    char result [64];               //Ergebnis
    HANDLE serial_handle;           //Verbindung zur seriellen Schnittstelle


    //Einlesen der Benutzereingabe
    printf("Bitte geben Sie eine einfache algebraische Rechnung ein, bestehend aus zwei algebraischen Termen mit einem Rechenoperator in der Mitte:\n");
    fgets(input_user, sizeof(input_user), stdin);


    //Serielle Schnittstelle mit Lese- und Schreibzugriff oeffnen
    serial_handle = CreateFile(com_port,
                                GENERIC_READ | GENERIC_WRITE,
                                0,
                                0,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                0);


    //Aufrufen der konfigurierten seriellen Schnittstelle
    configureSerialPort(serial_handle);


    //Senden der Benutzereingabe an den Mikrocontroller
    WriteFile(serial_handle, input_user, strlen(input_user), NULL, NULL);


    //Empfangen des Ergebnisses
    ReadFile(serial_handle, result, sizeof(result), NULL, NULL);
    printf("%s\n", result);


    //Protokollieren des Kommunikationsablaufs
    FILE *logfile = fopen("Kommunikationsprotokoll.txt", "a");        //Oeffnen der Datei im Anhaenge-Modus("a")


    //Schreiben des Kommunikationsablaufes in die Protokolldatei
    fprintf(logfile, "Benutzereingabe vom PC: %s\n Ergebnis vom Mikrocontroller: %s\n\n", input_user, result);
    fclose(logfile);


    //Schliessen der seriellen Schnittstelle
    CloseHandle(serial_handle);


    return 0;
}
