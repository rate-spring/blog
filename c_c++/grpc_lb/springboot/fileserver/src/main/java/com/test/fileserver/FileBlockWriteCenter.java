package com.test.fileserver;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;


class FileBlockWrite
{
	public FileBlockWrite(String path)
	{
		_path = path;
		_last_time = 0;
		
		File file = new File(path);
		if (file.exists())
		{
			file.delete();
		}
		
		try {
			_os = new FileOutputStream(_path);
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public int write(com.google.protobuf.ByteString data)
	{
		_last_time = System.currentTimeMillis();
		
		try {
			data.writeTo(_os);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			
			return -1;
		}
		
		return 0;
	}
	
	public void close()
	{
		if (_os != null)
		{
			try {
				_os.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			_os = null;
		}
	}
	
	public long getLastTime()
	{
		return _last_time;
	}
	
	private String _path;
	private FileOutputStream _os = null;
	private long _last_time;
	
}

public class FileBlockWriteCenter {
	
	public int write(String path, com.google.protobuf.ByteString data)
	{
		FileBlockWrite write = null;
		
		synchronized(_mutex)
		{
			write = _map.get(path);
		}
		
		if (write != null)
		{
			return write.write(data);
		}
		
		write = new FileBlockWrite(path);
		
		int r = write.write(data);
		
		synchronized(_mutex)
		{
			_map.put(path,write);
		}
		
		return r;
	}
	
	public void close(String path)
	{
		FileBlockWrite write = null;
		
		synchronized(_mutex)
		{
			write = _map.get(path);
			
			_map.remove(path);
		}
		
		if (write != null)
		{
			write.close();
		}
	}
	
	public int init()
	{
		_mutex = new Object();
		_map = new HashMap<String, FileBlockWrite>();
		_last_check_time = 0;
		
		return 0;
	}
	
	public void uninit()
	{
		synchronized(_mutex)
		{
			Iterator itr = _map.entrySet().iterator();
			
			while(itr.hasNext())
			{
				Map.Entry entry = (Map.Entry) itr.next();
				
				FileBlockWrite write = (FileBlockWrite) entry.getValue();
				
				write.close();
			}
			
			_map.clear();
		}
		
		_mutex = null;
		_map = null;
		_last_check_time = 0;
	}
	
	public void check()
	{
		long nowtime = System.currentTimeMillis();
		
		synchronized(_mutex)
		{
			Iterator itr = _map.entrySet().iterator();
			
			while(itr.hasNext())
			{
				Map.Entry entry = (Map.Entry) itr.next();
				
				FileBlockWrite write = (FileBlockWrite) entry.getValue();
				
				if (write.getLastTime() != 0 && nowtime - write.getLastTime() > 3 * 60 * 1000)
				{
					itr.remove();
					
					write.close();
				}	
			}
		}
		
		_last_check_time = nowtime;
	}
	
	public long getLastCheckTime()
	{
		return _last_check_time;
	}
	
	private HashMap<String, FileBlockWrite> _map = null;
	
	private Object _mutex = null;
	
	private long _last_check_time = 0;
	

}
