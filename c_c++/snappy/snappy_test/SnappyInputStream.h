#pragma once

#include <sstream>
#include "SnappyStreamHelper.h"

// Snappy 解压 输入流
// 1. 构造函数 传入 需要解压的输入流
// 2. read 读取 解压 后的数据
// 3. flush 刷新缓存区  所有
class SnappyInputStream :
	public std::istringstream
{
public:
	SnappyInputStream(std::istream * istream);
	~SnappyInputStream(void);
};

