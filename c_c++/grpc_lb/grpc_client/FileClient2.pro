TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

PKGCONFIG += gpr grpc grpc++ grpc_unsecure grpc++_unsecure

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

SOURCES += main.cpp \
    file.grpc.pb.cc \
    file.pb.cc

DISTFILES += \
    file.proto

HEADERS += \
    file.grpc.pb.h \
    file.pb.h
