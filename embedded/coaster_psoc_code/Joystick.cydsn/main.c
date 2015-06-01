/*  --------------------------------------------------------------------------
* Copyright 2010, Cypress Semiconductor Corporation.
*
* This software is owned by Cypress Semiconductor Corporation (Cypress)
* and is protected by and subject to worldwide patent protection (United
* States and foreign), United States copyright laws and international
* treaty provisions. Cypress hereby grants to licensee a personal,
* non-exclusive, non-transferable license to copy, use, modify, create
* derivative works of, and compile the Cypress Source Code and derivative
* works for the sole purpose of creating custom software in support of
* licensee product to be used only in conjunction with a Cypress integrated
* circuit as specified in the applicable agreement. Any reproduction,
* modification, translation, compilation, or representation of this
* software except as specified above is prohibited without the express
* written permission of Cypress.
* 
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND,EXPRESS OR IMPLIED,
* WITH REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the
* materials described herein. Cypress does not assume any liability arising
* out of the application or use of any product or circuit described herein.
* Cypress does not authorize its products for use as critical components in
* life-support systems where a malfunction or failure may reasonably be
* expected to result in significant injury to the user. The inclusion of
* Cypress' product in a life-support systems application implies that the
* manufacturer assumes all risk of such use and in doing so indemnifies
* Cypress against all charges.
* 
* Use may be limited by and subject to the applicable Cypress software
* license agreement.
* -----------------------------------------------------------------------------
* Copyright (c) Cypress MicroSystems 2000-2003. All Rights Reserved.
*
*****************************************************************************
*  Project Name: Joystick
*  Project Revision: 1.00
*  Software Version: PSoC Creator 2.0
*  Device Tested: CY8C3866AXI
*  Compilers Tested: Keil , ARM GCC
*  Related Hardware: CY8CKIT-001
*****************************************************************************
***************************************************************************** */

/* Project Description:
 * This project will create a simple USB HID joystick that can be used in PC gaming applications.
***************************************************************************** */

#include <device.h>
#include <stdio.h>

void StartUp (void); 		/* Function prototype for component startup API */
void ReadJoystick (void);   /* Function prototype for reading and altering data for joystick information */
void ReadButtons (void);    /* Function prototype for reading and altering data for button information */

static uint16 X_Axis=0, Y_Axis=0;
static int16 X_Data, Y_Data;
static int8 Joystick_Data[3] = {0, 0, 0}; /*[0] = X-Axis, [1] = Y-Axis, [2] = Buttons */
static unsigned char Buttons;

void main()
{
    StartUp(); 								/* Calls the proper start API for all the components */
	
	for(;;)
    {
    	while(!USBFS_1_bGetEPAckState(1)); 	/* Wait for ACK before loading data */
		ReadJoystick();						/* Calls function to read joystick movement */
		ReadButtons();						/* Calls function to monitor button presses */
		
		Joystick_Data[0] = X_Data;		
		Joystick_Data[1] = Y_Data;
		Joystick_Data[2] = Buttons;
		
		USBFS_1_LoadInEP(1, (uint8 *)Joystick_Data, 3); /* Load latest mouse data into EP1 and send to PC */
    }
}

void StartUp (void)
{
	CYGlobalIntEnable;           					/* Enable Global interrupts */
    ADC_DelSig_1_Start();        					/* Initialize ADC */
    CyIntEnable(ADC_DelSig_1_IRQ__INTC_NUMBER ); 	/* Enables ADC interrupts */
	AMux_1_Start( );         						/* Reset all channels */
    AMux_1_Select(0);        						/* Connect channel 0 */
	LCD_Char_1_Start(); 							/* Calls the start API for the Character LCD */
	LCD_Char_1_Position(0,0);						/* Moves LCD cursor to Row 0, Column 0 */
	LCD_Char_1_PrintString("PSoC 3 USB HID");
	LCD_Char_1_Position(1,0);
	LCD_Char_1_PrintString("Joystick Demo");
	USBFS_1_Start(0, USBFS_1_DWR_VDDD_OPERATION);	/* Start USBFS operation/device 0 and with 5V operation */
	while(!USBFS_1_bGetConfiguration());			/* Wait for Device to enumerate */
    USBFS_1_LoadInEP(1, (uint8 *)Joystick_Data, 3); /* Loads an inital value into EP1 and sends it out to the PC */
}

void ReadJoystick (void)
{
	AMux_1_Select(0);											/* Connects AMUX to channel 0 to connect ADC to joystick X-axis */  		
	ADC_DelSig_1_StartConvert();								/* End ADC conversion */
	ADC_DelSig_1_IsEndConversion(ADC_DelSig_1_WAIT_FOR_RESULT); /* Wait for ADC reading to complete */
    ADC_DelSig_1_StopConvert();									/* End ADC conversions */
	X_Axis = ADC_DelSig_1_GetResult16();						/* Get ADC reading and store in variable X_Axis */
	
	AMux_1_Select(1);											/* Connects AMUX to channel 1 to connect ADC to joystick Y-axis */
	ADC_DelSig_1_StartConvert();								/* Start ADC conversions */
	ADC_DelSig_1_IsEndConversion(ADC_DelSig_1_WAIT_FOR_RESULT); /* Wait for ADC reading to complete */
    ADC_DelSig_1_StopConvert();									/* End ADC conversions */
	Y_Axis = ADC_DelSig_1_GetResult16();						/* Get ADC reading and store in variable Y_Axis */
	
	X_Data = X_Axis - 127;										/* Adjust axis to center joystick */
	Y_Data = Y_Axis - 127;
	
	if(X_Data > 127)										
	X_Data = 127;
	if(Y_Data > 127)
	Y_Data = 127;
	if(X_Data < -127)
	X_Data = -127;
	if(Y_Data< -127)
	Y_Data = -127;
	
	Y_Data = Y_Data * -1;										/* Inverts Y-Axis for PC direction formatting */
}

void ReadButtons (void)
{
	if(Thumb_Button_Read() != 0)								/* Detect if button was pressed	*/
	Buttons |= 0x01;											/* If pressed, mask bit to indicate button press */
	else
	Buttons &= ~0x01;											/* If released, clear bit */
	 
	if(Button_A_Read() != 0)
	Buttons |= 0x02;
	else
	Buttons &= ~0x02;
	
	if(Button_B_Read() != 0)
	Buttons |= 0x04;
	else
	Buttons &= ~0x04;
	
	if(Button_C_Read() != 0)
	Buttons |= 0x08;
	else
	Buttons &= ~0x08;
	
	if(Button_D_Read() != 0)
	Buttons |= 0x10;
	else
	Buttons &= ~0x10;
}

/* End of File */

