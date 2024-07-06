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
#include "delay.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "delay.h"
#include "dot_matrix.h"
#include <stdio.h>
#include "temp_sensor.h"

/*********************************************************************************************************
 *** DEFINES PRIVADOS AL MODULO
 *********************************************************************************************************/
#define MAX_DISPLAY 192
#define BUFFER_SIZE 32
/*********************************************************************************************************
 *** MACROS PRIVADAS AL MODULO
 *********************************************************************************************************/

/*********************************************************************************************************
 *** TIPOS DE DATOS PRIVADOS AL MODULO
 *********************************************************************************************************/
typedef enum 
{
    MATRIX = 0,
    CERELCIUS,
    FAHRENHEIT
} system_state_e;

/*********************************************************************************************************
 *** TABLAS PRIVADAS AL MODULO
 *********************************************************************************************************/

/*********************************************************************************************************
 *** VARIABLES GLOBALES PUBLICAS
 *********************************************************************************************************/

char buffer[BUFFER_SIZE];

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

system_state_e system_state = MATRIX;

Ticker temp_1ms;

DigitalIn msgButton(D6);
DigitalIn celciusButton(D7);
DigitalIn fahrenheitButton(D8);



UnbufferedSerial uartUsb(USBTX, USBRX, 115200);
/*********************************************************************************************************
 *** VARIABLES GLOBALES PRIVADAS AL MODULO
 *********************************************************************************************************/

void inputsInit(void);

void system_fsm(void);

void matrix(void);

void buttons(void);

/*********************************************************************************************************
 *** PROTOTIPO DE FUNCIONES PRIVADAS AL MODULO
 *********************************************************************************************************/


/*********************************************************************************************************
 *** FUNCIONES PRIVADAS AL MODULO
 *********************************************************************************************************/


/*********************************************************************************************************
 *** FUNCIONES GLOBALES AL MODULO
 *********************************************************************************************************/

void inputsInit(void)
{
    msgButton.mode(PullDown);
    celciusButton.mode(PullDown);
    fahrenheitButton.mode(PullDown);
}


void system_fsm (void)
{
    switch(system_state)
    {
        case MATRIX:
            matrix();
            desplazar_izq(display, sizeof(display) / sizeof(display[0]));
        break;

        case CERELCIUS:
            temp_celcius_update(buffer, display, sizeof(display) / sizeof(display[0]));
        break;

        case FAHRENHEIT:
            temp_fahrenheit_update(buffer, display,sizeof(display) / sizeof(display[0]));
        break;
    }
}


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
            // Agregar un carácter nulo al final para finalizar la cadena
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
            buffer[index] = '\0';
             // Imprimir la cadena recibida
            uartUsb.write("Received: \r\n",12);
            // Reiniciar el índice para la próxima cadena
            index = 0;
            num_of_chars = 0;
        }
    }
}


void buttons (void)
{
    if(msgButton.read())
    {
        system_state = MATRIX;
        enviar_mensaje(display, sizeof(display) / sizeof(display[0]), (const uint8_t *)"TP1 Nicolas Ferragamo");
        uartUsb.write("Ingrese el mensaje a mostrar: \r\n",32);

    }
                
    if(celciusButton.read())
    {
        system_state = CERELCIUS;
    }

    if(fahrenheitButton.read())
    {
        system_state = FAHRENHEIT;
    }
}



/**
  \fn           main
  \brief        Eemplo de uso de la librería dot_matrix
  \author       Nombre
  \date         ${date}
  \return       tipo y descripción de retorno
*/
int main(void)
{
    temp_1ms.attach(&delay_1ms, 1ms);

    inicializar_matriz();
    inputsInit();

    temp_sensor_Init(100,1000);
    enviar_mensaje(display, sizeof(display) / sizeof(display[0]), (const uint8_t *)"TP1 Nicolas Ferragamo");
    while (true)
    {
        buttons();
        system_fsm();
        barrido_matriz(display);
    }
    return 1;
}