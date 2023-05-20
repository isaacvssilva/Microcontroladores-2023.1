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
 * @file    01-pratica_timer.c
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

#define led_externo_1 3
#define led_vermelho 18

void delay_ms(float tempo_ms){
	/*Calculando o número de ciclos para o delay*/
	uint32_t ciclos_por_delay = (tempo_ms*1000) / 122;

	/*configurando valor do registrador de contagem para obter o tempo*/
	TPM0->MOD = ciclos_por_delay - 1;

	while((TPM0->SC & (1 << 7)) == 0) { } /* wait until the TOF is set */
	TPM0->SC |= (1 << 7);/* clear TOF */

}

void delay_hz(uint32_t hertz){
    /*Calculando o número de ciclos para o delay*/
    float periodo = 1.0f / hertz;
    periodo *= 1000000;
    uint32_t ciclos_por_delay = ((int) periodo) / 122;

	/*configurando valor do registrador de contagem para obter o tempo*/
    TPM1->MOD = ciclos_por_delay - 1;

	while((TPM1->SC & (1 << 7)) == 0) { } /* wait until the TOF is set */
	TPM1->SC |= (1 << 7);/* clear TOF */
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

	/*TIMERS*/

	/*habilitando o clock para o grupo PORTD e PORTB*/
	SIM->SCGC5 |= (1 << 12) | (1 << 10);

	/*habilitando clock do modulo Timer/PWM TPM0 e TPM1*/
	SIM->SCGC6 |= (1 << 24);
	SIM->SCGC6 |= (1 << 25);

	/*Seleciona a fonte de relógio para o contador TPM*/
	SIM->SOPT2 |= (0b11 << 24); /*MCGIRCLK (32.768 kHz)*/

	/*configurando o prescaler select (divisao por 4)*/
	TPM0->SC |= (0b10 << 0);
	TPM1->SC |= (0b10 << 0);

	/*habilitando contador*/
	TPM0->SC |= (0b01 << 3);
	TPM1->SC |= (0b01 << 3);

	/*habilitando flag de interrupcao clear TOF*/
	TPM0->SC |= (1 << 7);
	TPM1->SC |= (1 << 7);

	/*Pino PTD3 e PTB18 multiplexidado como GPIO*/
	PORTD->PCR[led_externo_1] |= PORT_PCR_MUX(1);
	PORTB->PCR[led_vermelho] |= PORT_PCR_MUX(1); /*led vermelho*/

	/* Configurando o PTD3 e PTB18 como saída*/
	PTD->PDDR |= (1 << led_externo_1);
	PTB->PDDR |= (1 << led_vermelho);

    while (1) {
    	PTD->PTOR |= (1 << led_externo_1);
		delay_ms(1000);
		PTB->PTOR |= (1 << led_vermelho);
		delay_hz(1);
    }

}
