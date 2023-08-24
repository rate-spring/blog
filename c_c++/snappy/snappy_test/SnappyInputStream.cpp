#include "StdAfx.h"
#include "SnappyInputStream.h"


SnappyInputStream::SnappyInputStream()
{
	_readHeader = false;
}

SnappyInputStream::~SnappyInputStream(void)
{

}

// 读取解压数据 
// 分块压缩 data 空间 至少 大于一个分块 32k
// read 尽可能返回解压数据，直到空间不够
void SnappyInputStream::read_block(unsigned char *data, int &data_len)
{
	// 检查 stringstream 数据长度
	int len = get_stream_data_len();
	if (!_readHeader)
	{
		if (len < SnappyStreamHelper::HEADER_LEN)
		{
			// 长度不够 直接返回
			return;
		}

		// 长度够 解析头
		unsigned char buff[64] = {0};
		int buff_len = SnappyStreamHelper::HEADER_LEN;
		get_stream_data(buff,buff_len);

		_readHeader = true;
	}

	// 读取分块数据 直到 data_len 空间不够
	int source_data_len = data_len;
	data_len = 0;

	const int buff_len = SnappyStreamHelper::DEFAULT_BLOCK_LEN + 1024 * 6;
	unsigned char * buff = (unsigned char *) malloc(buff_len);

	while(true)
	{
		if (source_data_len < SnappyStreamHelper::DEFAULT_BLOCK_LEN)
		{
			break;
		}

		// peek 一个分块
		// 查看整块数据 是否完整
		int block_len = 0;
		bool haveBlock = check_block(block_len);
		if (!haveBlock)
		{
			break;
		}

		get_stream_data(buff,block_len);

		int out_len = 0;
		int pos = 0;
		SnappyStreamHelper::read_block(buff,block_len,pos,data + data_len, out_len);

		data_len += out_len;
		source_data_len -= out_len;
	}

	free(buff);
}

// istream 写入完成， 读取最后一块解压数据
// 超过一个分块数据 返回 -1， 先循环调用 read_block， 直到 data_len = 0
// data = NULL, 返回 需要 data空间大小
// data != NULL, data_len <  需要 data空间大小  返回该值 不填充 data
// data != NULL, data_len >= 需要 data空间大小  返回0， 填充 data
int SnappyInputStream::read_block_last(unsigned char *data, int &data_len)
{
	// 确定 stringstream 没有新数据写入
	// 剩余数据 全部解压 
	int source_data_len = data_len;
	data_len = 0;

	int len = get_stream_data_len();
	if (len == 0)
	{
		return 0;
	}

	// 查看是否超过一个分块
	int block_len = 0;
	bool haveBlock = check_block(block_len);
	if (!haveBlock)
	{
		return 0;
	}

	if (block_len < len)
	{
		return -1;
	}
	
	if (block_len < len)
	{
		// 数据不完整 最后分块数据大于缓存区数据
		return -2;
	}

	// 解压最后一个分块数据
	// 从内存中解析 不从流中读取 
	std::string str = this->str();

	// 解压后的数据 缓冲区
	const int out_len = SnappyStreamHelper::DEFAULT_BLOCK_LEN + 1024 * 6;
	unsigned char * out = (unsigned char *) malloc(out_len);

	int out_len2 = 0;
	int pos = 0;
	SnappyStreamHelper::read_block((unsigned char*)str.c_str(),str.size(), pos, out, out_len2);

	if (data == NULL)
	{
		// 返回 解压数据大小 
		free(out);

		return out_len2;
	}

	if (source_data_len < out_len2)
	{
		free(out);

		return out_len2;
	}

	memcpy(data,out,out_len2);
	data_len = out_len2;

	this->str("");

	free(out);

	_readHeader = false;

	return 0;

}

// 获取当前iostream中数据长度
int SnappyInputStream::get_stream_data_len()
{
	this->seekg(0,std::ios::end);
	int len = this->tellg();
	this->seekg(0);
	return len;
}

// 读取指定字节数据
// 读取的数据删除 剩余的数据迁移
int SnappyInputStream::get_stream_data(unsigned char *data, int &data_len)
{
	std::string str = this->str();

	if (str.size() < data_len)
	{
		return -1;
	}

	std::string data_in = str.substr(0,data_len);
	std::string data_left = str.substr(data_len);

	this->str(data_left);
	this->seekg(0);
	this->clear();

	memcpy(data,data_in.c_str(),data_len);

	return 0;
}

// 检查当前分块数据
bool SnappyInputStream::check_block(int &block_len)
{
	int len = get_stream_data_len();
	if (len < 4)
	{
		return false;
	}

	// 读取4字节
	unsigned char buff[64] = {0};
	this->read((char*)buff,4);
	this->seekg(0);

	int pos = 0;
	int block_data_len = SnappyStreamHelper::read_int(buff,4,pos);
	if (block_data_len + 4 > len)
	{
		return false;
	}

	block_len = 4 + block_data_len;

	return true;
}