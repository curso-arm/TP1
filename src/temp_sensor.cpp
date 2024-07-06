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
#include "temp_sensor.h"
#include "delay.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "dot_matrix.h"
#include <stdio.h>
#include <string.h>

/*********************************************************************************************************
 *** DEFINES PRIVADOS AL MODULO
 *********************************************************************************************************/
#define AVERAGE_SAMPLES   10

/*********************************************************************************************************
 *** MACROS PRIVADAS AL MODULO
 *********************************************************************************************************/

/*********************************************************************************************************
 *** TIPOS DE DATOS PRIVADOS AL MODULO
 *********************************************************************************************************/

/*********************************************************************************************************
 *** TABLAS PRIVADAS AL MODULO
 *********************************************************************************************************/

/*********************************************************************************************************
 *** VARIABLES GLOBALES PUBLICAS
 *********************************************************************************************************/
 extern UnbufferedSerial uartUsb;

/*********************************************************************************************************
 *** VARIABLES GLOBALES PRIVADAS AL MODULO
 *********************************************************************************************************/
AnalogIn tempSensor(A0);

static delay_t adc_sample_period;
static delay_t temp_update;

/*********************************************************************************************************
 *** PROTOTIPO DE FUNCIONES PRIVADAS AL MODULO
 *********************************************************************************************************/


/*********************************************************************************************************
 *** FUNCIONES PRIVADAS AL MODULO
 *********************************************************************************************************/


/*********************************************************************************************************
 *** FUNCIONES GLOBALES AL MODULO
 *********************************************************************************************************/

/**
  \fn           main
  \brief        Eemplo de uso de la librería dot_matrix
  \author       Nombre
  \date         ${date}
  \return       tipo y descripción de retorno
*/
void temp_sensor_Init(uint64_t fs, uint64_t screen_update)
{   
    delayInit(&adc_sample_period, fs);
    delayInit(&temp_update, screen_update);
    tempSensor.set_reference_voltage(3.3);
    uint8_t adc_index = 0;
}
    

float temp_sensor_read(void)
{
    static uint8_t  adc_index = 0;
    static float    adc_sample = 0, result = -99.9;

    if(delayRead(&adc_sample_period))
    {
        if (adc_index == AVERAGE_SAMPLES)
        {
            result = adc_sample / AVERAGE_SAMPLES;
            adc_index = 0;
            adc_sample = 0;
        }
        else
        {
            adc_sample = adc_sample + tempSensor.read_voltage();
            adc_index++;
        } 
        delayRead(&adc_sample_period);
    }

    return result;
}
    
    
float celcius2fahrenheit(float temp_c)
{
    return (temp_c * 9.0/5.0 + 32);
}


void temp_celcius_update(char* buffer, uint8_t* display,size_t display_leng)
{
    float temp;
    temp = temp_sensor_read();
    if(delayRead(&temp_update))
    {
      temp = temp * 30.3 - 10;
      sprintf(buffer, "%.1fC", temp);
      enviar_mensaje(display, display_leng, (const uint8_t *)buffer);
      sprintf(buffer, "Temp: %3.1f °C\r\n", temp);
      uartUsb.write(buffer, 16);
    }
    delayRead(&temp_update);
}


void temp_fahrenheit_update(char* buffer, uint8_t* display,size_t display_leng)
{   
    float temp;
    temp = temp_sensor_read();
    if(delayRead(&temp_update))
    {
        temp = temp * 30.3 - 10;
        temp = celcius2fahrenheit(temp);
        sprintf (buffer, "%.1fF",temp);
        enviar_mensaje(display,display_leng, (const uint8_t *)buffer);
        sprintf (buffer, "Temp: %3.1f °F\r\n", temp);
        uartUsb.write( buffer, 16);
    }
    delayRead(&temp_update);
}