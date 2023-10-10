﻿// FileClient.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"



// grpc 头文件
#include <grpcpp/grpcpp.h>

#include "file.pb.h"
#include "file.grpc.pb.h"

#include <fstream>
#include <sys\timeb.h>

// 命名空间
// grcp 命名空间
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;


static const char * __C = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static int __rand_seed = 0;
static int __rand_xh = 0;


std::string get_rand_string(int len)
{
	std::string str = "";

	int C_LEN = strlen(__C);

	srand(time(NULL) + __rand_seed++);
	for (int i = 0; i < len; i++)
	{
		int index = rand() % C_LEN;
		if (index < 0)
			index *= -1;

		str.append(__C + index, 1);
	}

	return str;
};

long long get_now_time()
{
	struct timeb rawtime;
	ftime(&rawtime);

	return (long long) ((long long)rawtime.time * 1000 + rawtime.millitm);
};


class FileClient
{
public:
	FileClient(std::shared_ptr<Channel> channel) 
		: _stub(::file::FileService::NewStub(channel)) {}

	// upload block
	int uploadBlock(const std::string &fid, const std::string &name, long long fsize, int bnum, int bsize, int lbsize,
		int bindex, unsigned char * data, int len, std::string &sfid)
	{
		::file::FileUploadBlockReq req;

		req.set_fid(fid);
		req.set_name(name);
		req.set_fsize(fsize);
		req.set_bnum(bnum);
		req.set_bsize(bsize);
		req.set_lbsize(lbsize);
		req.set_bindex(bindex);

		// 1次提交1M
		int one_time_len = 1024 * 1024;
		int num = len / one_time_len;
		int last_len = one_time_len;
		if (len % one_time_len != 0)
		{
			num += 1;
			last_len = len % one_time_len;
		}

		int pos = 0;

		::grpc::ClientContext context;
		::file::FileUploadBlockRspList list;
		std::unique_ptr< ::grpc::ClientWriter< ::file::FileUploadBlockReq>> writer(
			_stub->uploadBlock(&context, &list) );

		for (int i = 0; i < num; i++)
		{
			int cur_len = one_time_len;
			if (i == num - 1)
			{
				cur_len = last_len;
			}

			pos = i * one_time_len;

			req.set_rid(get_rand_string(8));
			req.set_offset(pos);
			req.set_len(cur_len);
			req.mutable_data()->assign((char*)data + pos, cur_len);
			req.set_bfin(i == num - 1 ? 1 : 0);

			writer->Write(req);
		}

		writer->WritesDone();

		Status status = writer->Finish();

		if (status.ok())
		{
			std::cout << "uploadBlock status: ok" << " bindex:" << bindex << std::endl;

			for (int i = 0; i < list.fileuploadblockrsp_size(); i++)
			{
				//std::cout << i << " " << list.fileuploadblockrsp(i).DebugString() << std::endl;

				sfid = list.fileuploadblockrsp(i).sfid();
			}
		}
		else
		{
			std::cout << "uploadBlock status: fail" << " bindex:" << bindex  << " error_message:" << status.error_message() << " error_details:" << status.error_details() <<  std::endl;
		}

		return 0;
	}

	// download 
	int download(const std::string &sfid, ::file::FileDownloadRsp &rsp)
	{
		::file::FileDownloadReq req;

		req.set_rid(get_rand_string(8));
		req.set_sfid(sfid);

		::grpc::ClientContext context;

		Status status = _stub->download(&context, req, &rsp);

		if (status.ok())
		{
			std::cout << "download status: ok" << std::endl;

			//std::cout << rsp.DebugString() << std::endl;
		}
		else
		{
			std::cout << "download status: fail" << std::endl;
		}


		return 0;
	}

	// download block
	int downloadBlock(const std::string &sfid, int bindex, std::string &block_path)
	{
		::file::FileDownloadBlockReq req;

		req.set_rid(get_rand_string(8));
		req.set_sfid(sfid);
		req.set_bindex(bindex);

		std::fstream fs;
		fs.open(block_path.c_str(), std::ios::out | std::ios::trunc);

		::grpc::ClientContext context;
		std::unique_ptr< ::grpc::ClientReader< ::file::FileDownloadBlockRsp>> reader(_stub->downloadBlock(&context, req));

		::file::FileDownloadBlockRsp rsp;
		while (reader->Read(&rsp))
		{
			fs.write(rsp.data().c_str(), rsp.data().size());
		}

		fs.close();

		Status status = reader->Finish();

		if (status.ok())
		{
			std::cout << "downloadBlock status: ok" << " sfid:" << sfid << " bindex:" << bindex << std::endl;
		}
		else
		{
			std::cout << "downloadBlock status: fail" << " sfid:" << sfid << " bindex:" << bindex << std::endl;
		}

		return 0;
	}

private:

	std::unique_ptr<::file::FileService::Stub> _stub;   // 存根，客户端代理
};


int main()
{
	// 服务器的地址
	std::string server_addr = "192.168.3.79:10080";

	// 创建请求通道 
	//FileClient client(
	//	grpc::CreateChannel(server_addr, grpc::InsecureChannelCredentials())
	//);

	::grpc::ChannelArguments args;
	args.SetLoadBalancingPolicyName("ip_hash");

	FileClient client(
		grpc::CreateCustomChannel(server_addr, grpc::InsecureChannelCredentials(),args)
	);

	std::string sfid;

	long long time11 = get_now_time();

	{
		// 文件块上传
		// 100M 文件 10M一块 分10次上传
		std::string fid = get_rand_string(16);
		std::string name = "a.txt";
		long long fsize = 1024 * 1024 * 10 * 10;
		int bnum = 10;
		int bsize = 1024 * 1024 * 10;
		int lbsize = 1024 * 1024 * 10;

		int len = 1024 * 1024 * 10;
		unsigned char * data = (unsigned char*) malloc(len);
	
		for(int j = 0; j < 10; j++)
		{
			for (int i = 0; i < len; i++)
			{
				data[i] = j % 52 + 'a';
			}

			long long time1 = get_now_time();

			client.uploadBlock(fid, name, fsize, bnum, bsize, lbsize, j, data, len, sfid);

			long long time2 = get_now_time();

			std::cout << "uploadBlock bindex:" << j << " time:" << (time2 - time1) << std::endl;
		}
	}

	// 获取文件整体信息
	::file::FileDownloadRsp downloadRsp;

	{
		long long time1 = get_now_time();

		client.download(sfid, downloadRsp);

		long long time2 = get_now_time();

		std::cout << "download time:" << (time2 - time1) << std::endl;
	}
	
	// 文件块下载
	{
		int bnum = downloadRsp.bnum();

		for(int i = 0; i < bnum; i++)
		{
			std::string path = "c:/data2/";
			path += sfid;
			path += "_";

			char tmp[64] = { 0 };
			sprintf_s(tmp, "%d", i);
			path += tmp;

			long long time1 = get_now_time();

			client.downloadBlock(sfid, i, path);

			long long time2 = get_now_time();

			std::cout << "downloadBlock bindex:" << i << " time:" << (time2 - time1) << std::endl;
		}
	}
	
	long long time22 = get_now_time();

	std::cout << "all time:" << (time22 - time11) << std::endl;

    return 0;
}

