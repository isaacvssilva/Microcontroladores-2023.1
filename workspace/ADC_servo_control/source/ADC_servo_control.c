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
 * @file    ADC_servo_control.c
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
#define led_azul 1
#define led_externo_1 3
#define minimumPulseWidth 544444
#define maximumPulseWidth 2500000

void delay_ms(double tempo_ms){
	/*Calculando o número de ciclos para o delay*/
	double ciclos_por_delay = (tempo_ms*1000) / 122;

	/*configurando valor do registrador de contagem para obter o tempo*/
	TPM1->MOD = ciclos_por_delay - 1;

	while((TPM1->SC & (1 << 7)) == 0) { } /* wait until the TOF is set */
	TPM1->SC |= (1 << 7);/* clear TOF */
}


void pwm_delay_ms(float duty_cycle, double periodo) {
	 /* verificando se o valor do duty cycle esta no intervalo valido (0 a 100) */
	if(duty_cycle > 100){
		duty_cycle = 100;
	}else if(duty_cycle < 0){
		duty_cycle = 0;
	}

	double ciclos_por_delay = (periodo*1000) / 122;

	/*configurando valor do registrador de contagem para obter o tempo*/
	TPM0->MOD = ciclos_por_delay - 1;

    /*gerando sinal PWM com dutyCycle informado em porcentagem*/
	double duty_cycles = (ciclos_por_delay * duty_cycle) / 100;
    TPM0->CONTROLS[3].CnV =  duty_cycles;
}

void ADC0_init(void){
	SIM->SCGC5 |= (1 << 13);/* clock to PORTE */
	PORTE->PCR[20] = 0;/* PTE20 analog input */

	/*habilitando clock para ADC*/
	SIM->SCGC6 |= (1 << 27);

	/*12-bit conversao*/
	ADC0->CFG1 |= (0b10 << 2);

	/*selecionando acionamento por software trigger, para iniciar as conversoes ADC*/
	ADC0->SC2 &= ~(1 << 6);

	/*selecionano taxa de clock usada ADC (dividido por 4)*/
	ADC0->CFG1 |= (0b10 << 5);

	/*selecionando clock de entrada (Bus clock)*/
	ADC0->CFG1 |= (0b00 << 0);

	/*leitura precisa: Sample time configuration*/
	ADC0->CFG1 |= (1 << 4);
}

void clock_geral(void){
	/*habilitando o clock para o grupo PORTD e PORTB*/
	SIM->SCGC5 |= (1 << 12) | (1 << 10);

	/*habilitando clock do modulo Timer/PWM TPM0 */
	SIM->SCGC6 |= (1 << 25) |(1 << 24);

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
}

void setAngulo(int novoAngulo) {
    double mapeamento = (maximumPulseWidth - minimumPulseWidth) / 180.0 * novoAngulo + minimumPulseWidth;

    /*Verificando se o mapeamento está dentro dos limites*/
    if (mapeamento < minimumPulseWidth) {
        mapeamento = minimumPulseWidth;
    } else if (mapeamento > maximumPulseWidth) {
        mapeamento = maximumPulseWidth;
    }

    float duty_cycle = ((mapeamento - minimumPulseWidth) / (maximumPulseWidth - minimumPulseWidth) * 10.0) + 2.5;

    /*Verificando se o duty_cycle está dentro dos limites*/
    if (duty_cycle < 2.5) {
        duty_cycle = 2.5;
    } else if (duty_cycle > 12.5) {
        duty_cycle = 12.5;
    }

    pwm_delay_ms(duty_cycle, 20);
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

    /*configurando clock's*/
    clock_geral();
    /*configurando ADC*/
    ADC0_init();

	/*Pino multiplexidado com PWM TPM0_CH0*/
	PORTD->PCR[led_externo_1] |= PORT_PCR_MUX(4);

	/*inicializando canal PWM*/
	/* Desativando o PWM no modulo TPM0 canal 3*/
	TPM0->CONTROLS[3].CnSC = (0 << 5) | (0 << 4) | (0 << 3) | (0 << 2);

	/* Ativar o PWM no modulo TPM0 canal 3*/
	TPM0->CONTROLS[3].CnSC = (1 << 5) | (0 << 4) | (1 << 3) | (0 << 2);


	short int valorAnalogico = 0, anguloServo = 0;;

	while (1) {
		ADC0->SC1[0] = 0; /* start conversion on channel 0 */

		while(!(ADC0->SC1[0] & (1 << 7))) { } /* wait for conversion complete */

		valorAnalogico = ADC0->R[0];/* read conversion result and clear COCO flag */
		anguloServo = valorAnalogico / 1023.0 * 180;
		PRINTF("Angulo: %d\n", anguloServo);
		setAngulo(anguloServo);
	}
}
