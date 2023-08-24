#include "StdAfx.h"
#include "SnappyInputStream.h"


SnappyInputStream::SnappyInputStream()
{
	_readHeader = false;
}

SnappyInputStream::~SnappyInputStream(void)
{

}

// ��ȡ��ѹ���� 
// �ֿ�ѹ�� data �ռ� ���� ����һ���ֿ� 32k
// read �����ܷ��ؽ�ѹ���ݣ�ֱ���ռ䲻��
void SnappyInputStream::read_block(unsigned char *data, int &data_len)
{
	// ��� stringstream ���ݳ���
	int len = get_stream_data_len();
	if (!_readHeader)
	{
		if (len < SnappyStreamHelper::HEADER_LEN)
		{
			// ���Ȳ��� ֱ�ӷ���
			return;
		}

		// ���ȹ� ����ͷ
		unsigned char buff[64] = {0};
		int buff_len = SnappyStreamHelper::HEADER_LEN;
		get_stream_data(buff,buff_len);

		_readHeader = true;
	}

	// ��ȡ�ֿ����� ֱ�� data_len �ռ䲻��
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

		// peek һ���ֿ�
		// �鿴�������� �Ƿ�����
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

// istream д����ɣ� ��ȡ���һ���ѹ����
// ����һ���ֿ����� ���� -1�� ��ѭ������ read_block�� ֱ�� data_len = 0
// data = NULL, ���� ��Ҫ data�ռ��С
// data != NULL, data_len <  ��Ҫ data�ռ��С  ���ظ�ֵ ����� data
// data != NULL, data_len >= ��Ҫ data�ռ��С  ����0�� ��� data
int SnappyInputStream::read_block_last(unsigned char *data, int &data_len)
{
	// ȷ�� stringstream û��������д��
	// ʣ������ ȫ����ѹ 
	int source_data_len = data_len;
	data_len = 0;

	int len = get_stream_data_len();
	if (len == 0)
	{
		return 0;
	}

	// �鿴�Ƿ񳬹�һ���ֿ�
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
		// ���ݲ����� ���ֿ����ݴ��ڻ���������
		return -2;
	}

	// ��ѹ���һ���ֿ�����
	// ���ڴ��н��� �������ж�ȡ 
	std::string str = this->str();

	// ��ѹ������� ������
	const int out_len = SnappyStreamHelper::DEFAULT_BLOCK_LEN + 1024 * 6;
	unsigned char * out = (unsigned char *) malloc(out_len);

	int out_len2 = 0;
	int pos = 0;
	SnappyStreamHelper::read_block((unsigned char*)str.c_str(),str.size(), pos, out, out_len2);

	if (data == NULL)
	{
		// ���� ��ѹ���ݴ�С 
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

// ��ȡ��ǰiostream�����ݳ���
int SnappyInputStream::get_stream_data_len()
{
	this->seekg(0,std::ios::end);
	int len = this->tellg();
	this->seekg(0);
	return len;
}

// ��ȡָ���ֽ�����
// ��ȡ������ɾ�� ʣ�������Ǩ��
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

// ��鵱ǰ�ֿ�����
bool SnappyInputStream::check_block(int &block_len)
{
	int len = get_stream_data_len();
	if (len < 4)
	{
		return false;
	}

	// ��ȡ4�ֽ�
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