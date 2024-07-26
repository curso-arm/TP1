/*********************************************************************************************************
 * <Module name>
 * <Module description
 *
 * <Copyright>
 *
 * <Copyright or distribution terms>
 *
 *
 *********************************************************************************************************/

/*********************************************************************************************************
 * <File description>
 *
 * Filename       : Filename
 * Version        : Module version
 * Programmer(s)  : Programmer initial(s)
 **********************************************************************************************************
 *  Note(s):
 *
 *
 *
 *********************************************************************************************************/

/*********************************************************************************************************
 *
 * \file        ${file_name}
 * \brief       Descripción del modulo
 * \date        ${date}
 * \author      Nicolas Ferragamo nferragamo@est.frba.utn.edu.ar
 * \version
 *********************************************************************************************************/

/*********************************************************************************************************
 *** INCLUDES
 *********************************************************************************************************/
#include "mbed.h"
#include "arm_book_lib.h"
#include "delay.h"
#include "dot_matrix.h"
#include <stdio.h>
#include "temp_sensor.h"

/*********************************************************************************************************
 *** DEFINES PRIVADOS AL MODULO
 *********************************************************************************************************/
#define MAX_DISPLAY 192     //!<  limita la cantidad de columnas de la matriz en 192

#define BUFFER_SIZE 32      //!< cantidad máxima de caracteres que se pueden mostrar en la matriz
/*********************************************************************************************************
 *** MACROS PRIVADAS AL MODULO
 *********************************************************************************************************/

/*********************************************************************************************************
 *** TIPOS DE DATOS PRIVADOS AL MODULO
 *********************************************************************************************************/

/**
 *  \enum       system_state_e
 *  \brief      Estados de la máquina de estados principal, indica que tarea va a ejecutar
    \author     Nicolas Ferragamo
    \date       ${date}
*/
typedef enum 
{
    MATRIX = 0,     //!< Muestra el mensaje en la matriz
    CELCIUS,      //!< Muestra la temperatura en grados celcius
    FAHRENHEIT      //!< Muestra la temperatura en grados fahrenheit
} system_state_e;

/*********************************************************************************************************
 *** TABLAS PRIVADAS AL MODULO
 *********************************************************************************************************/

/*********************************************************************************************************
 *** VARIABLES GLOBALES PUBLICAS
 *********************************************************************************************************/

char buffer[BUFFER_SIZE];   //!< buffer para almacenar los caracteres recibidos por la uart

uint8_t display[MAX_DISPLAY]; //!< cada bit representa una fila, cada byte una columna /*
/*  columna 1 :  | x | F7 | F6 | F5 | F4 | F3 | F2 | F1 | F0 | 
    columna 2 :  | x | F7 | F6 | F5 | F4 | F3 | F2 | F1 | F0 | 
    .
    .
    .
    columna n-1: | x | F7 | F6 | F5 | F4 | F3 | F2 | F1 | F0 |
    columna n:   | x | F7 | F6 | F5 | F4 | F3 | F2 | F1 | F0 |

    hay que pensar cada caracter de forma vertical, como si la memoria fuera una sucesión de columnas
*/

system_state_e system_state = MATRIX;       //!< estado de la máquina de estados

Ticker temp_1ms;                            //!< temporizador de 1ms se usa para dar el tiempo a la librería delay

InterruptIn msgButton(D6);                    //!< botón para mostrar un mensaje en la matriz
DigitalIn celciusButton(D7);                //!< botón para mostrar la temperatura en grados celcius
DigitalIn fahrenheitButton(D8);             //!< botón para mostrar la temperatura en grados fahrenheit



UnbufferedSerial uartUsb(USBTX, USBRX, 115200);     //!< comunicación serial
/*********************************************************************************************************
 *** VARIABLES GLOBALES PRIVADAS AL MODULO
 *********************************************************************************************************/
bool msgButtonState = false;

/*********************************************************************************************************
 *** PROTOTIPO DE FUNCIONES PRIVADAS AL MODULO
 *********************************************************************************************************/
void inputsInit(void);      

void system_fsm(void);

void matrix(void);

void buttons(void);

void msgButtonCallback (void);

/*********************************************************************************************************
 *** FUNCIONES PRIVADAS AL MODULO
 *********************************************************************************************************/


/*********************************************************************************************************
 *** FUNCIONES GLOBALES AL MODULO
 *********************************************************************************************************/

