package com.test.fileserver;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.context.ConfigurableApplicationContext;


@SpringBootApplication
public class App {

	public static void main(String[] args) {
 
		ConfigurableApplicationContext run = SpringApplication.run(App.class, args);
		
		FileService fileService = (FileService) run.getBean("fileService");
		try {
			fileService.start();
		} catch (Exception e) {
			e.printStackTrace();
		}
		
	}

}
