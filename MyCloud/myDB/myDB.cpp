#include "myDB.h"

MyDB::MyDB()
{
    mysql_=mysql_init(NULL);
    if(mysql_==NULL){
        perror("error:MyDB() mysql_init");
        exit(1);
    }
}

MyDB::~MyDB()
{
    if(mysql_ != NULL){
        mysql_close(mysql_);
    }
}


int MyDB::init()
{
    
    mysql_=mysql_real_connect(mysql_,"0.0.0.0","root","1234","iCloud",0,NULL,0);
    if(mysql_ == NULL)
    {
        perror("error:init() mysql_real_connect");
        return -1;
    }
    return 0;
}

int MyDB::exec_sql(const std::string sql)
{

    res_.resize(0);
    if(mysql_query(mysql_,sql.c_str())){
        perror("error:exec_sql() mysql_query");
        return -1;
    }else{
        //获取结果集
        result_=mysql_store_result(mysql_);
        if(result_ == NULL)
            return 0;
        while(row_=mysql_fetch_row(result_)){//获取字段数据，一行
            for(int i=0;i<mysql_num_fields(result_);++i){//获取字段个数
                res_.push_back(row_[i]);
            }
        }
    }
    return 0;
    /*     return -1; */
    /* }else{ */
    /*     //获取结果集 */
    /*     result_=mysql_store_result(mysql_); */
        
    /*     if(result_ == NULL) */
    /*         return 0; */
        /* for(int i=0;i<mysql_field_count(mysql_);++i){//多少行 */
        /*     row_=mysql_fetch_row(result_);//获取字段数据，一行 */
        /*     if(row_<=0){ */
        /*         break; */
        /*     } */
        /*     for(int j=0;j<mysql_num_fields(result_);++j){//获取字段个数 */
        /*         res_.push_back(row_[j]); */
        /*     } */
        /* } */

    /* } */
}

std::vector<std::string> MyDB::get_res()
{
    return res_;
}
