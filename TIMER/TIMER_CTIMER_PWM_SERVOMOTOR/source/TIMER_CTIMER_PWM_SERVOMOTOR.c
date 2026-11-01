/*
 * AUTOR 		: UGB_Dev
 * DATE  		: 12/10/2026
 * VERSION 		: V1.0.0
 * DESCRIPTION	: CTIMER SERVO; Se configura el periferico CTIMER2 como base de tiempo para generar
 *                PWM a 50 Hz como senal del servo .
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
void Set_Duty_PWM(uint16_t D);

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

    /* PASO 1: CONFIGURACION DEL RELOJ */
    SYSCON -> AHBCLKCTRL0 |= SYSCON_AHBCLKCTRL0_GPIO1_MASK | SYSCON_AHBCLKCTRL0_PORT1_MASK ;
    SYSCON -> AHBCLKCTRL1 |= SYSCON_AHBCLKCTRL1_TIMER2_MASK;

    /* PASO 2: CONFIGURACION DE PIN COMO SALIDA */
    PORT1 -> PCR[10]  |=  0x4 << PORT_PCR_MUX_SHIFT; // PIN GPIO1_10 COMO ALT4
    GPIO1 -> PDDR |= GPIO_PDDR_PDD19_MASK; // PIN GPIO1_10 COMO SALIDA PWM ALTERN FUNCTION ALT4

    /* PASO 3: CONFIGURACION DEL TIMER */
    SYSCON -> CTIMERCLKSEL[2] &= ~SYSCON_CTIMERCLKSEL_SEL_MASK; // BORRA CONFIGURACION POR DEFECTO
	SYSCON -> CTIMERCLKSEL[2] |= 0x1; // FUENTE DE RELOJ POR PLL0 (150 MHz) PARA EL TIMER 3
	SYSCON -> CTIMERCLKDIV[2] &= ~SYSCON_CTIMERXCLKDIV_CTIMERCLKDIV_HALT_MASK; // BORRA BIT HALT ( EVITA BLOQUEO EN MICROCCONTROLADOR)

	CTIMER2 -> MCR |= CTIMER_MCR_MR3R_MASK; // RESET TIMER EN CANAL 3 (CANAL 3 COMO PERIODO DEL PWM)
    CTIMER2 -> PWMC |= CTIMER_PWMC_PWMEN0_MASK; // PWM EN CANAL 0 (CANAL 0 COMO DUTY CYCLE)
	CTIMER2 -> PR = (SystemCoreClock / 1000000) -1; // PR = (150 MHz / 1 MHz)-1 = 149  TICKS PARA UNA BASE DE TIEMPO DE 1us
    CTIMER2 -> MR[3] = 20000 ; // 20000 * 1usbase = 20 ms PARA PERIODO (T)
    CTIMER2 -> MR[0] = 20000 - 1500 ; // 1000 * 1usbase = 1 ms PARA DUTY CYCLE (D)

    CTIMER2 -> TCR |= CTIMER_TCR_CEN_MASK; // HABILITA EL CONTADOR

    uint16_t grado=0;

    while(1) {
    	Set_Duty_PWM(500 + (grado*2000)/180);
    	Delay_MS(5);
    	grado++;
    	if(grado>180){
    		grado=0;
    		Set_Duty_PWM(500 + (grado*2000)/180);
    		Delay_MS(500);
    	}
    }
}

void Set_Duty_PWM(uint16_t D){
	CTIMER2 -> MR[0] = 20000 - D ; // 1000 * 1usbase = 1 ms PARA DUTY CYCLE (D)
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
