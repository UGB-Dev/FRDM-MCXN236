/*
 * AUTOR 		: UGB_Dev
 * DATE  		: 2/09/2026
 * VERSION 		: V1.0.0
 * DESCRIPTION	: BINARY COUNTER; Contador de pulsos los cuales se observaran
 *                en los leds conectados desde GPIO0_22 a GPIO0_29.
 */

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"

uint8_t Tick_us = 0;
uint32_t Tick_ms=0;

void Delay_Init(void);
void Delay_MS(uint32_t ms);

int main(void) {
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    Delay_Init();

    /* PASO 1: ACTIVACION DEL RELOJ PARA EL PERIFERICO */
    SYSCON -> AHBCLKCTRL0 |= SYSCON_AHBCLKCTRL0_GPIO0_MASK|
    						 SYSCON_AHBCLKCTRL0_GPIO1_MASK|SYSCON_AHBCLKCTRL0_PORT1_MASK;

	/* PASO 2: CONFIGURARACION DE LOS PINES COMO ENTRADA Y/O SALIDA */
    GPIO0 -> PDDR |= GPIO_PDDR_PDD22_MASK | GPIO_PDDR_PDD23_MASK | GPIO_PDDR_PDD24_MASK |
    		         GPIO_PDDR_PDD25_MASK | GPIO_PDDR_PDD26_MASK | GPIO_PDDR_PDD27_MASK |
					 GPIO_PDDR_PDD28_MASK | GPIO_PDDR_PDD29_MASK;

    PORT1 -> PCR[17] |= PORT_PCR_PE_MASK | PORT_PCR_IBE_MASK | PORT_PCR_PS_MASK;

	PRINTF("CONFIGURACION DEL PERIFERICOS FINALIZADA ...... \r\n");

	uint8_t count=0;

    while(1) {
    	GPIO0 -> PDOR = count<<22; // DESPLAZAMIENTO PARA INICIAR DESDE PIN P0_22
    	PRINTF("Count = %i\r\n", count);
    	Delay_MS(100);

    	if(!(GPIO1 -> PDIR & GPIO_PDIR_PDI17_MASK)){
    		count ++;
    		Delay_MS(100);
    	}
    }
}

/* FUNCIONES PARA GENERAR DELAY POR SYSTICK*/
void Delay_Init(void){
	SysTick -> CTRL = SysTick_CTRL_CLKSOURCE_Msk; // FUENTE DE RELOJ PRINCIPAL (MAIN = 150 MHz)

	Tick_us = SystemCoreClock / 1000000;
	Tick_ms = Tick_us*1000 -1;
}

void Delay_MS(uint32_t ms){
	for( ; ms>0; ms--){ // TIMEmax = (2^24)/150MHz = 111.848 ms
		SysTick -> LOAD = Tick_ms; // 150MHz/Fbase = 150MHz/1000 = 150K Tick = 150K Tick/150MHz = 1ms por cada Tick
		SysTick -> VAL = 0;
		SysTick -> CTRL |= SysTick_CTRL_ENABLE_Msk; // HABILITA SYSTICK
		while(!(SysTick -> CTRL & SysTick_CTRL_COUNTFLAG_Msk)); // ESPERA HASTA QUE VAL SEA CERO
		SysTick -> CTRL &= ~SysTick_CTRL_ENABLE_Msk; // DESHABILITA SYSTICK
	}
}
