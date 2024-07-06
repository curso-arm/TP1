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

/*********************************************************************************************************
 *** DEFINES PRIVADOS AL MODULO
 *********************************************************************************************************/

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

/*********************************************************************************************************
 *** VARIABLES GLOBALES PRIVADAS AL MODULO
 *********************************************************************************************************/
static volatile uint64_t total_ms = 0;
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
  \fn           delayInit(delay_t *delay, uint64_t duration)
  \brief        Inicio del temporizador
  \param        delay [in] estructura del temporizador
  \param        duration [in] duración del temporizador
  \author       Nombre
  \date         ${date}
  \return       tipo y descripción de retorno
*/

void delayInit(delay_t *delay, uint64_t duration)
{
    delay->duration = duration;
    delay->running = false;
    delay->startTime = 0;
}


/**
  \fn           delayRead(delay_t *delay)
  \brief        Comienza el gtemporizador si esta apagado y
                devuelve true cuando el tiempo finalizo
  \param        delay [in] estructura del temporizador
  \return       true : si el tiempo finalizó
                false: si el tiempo no finalizó
  \author       Nombre
  \date         ${date}
*/
bool delayRead(delay_t *delay)
{
    bool timerFinish = false;

    if (delay->running == false)
    {
        delay->startTime = total_ms;
        delay->running = true;
    }
    else
    {
        if (total_ms - delay->startTime >= delay->duration)
        {
            delay->running = false;
            delay->startTime = 0;
            timerFinish = true;
        }
    }
    return timerFinish;
}


/**
  \fn           delayWrite(delay_t *delay, uint64_t duration)
  \brief        Establece el tiempo del demporizador
  \param        delay [in] estructura del temporizador
  \param        duration [in] duración del temporizador
  \author       Nombre
  \date         ${date}
*/
void delayWrite(delay_t *delay, uint64_t duration)
{
    delay->duration = duration;
}


/**
  \fn           delay_1ms(void)
  \brief        Esta función se usa como callback de un temporizador que actualiza
                cada 1ms. este tiempo se utiliza para el barrid y para el desplaamiento
                del cartel
  \author       Nombre
  \date         ${date}
*/
void delay_1ms (void)
{
    total_ms++;
}