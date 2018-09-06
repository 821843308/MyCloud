#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include<string.h>
#include "md5.h"

//! 4轮运算中FF(第1轮), GG(第2轮), HH(第3轮), and II(第4轮)转换
void FF(unsigned int &a,unsigned int b,unsigned int c,unsigned int d,unsigned int x,unsigned int s,unsigned ac)
{
    a += F ((b), (c), (d)) + (x) + (unsigned  int)(ac);
    a = ROTATE_LEFT ((a), (s));
    a+= (b);
}
 
void GG(unsigned int &a,unsigned int b,unsigned int c,unsigned int d,unsigned int x,unsigned int s,unsigned ac)
{
    a += G ((b), (c), (d)) + (x) + (unsigned  int)(ac);
    a = ROTATE_LEFT ((a), (s));
    a += (b);
}
 
void HH(unsigned int &a,unsigned int b,unsigned int c,unsigned int d,unsigned int x,unsigned int s,unsigned ac)
{
    a += H ((b), (c), (d)) + (x) + (unsigned  int)(ac);
    a = ROTATE_LEFT ((a), (s));
    a+= (b);
}
 
void II(unsigned int &a,unsigned int b,unsigned int c,unsigned int d,unsigned int x,unsigned int s,unsigned ac)
{
    a += I ((b), (c), (d)) + (x) + (unsigned  int)(ac);
    a = ROTATE_LEFT ((a), (s));
    a += (b);
}

MD5::MD5(){
    init();
}

int MD5::handle_str(const std::string& input)
{
    /* std::cout<<input.size()<<input<<std::endl; */   
    //初始化
    init();
    //从字符串读取数据
    size_t str_sz=input.size();
    size_t index=0,read_sz=0;
    while(index < str_sz){
        memset(buffer_,0x00,sizeof(buffer_));
        for(read_sz=0;read_sz<sizeof(buffer_);){
            buffer_[read_sz]=input[index];
            ++read_sz;
            ++index;
            if(index >= str_sz)
                break;
        }
        /* std::cout<<read_sz<<":"<<index<<std::endl; */
        count_+=read_sz*8;
        if(read_sz < sizeof(buffer_)){
              fill(read_sz);
        } 
        transform();
    }
    if(read_sz == sizeof(buffer_)){
        fill(0);
        transform();
    }
    //将计算出的md5值放入result中
    for(size_t i=0;i<4;++i){
        for(size_t j=0;j<4;++j){
            result_.push_back(HEX[((*((char*)&state_[i]+j))&(0xf0))>>4]);
            result_.push_back(HEX[((*((char*)&state_[i]+j))&(0x0f))]);
        }
    }
}

int MD5::handle_file(std::ifstream& in)
{
    if(!in){
        perror("error:in is NULL！");
        return -1;
    }
    //初始化
    init();
    //从文件读取数据
    size_t read_sz;
    while(!in.eof()){
      memset(buffer_,0x00,sizeof(buffer_));
      in.read((char*)buffer_,(std::streamsize)sizeof(buffer_));
      read_sz=(size_t)in.gcount();
      count_+=read_sz*8;
      if(read_sz < sizeof(buffer_)){
          fill(read_sz);
      } 
      transform();
    }
    //将计算出的md5值放入result中
    for(size_t i=0;i<4;++i){
        for(size_t j=0;j<4;++j){
            result_.push_back(HEX[((*((char*)&state_[i]+j))&(0xf0))>>4]);
            result_.push_back(HEX[((*((char*)&state_[i]+j))&(0x0f))]);
        }
    }
}

void MD5::init()
{
    count_=0;
    state_[0] = 0x67452301;
	state_[1] = 0xefcdab89;
	state_[2] = 0x98badcfe;
	state_[3] = 0x10325476;
    result_="";
}

void MD5::fill_len()
{
    char* start=(char*)&count_;
    unsigned int num=1;

    //填充开始位置在buffer_的55个字节后第一个
    for(unsigned int i=56;i<=63;++i){
        buffer_[i]=*start;
        ++start;
    }
}

