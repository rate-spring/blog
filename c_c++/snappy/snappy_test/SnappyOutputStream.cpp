#include "StdAfx.h"
#include "SnappyOutputStream.h"


// 构造函数 传入 输出流
SnappyOutputStream::SnappyOutputStream(std::ostream *ostream)
{
}

SnappyOutputStream::~SnappyOutputStream(void)
{
}

// 原始数据写入 
int SnappyOutputStream::write(unsigned char * data, int len)
{
	return 0;
}

// 刷新输出
void SnappyOutputStream::flush()
{
}