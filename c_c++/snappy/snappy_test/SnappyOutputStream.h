#pragma once

#include <sstream>
#include "SnappyStreamHelper.h"

// Snappy 压缩 输出流
// 1. 初始化 传入 压缩数据 输出流
// 2. write 压缩数据 写入流 将 压缩数据 通过 输出流输出
// 3. flush 刷新缓存区 所有数据 压缩完成 通过 输出流输出
class SnappyOutputStream :
	public std::ostringstream
{
public:
	// 构造函数 传入 输出流
	SnappyOutputStream(std::ostream *ostream);

	~SnappyOutputStream(void);

	// 原始数据写入 
	int write(unsigned char * data, int len);

	// 刷新输出
	void flush();

private:

	// 输出流
	std::ostream * _ostream;

	// 是否写头
	bool           _writeHeader;




};