void MD5::fill(const unsigned int& sz)
{
    unsigned int index = sz;//从buffer_中读取到的最后一个字节后开始填充
    char ch=1;

    //R=0时，需要补位，单补上一个512位的分组，因为还要加入最后64个位的字符串长度。
    //R<448时，则需要补位到448位，后面添加64位的字符串长度。
    //R>448时，除了补满这一分组外，还要再补上一个512位的分组后面添加64位的字符串长度。
    if(sz*8<448 && sz*8>0){
        buffer_[index]=ch<<7;//第一处填充1000 0000
        ++index;
        for(;index*8<448;++index){
            buffer_[index]=0;
        }
        fill_len();
    } else if(sz*8 >= 448){
        buffer_[index]=ch<<7;//第一处填充1000 0000
        ++index;
        for(;index*8<512;++index){
            buffer_[index]=0;
        }
        transform();
        for(index=0;index*8<448;++index){
            buffer_[index]=0;
        }
        fill_len();
    } else if(sz == 0){
        buffer_[0]=1<<7;
        for(index=1;index*8<448;++index){
            buffer_[index]=0;
        }
        fill_len();
   }
}


void MD5::decode( unsigned  int *output, unsigned char *input, unsigned int len )
{
    //将明文（char 数组）的每32位为一组存到一个32位的unsigned  int数组中，后面将会用来做运算
    unsigned int i, j;
    for( i=0, j=0; j<len; i++, j+=4 )
    {
        output[i] = ((unsigned  int)input[j]) | (((unsigned  int)input[j+1]) << 8) |
                    (((unsigned  int)input[j+2]) << 16) | (((unsigned  int)input[j+3]) << 24);
    }
}

