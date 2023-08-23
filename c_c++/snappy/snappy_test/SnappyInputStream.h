#pragma once

#include <sstream>
#include "SnappyStreamHelper.h"

// Snappy ��ѹ ������
// 1. ���캯�� ���� ��Ҫ��ѹ��������
// 2. read ��ȡ ��ѹ �������
// 3. flush ˢ�»�����  ����
class SnappyInputStream :
	public std::istringstream
{
public:
	SnappyInputStream(std::istream * istream);
	~SnappyInputStream(void);
};

