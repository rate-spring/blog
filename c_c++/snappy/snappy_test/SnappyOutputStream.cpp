#include "StdAfx.h"
#include "SnappyOutputStream.h"
#include "SnappyStreamHelper.h"

using namespace SnappyStreamHelper;


// ���캯�� ���� �����
SnappyOutputStream::SnappyOutputStream(std::ostream *ostream)
{
	_ostream = ostream;
	_writeHeader = false;
}

SnappyOutputStream::~SnappyOutputStream(void)
{
}

// ԭʼ����д�� 
void SnappyOutputStream::write(unsigned char *data, int data_len)
{
	// ����д�뵽 ostringstream
	this->write(data,data_len);

	// ��ȡ���ݳ���
	int len = this->tellp();

	// ����ָ���ֿ� ��ȡ���� ѹ��д�뵽 ��� ostream
	if (len >= SnappyStreamHelper::DEFAULT_BLOCK_LEN)
	{
		// ��ȡһ���ֿ�
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

// ˢ�����
void SnappyOutputStream::flush()
{
	// ��ȡ���ݳ���
	int len = this->tellp();

	// ȡ�������� ѹ��
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
