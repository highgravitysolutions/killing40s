/*******************************************************************************
* File Name: main.c
*
* Version: 40.0
*
* Description:
*   Reads from 2 pairs of 3-wire load cells to get a 40 oz value and maps these to 
*   hid output (5-bit number represented as 5 joystick buttons)
*
*   Currently sends adc and 40 value over usb uart for debugging
*
********************************************************************************
* High Gravity Solutions
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
#define NUM_SAMPLES 30
#define EMPTY 0
#define FULL 1

char8 *parity[] = { "None", "Odd", "Even", "Mark", "Space" };
char8 *stop[] = { "1", "1.5", "2" };

/* Function that encapsulates the process of writing text strings to USBUART */
void PrintToUSBUART(char8 * outText);
int8 convertToOunces(uint16 calibrate_full, uint16 calibrate_empty, uint16 counts);
uint16 calibrateTo40();

int main()
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
    uint8 button_state = 0;
    uint8 last_button_state = 2;
    uint16 empty_value = 500;
    uint16 full_value =  1500;
    /* Enable Global Interrupts */
    CyGlobalIntEnable;                        
    //ADC_SAR_1_Start();
    adc_1_Start();
    AMuxSeq_1_Start();
    /* Start USBFS Operation with 5V operation */
    USBUART_1_Start(0u, USBUART_1_5V_OPERATION);
    
    /* Wait for Device to enumerate */
    while(!USBUART_1_GetConfiguration());

    /* Enumeration is done, enable OUT endpoint for receive data from Host */
    USBUART_1_CDC_Init();
    
    char add[50];
    int i = 0;
    adc_1_StartConvert();
    /* Main Loop: */
    for(;;)
    { 
        button_state = button_Read();
        
        if(button_state != last_button_state)
        {
            if(button_state == 0)
                empty_value = calibrateTo40();  
            else
                full_value = calibrateTo40();
        }
        
        i = 0;
        adc_buffer = 0;
        for(i = 0; i < NUM_SAMPLES; i++)
        {
            adc_buffer += adc_1_Read16();
        }
        adcReading = adc_buffer/NUM_SAMPLES;
        ounces = convertToOunces(full_value,empty_value,adcReading);
        volts = adc_1_CountsTo_Volts(adcReading);
        sprintf(add,"counts: %hu\r\n",adcReading);
        PrintToUSBUART(add);
        //sprintf(add,"counts: %f\r\n",volts);
        //PrintToUSBUART(add);
        sprintf(add,"ounces: %i\r\n",ounces);
        PrintToUSBUART(add);
        
        //Not drinking...
        if(ounces >= 0)
        {
            //update ounce value
        }
        last_button_state = button_state;
        CyDelay(200);
       // }
    }   
}

int8 convertToOunces(uint16 calibrate_full, uint16 calibrate_empty, uint16 counts)
{
    //take initial calibration value as 40 ounces
    //stored 0 ounce value
    uint16 empty = 6500;
    uint16 full = 6417;
    
    //float current = 40 - ((counts - full)/2); 
    float current = 40 - ((counts - full)/((empty-full)/40));
    
    return (uint8)current;
}

//Calibrate to initial full 40 value on boot
uint16 calibrateTo40()
{
    uint16 adcReading = 0u;
    //uint16 adc_buffer[NUM_SAMPLES];
    uint32 adc_buffer_1 = 0u; //load cell pair one
    uint32 adc_buffer_2 = 0u; //load cell pair two
    int i = 0;
    //adc_buffer = 0;
    for(i = 0; i < NUM_SAMPLES; i++)
    {
        if(AMuxSeq_1_GetChannel() == 0)
            adc_buffer_1 += adc_1_Read16();
        else if(AMuxSeq_1_GetChannel() == 1)
            adc_buffer_2 += adc_1_Read16();
            
        AMuxSeq_1_Next();
    }
    
    adcReading = ((adc_buffer_2/NUM_SAMPLES)+(adc_buffer_1/NUM_SAMPLES))/2;
    
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
