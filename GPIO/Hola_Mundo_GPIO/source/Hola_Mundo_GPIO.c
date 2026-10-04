/*
 * AUTOR 		: UGB_Dev
 * DATE  		: 30/09/2026
 * VERSION 		: V1.0.0
 * DESCRIPTION	: HELLO WORLD; Cambios logicos en pines 17, 18 y 19.
 */

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"

#define Led_Rojo_Out	GPIO_PDDR_PDD18_MASK
#define Led_Rojo_H		GPIO_PDOR_PDO18_MASK
#define Led_Azul_Out	GPIO_PDDR_PDD17_MASK
#define Led_Azul_H		GPIO_PDOR_PDO17_MASK
#define Led_Verde_Out	GPIO_PDDR_PDD19_MASK
#define Led_Verde_H		GPIO_PDOR_PDO19_MASK

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

    PRINTF("Hello World\r\n");
    
	/* PASO 1: ACTIVACION DEL RELOJ PARA EL PERIFERICO */
    SYSCON -> AHBCLKCTRLSET[0] |= SYSCON_AHBCLKCTRL0_GPIO4_MASK; // HABILITA EL RELOJ DEL GRUPO 4 DE GPIO

    PRINTF("CONFIGURACION DEL GRUPO GPIO 4\r\n");

	/* PASO 2: CONFIGURARACION DE LOS PINES COMO SALIDA */
    GPIO4 -> PDDR |= Led_Rojo_Out | Led_Azul_Out | Led_Verde_Out; // PIN P4_17, P4_18 y P4_19
    GPIO4 -> PDOR |= Led_Rojo_H | Led_Azul_H | Led_Verde_H; // SALIDA EN 1 LOGICO EN PIN P4_17, P4_18 y P4_19 (TODOS LOS LEDS APAGADOS)

    while(1) {
    	GPIO4 -> PDOR &= ~Led_Rojo_H;
    	PRINTF("LED ROJO ON\r\n");
    	Delay_MS(500);
    	GPIO4 -> PDOR |= Led_Rojo_H;
    	PRINTF("LED ROJO OFF\r\n");
    	Delay_MS(500);

    	GPIO4 -> PDOR &= ~Led_Azul_H;
    	PRINTF("LED AZUL ON\r\n");
    	Delay_MS(500);
    	GPIO4 -> PDOR |= Led_Azul_H;
    	PRINTF("LED AZUL OFF\r\n");
    	Delay_MS(500);

    	GPIO4 -> PDOR &= ~Led_Verde_H;
    	PRINTF("LED VERDE ON\r\n");
    	Delay_MS(500);
    	GPIO4 -> PDOR |= Led_Verde_H;
    	PRINTF("LED VERDE OFF\r\n");
    	Delay_MS(500);
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
