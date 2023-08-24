#pragma once

#include <sstream>
#include "SnappyStreamHelper.h"

// Snappy ��ѹ ������
// ��δ֪istream������ɵ�����£� ��Ҫѭ������ read ��ȡ��ѹ�������
// ��֪�� istream ������ɣ� ���� read_last ��ȡʣ��Ľ�ѹ����
// ��֧�ֶ��߳� �� stringstream ���в��� ���౻���Ϊ���³���
// libcurl �����ļ�����
// 1. CURLOPT_WRITEFUNCTION ���ػص���������ȡ�ӷ�������ص�����
// 2. �ص�������  stringstream << д�����ص�ѹ������
// 3. �ص�������  ���� read �� iostream ��ȡ���ݣ�����ﵽһ�ݷֿ飬��ѹ���أ� ����һ���ֿ��ʣ������ ����д��stringstream��
// 4. libcurl curl_easy_perform ���أ�������� stringstream �л�������δ��ѹ�����ݣ����� read_last ��ȡ����ѹ����

// 1. ���캯�� 
// 2. read_block ��ȡ ��ѹ �������
// 3. read_block_last stringstream д����ɣ� read_block_last ��ȡ���Ľ�ѹ����
class SnappyInputStream :
	public std::stringstream
{
public:
	SnappyInputStream();

	~SnappyInputStream(void);

	// ��ȡ��ѹ���� 
	// �ֿ�ѹ�� data �ռ� ���� ����һ���ֿ� 32k
	// read_block �����ܷ��ؽ�ѹ���ݣ�ֱ���ռ䲻��
	// ����-1  data_len ���� 32k
	// ����-2  ѹ�����ݲ���
	// ����0  �ɹ�
	int read_block(unsigned char *data, int &data_len);

	// stringstream д����ɣ� ��ȡ���һ���ѹ����
	// ����һ���ֿ����� ���� -1�� ��ѭ������ read_block�� ֱ�� data_len = 0
	// ʵ������С��һ���ֿ����� ���� -2  (���ݲ�����)
	// data = NULL, ������Ҫ data�ռ��С
	// data != NULL, data_len <  ʵ��ռ�ÿռ��С  ���ظ�ֵ ����� data
	// data != NULL, data_len >= ʵ��ռ�ÿռ�  ����0�� ��� data
	int read_block_last(unsigned char *data, int &data_len);

private:
	// ��ȡ��ǰstringstream�����ݳ���
	int get_stream_data_len();

	// ��ȡָ���ֽ�����
	// ��ȡ������ɾ�� ʣ�������Ǩ��
	int get_stream_data(unsigned char *data, int &data_len);

	// ��鵱ǰ�ֿ�����
	bool check_block(int &block_len);

private:

	// �Ƿ��ȡ�� ͷ
	bool		 _readHeader;

	unsigned char _magicHeader[8];
	int           _version;
	int           _compatibleVersion;
};

