#pragma once

#include <sstream>
#include "SnappyStreamHelper.h"

// Snappy 解压 输入流
// 在未知istream输入完成的情况下， 需要循环调用 read 获取解压后的数据
// 当知道 istream 输入完成， 调用 read_last 获取剩余的解压数据
// 不支持多线程 对 stringstream 进行操作 该类被设计为如下场景
// libcurl 下载文件场景
// 1. CURLOPT_WRITEFUNCTION 下载回调函数，读取从服务端下载的数据
// 2. 回调函数中  stringstream << 写入下载的压缩数据
// 3. 回调函数中  调用 read 从 iostream 读取数据，如果达到一份分块，解压返回， 多余一个分块的剩余数据 依旧写入stringstream中
// 4. libcurl curl_easy_perform 返回，请求完成 stringstream 中还保留有未解压的数据，调用 read_last 读取最后解压数据

// 1. 构造函数 传入 需要解压的输入流
// 2. read 读取 解压 后的数据
// 3. read_last istream 写入完成， read_last 获取最后的解压数据
class SnappyInputStream :
	public std::stringstream
{
public:
	SnappyInputStream();

	~SnappyInputStream(void);

	// 读取解压数据 
	// 分块压缩 data 空间 至少 大于一个分块 32k
	// read 尽可能返回解压数据，直到空间不够
	void read_block(unsigned char *data, int &data_len);

	// istream 写入完成， 读取最后一块解压数据
	// 超过一个分块数据 返回 -1， 先循环调用 read_block， 直到 data_len = 0
	// 实际数据小于一个分块数据 返回 -2  (数据不完整)
	// data = NULL, 返回需要 data空间大小
	// data != NULL, data_len <  实际占用空间大小  返回该值 不填充 data
	// data != NULL, data_len >= 实际占用空间  返回0， 填充 data
	int read_block_last(unsigned char *data, int &data_len);

private:
	// 获取当前iostream中数据长度
	int get_stream_data_len();

	// 读取指定字节数据
	// 读取的数据删除 剩余的数据迁移
	int get_stream_data(unsigned char *data, int &data_len);

	// 检查当前分块数据
	bool check_block(int &block_len);

private:

	// 输入流
	std::stringstream * _stringstream;

	// 是否读取到 头
	bool		 _readHeader;

	unsigned char _magicHeader[8];
	int           _version;
	int           _compatibleVersion;
};

