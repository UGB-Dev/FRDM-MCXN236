/*
 * AUTOR 		: UGB_Dev
 * DATE  		: 01/10/2026
 * VERSION 		: V1.0.0
 * DESCRIPTION	: GPIO_INPUT; Lectura en pin P1_14 para generar cambio logico en el pin P4_18
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
    SYSCON -> AHBCLKCTRLSET[0] |= SYSCON_AHBCLKCTRL0_GPIO4_MASK |
    		                      SYSCON_AHBCLKCTRL0_GPIO1_MASK | SYSCON_AHBCLKCTRL0_PORT1_MASK;

    /* PASO 2: CONFIGURARACION DE LOS PINES COMO ENTRADA Y/O SALIDA */
    PORT1 -> PCR[14] |= PORT_PCR_PE_MASK | PORT_PCR_IBE_MASK | PORT_PCR_PS_MASK; // PIN P1_14 COMO ENTRADA CON RESISTENCIA PULL-UP
    GPIO1 -> PDDR &= ~GPIO_PDDR_PDD14_MASK; // GPIO1_14 COMO ENTRADA ; POR DEFECTO NO ES NECESARIO ESTA LINEA

    GPIO4 -> PDDR |= GPIO_PDDR_PDD18_MASK; // GPIO4_18 COMO SALIDA
    GPIO4 -> PDOR |= GPIO_PDOR_PDO18_MASK;

    PRINTF("CONFIGURACION DEL PERIFERICO GPIO1 Y GPIO4 FINALIZADA ...... \r\n");

    while(1) {
    	if(!(GPIO1 -> PDIR & GPIO_PDIR_PDI14_MASK)){
    		GPIO4 -> PDOR &= ~GPIO_PDOR_PDO18_MASK;
    		PRINTF("LED ROJO ON \r\n");
    		Delay_MS(100);
    	}
    	else {
    		GPIO4 -> PDOR |= GPIO_PDOR_PDO18_MASK;
    		PRINTF("GPIO4 OFF \r\n");
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
