//
//  save_Data.h
//  C_test
//
//  Created by admin on 15/7/29.
//  Copyright (c) 2015年 admin. All rights reserved.
//

#ifndef __C_test__save_Data__
#define __C_test__save_Data__

#include <stdio.h>

struct data
{
    char userId[20];
    char userName[50];
    
    char state[20];
    int is_cheng;
    
    int month;
    int date;
    int hours;
    int minte;
    int seconds;
    
}typedef Data_struct;


int save_data(char *data);



#endif /* defined(__C_test__save_Data__) */


//char state[20] = {0};
//char month[5] = {0};
//char date[5] = {0};
//char hours[5] = {0};
//char minte[5] = {0};
//char seconds[5] = {0};