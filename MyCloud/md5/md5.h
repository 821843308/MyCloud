#ifndef _MD5_H_
#define _MD5_H_


#include <fstream>
#include <iostream>

const char HEX[16]={
    '0','1','2','3',
    '4','5','6','7',
    '8','9','a','b',
    'c','d','e','f'
};
//! F, G, H and I 基本MD5函数
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

//! 将x循环左移n位
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))
 
/* //! 4轮运算中FF(第1轮), GG(第2轮), HH(第3轮), and II(第4轮)转换 */
/* void FF(unsigned int &a,unsigned int b,unsigned int c,unsigned int d,unsigned int x,unsigned int s,unsigned ac) */
/* { */
/*     a += F ((b), (c), (d)) + (x) + (unsigned  int)(ac); */
/*     a = ROTATE_LEFT ((a), (s)); */
/*     a+= (b); */
/* } */
 
/* void GG(unsigned int &a,unsigned int b,unsigned int c,unsigned int d,unsigned int x,unsigned int s,unsigned ac) */
/* { */
/*     a += G ((b), (c), (d)) + (x) + (unsigned  int)(ac); */
/*     a = ROTATE_LEFT ((a), (s)); */
/*     a += (b); */
/* } */
 
/* void HH(unsigned int &a,unsigned int b,unsigned int c,unsigned int d,unsigned int x,unsigned int s,unsigned ac) */
/* { */
/*     a += H ((b), (c), (d)) + (x) + (unsigned  int)(ac); */
/*     a = ROTATE_LEFT ((a), (s)); */
/*     a+= (b); */
/* } */
 
/* void II(unsigned int &a,unsigned int b,unsigned int c,unsigned int d,unsigned int x,unsigned int s,unsigned ac) */
/* { */
/*     a += I ((b), (c), (d)) + (x) + (unsigned  int)(ac); */
/*     a = ROTATE_LEFT ((a), (s)); */
/*     a += (b); */
/* } */


class MD5{
public:
    MD5();
    //处理子符串
    int handle_str(const std::string& input);
    //处理文件
    int handle_file(std::ifstream& in);
    //返回文件md5算法生成的的string
    const std::string& md5_string();
private:
    //为文件做填充 sz:buffer_中的数据字节数
    void fill(const unsigned int& sz);
    //填充长度
    void fill_len();
    //对一组明文进行进行四轮运算
    void transform();
    //初始化state数组和count数组
    void init();
    //对明文进行分组16
    void decode( unsigned int *output, unsigned   char *input, unsigned int len );
private:
    unsigned int state_[4];//四个字符串A,B,C,D
    unsigned long count_;//填充长度,8字节
    unsigned char buffer_[64];//输入缓冲，暂存明文
    std::ifstream in_;//文件输入
    std::string result_;//存储文件处理后的结果
};//end class MD5 
#endif
