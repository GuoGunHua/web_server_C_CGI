//
//  Debug.h
//  C_test
//
//  Created by admin on 15/5/14.
//  Copyright (c) 2015年 admin. All rights reserved.
//

#ifndef __C_test__Debug__
#define __C_test__Debug__

#include <stdio.h>

#define __DEBUG__

#ifdef __DEBUG__
    #define DEBUG(format,...) printf("File: "__FILE__", Line: %05d: "format"\n", __LINE__, ##__VA_ARGS__)
#else
    #define DEBUG(format,...)
#endif


#define FILE_PATH /Users/admin/Desktop/C_test/C_test



#endif /* defined(__C_test__Debug__) */
