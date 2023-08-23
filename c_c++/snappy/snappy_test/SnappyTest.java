package mvntest;
import org.xerial.snappy.*;
import java.io.*;

public class SnappyTest {

	public static void main(String[] args) {
		// TODO Auto-generated method stub

		snappy_test();
		
		snappy_stream_compress_test();
		
		snappy_stream_uncompress_test();
	}
	
	public static void snappy_test()
	{
		int buff_len = 10 * 1024 * 1024;
		byte[] buff = new byte[buff_len];
		for(int i = 0; i < buff_len; i++)
			buff[i] = (byte)('a' + i % 26);
		
		write_fs("c:\\tmp\\snappy.txt", buff);
		
		byte[] compressed;
		try {
			compressed = Snappy.compress(buff);
			
			write_fs("c:\\tmp\\snappy_compress.txt", compressed);
			
			byte[] uncompressed = Snappy.uncompress(compressed);
			
			write_fs("c:\\tmp\\snappy_uncompress.txt", uncompressed);

			//String result = new String(uncompressed, "UTF-8");
			//System.out.println(result);
			
			
			
		} catch (UnsupportedEncodingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		
	}
	
	public static void snappy_stream_compress_test()
	{
		// 源文件路径
		int buff_len = 10 * 1024 * 1024;
		byte[] buff = new byte[buff_len];
		for(int i = 0; i < buff_len; i++)
			buff[i] = (byte)('a' + i % 26);
		
		String sourceFilePath = "path/to/source/file.txt";

		// 创建源数据的输入流
		java.io.ByteArrayInputStream inputStream = new java.io.ByteArrayInputStream(buff);
		
		// 目标文件路径
		String targetFilePath = "c:\\tmp\\snappy_compress_stream.txt"; 

		// 创建目标数据的输出流
		OutputStream outputStream;
		try {
			outputStream = new FileOutputStream(targetFilePath);
			
			// 使用SnappyCompressorOutputStream进行封装
			SnappyOutputStream snappyOutputStream = new SnappyOutputStream(outputStream);
			
			// 缓冲区大小
			byte[] buffer = new byte[4096];
			int bytesRead;

			// 读取源数据输入流，并将数据写入目标数据输出流
			while ((bytesRead = inputStream.read(buffer)) != -1) {
			    snappyOutputStream.write(buffer, 0, bytesRead);
			}

			// 刷新输出流，确保所有数据都被写入目标数据输出流
			snappyOutputStream.flush();
			
			// 关闭源数据输入流
			inputStream.close();

			// 关闭目标数据输出流
			snappyOutputStream.close();			
			
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		

	}
	
	public static void snappy_stream_uncompress_test()
	{
		// 源文件路径
		int buff_len = 10 * 1024 * 1024;
		byte[] buff = new byte[buff_len];
		for(int i = 0; i < buff_len; i++)
			buff[i] = (byte)('a' + i % 26);
		
		
		try {
			String sourceFilePath = "c:\\tmp\\snappy_compress_stream.txt";
			//String sourceFilePath = "c:\\tmp\\snapyy_compress.txt";
			

			// 创建源数据的输入流
			java.io.FileInputStream inputStream = new java.io.FileInputStream(sourceFilePath);
			
			// 目标文件路径
			String targetFilePath = "c:\\tmp\\snappy_uncompress_stream.txt";

			// 创建目标数据的输出流
			OutputStream outputStream;
			
			outputStream = new FileOutputStream(targetFilePath);
			
			// 使用SnappyCompressorOutputStream进行封装
			SnappyInputStream snappyInputStream = new SnappyInputStream(inputStream);
			BufferedInputStream input = new BufferedInputStream(snappyInputStream);
			
			// 缓冲区大小
			byte[] buffer = new byte[4096];
			int bytesRead;

			// 读取源数据输入流，并将数据写入目标数据输出流
			while ((bytesRead = input.read(buffer)) != -1) {
				outputStream.write(buffer, 0, bytesRead);
			}

			// 刷新输出流，确保所有数据都被写入目标数据输出流
			outputStream.flush();
			
			// 关闭源数据输入流
			outputStream.close();

			// 关闭目标数据输出流
			input.close();
			snappyInputStream.close();	
			inputStream.close();
			
			
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		

	}
	
	
	public static void write_fs(String path, byte[] buff)
	{
		try {
			java.io.FileOutputStream out = new java.io.FileOutputStream(path);
			out.write(buff, 0, buff.length);
			out.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		
	}

}
