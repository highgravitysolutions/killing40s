 /*******************************************************************************
* File Name: main.c
*
* Version: 1.0
*
* Description:
*   Enumerates as a Virtual Com port.  Receives data from hyper terminal, then 
*   send received data backward. LCD shows the Line settings.
*   
*  To test project:
*   1. Build the project and program the hex file on to the target device.
*   2. Select 3.3V in SW3 and plug-in power to the CY8CKIT-001
*   3. Connect USB cable from the computer to the CY8CKIT-001.
*   4. Select the USB_UART.inf file from the project directory, as the driver 
*      for this example once Windows asks for it.
*   5. Open "Device Manager" and note the COM port number for "Example Project"
*      device.
*   6. Open "HyperTerminal" application and make new connection to noted COM port
*   7. Type the message, observe echo data received.
*
* Related Document:
*  Universal Serial Bus Specification Revision 2.0 
*  Universal Serial Bus Class Definitions for Communications Devices 
*  Revision 1.2
*
********************************************************************************
* Copyright 2012, Cypress Semiconductor Corporation. All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

#include <device.h>
#include "stdio.h"

#if defined (__GNUC__)
    /* Add an explicit reference to the floating point printf library */
    /* to allow the usage of floating point conversion specifiers. */
    /* This is not linked in by default with the newlib-nano library. */
    asm (".global _printf_float");
#endif

/* The size of the buffer is equal to maximum packet size of the 
*  IN and OUT bulk endpoints. 
*/
#define BUFFER_LEN  64u
#define NUM_SAMPLES 50

char8 *parity[] = { "None", "Odd", "Even", "Mark", "Space" };
char8 *stop[] = { "1", "1.5", "2" };

/* Function that encapsulates the process of writing text strings to USBUART */
void PrintToUSBUART(char8 * outText);
uint8 convertToOunces(uint16 calibration, uint16 counts);

void main()
{
    uint16 count;
    uint8 buffer[BUFFER_LEN];
    char8 lineStr[20];
    uint8 state;
     uint16 adcReading = 0u;
    //uint16 adc_buffer[NUM_SAMPLES];
    uint32 adc_buffer = 0u;
    float volts = 0;
    uint8 ounces = 40; 
    /* Enable Global Interrupts */
    CyGlobalIntEnable;                        
    //ADC_SAR_1_Start();
    adc_1_Start();
    /* Start USBFS Operation with 3V operation */
    USBUART_1_Start(0u, USBUART_1_5V_OPERATION);
    /* Starts PGA component */
    //PGA_1_Start();
//    PGA_Inv_1_Start();
    /* Start LCD */
    LCD_Start();
    LCD_PrintString("USB-UART example");
    PGA_1_Start();
    /* Wait for Device to enumerate */
    while(!USBUART_1_GetConfiguration());

    /* Enumeration is done, enable OUT endpoint for receive data from Host */
    USBUART_1_CDC_Init();
    
    char add[50];
    adc_1_StartConvert();
    /* Main Loop: */
    for(;;)
    {
        //PrintToUSBUART("Please choose the channel (0-7) \n\r");
        //if(USBUART_1_DataIsReady() != 0u)               /* Check for input data from PC */
        //{   
        //ADC_SAR_1_StartConvert();
        
        //ADC_SAR_1_IsEndConversion(ADC_SAR_1_WAIT_FOR_RESULT);
        //adcReading = ADC_SAR_1_GetResult16(); 
        int i = 0;
        adc_buffer = 0;
        for(i = 0; i < NUM_SAMPLES; i++)
        {
            adc_buffer += adc_1_Read16();
        }
        adcReading = adc_buffer/NUM_SAMPLES;
        ounces = convertToOunces(100,adcReading);
        //volts = adc_1_CountsTo_Volts(adcReading);
        sprintf(add,"counts: %hu\r\n",adcReading);
        PrintToUSBUART(add);
        sprintf(add,"ounces: %i\r\n",ounces);
        PrintToUSBUART(add);
        
        CyDelay(200);
       // }
    }   
}

uint8 convertToOunces(uint16 calibration,uint16 counts)
{
    //take initial calibration value as 40 ounces
    //stored 0 ounce value
    uint16 empty = 6500;
    uint16 full = 6417;
    
    float current = 40 - ((counts - full)/2); 
    
    return (uint8)current;
}

//Calibrate to initial full 40 value
uint16 calibrateTo40()
{
    uint16 adcReading = 0u;
    //uint16 adc_buffer[NUM_SAMPLES];
    uint32 adc_buffer = 0u;
    int i = 0;
    adc_buffer = 0;
    for(i = 0; i < NUM_SAMPLES; i++)
    {
        adc_buffer += adc_1_Read16();
    }
    adcReading = adc_buffer/NUM_SAMPLES;
    
    return adcReading;
}

void PrintToUSBUART(char8 * outText)
{
    /* Wait till the CDC device is ready before sending data */
    while(USBUART_1_CDCIsReady() == 0u);
    /* Send strlen number of characters of wrBuffer to USBUART */
    USBUART_1_PutData((uint8 *)outText, strlen(outText));
}

/* [] END OF FILE */
