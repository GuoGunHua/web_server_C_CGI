//
//  save_Data.c
//  C_test
//
//  Created by admin on 15/7/29.
//  Copyright (c) 2015年 admin. All rights reserved.
//

#include "save_data.h"
#include "standard_c_interface.h"
#include <stdio.h>
#include "sqlite3.h"


#define ISspace(x) isspace((int)(x))
int is_space(char c);
void CreateTable(void);
int save_to_db(Data_struct data);
int OpenDB(char *filePath);
int execSQL(char *sql);
void CreateTable(void);


sqlite3 *data_base = NULL;

/*
 //
 //对C标准中空白字符(空格' ',回车符(\r),换行符(\n),水平制表符(\t),垂直制表符(\v),换页符(\f))的理解
 */

int is_space(char c)
{
    switch (c)
    {
        case ' ':
            return 1;
            
        case '\r':
            return 1;
            
        case '\n':
            return 1;
            
        case '\t':
            return 1;
            
        case '\v':
            return 1;
            
        case '\f':
            return 1;
            
        default:
            break;
    }
    
    return 0;
}

/*
   存入数据库
*/
int save_to_db(Data_struct data)
{
//    "/Users/admin/Desktop/C_test/C_test/index.html"
    
    int result = OpenDB("./weibo.db");
    if (result == 0)
        return 0;
    
    
    char sql[256] = {0};
    sprintf(sql,"insert into weiboLogin(userId,state,chenged,month,date,hours,minte,seconds,userName) values('%s','%s',%d,%d,%d,%d,%d,%d,'%s')",data.userId,data.state,data.is_cheng,data.month,data.date,data.hours,data.minte,data.seconds,data.userName);
    
    DEBUG("%s",sql);
    result = execSQL(sql);
    if (result == 1)
    {
       sqlite3_close(data_base);
    }
    else
    {
        sqlite3_close(data_base);
    }
    return 0;
}

int OpenDB(char *filePath)
{
    int result = sqlite3_open(filePath,&data_base);
    if (result != SQLITE_OK)
    {
        printf("sqlite3_open 失败 \n");
        return 0;
    }
    
    CreateTable();
    return 1;
}

int execSQL(char *sql)
{
    char *err;
    if (sqlite3_exec(data_base,sql, NULL, NULL, &err) != SQLITE_OK)
    {
        sqlite3_close(data_base);
        printf("数据库操作失败 %s:%s",err,sql);
        return 0;
    }
    return 1;
}

void CreateTable(void)
{
    // 是在数据库每一次被创建的时候调用的
    execSQL("CREATE TABLE if not exists weiboLogin(ID integer primary key autoincrement,userId varchar(30),state varchar(30),chenged varchar(30),month integer,date integer,hours integer,minte integer,seconds intrger,userName varchar(50))");
}


//
int save_data(char *data)
{
    if (data == NULL || strlen(data)==0)
        return 0;
    DEBUG("%s",data);
    DEBUG("需要存储的信息 %s",data);
   // printf("%s",data);
   // GET /offline_7_29_16_38_11 HTTP/1.1
    
    char method[1024]={0};
    char state[20] = {0};
    char month[5] = {0};
    char date[5] = {0};
    char hours[5] = {0};
    char minte[5] = {0};
    char seconds[5] = {0};
    char chenged[5] = {0};
    char userId[20] = {0};
    char userName[50] = {0};
    
//    //拷贝第一个单词
//    int i=0,j=0;
//    while (!is_space(data[j]) && (i < sizeof(method) - 1))
//    {
//        method[i] = data[j];
//        i++; j++;
//    }
//    method[i] = '\0';
//    
//    if(strlen(method) == 0 || strcasecmp(method, "GET"))
//        return 0;
    
    
//    //拷贝第2 串，路径
//    i=0,j++;
//    while (!is_space(data[j]) && (i < sizeof(method) - 1))
//    {
//        method[i] = data[j];
//        i++; j++;
//    }
//    method[i] = '\0';
//    printf("%s",method);
    
    strcpy(method,data);
    
    if (strstr(method,"line") == NULL  || strlen(data) > 768)
        return 0;
    
    //提取 状态
    int i=0,j=1;
    while (method[j] != '_' && (i < sizeof(state) - 1) && method[j] != '\0')
    {
        state[i] = method[j];
        i++; j++;
    }
    state[i] = '\0';
    printf("state=%s ",state);
    
    //提取 月份
    i=0,j++;
    while (method[j] != '_' && (i < sizeof(month) - 1) && method[j] != '\0')
    {
        month[i] = method[j];
        i++; j++;
    }
    month[i] = '\0';
    printf("month=%s",month);
    
    //提取 日期
    i=0,j++;
    while (method[j] != '_' && (i < sizeof(date) - 1) && method[j] != '\0')
    {
        date[i] = method[j];
        i++; j++;
    }
    date[i] = '\0';
    printf("date=%s",date);
    
    //提取 小时
    i=0,j++;
    while (method[j] != '_' && (i < sizeof(hours) - 1) && method[j] != '\0')
    {
        hours[i] = method[j];
        i++; j++;
    }
    hours[i] = '\0';
    printf("hours=%s",hours);
    
    
    //提取 分钟
    i=0,j++;
    while (method[j] != '_' && (i < sizeof(minte) - 1) && method[j] != '\0')
    {
        minte[i] = method[j];
        i++; j++;
    }
    minte[i] = '\0';
    printf("minte=%s",minte);
    
    //提取 秒
    i=0,j++;
    while (method[j] != '_' && (i < sizeof(seconds) - 1) && method[j] != '\0')
    {
        seconds[i] = method[j];
        i++; j++;
    }
    seconds[i] = '\0';
    printf("seconds=%s",seconds);
    
    
    //提取 改变状态
    i=0,j++;
    while (method[j] != '_' && (i < sizeof(chenged) - 1) && method[j] != '\0')
    {
        chenged[i] = method[j];
        i++; j++;
    }
    chenged[i] = '\0';
    printf("chenged=%s",chenged);
    
    //提取 用户id
    i=0,j++;
    while (method[j] != '_' && (i < sizeof(userId) - 1) && method[j] != '\0')
    {
        userId[i] = method[j];
        i++; j++;
    }
    userId[i] = '\0';
    printf("userId=%s",userId);
    
    //提取 用户名
    i=0,j++;
    while (method[j] != '_' && (i < sizeof(userName) - 1) && method[j] != '\0')
    {
        userName[i] = method[j];
        i++; j++;
    }
    userName[i] = '\0';
    printf("userName=%s \n",userName);
    
    
    //保存数据
    Data_struct user_data;
    strcpy(user_data.state,state);
    user_data.month = atoi(month);
    user_data.date = atoi(date);
    user_data.hours = atoi(hours);
    user_data.minte = atoi(minte);
    user_data.seconds = atoi(seconds);
    user_data.is_cheng = atoi(chenged);
    
    strcpy(user_data.userId, userId);
    strcpy(user_data.userName,userName);
    
    if (strlen(state)<= 0 || strlen(userId)<= 0 || strlen(userName) <= 0)
        return 0;
    
    if (user_data.month > 12 || user_data.date > 31 || user_data.hours > 24 || user_data.minte > 60 || user_data.seconds > 60)
        return 0;
    
    save_to_db(user_data);
    return 0;
}

