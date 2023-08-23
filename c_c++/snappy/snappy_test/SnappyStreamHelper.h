
#ifndef SNAPPY_STREAM_H
#define SNAPPY_STREAM_H


#include "snappy.h"
#include <iostream>
#include <sys/timeb.h>
#include <fstream>

namespace SnappyStreamHelper
{


extern const char MAGIC_HEADER [8];
extern const int DEFAULT_VERSION ;
extern const int MININUM_COMPATIBLE_VERSION ;
extern const int HEADER_LEN;
extern const int DEFAULT_BLOCK_LEN ;

long long get_now_time_in_ms();

void write_fs(const char *path, unsigned char *data, int len);

// д������ 4�ֽ�
void write_int(unsigned char *out, int &pos, int value);

// ��ȡ���� 4�ֽ�
int read_int(unsigned char *in, int in_len, int &pos);

// д��ͷ  mgaic 8 �ֽ� +  �汾 4�ֽ� + ���ݰ汾 4�ֽ�
void write_header(unsigned char *out, int &pos);

// ��ȡͷ  mgaic 8 �ֽ� +  �汾 4�ֽ� + ���ݰ汾 4�ֽ�
int read_header(unsigned char *in, int in_len, int &pos, unsigned char *magic, int &version, int &compatibleVersion);

// д��ֿ�����
void write_block(unsigned char *out, int &pos, unsigned char *in, int len);

// ��ȡ�ֿ�����
bool read_block(unsigned char *in, int in_len, int &pos, unsigned char *out, int &out_len);


int snappy_compress_stream(unsigned char *in, int in_len, unsigned char *out, int &out_len);

int snappy_uncompress_stream(unsigned char *in, int in_len, unsigned char *out, int &out_len);

}

#endif