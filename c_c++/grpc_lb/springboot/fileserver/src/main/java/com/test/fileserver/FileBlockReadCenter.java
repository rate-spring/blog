package com.test.fileserver;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;


class FileBlockRead
{
	public FileBlockRead(String path)
	{
		_path = path;
		_last_time = 0;
		
		try {
			_is = new FileInputStream(_path);
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public int read(byte[] data)
	{
		_last_time = System.currentTimeMillis();
		
		int r = 0;
		try {
			r = _is.read(data);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			
			return -1;
		}
		
		return r;
	}
	
	public void close()
	{
		if (_is != null)
		{
			try {
				_is.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			_is = null;
		}
	}
	
	public long getLastTime()
	{
		return _last_time;
	}
	
	private String _path;
	private FileInputStream _is = null;
	private long _last_time;
	
}

public class FileBlockReadCenter {
	
	public long readyRead(String path)
	{
		File file = new File(path);
		if (!file.exists())
		{
			return -1;
		}
		
		long fsize = file.length();
		
		synchronized(_mutex)
		{
			FileBlockRead read = _map.get(path);
			
			if (read != null)
			{
				read.close();
				
				_map.remove(path);
			}
			
			read = new FileBlockRead(path);
			
			_map.put(path,read);
		}
		
		return fsize;
	}

	public int read(String path, byte[] data)
	{
		FileBlockRead read = null;
		
		synchronized(_mutex)
		{
			read = _map.get(path);
		}
		
		if (read != null)
		{
			return read.read(data);
		}
		
		read = new FileBlockRead(path);
		
		int r = read.read(data);
		
		synchronized(_mutex)
		{
			_map.put(path,read);
		}
		
		return r;
	}
	
	public void close(String path)
	{
		FileBlockRead read = null;
		
		synchronized(_mutex)
		{
			read = _map.get(path);
			
			_map.remove(path);
		}
		
		if (read != null)
		{
			read.close();
		}
	}
	
	public int init()
	{
		_mutex = new Object();
		_map = new HashMap<String, FileBlockRead>();
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
				
				FileBlockRead read = (FileBlockRead) entry.getValue();
				
				read.close();
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
				
				FileBlockRead read = (FileBlockRead) entry.getValue();
				
				if (read.getLastTime() != 0 && nowtime - read.getLastTime() > 3 * 60 * 1000)
				{
					itr.remove();
					
					read.close();
				}	
			}
		}
		
		_last_check_time = nowtime;
	}
	
	public long getLastCheckTime()
	{
		return _last_check_time;
	}
	
	private HashMap<String, FileBlockRead> _map = null;
	
	private Object _mutex = null;
	
	private long _last_check_time = 0;
	
}
