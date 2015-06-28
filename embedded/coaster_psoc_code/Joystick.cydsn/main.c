/*******************************************************************************
* File Name: main.c
*
* Version: 2.1
*
* Description:
*  This project enumarates on the computer as a 3-button  mouse. 
*  When the code is run, mouse cursor moves from the right to the left. 
*
* Refer to description on schematic for test procedure.
*
* Related Document:
*  Device Class Definition for Human Interface Devices (HID)
*  Firmware Specification- Version 1.11
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

#define MOUSE_ENDPOINT      (0x01u)
#define MOUSE_DATA_LEN      (0x03u)
#define CURSOR_STEP         (0x05u)     /* Step size */

/* Mouse packet array: button, X, Y */
uint8 mouseData[MOUSE_DATA_LEN] = {0u, 0u, 0u}; 
uint8 bSNstring[16]={0x0Eu, 0x03u, 'F', 0u, 'W', 0u, 'S', 0u, 'N', 0u, '0', 0u, '1', 0u};

uint8 FortyToByte(uint8 oz);

void main()
{
    uint8 counter = 0u;
    
    /* Enable Global Interrupts */
    CyGlobalIntEnable;
    
    /* Set user defined Serial Number string descriptor */
    //USBFS_1_SerialNumString(&bSNstring[0u]);
    
    /* Start USBFS device 0 with 3V operation */
    USBFS_1_Start(0u, USBFS_1_5V_OPERATION); 

    /* Wait for Device to enumerate */
    while(!USBFS_1_GetConfiguration());

    /* Enumeration is done, load endpoint 1. Do not toggle the first time. */
    USBFS_1_LoadInEP(MOUSE_ENDPOINT, mouseData, MOUSE_DATA_LEN);
    
    while(1)
    {
        /* Wait for ACK before loading data */
        while(!USBFS_1_GetEPAckState(MOUSE_ENDPOINT));
        
        /* ACK has occurred, load the endpoint and toggle the data bit */
        USBFS_1_LoadInEP(1u, mouseData, MOUSE_DATA_LEN);
        
        /* When our counts hits 128 */
        if(counter == 128u)
        {
            /* Start moving the mouse to the right */
            mouseData[1u] = 40u;
            //mouseData[2u] |= 0x01;
            //mouseData[2u] &= ~0x01;
            
            mouseData[2u] |= 0x01;
            mouseData[2u] |= 0x02;
            mouseData[2u] |= 0x04;
            //mouseData[2u] |= 0x03;
            //mouseData[2u] &= ~0x03;
            //mouseData[2u] |= 0x04;
            //mouseData[2u] &= ~0x04;
            //mouseData[2u] |= 0x05;
            //mouseData[2u] &= ~0x05;
        }
        else if(counter == 255u)
        {
            /* Start moving the mouse to the left */
            //mouseData[1u] = (uint8)-(int8)CURSOR_STEP;
            //mouseData[2u] &= ~0x01;
            //mouseData[2u] &= ~0x02;
            mouseData[2u] &= ~0x04;
            mouseData[1u] = 0u;
        }
        //CyDelay(200);
        /*mouseData[2u] &= ~0x02;
        CyDelay(200);
        mouseData[2u] &= ~0x03;
        CyDelay(200);
        mouseData[2u] &= ~0x04;
        CyDelay(200);
        mouseData[2u] &= ~0x05;
        CyDelay(200);*/
        //}

        counter++;
    }
}

uint8 FortyToByte(uint8 oz)
{
    
}


/* [] END OF FILE */