void MD5::transform()
{
    unsigned  int a = state_[0], b = state_[1], c = state_[2], d = state_[3], x[16];
    
    decode( x, buffer_, 64 );  //对512位明文分成16组
    
    //! 第1轮
    
    FF (a, b, c, d, x[0],  7,  0xd76aa478);  // 1
    FF (d, a, b, c, x[ 1], 12, 0xe8c7b756);  // 2
    FF (c, d, a, b, x[ 2], 17, 0x242070db);  // 3
    FF (b, c, d, a, x[ 3], 22, 0xc1bdceee);  // 4
    FF (a, b, c, d, x[ 4], 7,  0xf57c0faf);  // 5
    FF (d, a, b, c, x[ 5], 12, 0x4787c62a);  // 6
    FF (c, d, a, b, x[ 6], 17, 0xa8304613);  // 7
    FF (b, c, d, a, x[ 7], 22, 0xfd469501);  // 8
    FF (a, b, c, d, x[ 8], 7,  0x698098d8);  // 9
    FF (d, a, b, c, x[ 9], 12, 0x8b44f7af);  // 10
    FF (c, d, a, b, x[10], 17, 0xffff5bb1);  // 11
    FF (b, c, d, a, x[11], 22, 0x895cd7be);  // 12
    FF (a, b, c, d, x[12], 7,  0x6b901122);  // 13
    FF (d, a, b, c, x[13], 12, 0xfd987193);  // 14
    FF (c, d, a, b, x[14], 17, 0xa679438e);  // 15
    FF (b, c, d, a, x[15], 22, 0x49b40821);  // 16
    
    //! 第2轮
    GG (a, b, c, d, x[ 1], 5,  0xf61e2562);  // 17
    GG (d, a, b, c, x[ 6], 9,  0xc040b340);  // 18
    GG (c, d, a, b, x[11], 14, 0x265e5a51);  // 19
    GG (b, c, d, a, x[ 0], 20, 0xe9b6c7aa);  // 20
    GG (a, b, c, d, x[ 5], 5,  0xd62f105d);  // 21
    GG (d, a, b, c, x[10], 9,  0x2441453);   // 22
    GG (c, d, a, b, x[15], 14, 0xd8a1e681);  // 23
    GG (b, c, d, a, x[ 4], 20, 0xe7d3fbc8);  // 24
    GG (a, b, c, d, x[ 9], 5,  0x21e1cde6);  // 25
    GG (d, a, b, c, x[14], 9,  0xc33707d6);  // 26
    GG (c, d, a, b, x[ 3], 14, 0xf4d50d87);  // 27
    GG (b, c, d, a, x[ 8], 20, 0x455a14ed);  // 28
    GG (a, b, c, d, x[13], 5,  0xa9e3e905);  // 29
    GG (d, a, b, c, x[ 2], 9,  0xfcefa3f8);  // 30
    GG (c, d, a, b, x[ 7], 14, 0x676f02d9);  // 31
    GG (b, c, d, a, x[12], 20, 0x8d2a4c8a);  // 32
    
    //! 第3轮
    HH (a, b, c, d, x[ 5], 4,  0xfffa3942);  // 33
    HH (d, a, b, c, x[ 8], 11, 0x8771f681);  // 34
    HH (c, d, a, b, x[11], 16, 0x6d9d6122);  // 35
    HH (b, c, d, a, x[14], 23, 0xfde5380c);  // 36
    HH (a, b, c, d, x[ 1], 4,  0xa4beea44);  // 37
    HH (d, a, b, c, x[ 4], 11, 0x4bdecfa9);  // 38
    HH (c, d, a, b, x[ 7], 16, 0xf6bb4b60);  // 39
    HH (b, c, d, a, x[10], 23, 0xbebfbc70);  // 40
    HH (a, b, c, d, x[13], 4,  0x289b7ec6);  // 41
    HH (d, a, b, c, x[ 0], 11, 0xeaa127fa);  // 42
    HH (c, d, a, b, x[ 3], 16, 0xd4ef3085);  // 43
    HH (b, c, d, a, x[ 6], 23, 0x4881d05);   // 44
    HH (a, b, c, d, x[ 9], 4,  0xd9d4d039);  // 45
    HH (d, a, b, c, x[12], 11, 0xe6db99e5);  // 46
    HH (c, d, a, b, x[15], 16, 0x1fa27cf8);  // 47
    HH (b, c, d, a, x[ 2], 23, 0xc4ac5665);  // 48
    
    //! 第4轮
    II (a, b, c, d, x[ 0], 6,  0xf4292244);  // 49
    II (d, a, b, c, x[ 7], 10, 0x432aff97);  // 50
    II (c, d, a, b, x[14], 15, 0xab9423a7);  // 51
    II (b, c, d, a, x[ 5], 21, 0xfc93a039);  // 52
    II (a, b, c, d, x[12], 6,  0x655b59c3);  // 53
    II (d, a, b, c, x[ 3], 10, 0x8f0ccc92);  // 54
    II (c, d, a, b, x[10], 15, 0xffeff47d);  // 55
    II (b, c, d, a, x[ 1], 21, 0x85845dd1);  // 56
    II (a, b, c, d, x[ 8], 6,  0x6fa87e4f);  // 57
    II (d, a, b, c, x[15], 10, 0xfe2ce6e0);  // 58
    II (c, d, a, b, x[ 6], 15, 0xa3014314);  // 59
    II (b, c, d, a, x[13], 21, 0x4e0811a1);  // 60
    II (a, b, c, d, x[ 4], 6,  0xf7537e82);  // 61
    II (d, a, b, c, x[11], 10, 0xbd3af235);  // 62
    II (c, d, a, b, x[ 2], 15, 0x2ad7d2bb);  // 63
    II (b, c, d, a, x[ 9], 21, 0xeb86d391);  // 64
    
    state_[0] += a;
    state_[1] += b;
    state_[2] += c;
    state_[3] += d;
}

const std::string& MD5::md5_string()
{
    return result_;
}

/* int main() */
/* { */
/*     MD5 md5; */
/*     md5.handle_str("1223"); */

/*     std::cout<<md5.md5_string()<<std::endl; */
/*     return 0; */
/* } */
