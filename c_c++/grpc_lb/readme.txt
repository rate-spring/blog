
GRPC + Nginx负载 + SpringBoot 开发实践

需求：
1. GRPC c客户端 上传下载文件
2. 服务端 使用 SpringBoot java 开发
3. 服务端 需要多节点部署，使用 Nginx 做负载分发
4. GRPC 与 Nginx 之间使用 ssl/tls 加密通信

实现：
1.GRPC (c++/linux/windows)
1.46.7 是最后一个支持c++11标准的版本，1.47.0使用c++14. windows 上面IDE是 vs2015 sp3.

git clone -b v1.46.7 https://github.com/grpc/grpc
cd grpc
git submodule update --init

linux:

mkdir build
cd build
cmake .. -DgRPC_INSTALL=ON -DCMAKE_BUILD_TYPE=Release
make
make install
cp *.a /usr/local/lib
cp third_party/boringssl-with-bazel/libssl.a /usr/local/lib
cp third_party/boringssl-with-bazel/libcrypto.a /usr/local/lib
cp grpc_cpp_plugin /usr/local/bin

linux下 qt creator pro文件：

INCLUDEPATH += $$system(pkg-config --cflags gpr) $$system(pkg-config --cflags grpc) $$system(pkg-config --cflags grpc++) $$system(pkg-config --cflags grpc_unsecure) \
        $$system(pkg-config --cflags grpc++_unsecure)

unix {
    message(proto file 开始编译...)
    PROTO_MESSAGE_C1 = $$system("protoc -I=./ --cpp_out=./ --grpc_out=./ --plugin=protoc-gen-grpc="/usr/local/bin/grpc_cpp_plugin" file.proto")
    message(编译完成.)
}

LIBS += -L/usr/local/lib -lpthread -lprotobuf  -lcrypto -lssl \
        $$system(pkg-config --libs gpr) \
        $$system(pkg-config --libs grpc) \
        $$system(pkg-config --libs grpc++) \
        $$system(pkg-config --libs grpc_unsecure) \
        $$system(pkg-config --libs grpc++_unsecure)

windows:

cmake-gui 选择目录，选择vs2015 ,设置 win32
configure
generate
vs2015 打开 grpc.sln ,选择 release win32  ALL_BUILD 工程 编译
选择 INSTALL 工程 安装
从 c:\program files(x86)\grpc\ 下面拷贝文件，包含 Include lib bin

windows 程序连接库:

absl_bad_any_cast_impl.lib
absl_bad_optional_access.lib
absl_bad_variant_access.lib
absl_base.lib
absl_city.lib
absl_civil_time.lib
absl_cord.lib
absl_cordz_functions.lib
absl_cordz_handle.lib
absl_cordz_info.lib
absl_cordz_sample_token.lib
absl_cord_internal.lib
absl_debugging_internal.lib
absl_demangle_internal.lib
absl_examine_stack.lib
absl_exponential_biased.lib
absl_failure_signal_handler.lib
absl_flags.lib
absl_flags_commandlineflag.lib
absl_flags_commandlineflag_internal.lib
absl_flags_config.lib
absl_flags_internal.lib
absl_flags_marshalling.lib
absl_flags_parse.lib
absl_flags_private_handle_accessor.lib
absl_flags_program_name.lib
absl_flags_reflection.lib
absl_flags_usage.lib
absl_flags_usage_internal.lib
absl_graphcycles_internal.lib
absl_hash.lib
absl_hashtablez_sampler.lib
absl_int128.lib
absl_leak_check.lib
absl_leak_check_disable.lib
absl_log_severity.lib
absl_low_level_hash.lib
absl_malloc_internal.lib
absl_periodic_sampler.lib
absl_random_distributions.lib
absl_random_internal_distribution_test_util.lib
absl_random_internal_platform.lib
absl_random_internal_pool_urbg.lib
absl_random_internal_randen.lib
absl_random_internal_randen_hwaes.lib
absl_random_internal_randen_hwaes_impl.lib
absl_random_internal_randen_slow.lib
absl_random_internal_seed_material.lib
absl_random_seed_gen_exception.lib
absl_random_seed_sequences.lib
absl_raw_hash_set.lib
absl_raw_logging_internal.lib
absl_scoped_set_env.lib
absl_spinlock_wait.lib
absl_stacktrace.lib
absl_status.lib
absl_statusor.lib
absl_strerror.lib
absl_strings.lib
absl_strings_internal.lib
absl_str_format_internal.lib
absl_symbolize.lib
absl_synchronization.lib
absl_throw_delegate.lib
absl_time.lib
absl_time_zone.lib
address_sorting.lib
cares.lib
crypto.lib
gpr.lib
grpc++.lib
grpc++_alts.lib
grpc++_error_details.lib
grpc++_reflection.lib
grpc.lib
grpcpp_channelz.lib
libprotobuf.lib
re2.lib
ssl.lib
upb.lib
zlibstatic.lib

2.Nginx
Nginx 1.13.10 后支持 grpc 反向代理。从官网下载 1.25.2 后解压

