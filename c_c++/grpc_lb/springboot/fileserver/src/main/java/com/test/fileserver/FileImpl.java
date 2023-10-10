package com.test.fileserver;

import io.grpc.stub.StreamObserver;

import org.springframework.stereotype.Service;

import com.test.services.grpc.file.*;
import com.test.services.grpc.file.File.FileDownloadBlockReq;
import com.test.services.grpc.file.File.FileDownloadBlockRsp;
import com.test.services.grpc.file.File.FileDownloadReq;
import com.test.services.grpc.file.File.FileDownloadRsp;
import com.test.services.grpc.file.File.FileUploadBlockReq;
import com.test.services.grpc.file.File.FileUploadBlockRsp;
import com.test.services.grpc.file.File.FileUploadBlockRspList;

@Service
public class FileImpl extends FileServiceGrpc.FileServiceImplBase {
	
	public static final String BLOCK_DIR = "d:/data";

	public FileImpl()
	{
		_writeCenter = new FileBlockWriteCenter();
		_writeCenter.init();
		
		_readCenter = new FileBlockReadCenter();
		_readCenter.init();
	}
	
	public void finalize()
	{
		_writeCenter.uninit();
		_writeCenter = null;
		
		_readCenter.uninit();
		_readCenter = null;
	}
	
	@Override
	public StreamObserver<FileUploadBlockReq> uploadBlock(StreamObserver<FileUploadBlockRspList> responseObserver) {
		// TODO Auto-generated method stub
		return new StreamObserver<FileUploadBlockReq> ()
		{
			FileUploadBlockRspList.Builder builder = FileUploadBlockRspList.newBuilder();
			
	
			@Override
			public void onNext(FileUploadBlockReq value) {
				// TODO Auto-generated method stub
				// 将一块数据写入到磁盘文件
				// 文件名 fid_bindex
				// 一次处理一个分块
				
				String path = BLOCK_DIR;
				path += "/";
				path += value.getFid();
				path += "_";
				path += String.valueOf(value.getBindex());
				
				_writeCenter.write(path, value.getData());				
				
				if (value.getBfin() != 0)
				{
					_writeCenter.close(path);
				}
				
				System.out.println("path:" + path);
				System.out.println("fid:" + value.getFid() + " bindex:" + value.getBindex() + " offset:" + value.getOffset() + " len:" + value.getLen());
				
				FileUploadBlockRsp.Builder rspBuilder = FileUploadBlockRsp.newBuilder();
				
				rspBuilder.setCode(0);
				rspBuilder.setMsg("ok");
				rspBuilder.setRid(value.getRid());
				rspBuilder.setFid(value.getFid());
				rspBuilder.setBindex(value.getBindex());
				rspBuilder.setOffset(value.getOffset());
				rspBuilder.setLen(value.getLen());
				
				if (value.getBfin() != 0)
				{
					rspBuilder.setSbid(value.getFid() + "_" + String.valueOf(value.getBindex()));
				}
				else
				{
					rspBuilder.setSbid("");
				}
				
				if (value.getBindex() == value.getBnum() - 1)
				{
					rspBuilder.setSfid(value.getFid());
				}
				else
				{
					rspBuilder.setSfid("");
				}

				builder.addFileUploadBlockRsp(rspBuilder.build());				
			}

			@Override
			public void onError(Throwable t) {
				// TODO Auto-generated method stub
				responseObserver.onError(t);
			}

			@Override
			public void onCompleted() {
				// TODO Auto-generated method stub
				responseObserver.onNext(builder.build());
				responseObserver.onCompleted();

			}
		};
	}

	@Override
	public void download(FileDownloadReq request, StreamObserver<FileDownloadRsp> responseObserver) {
		// TODO Auto-generated method stub

		FileDownloadRsp.Builder builder = FileDownloadRsp.newBuilder();
		
		builder.setCode(0);
		builder.setMsg("ok");
		builder.setName("a.txt");
		builder.setSize(1024 * 1024 * 10 * 10);
		builder.setBnum(10);
		builder.setBsize(1024 * 1024 * 10);
		builder.setLbsize(1024 * 1024 * 10);
		
		responseObserver.onNext(builder.build());
		responseObserver.onCompleted();		
	}

	@Override
	public void downloadBlock(FileDownloadBlockReq request, StreamObserver<FileDownloadBlockRsp> responseObserver) {
		// TODO Auto-generated method stub
		
		String path = BLOCK_DIR;
		path += "/";
		path += request.getSfid();
		path += "_";
		path += String.valueOf(request.getBindex());
		
		// 读取文件文件大小
		// 每次读取1M数据下发
		long fsize = _readCenter.readyRead(path);
		
		byte[] data = new byte[1024 * 1024];
		
		int one_time_len = 1024 * 1024;
		
		int num = ((int) fsize) / one_time_len;
		int last_len = one_time_len;
		if (fsize % one_time_len != 0)
		{
			num += 1;
			
			last_len = ((int) fsize) % one_time_len;
		}
		
		for(int i = 0; i < num; i++)
		{
			int cur_len = one_time_len;
			if (i == num - 1)
			{
				cur_len = last_len;
			}
			
			int rlen = _readCenter.read(path,data);
			
			if (rlen != cur_len)
			{
				System.out.println("read rlen error. rlen:" + rlen + ", cur_len:" + cur_len);
			}
			
			FileDownloadBlockRsp.Builder builder = FileDownloadBlockRsp.newBuilder();
			
			builder.setRid(request.getRid());
			builder.setData(com.google.protobuf.ByteString.copyFrom(data,0,rlen));
			
			responseObserver.onNext(builder.build());
		}
		
		responseObserver.onCompleted();	
	}
	
	private FileBlockWriteCenter _writeCenter = null;
	
	private FileBlockReadCenter _readCenter = null;

}