/**
    \fn         void inputsInit(void)
    \brief      Se configuran los pines de entrada para los botones con resisrencia de pull down
    \author     Nicolas Ferragamo
    \date       ${date}
*/
void inputsInit(void)
{
    msgButton.mode(PullDown);
    msgButton.rise(&msgButtonCallback);
    celciusButton.mode(PullDown);
    fahrenheitButton.mode(PullDown);
}


/**
    \fn         void system_fsm(void)
    \brief      Máquina de estados principal
                MATRIX: muestra un mensaje en la matriz, el mensaje se recibe por la uart,
                        desplaza el texto a la izquierda.
                CELCIUS: muestra la temperatura en grados celcius
                FAHRENHEIT: muestra la temperatura en grados fahrenheit
    \author     Nicolas Ferragamo
    \date       ${date}
*/
void system_fsm (void)
{
    switch(system_state)
    {
        case MATRIX:
            matrix();
            desplazar_izq(display, sizeof(display) / sizeof(display[0]));
        break;

        case CELCIUS:
            temp_celcius_update(buffer, display, sizeof(display) / sizeof(display[0]));
        break;

        case FAHRENHEIT:
            temp_fahrenheit_update(buffer, display,sizeof(display) / sizeof(display[0]));
        break;
    }
}


/**
    \fn         void matrix(void)
    \brief      Muestra un mensaje en la matriz, el mensaje se recibe por la uart
    \details    Se recibe un mensaje por la uart, se muestra en la matriz y se desplaza a la izquierda
                Se espera recbir un '\r' ó llegar al límite del  buffer de tamaño BUFFER_SIZE - 1 
                para dejar de guardar los caracteres recibidos en el buffer.
                se espera recibie un '\n' para mostrar el mensaje en la matriz

    \author     Nicolas Ferragamo
    \date       ${date}
*/
void matrix(void)
{
char receivedChar;
    static uint8_t num_of_chars=0, index = 0;
    if(uartUsb.readable())
    {
        uartUsb.read( &receivedChar, 1 );
        num_of_chars++;
        if (receivedChar == '\r' || num_of_chars == BUFFER_SIZE - 1)
        {
            // Agregar un carácter nulo al final para finalizar la cadena es necesario para la función envir_mensaje
            buffer[index++] = '\0';
            enviar_mensaje(display, sizeof(display) / sizeof(display[0]), (const uint8_t *)buffer);
        }
        else
        {
            // Si el índice es menor que el tamaño del buffer, añadir el carácter
            if (index < BUFFER_SIZE - 1)
            {
                buffer[index++] = receivedChar;
            }
        }
        if (receivedChar == '\n')
        {
            buffer[index] = '\0';   //se prodría eliminar esta línea
             // Imprimir la cadena recibida
            uartUsb.write("Received: \r\n",12);  //se envia a la consola un mensaje que indica que se recibió el mensaje completo
            // Reiniciar el índice para la próxima cadena
            index = 0;
            num_of_chars = 0;
        }
    }
}


/**
    \fn         void buttons(void)
    \brief      Se encarga de leer los botones y cambiar el estado de la máquina de estados
    \author     Nicolas Ferragamo
    \date       ${date}
*/
void buttons (void)
{
    if(msgButtonState == true)
    {
        msgButtonState = false;
        system_state = MATRIX;
        enviar_mensaje(display, sizeof(display) / sizeof(display[0]), (const uint8_t *)"TP1 Nicolas Ferragamo");
        uartUsb.write("Ingrese el mensaje a mostrar: \r\n",32);
    }
                
    if(celciusButton.read())
    {
        system_state = CELCIUS;
        msgButton.rise(&msgButtonCallback);
    }

    if(fahrenheitButton.read())
    {
        system_state = FAHRENHEIT;
        msgButton.rise(&msgButtonCallback);
    }
}

void msgButtonCallback (void)
{
    msgButtonState = true;
    msgButton.rise(NULL);
}

/**
    \fn         int main(void)
    \brief      Función principal
    \author     Nicolas Ferragamo
    \date       ${date}
*/
int main(void)
{
    temp_1ms.attach(&delay_1ms, 1ms);

    inicializar_matriz();
    inputsInit();

    temp_sensor_Init(100,1000);     //se actualiza la temperatura cada 100ms y se muestra en la matriz cada 1000ms
    //  Mensae por defecto de la matriz
    enviar_mensaje(display, sizeof(display) / sizeof(display[0]), (const uint8_t *)"TP1 Nicolas Ferragamo");
    while (true)
    {
        buttons();
        system_fsm();
        barrido_matriz(display);    //se realiza el barrido de la matriz cada 2ms
    }
    return 1;
}