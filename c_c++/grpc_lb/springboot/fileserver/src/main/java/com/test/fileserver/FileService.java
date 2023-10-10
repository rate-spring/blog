package com.test.fileserver;

import io.grpc.Server;
import io.grpc.ServerBuilder;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.io.IOException;
import java.util.concurrent.TimeUnit;
import java.util.logging.Logger;

@Service
public class FileService {

    private static final Logger logger = Logger.getLogger(FileService.class.getName());
    
    public static final int RUN_PORT = 9897;

    private Server server;


    @Autowired
    FileImpl fileImplService;


    public  void start(){

        try {
            server = ServerBuilder.forPort(RUN_PORT)
                    .addService(fileImplService)
                    .build()
                    .start();
        } catch (IOException e) {
            e.printStackTrace(System.err);
        }
        logger.info("Server started, listening on " + RUN_PORT);
        

        Runtime.getRuntime().addShutdownHook(new Thread() {
            @Override
            public void run() {
                System.err.println("*** shutting down gRPC server since JVM is shutting down");
                FileService.this.stop();
                System.err.println("*** server shut down");
            }
        });
        FileService.this.blockUntilShutdown();
    }


    private void stop() {
        if (server != null) {
            try {
                server.shutdown().awaitTermination(30, TimeUnit.SECONDS);
            } catch (InterruptedException e) {
                e.printStackTrace(System.err);
            }
        }
    }

    private void blockUntilShutdown() {
        if (server != null) {
            try {
                server.awaitTermination();
            } catch (InterruptedException e) {
                e.printStackTrace(System.err);
            }
        }
    }


}