/*
 * mytools.h
 *
 *  Created on: Dec 2, 2020
 *      Author: ck
 */

#ifndef SRC_MYTOOLS_H_
#define SRC_MYTOOLS_H_

#include <stdio.h>
#include <stdlib.h>

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define IMAGE_ROWS 16
#define IMAGE_COLS 16
#define CHANNELS 3
#define WINDOW_SIZE 5
#define NUM_POINT 1
typedef unsigned char Mat;


Mat** Preprocessing(Mat image[IMAGE_ROWS][IMAGE_COLS][CHANNELS]);
Mat** Harris(Mat** preImage);
Mat* CalculateDifference(Mat** preImage,Mat** prePt,Mat** nextImage,Mat** nextPt);

#endif /* SRC_MYTOOLS_H_ */
