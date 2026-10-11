/*
 * AUTOR 		: UGB_Dev
 * DATE  		: 10/10/2026
 * VERSION 		: V1.0.0
 * DESCRIPTION	: INTERRUPCION GPIO; Se configura al MCU para capturar eventos de interrupcion
 *                mediante sus banderas.
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

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    Delay_Init();


     /* PASO 1: ACTIVACION DEL RELOJ PARA EL PERIFERICO */
    SYSCON -> AHBCLKCTRL0 |= SYSCON_AHBCLKCTRL0_GPIO1_MASK | SYSCON_AHBCLKCTRL0_PORT1_MASK;

    /* PASO 2: CONFIGURARACION DE LOS PINES COMO ENTRADA Y/O SALIDA */
    PORT1 -> PCR[17] |= PORT_PCR_PE_MASK  | // HABILITACION DE RESISTENCIA EN PULL
    		            PORT_PCR_IBE_MASK | // HABILITACION DE BUFFER DE ENTRADA (NECESARIO PARA ENTRADAS)
						PORT_PCR_PS_MASK  ; // SELECCIONA LA RESISTENCIA DE PULL-UP

    GPIO1 -> ICR[17] |= 0xA<<GPIO_ICR_IRQC_SHIFT | // HABILITA ISF E INTERRUPCION POR FLANCO DE BAJADA
    		            GPIO_ICR_IRQS_MASK;        // HABILITA INTERRUPCION PARA EL CANAL 1 (IRQS=1)

    while(1) {
    	if (GPIO1 -> ISFR[1] & GPIO_ISFR_ISF17_MASK ){
    		PRINTF("BANDERA DE INTERRUPCION ACTIVA EN GPIO1_17 .........  \r\n");
    		GPIO1 -> ISFR[1] |= GPIO_ISFR_ISF17_MASK; // BORRA BANDERA DE INTERRUPCION ESCRIBIENDO UNO LOGICO
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
