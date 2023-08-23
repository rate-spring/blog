#pragma once

#include <sstream>
#include "SnappyStreamHelper.h"

// Snappy ѹ�� �����
// 1. ��ʼ�� ���� ѹ������ �����
// 2. write ѹ������ д���� �� ѹ������ ͨ�� ��������
// 3. flush ˢ�»����� �������� ѹ����� ͨ�� ��������
class SnappyOutputStream :
	public std::ostringstream
{
public:
	// ���캯�� ���� �����
	SnappyOutputStream(std::ostream *ostream);

	~SnappyOutputStream(void);

	// ԭʼ����д�� 
	int write(unsigned char * data, int len);

	// ˢ�����
	void flush();

private:

	// �����
	std::ostream * _ostream;

	// �Ƿ�дͷ
	bool           _writeHeader;




};

