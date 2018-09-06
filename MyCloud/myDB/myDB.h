#ifndef _MYDB_H_
#define _MYDB_H_
#include <mysql/mysql.h>
#include <string>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

class MyDB{
public:
    MyDB();
    ~MyDB();
    int init();
    int exec_sql(const std::string sql);
    std::vector<std::string> get_res();
private:
    MYSQL* mysql_;
    MYSQL_ROW row_;
    MYSQL_RES* result_;
    std::vector<std::string> res_;
};//end class MyDB 

#endif
