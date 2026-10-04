/*
 * AUTOR 		: UGB_Dev
 * DATE  		: 04/10/2026
 * VERSION 		: V1.0.0
 * DESCRIPTION	: GPIO DISPLAY COUNT: Generacion de un conteo de 0 a 99 usando un
 *                dos display de 7 segmentos y multiplexacion
 */

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"

#define Q1_H	(GPIO4 -> PDOR |= GPIO_PDOR_PDO0_MASK)
#define Q1_L	(GPIO4 -> PDOR &= ~GPIO_PDOR_PDO0_MASK)
#define Q2_H	(GPIO4 -> PDOR |= GPIO_PDOR_PDO6_MASK)
#define Q2_L	(GPIO4 -> PDOR &= ~GPIO_PDOR_PDO6_MASK)
#define A_H		(GPIO0 -> PDOR |= GPIO_PDOR_PDO24_MASK)
#define A_L		(GPIO0 -> PDOR &= ~GPIO_PDOR_PDO24_MASK)
#define B_H		(GPIO4 -> PDOR |= GPIO_PDOR_PDO20_MASK)
#define B_L		(GPIO4 -> PDOR &= ~GPIO_PDOR_PDO20_MASK)
#define C_H		(GPIO4 -> PDOR |= GPIO_PDOR_PDO23_MASK)
#define C_L		(GPIO4 -> PDOR &= ~GPIO_PDOR_PDO23_MASK)
#define D_H		(GPIO0 -> PDOR |= GPIO_PDOR_PDO21_MASK)
#define D_L		(GPIO0 -> PDOR &= ~GPIO_PDOR_PDO21_MASK)
#define E_H		(GPIO0 -> PDOR |= GPIO_PDOR_PDO15_MASK)
#define E_L		(GPIO0 -> PDOR &= ~GPIO_PDOR_PDO15_MASK)
#define F_H		(GPIO3 -> PDOR |= GPIO_PDOR_PDO1_MASK)
#define F_L		(GPIO3 -> PDOR &= ~GPIO_PDOR_PDO1_MASK)
#define G_H		(GPIO2 -> PDOR |= GPIO_PDOR_PDO3_MASK)
#define G_L		(GPIO2 -> PDOR &= ~GPIO_PDOR_PDO3_MASK)

#define CERO	A_H; B_H; C_H; D_H; E_H; F_H; G_L;
#define UNO 	A_L; B_H; C_H; D_L; E_L; F_L; G_L;
#define DOS 	A_H; B_H; C_L; D_H; E_H; F_L; G_H;
#define TRES 	A_H; B_H; C_H; D_H; E_L; F_L; G_H;
#define CUATRO 	A_L; B_H; C_H; D_L; E_L; F_H; G_H;
#define CINCO 	A_H; B_L; C_H; D_H; E_L; F_H; G_H;
#define SEIS 	A_H; B_L; C_H; D_H; E_H; F_H; G_H;
#define SIETE 	A_H; B_H; C_H; D_L; E_L; F_L; G_L;
#define OCHO 	A_H; B_H; C_H; D_H; E_H; F_H; G_H;
#define NUEVE 	A_H; B_H; C_H; D_L; E_L; F_H; G_H;
#define Unidad  0
#define Decena  1

uint8_t Tick_us = 0;
uint32_t Tick_ms = 0;

void Delay_Init(void);
void Delay_MS(uint32_t ms);
void Imprimir_Numero(uint8_t Num, uint8_t UD);

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
    						 SYSCON_AHBCLKCTRL0_GPIO2_MASK|
							 SYSCON_AHBCLKCTRL0_GPIO3_MASK|
							 SYSCON_AHBCLKCTRL0_GPIO4_MASK;

	/* PASO 2: CONFIGURARACION DE LOS PINES COMO ENTRADA Y/O SALIDA */
   	GPIO0 -> PDDR |= GPIO_PDDR_PDD15_MASK | GPIO_PDDR_PDD21_MASK | GPIO_PDDR_PDD24_MASK; // GPIO0_15, GPIO0_21 Y GPIO0_24 COMO SALIDA
	GPIO2 -> PDDR |= GPIO_PDDR_PDD3_MASK; // GPIO2_3 COMO SALIDA
	GPIO3 -> PDDR |= GPIO_PDDR_PDD1_MASK; // GPIO3_1 COMO SALIDA
	GPIO4 -> PDDR |= GPIO_PDDR_PDD0_MASK | GPIO_PDDR_PDD6_MASK | GPIO_PDDR_PDD20_MASK | GPIO_PDDR_PDD23_MASK;

	PRINTF("CONFIGURACION DEL PERIFERICOS FINALIZADA ...... \r\n");

	uint8_t count=0;
	uint8_t U=0, D=0;

    while(1) {
    	U = count%10;
    	D = count/10;

    	for(uint8_t i=0; i<60; i++){ // MULTIPLEXEA EN 1 SEGUNDO
    		Imprimir_Numero(U, Unidad);
    		Imprimir_Numero(D, Decena);
    	}

    	if(count == 99) count = 0;
    	else count++;
    }
}

void Imprimir_Numero(uint8_t Num, uint8_t UD){
	switch(Num){
		case 0:		CERO;	break;
		case 1:		UNO;	break;
		case 2:		DOS;	break;
		case 3:		TRES;	break;
		case 4:		CUATRO;	break;
		case 5:		CINCO;	break;
		case 6:		SEIS;	break;
		case 7:		SIETE;	break;
		case 8:		OCHO;	break;
		case 9:		NUEVE;	break;
	}

	if(UD == Unidad){ Q1_L; Q2_H;}
	else{ Q1_H; Q2_L;}
	Delay_MS(8);
}

/* FUNCIONES PARA GENERAR DELAY POR SYSTICK*/
void Delay_Init(void){
	SysTick -> CTRL = SysTick_CTRL_CLKSOURCE_Msk; // FUENTE DE RELOJ PRINCIPAL (MAIN = 150 MHz)

	Tick_us = SystemCoreClock / 1000000;
	Tick_ms = Tick_us*1000 -1;
}

void Delay_MS(uint32_t ms){
	for( ; ms>0; ms--){
		SysTick -> LOAD = Tick_ms; // 150MHz/Fbase = 150MHz/1000 = 150K Tick = 150K Tick/150MHz = 1ms por cada Tick
		SysTick -> VAL = 0;
		SysTick -> CTRL |= SysTick_CTRL_ENABLE_Msk; // HABILITA SYSTICK
		while(!(SysTick -> CTRL & SysTick_CTRL_COUNTFLAG_Msk)); // ESPERA HASTA QUE VAL SEA CERO
		SysTick -> CTRL &= ~SysTick_CTRL_ENABLE_Msk; // DESHABILITA SYSTICK
	}
}
