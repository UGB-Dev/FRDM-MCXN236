/*
 * AUTOR 		: UGB_Dev
 * DATE  		: 12/10/2026
 * VERSION 		: V1.0.0
 * DESCRIPTION	: CTIMER; Se configura el periferico CTIMER3 como base de tiempo para generar
 *                un toogle en el GPIO4_19 por interrupcion.
 */

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"

int main(void) {

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    /* PASO 1: CONFIGURACION DEL RELOJ */
    SYSCON -> AHBCLKCTRL0 |= SYSCON_AHBCLKCTRL0_GPIO4_MASK ;
    SYSCON -> AHBCLKCTRL2 |= SYSCON_AHBCLKCTRL2_TIMER3_MASK;

    /* PASO 2: CONFIGURACION DE PIN COMO SALIDA */
    GPIO4 -> PDDR |= GPIO_PDDR_PDD17_MASK; // PIN GPIO4_17 (LED AZUL) COMO SALIDA

    /* PASO 3: CONFIGURACION DEL TIMER */
    SYSCON -> CTIMERCLKSEL[3] &= ~SYSCON_CTIMERCLKSEL_SEL_MASK; // BORRA CONFIGURACION POR DEFECTO
	SYSCON -> CTIMERCLKSEL[3] |= 0x1; // FUENTE DE RELOJ POR PLL0 (150 MHz) PARA EL TIMER 3
	SYSCON -> CTIMERCLKDIV[3] &= ~SYSCON_CTIMERXCLKDIV_CTIMERCLKDIV_HALT_MASK; // BORRA BIT HALT ( EVITA BLOQUEO EN MICROCCONTROLADOR)

	CTIMER3 -> MCR |= CTIMER_MCR_MR3I_MASK | CTIMER_MCR_MR3R_MASK; // RESET TIMER E INTERRUPCION POR MATCH
    CTIMER3 -> PR = (SystemCoreClock / 1000) -1; // PR = (150 MHz / 1 KHz)-1 = 149 k TICKS PARA UNA BASE DE TIEMPO DE 1ms
    CTIMER3 -> MR[3] = 500 ; // 500 * 1msbase = 500 ms PARA TIEMPO DE INTERRUPCION

    CTIMER3 -> TCR |= CTIMER_TCR_CEN_MASK; // HABILITA EL CONTADOR

    /* PASO 4: CONFIGURACION DE VECTOR DE INTERRUPCION */
    NVIC -> ISER[CTIMER3_IRQn/32] = 1<<(CTIMER3_IRQn%32);

    while(1) {
    }
}

void CTIMER3_DriverIRQHandler(void){
	if(CTIMER3 -> IR & CTIMER_IR_MR3INT_MASK){
		GPIO4 -> PDOR ^= GPIO_PDOR_PDO17_MASK;
		CTIMER3 -> IR |= CTIMER_IR_MR3INT_MASK; // BORRA BANDERA DE INTERRUPCION
	}
}
