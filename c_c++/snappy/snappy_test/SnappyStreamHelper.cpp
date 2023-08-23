
#include "SnappyStreamHelper.h"

namespace SnappyStreamHelper
{

long long get_now_time_in_ms()
{
	struct timeb nowtime;
	ftime(&nowtime);

	long long curtime = nowtime.time * 1000 + nowtime.millitm;

	return curtime;
}

void write_fs(const char *path, unsigned char *data, int len)
{
	std::fstream fs;
	fs.open(path,std::ios::out | std::ios::trunc | std::ios::binary);
	fs.write((const char*)data,len);
	fs.flush();
	fs.close();
}


const char MAGIC_HEADER [8] = {-126, 'S', 'N', 'A', 'P', 'P', 'Y', 0};
const int DEFAULT_VERSION = 1;
const int MININUM_COMPATIBLE_VERSION = 1;
const int HEADER_LEN = 16;
const int DEFAULT_BLOCK_LEN = 32 * 1024;

// 写入整形 4字节
void write_int(unsigned char *out, int &pos, int value)
{
	out[pos++] = (unsigned char)((value >>  24) & 0xff);
	out[pos++] = (unsigned char)((value >>  16) & 0xff);
	out[pos++] = (unsigned char)((value >>  8) & 0xff);
	out[pos++] = (unsigned char)((value >>  0) & 0xff);
}

// 读取整形 4字节
int read_int(unsigned char *in, int in_len, int &pos)
{
	if (pos + 4 > in_len)
		return 0;

	int b1 = (in[pos++] & 0xff) << 24;
	int b2 = (in[pos++] & 0xff) << 16;
	int b3 = (in[pos++] & 0xff) << 8;
	int b4 = (in[pos++] & 0xff);

	return  (b1 | b2 | b3 | b4);
}

// 写入头  mgaic 8 字节 +  版本 4字节 + 兼容版本 4字节
void write_header(unsigned char *out, int &pos)
{
	memcpy(out, MAGIC_HEADER, sizeof(MAGIC_HEADER));
	pos += sizeof(MAGIC_HEADER);

	write_int(out, pos, DEFAULT_VERSION);
	write_int(out, pos, MININUM_COMPATIBLE_VERSION);
}

// 读取头  mgaic 8 字节 +  版本 4字节 + 兼容版本 4字节
int read_header(unsigned char *in, int in_len, int &pos, unsigned char *magic, int &version, int &compatibleVersion)
{
	if (pos + 16 > in_len)
		return 1;

	memcpy(magic, in + pos, sizeof(MAGIC_HEADER));
	pos += sizeof(MAGIC_HEADER);

	if (memcmp(magic, MAGIC_HEADER, sizeof(MAGIC_HEADER)) != 0)
	{
		return 2;
	}

	version = read_int(in, in_len, pos);
	compatibleVersion = read_int(in, in_len, pos);

	return 0;
}


// 写入分块数据
void write_block(unsigned char *out, int &pos, unsigned char *in, int len)
{
	// 32 k 
	// len = 32K  
	// 32 + source_len + source_len/6  38K
	const int buff_len = DEFAULT_BLOCK_LEN + 1024 * 6;
	char buff[buff_len] = {0};
	int buff_len2 = buff_len;

	snappy::RawCompress((const char*)in,len,buff,(size_t*)&buff_len2);

	//std::cout << "write_block compressed_length:" << buff_len2 << std::endl;

	write_int(out,pos,buff_len2);
	memcpy(out + pos, buff, buff_len2);
	pos += buff_len2;
}

// 读取分块数据
bool read_block(unsigned char *in, int in_len, int &pos, unsigned char *out, int &out_len)
{
	const int buff_len = DEFAULT_BLOCK_LEN + 1024 * 6;
	char buff[buff_len] = {0};

	int compressed_length = read_int(in, in_len, pos);

	if (compressed_length <= 0)
		return false;

	//std::cout << "read_block compressed_length:" << compressed_length << std::endl;

	bool ok = snappy::RawUncompress((const char*)in + pos,(size_t)compressed_length,(char*)buff);
	if (ok)
	{
		int len = 0;

		snappy::GetUncompressedLength((const char*)in + pos,(size_t)compressed_length,(size_t*)&len);
		
		memcpy(out + out_len, buff, len);
		
		out_len += len;
		pos += compressed_length;
	}

	return ok;
}


int snappy_compress_stream(unsigned char *in, int in_len, unsigned char *out, int &out_len)
{
	// header  mgaic 8 字节 +  版本 4字节 + 兼容版本 4字节
	// 分块1 数据压缩后数据长度 4字节 +  压缩数据
	// 分块1 数据压缩后数据长度 4字节 +  压缩数据
	int block_num = in_len / DEFAULT_BLOCK_LEN;
	int last_block_len = DEFAULT_BLOCK_LEN;
	if (in_len % DEFAULT_BLOCK_LEN != 0)
	{
		block_num += 1;
		last_block_len = in_len % DEFAULT_BLOCK_LEN;
	}

	out_len = 0;

	write_header(out,out_len);

	for(int i = 0; i < block_num; i++)
	{
		int block_len = DEFAULT_BLOCK_LEN;
		if (i == block_num - 1)
		{
			block_len = last_block_len;
		}

		write_block(out,out_len,in + i * DEFAULT_BLOCK_LEN, block_len);
	}

	return 0;
}

int snappy_uncompress_stream(unsigned char *in, int in_len, unsigned char *out, int &out_len)
{
	unsigned char magic[8] = {0};
	int version = 0;
	int compatibleVersion = 0;

	int pos = 0;

	int r = read_header(in, in_len, pos, magic, version, compatibleVersion);
	if (r != 0)
	{
		return r;
	}
	//std::cout << "read_header pos " << pos << std::endl;

	while(true)
	{
		bool ok = read_block(in, in_len, pos, out, out_len);
		if (!ok)
		{
			break;
		}
	}

	return 0;
}

}