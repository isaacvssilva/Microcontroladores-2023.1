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
 * @file    00-blink_internal.c
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

/* TODO: insert other definitions and declarations here. */
#define led_vermelho 18
#define led_verde 19
#define led_azul 1

void delay_ms(int ms) {
	int i, j;
	for(i = 0; i < ms; i++) {
		for(j = 0; j < 7000; j++);
	}
}
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

    /*habilitando o clock para o grupo PORTD e PORTB*/
    SIM->SCGC5 |= (1 << 12) | (1 << 10);

    /*configurando o pino como GPIO (Multiplexacao)*/
    PORTD->PCR[led_azul] |= PORT_PCR_MUX(1); /*led azul*/
    PORTB->PCR[led_verde] |= PORT_PCR_MUX(1); /*led verde*/
    PORTB->PCR[led_vermelho] |= PORT_PCR_MUX(1); /*led vermelho*/

    while(1){
    	/*configurando GPIO como saida (Port Data Output Register)*/
		GPIOB->PDDR |= (1 << led_vermelho); /*setando 1 no bit referente ao led vermelho*/
		delay_ms(100);
		GPIOB->PDDR &= ~(1 << led_vermelho);
		delay_ms(100);
		GPIOB->PDDR |= (1 << led_verde); /*setando 1 no bit referente ao led verde*/
		delay_ms(100);
		GPIOB->PDDR &= ~(1 << led_verde);
		delay_ms(100);
		GPIOD->PDDR |= (1 << led_azul); /*setando 1 no bit referente ao led azul*/
		delay_ms(100);
		GPIOD->PDDR &= ~(1 << led_azul);
		delay_ms(100);

		/*rgb(255,255,0) -> amarelo*/
		GPIOB->PDDR |= (1 << led_vermelho); /*setando 1 no bit referente ao led vermelho*/
		GPIOB->PDDR |= (1 << led_verde); /*setando 1 no bit referente ao led verde*/
		delay_ms(300);

		/*rgb(0,255,255) -> ciano*/
		GPIOB->PDDR &= ~(1 << led_vermelho);
		GPIOD->PDDR |= (1 << led_azul); /*setando 1 no bit referente ao led azul*/
		delay_ms(300);
		GPIOB->PDDR &= ~(1 << led_verde);
		GPIOD->PDDR &= ~(1 << led_azul);
		delay_ms(50);
    }

    return 0;
}
