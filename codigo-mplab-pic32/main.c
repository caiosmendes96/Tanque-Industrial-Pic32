/*
 * ENGG53 - LABORATÓRIO INTEGRADO II
 * Integrantes: David Oliveira, David Ferrari, André Lino e Caio Sousa Mendes.
 * File:   main.c
 *
 */

//NOME LCD: TFT_TM022HDH26 ILI9341

#include <xc.h>

// Configurações do microcontrolador
#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_8

#pragma config DEBUG = ON               // Background Debugger Enable (Debugger is enabled)
#pragma config ICESEL = ICS_PGx1        // ICE/ICD Comm Channel Select (ICE EMUC1/EMUD1 pins shared with PGC1/PGD1)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)

// Definindo os LEDs nos pinos RA0, RA1 e RA2.
#define BUZZER LATAbits.LATA0
#define LED1   LATAbits.LATA1
#define LED2   LATAbits.LATA2
#define LED3   LATAbits.LATA3

#define SYS_FREQ 80000000L // Defina o clock do sistema para 80 MHz

// Definindo os sensores e o alarme nos pinos RF0, RF1 e RF2.
#define BOTAO_ALARME PORTAbits.RA5
#define SENSOR1      PORTAbits.RA6
#define SENSOR2      PORTAbits.RA9
 
#define SHORT_DELAY (100)
#define VOLTAGE_THRESHOLD 512         // Limite de tensão para acionar o LED2 e o buzzer

volatile unsigned int adc_value = 0;

void delayMs(unsigned t) {
    unsigned long startTime = _CP0_GET_COUNT(); // Obtenha o valor inicial do Core Timer
    unsigned long delayTicks = (SYS_FREQ / 2) / 1000; // 1 ms em ticks do Core Timer

    while (t--) {
        while (_CP0_GET_COUNT() - startTime < delayTicks); // Aguarde 1 ms
        startTime += delayTicks; // Atualize o valor inicial para o próximo atraso
    }
}

void inicializarADC(void) {
    AD1CON1 = 0x00E0;       // Auto-convert mode
    AD1CHS = 0;       // Selecionar o pino ADC
    AD1CSSL = 0;
    AD1CON3 = 0x1F02;       // Tempo de amostragem manual
    AD1CON2 = 0;
    AD1CON1bits.ADON = 1;   // Ligar o ADC
}

int lerADC() {
    // Iniciar a amostragem e conversão do ADC
        AD1CON1bits.SAMP = 1;  // Iniciar amostragem
        delayMs(1);
        AD1CON1bits.SAMP = 0;  // Iniciar conversão

        while (!AD1CON1bits.DONE);  // Aguardar a conversão terminar
        int adc_value = ADC1BUF0;  // Ler valor ADC
        return adc_value;
}

main(void) {
    inicializarADC();
    int sensorAtivo2 = 0;
    int sensorAtivo1 = 0;
    int botaoAlarme = 0;
    DDPCONbits.JTAGEN = 0;
    TRISA = 11110000;    TRISB = 1;
    T1CON = 0x8030; // TMR1 on, prescale 1:256 PB
    BUZZER = 0;
    LED1 = 0;
    LED2 = 0;
    LED3 = 0;
    
    while(1) {
      
        
        if(SENSOR2 == 0 && sensorAtivo2 == 0) {
            
            LED2 = 1;
            BUZZER = 1;
            sensorAtivo2 = 1;
            delayMs(SHORT_DELAY);
            while(SENSOR2 == 0){}
        }
        
        if(SENSOR2 == 0 && sensorAtivo2 == 1){
            LED2 = 0;
            BUZZER = 0;
            sensorAtivo2 = 0;
            delayMs(SHORT_DELAY);
            while(SENSOR2 == 0){}
        }
      if(SENSOR1 == 0 && sensorAtivo1 == 0) {
            
            LED1 = 1;
            BUZZER = 1;
            sensorAtivo1 = 1;
            delayMs(SHORT_DELAY);
            while(SENSOR1 == 0){}
        }
        
        if(SENSOR1 == 0 && sensorAtivo1 == 1){
            LED1 = 0;
            BUZZER = 0;
            sensorAtivo1 = 0;
            delayMs(SHORT_DELAY);
            while(SENSOR1 == 0){}
        }
        if(BOTAO_ALARME == 0) {
            BUZZER = 0;
        }
         // Iniciar a amostragem e conversão do ADC
        AD1CON1bits.SAMP = 1;  // Iniciar amostragem
        delayMs(1);  // Aguardar 1 ms
        AD1CON1bits.SAMP = 0;  // Iniciar conversão

        while (!AD1CON1bits.DONE);  // Aguardar a conversão terminar
        adc_value = ADC1BUF0;  // Ler valor ADC.
        if (adc_value > VOLTAGE_THRESHOLD && botaoAlarme == 0) {
            LED3 = 1;
            BUZZER = 1;
            botaoAlarme = 1;
        }
        if(adc_value < VOLTAGE_THRESHOLD && botaoAlarme == 1){
            LED3 = 0;
            BUZZER = 0;
            botaoAlarme = 0;
        }
    }
    
}