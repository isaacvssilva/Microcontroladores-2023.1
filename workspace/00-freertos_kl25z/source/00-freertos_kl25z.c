/*
 * Copyright 2016-2023 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    00-freertos_kl25z.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
/* TODO: insert other include files here. */
#include "FreeRTOS.h"
#include "task.h"
/* TODO: insert other definitions and declarations here. */

#define led_vermelho 18
#define led_verde 19
#define led_azul 1

/*fazendo as configuacoes para habilitar os leds interno*/
void config_led(void){
	 /*habilitando o clock para o grupo PORTD e PORTB*/
	    SIM->SCGC5 |= (1 << 12) | (1 << 10);

	    /*configurando o pino como GPIO (Multiplexacao)*/
	    PORTD->PCR[led_azul] |= PORT_PCR_MUX(1); /*led azul*/
	    PORTB->PCR[led_verde] |= PORT_PCR_MUX(1); /*led verde*/
	    PORTB->PCR[led_vermelho] |= PORT_PCR_MUX(1); /*led vermelho*/
}

void static tarefa1(void *parameters){
	while(1){
		GPIOB->PDDR |= (1 << led_vermelho); /*setando 1 no bit referente ao led vermelho*/
		vTaskDelay(100);
		GPIOB->PDDR &= ~(1 << led_vermelho);
		vTaskDelay(100);
	}

}

void static tarefa2(void *parameters){
	while(1){
		GPIOD->PDDR |= (1 << led_azul); /*setando 1 no bit referente ao led azul*/
		vTaskDelay(100);
		GPIOD->PDDR &= ~(1 << led_azul);
		vTaskDelay(100);
	}

}

TaskHandle_t t1; 	/*identificador da tarefa 1*/
TaskHandle_t t2;	/*identificador da tarefa 2*/

/*
 * @brief   Application entry point.
 */
int main(void) {

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif
    config_led(); /*inicializando configuracoes*/
    xTaskCreate(tarefa1, "task1", configMINIMAL_STACK_SIZE, NULL, 1, &t1);
    xTaskCreate(tarefa2, "task2", configMINIMAL_STACK_SIZE, NULL, 1, &t2);
    vTaskStartScheduler();

    return 0 ;
}
