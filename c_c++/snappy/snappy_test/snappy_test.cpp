// snappy_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SnappyStreamHelper.h"

using namespace SnappyStreamHelper;

int _tmain(int argc, _TCHAR* argv[])
{
	std::cout << "hello world..." << std::endl;

	{
		int buff_len = 10 * 1024 * 1024;
		buff_len = 0;

		unsigned char * buff = (unsigned char*) malloc(buff_len);
		for(int i = 0; i < buff_len ; i++)
			buff[i] = 'a' + i % 26;


		std::string outstring = "";

		long long time1 = get_now_time_in_ms();
		snappy::Compress((const char*)buff,(size_t)buff_len,&outstring);
		long long time2 = get_now_time_in_ms();

		std::cout << "outstring size: " << outstring.size() << std::endl;
		std::cout << "Compress time: " << (time2 - time1) << std::endl;

		write_fs("c:\\tmp\\snappy_compress.txt", (unsigned char*) outstring.c_str(), outstring.size());

		std::string outstring2 = "";

		long long time3 = get_now_time_in_ms();
		snappy::Uncompress(outstring.c_str(), outstring.size(), &outstring2);
		long long time4 = get_now_time_in_ms();

		std::cout << "outstring2 size: " << outstring2.size() << std::endl;

		std::cout << "Uncompress time: " << (time4 - time3) << std::endl;

		write_fs("c:\\tmp\\snappy_uncompress.txt", (unsigned char*) outstring2.c_str(), outstring2.size());

		if (outstring2 == (char*) buff)
		{
			std::cout << "outstring2 = buff" << std::endl;
		}
		else
		{
			std::cout << "outstring2 != buff" << std::endl;
		}

		free(buff);
	}


	std::cout << std::endl;

	{
		int buff_len = 10 * 1024 * 1024;
		buff_len = 0;

		unsigned char * buff = (unsigned char*) malloc(buff_len);
		for(int i = 0; i < buff_len ; i++)
			buff[i] = 'a' + i % 26;

		write_fs("c:\\tmp\\snappy.txt", (unsigned char*)buff, buff_len);
		std::cout << "buff_len: " << buff_len << std::endl  << " buff: " << (void*) buff  << std::endl;

		unsigned char * buff2 = (unsigned char*) malloc(buff_len);
		int buff2_len = 0;

		snappy_compress_stream(buff,buff_len,buff2,buff2_len);
		write_fs("c:\\tmp\\snappy_compress_stream.txt", (unsigned char*)buff2, buff2_len);

		std::cout << "buff2_len: " << buff2_len << std::endl << " buff2: " << (void*) buff2 << std::endl;


		unsigned char * buff3 = (unsigned char*) malloc(buff_len);
		int buff3_len = 0;

		snappy_uncompress_stream(buff2,buff2_len,buff3,buff3_len);
		write_fs("c:\\tmp\\snappy_uncompress_stream.txt", (unsigned char*)buff3, buff3_len);

		std::cout << "buff3_len: " << buff3_len << std::endl << " buff3: " << (void*) buff3  << std::endl;

		free(buff);

		std::cout << "free buff" << std::endl;

		free(buff2);

		std::cout << "free buff2" << std::endl;

		free(buff3);

		std::cout << "free buff3" << std::endl;
	}


	

	return 0;
}

