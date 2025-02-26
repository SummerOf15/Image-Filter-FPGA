/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "mytools.h"
#include "xtmrctr.h"
#include "xparameters.h"

#define TMRCTR_DEVICE_ID	XPAR_AXI_TIMER_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_INTC_0_DEVICE_ID

/*
 * The following constant determines which timer counter of the device that is
 * used for this example, there are currently 2 timer counters in a device
 * and this example uses the first one, 0, the timer numbers are 0 based
 */
#define TIMER_CNTR_0	 0

/*
 * The following constant is used to set the reset value of the timer counter,
 * making this number larger reduces the amount of time this example consumes
 * because it is the value the timer counter is loaded with when it is started
 */
#define RESET_VALUE	 0xF0000000



int main()
{
	init_platform();
	int Status;
	XTmrCtr TimerCounterInst;   /* The instance of the Timer Counter */
	print("run start\n\r");
	/*
	 * Initialize the timer counter so that it's ready to use,
	 * specify the device ID that is generated in xparameters.h
	 */
	Status = XTmrCtr_Initialize(&TimerCounterInst, TMRCTR_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test to ensure that the hardware was built
	 * correctly, use the 1st timer in the device (0)
	 */
	Status = XTmrCtr_SelfTest(&TimerCounterInst, TIMER_CNTR_0);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Enable the interrupt of the timer counter so interrupts will occur
	 * and use auto reload mode such that the timer counter will reload
	 * itself automatically and continue repeatedly, without this option
	 * it would expire once only
	 */
	XTmrCtr_SetOptions(&TimerCounterInst, TIMER_CNTR_0,XTC_AUTO_RELOAD_OPTION);


	XTmrCtr_Reset(&TimerCounterInst, TIMER_CNTR_0);

	/*
	 * Start the timer counter such that it's incrementing by default,
	 * then wait for it to timeout a number of times
	 */
	XTmrCtr_Start(&TimerCounterInst, TIMER_CNTR_0);
	u32 start=XTmrCtr_GetValue(&TimerCounterInst, TIMER_CNTR_0);
	xil_printf("start time %d ns\r\n",(start));

	Mat preImage[IMAGE_ROWS][IMAGE_COLS][CHANNELS];
	Mat nextImage[IMAGE_ROWS][IMAGE_COLS][CHANNELS];

	for(int i=0;i<IMAGE_ROWS;i++)
	{

		for (int j=0;j<IMAGE_COLS;j++)
		{
			for(int k=0;k<CHANNELS;k++)
			{
				if (i==j)
				{
					preImage[i][j][k]=255;
					nextImage[i][j][k]=0;
				}
				else if(i==j+1)
				{
					preImage[i][j][k]=0;
					nextImage[i][j][k]=255;
				}
				else
				{
					preImage[i][j][k]=0;
					nextImage[i][j][k]=0;
				}
			}
		}
	}


	u32 end=XTmrCtr_GetValue(&TimerCounterInst, TIMER_CNTR_0);
	xil_printf("generate image time %d ns\r\n",(end-start));
	start=end;

    Mat** preGray=Preprocessing(preImage);
    Mat** nextGray=Preprocessing(nextImage);

    end=XTmrCtr_GetValue(&TimerCounterInst, TIMER_CNTR_0);
    xil_printf("preprocessing time %d ns\r\n",(end-start));
    start=end;

    Mat** prePt=Harris(preGray);
    Mat** nextPt=Harris(nextGray);

    end=XTmrCtr_GetValue(&TimerCounterInst, TIMER_CNTR_0);
    xil_printf("harris time %d ns\r\n",(end-start));
    start=end;

    Mat* matchingPoint=CalculateDifference(preGray,prePt,nextGray,nextPt);

    end=XTmrCtr_GetValue(&TimerCounterInst, TIMER_CNTR_0);
    xil_printf("find matching time %d ns\r\n",(end-start));
    start=end;

    Mat translation[2]; //translation in x and y direction
    translation[0]=nextPt[matchingPoint[1]][0]-prePt[matchingPoint[0]][0];
    translation[1]=nextPt[matchingPoint[1]][1]-prePt[matchingPoint[0]][1];

	end=XTmrCtr_GetValue(&TimerCounterInst, TIMER_CNTR_0);
	xil_printf("calculate translation time %d ns\r\n",end-start);

	xil_printf("x=%d,y=%d\r\n",(int)translation[0],(int)translation[1]);
	free(preGray);
	free(nextGray);
	XTmrCtr_Stop(&TimerCounterInst, TIMER_CNTR_0);
    cleanup_platform();
    return 0;
}
