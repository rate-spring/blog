#include "StdAfx.h"
#include "SnappyOutputStream.h"
#include "SnappyStreamHelper.h"

using namespace SnappyStreamHelper;


// 构造函数 传入 输出流
SnappyOutputStream::SnappyOutputStream(std::ostream *ostream)
{
	_ostream = ostream;
	_writeHeader = false;
}

SnappyOutputStream::~SnappyOutputStream(void)
{
}

// 原始数据写入 
void SnappyOutputStream::write(unsigned char *data, int data_len)
{
	// 数据写入到 ostringstream
	this->write(data,data_len);

	// 获取数据长度
	int len = this->tellp();

	// 超过指定分块 读取出来 压缩写入到 输出 ostream
	if (len >= SnappyStreamHelper::DEFAULT_BLOCK_LEN)
	{
		// 获取一个分块
		std::string str = this->str();

		unsigned char * out = (unsigned char *) malloc(SnappyStreamHelper::DEFAULT_BLOCK_LEN + 1024 * 6);

		while(str.size() >= SnappyStreamHelper::DEFAULT_BLOCK_LEN)
		{
			std::string block_data = str.substr(0,SnappyStreamHelper::DEFAULT_BLOCK_LEN);
			str = str.substr(SnappyStreamHelper::DEFAULT_BLOCK_LEN);

			if (!_writeHeader)
			{
				_writeHeader = true;

				int pos = 0;
				SnappyStreamHelper::write_header(out,pos);

				_ostream->write((const char*)out,pos);
			}

			{
				int pos = 0;
				SnappyStreamHelper::write_block(out,pos,(unsigned char*)block_data.c_str(),block_data.size());

				_ostream->write((const char*)out,pos);

				
			}
		}

		free(out);

		this->str(str);
	}
}

// 刷新输出
void SnappyOutputStream::flush()
{
	// 获取数据长度
	int len = this->tellp();

	// 取所有数据 压缩
	std::string str = this->str();

	unsigned char * out = (unsigned char *) malloc(SnappyStreamHelper::DEFAULT_BLOCK_LEN + 1024 * 6);

	while(str.size() > 0)
	{
		std::string block_data;
		
		if (str.size() >= SnappyStreamHelper::DEFAULT_BLOCK_LEN)
		{
			block_data = str.substr(0,SnappyStreamHelper::DEFAULT_BLOCK_LEN);
			str = str.substr(SnappyStreamHelper::DEFAULT_BLOCK_LEN);
		}
		else
		{
			block_data = str;
			str = "";
		}
		
		if (!_writeHeader)
		{
			_writeHeader = true;

			int pos = 0;
			SnappyStreamHelper::write_header(out,pos);

			_ostream->write((const char*)out,pos);
		}

		{
			int pos = 0;
			SnappyStreamHelper::write_block(out,pos,(unsigned char*)block_data.c_str(),block_data.size());

			_ostream->write((const char*)out,pos);
		}
	}

	free(out);

	this->str(str);
}
