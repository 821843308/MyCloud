#include <iostream>
#include "md5.h"
int main()
{
    MD5 md5;
    md5.handle_str("你好陕西科技大学。");
    std::cout<<"f1: "<<md5.md5_string()<<std::endl;

    md5.handle_str("你好新同学，欢饮来到陕西科技大学。");
    std::cout<<"f2: "<<md5.md5_string()<<std::endl;
    
    md5.handle_str("你好 陕西 科技 大学 。");
    std::cout<<"f3: "<<md5.md5_string()<<std::endl;

    md5.handle_str("你好陕西科技大学?");
    std::cout<<"f4: "<<md5.md5_string()<<std::endl;

    md5.handle_str("你好,陕西科技大学。");
    std::cout<<"f5: "<<md5.md5_string()<<std::endl;

    md5.handle_str("你好陕西      科技大学。");
    std::cout<<"f6: "<<md5.md5_string()<<std::endl;

    md5.handle_str("           你好陕西科技大学。");
    std::cout<<"f7: "<<md5.md5_string()<<std::endl;
    return 0;
}