修改文件： src/http/modules/ngx_http_upstream_ip_hash_module.c
搜索 ngx_http_upstream_init_ip_hash_peer( 函数，修改 AF_INET 下面
    ...
    iphp_addrlen = 4;   // 默认是3;
让nginx ip_hash 负载 按照客户端Ip4位长度分发。

./configure --with-http_ssl_module --with-http_v2_module --with-http_gzip_static_module --with-http_stub_status_module
make
su make install

默认程序安装到 /usr/local/nginx 目录下

使用openssl 生成 证书


openssl genrsa -passout pass:andin -des3 -out ca.key 4096
openssl req -passin pass:andin -new -x509 -days 365 -key ca.key -out ca.crt -subj "/C=CN/ST=Hubei/L=Wuhan/O=andin/OU=andin/CN=Root"

openssl genrsa -passout pass:andin -des3 -out server.key 4096
openssl req -passin pass:andin -new -key server.key -out server.csr -subj "/C=CN/ST=Hubei/L=Wuhan/O=andin/OU=andin/CN=fileserver.test.com"
openssl x509 -req -passin pass:andin -days 10000 -in server.csr -CA ca.crt -CAkey ca.key -set_serial 01 -out server.crt

openssl rsa -passin pass:andin -in server.key -out server.key

openssl genrsa -passout pass:andin -des3 -out client.key 4096
openssl req -passin pass:andin -new -key client.key -out client.csr -subj "/C=CN/ST=Hubei/L=Wuhan/O=andin/OU=andin/CN=fileclient"
openssl x509 -passin pass:andin -req -days 10000 -in client.csr -CA ca.crt -CAkey ca.key -set_serial 01 -out client.crt

openssl rsa -passin pass:andin -in client.key -out client.key

nginx.conf 配置如下：

server.crt server.key 为上面生成。

http {
    include       mime.types;
    default_type  application/octet-stream;

    #log_format  main  '$remote_addr - $remote_user [$time_local] "$request" '
    #                  '$status $body_bytes_sent "$http_referer" '
    #                  '"$http_user_agent" "$http_x_forwarded_for"';

    #access_log  logs/access.log  main;

    sendfile        on;
    #tcp_nopush     on;

    #keepalive_timeout  0;
    #keepalive 2000 ;
    keepalive_timeout  120s ;
    keepalive_requests 4294967295 ;
    reset_timedout_connection on;

    client_max_body_size  150m ;
    client_body_buffer_size  150m ;

    proxy_connect_timeout  300s ;
    proxy_send_timeout     300s ;
    proxy_read_timeout     300s ;
    send_timeout           300s ;

    #gzip  on;

    upstream grpcservers {
        ip_hash;
        server 192.168.3.82:9898;
        server 192.168.3.82:9897;

        keepalive 2000 ;
        keepalive_timeout  120s ;
        keepalive_requests 4294967295 ;
    }

    server {
        listen 9899 ssl  ;
        http2 on ;
        #server_name  localhost;

        ssl_certificate "/usr/local/nginx/conf/server.crt" ;
        ssl_certificate_key "/usr/local/nginx/conf/server.key" ;

	# intermediate configuration
    	ssl_protocols TLSv1.2;
    	ssl_ciphers ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:DHE-RSA-AES128-GCM-SHA256:DHE-RSA-AES256-GCM-SHA384:DHE-RSA-CHACHA20-POLY1305;
    	ssl_prefer_server_ciphers off;

        #charset koi8-r;

        #access_log  logs/host.access.log  main;

        #location / {
        #    root   html;
        #    index  index.html index.htm;
        #}

        location / {
            grpc_pass grpc://grpcservers;
        }
}



3.SpringBoot

springboot 工程，pom 依赖：

	<dependency>
	       <groupId>org.springframework.boot</groupId>
	       <artifactId>spring-boot-starter-web</artifactId>
	       <version>2.7.14</version>
	   </dependency>
	   
	   <!--springboot-->
		<dependency>
			<groupId>org.springframework.boot</groupId>
			<artifactId>spring-boot-starter</artifactId>
		</dependency>
		
		<!--Springboottest-->
		<dependency>
			<groupId>org.springframework.boot</groupId>
			<artifactId>spring-boot-starter-test</artifactId>
			<scope>test</scope>
		</dependency>
		
		<!--grpc-->
		<dependency>
			<groupId>io.grpc</groupId>
			<artifactId>grpc-api</artifactId>
			<version>1.46.1</version>
		</dependency>
		<dependency>
			<groupId>io.grpc</groupId>
			<artifactId>grpc-protobuf</artifactId>
			<version>1.46.1</version>
		</dependency>
		
		<dependency>
			<groupId>io.grpc</groupId>
			<artifactId>grpc-stub</artifactId>
			<version>1.46.1</version>
		</dependency>
		
		<dependency>
			<groupId>io.grpc</groupId>
			<artifactId>grpc-netty</artifactId>
			<version>1.46.1</version>
		</dependency>

详见 java 代码

4. 其他
GRPC 客户端 使用 Openssl 生成的相关证书： ca,crt, client.key, client.crt
客户端连接的nginx 9899 端口，服务器地址使用域名，不能使用Ip, 域名跟 server.crt 中的一致。上面例子中为: fileserver.test.com
因此需要搭建自己的dns服务器。
如果有现成的dns服务器，直接加配置，没有的话，需要安装bind9, 或从源码编译。
