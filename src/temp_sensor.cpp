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
#define AVERAGE_SAMPLES 10 //!< Cantidad de muestras a promediar

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
extern UnbufferedSerial uartUsb; //!< Comunicación serial

/*********************************************************************************************************
 *** VARIABLES GLOBALES PRIVADAS AL MODULO
 *********************************************************************************************************/
AnalogIn tempSensor(A0); //!< Sensor de temperatura

static delay_t adc_sample_period; //!< Periodo de muestreo del ADC
static delay_t temp_update;       //!< Periodo de actualización de la temperatura en la pantalla

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
    \fn         void temp_sensor_Init(uint64_t fs, uint64_t screen_update)
    \brief      Inicializa el sensor de temperatura, se establece la frecuencia
                de muestreo del ADC y la frecuencia de actualización de la pantalla,
                se setea la referencia del sensor de temperatura a 3.3V
    \param      fs [in] Frecuencia de muestreo del ADC
    \param      screen_update [in] Frecuencia de actualización de la pantalla
    \author     Nicolas Ferragamo
    \date       ${date}
*/
void temp_sensor_Init(uint64_t fs, uint64_t screen_update)
{
    delayInit(&adc_sample_period, fs);
    delayInit(&temp_update, screen_update);
    tempSensor.set_reference_voltage(3.3);
}

/**
    \fn         float temp_sensor_read(void)
    \brief      Lee el sensor de temperatura, promedia AVERAGE_SAMPLES muestras.
                Se lee el sensor cada fs segundos. (es un filtrado)
    \author     Nicolas Ferragamo
    \return     Temperatura en volts, con el último valor promediado, no la última muestra.
    \date       ${date}
*/
float temp_sensor_read(void)
{
    static uint8_t adc_index = 0;
    static float adc_sample = 0, result = -99.9; // Inicializo la variable result con un valor inválido

    if (delayRead(&adc_sample_period))
    {
        if (adc_index == AVERAGE_SAMPLES)
        {
            result = adc_sample / AVERAGE_SAMPLES;
            adc_index = 0;
            adc_sample = 0;
        }
        else
        {
            adc_sample = adc_sample + tempSensor.read_voltage(); // lee y acumula las muestras
            adc_index++;
        }
        delayRead(&adc_sample_period); // Reinicio el delay
    }

    return result;
}

/**
    \fn         float celcius2fahrenheit(float temp_c)
    \brief      Convierte la temperatura de celcius a fahrenheit
    \param      temp_c [in] Valor de temperatura en celcius
    \return     Temperatura en fahrenheit
    \author     Nicolas Ferragamo
    \date       ${date}
*/

float celcius2fahrenheit(float temp_c)
{
    return (temp_c * 9.0 / 5.0 + 32);
}

/**
    \fn         void temp_celcius_update(char* buffer, uint8_t* display,size_t display_leng)
    \brief      Se encarga de actualizar la temperatura en la pantalla en celcius cada screen_update segundos
                Convierte la tensión leida por el sensor a temperatura en celcius.
                Envia la temperatura por el puerto serie.
    \param      buffer [in] Buffer para almacenar la temperatura en celcius
    \param      display [in] Puntero a la pantalla
    \param      display_leng [in] Longitud de la pantalla
    \author     Nicolas Ferragamo
    \date       ${date}
*/
void temp_celcius_update(char *buffer, uint8_t *display, size_t display_leng)
{
    float temp;
    temp = temp_sensor_read();
    if (delayRead(&temp_update))
    {
        temp = temp * 30.3 - 10;        // Convierto la tensión a temperatura en celcius
        sprintf(buffer, "%.1fC", temp); // Convierto la temperatura a string
        enviar_mensaje(display, display_leng, (const uint8_t *)buffer); // Envio la temperatura a la pantalla
        sprintf(buffer, "Temp: %3.1f °C\r\n", temp); // Convierto la temperatura a string
        uartUsb.write(buffer, 16); // Envio la temperatura por el puerto serie
    }
    delayRead(&temp_update);        // Reinicio el delay
}


/**
    \fn         temp_fahrenheit_update(char *buffer, uint8_t *display, size_t display_leng)
    \brief      Se encarga de actualizar la temperatura en la pantalla en fahrenheit cada screen_update segundos
                Convierte la temperatura leida en  fahrenheit.
                Envia la temperatura por el puerto serie.
    \param      buffer [in] Buffer para almacenar la temperatura en fahrenheit
    \param      display [in] Puntero a la pantalla
    \param      display_leng [in] Longitud de la pantalla
    \author     Nicolas Ferragamo
    \date       ${date}
*/
void temp_fahrenheit_update(char *buffer, uint8_t *display, size_t display_leng)
{
    float temp;
    temp = temp_sensor_read();
    if (delayRead(&temp_update))
    {
        temp = temp * 30.3 - 10;            // Convierto la tensión a temperatura en celcius
        temp = celcius2fahrenheit(temp);    // Convierto la temperatura a fahrenheit
        sprintf(buffer, "%.1fF", temp);     // Convierto la temperatura a string
        enviar_mensaje(display, display_leng, (const uint8_t *)buffer);   // Envio la temperatura a la pantalla
        sprintf(buffer, "Temp: %3.1f °F\r\n", temp);   // Convierto la temperatura a string
        uartUsb.write(buffer, 16);
    }
    delayRead(&temp_update);        // Reinicio el delay
}