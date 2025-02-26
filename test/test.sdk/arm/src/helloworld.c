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
#include "xtime_l.h"

int main()
{
	init_platform();
	XTime start, end;

	print("start running");

	XTime_GetTime(&start);
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

	XTime_GetTime(&end);
	float t=(end - start)*1.0 / (COUNTS_PER_SECOND/1000);
	printf("read image time %f ms.\r\n",t);

    XTime_GetTime(&start);
    Mat** preGray=Preprocessing(preImage);
    Mat** nextGray=Preprocessing(nextImage);

    XTime_GetTime(&end);
    t=(end - start)*1.0 / (COUNTS_PER_SECOND/1000);
    printf("preprocessing image time %f ms.\r\n",t);

    XTime_GetTime(&start);
    Mat** prePt=Harris(preGray);
    Mat** nextPt=Harris(nextGray);

    XTime_GetTime(&end);
    t=(end - start) *1.0 / (COUNTS_PER_SECOND/1000);
    printf("harris time %f ms.\r\n",t);

    XTime_GetTime(&start);
    Mat* matchingPoint=CalculateDifference(preGray,prePt,nextGray,nextPt);

    XTime_GetTime(&end);
    t=(end - start) *1.0 / (COUNTS_PER_SECOND/1000);
    printf("matching time %f ms.\r\n",t);

    XTime_GetTime(&start);
    Mat translation[2]; //translation in x and y direction
    translation[0]=nextPt[matchingPoint[1]][0]-prePt[matchingPoint[0]][0];
    translation[1]=nextPt[matchingPoint[1]][1]-prePt[matchingPoint[0]][1];

    XTime_GetTime(&end);
    t=(end - start) *1.0 / (COUNTS_PER_SECOND/1000);
    printf("calculate translation time %f ms.\r\n",t);

	printf("x=%d,y=%d\n",(int)translation[0],(int)translation[1]);

	free(preGray);
	free(nextGray);
    cleanup_platform();
    return 0;
}
