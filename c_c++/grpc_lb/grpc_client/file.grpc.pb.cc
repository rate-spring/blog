// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: file.proto

#include "file.pb.h"
#include "file.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/message_allocator.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace file {

static const char* FileService_method_names[] = {
  "/file.FileService/uploadBlock",
  "/file.FileService/download",
  "/file.FileService/downloadBlock",
};

std::unique_ptr< FileService::Stub> FileService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< FileService::Stub> stub(new FileService::Stub(channel, options));
  return stub;
}

FileService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_uploadBlock_(FileService_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::CLIENT_STREAMING, channel)
  , rpcmethod_download_(FileService_method_names[1], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_downloadBlock_(FileService_method_names[2], options.suffix_for_stats(),::grpc::internal::RpcMethod::SERVER_STREAMING, channel)
  {}

::grpc::ClientWriter< ::file::FileUploadBlockReq>* FileService::Stub::uploadBlockRaw(::grpc::ClientContext* context, ::file::FileUploadBlockRspList* response) {
  return ::grpc::internal::ClientWriterFactory< ::file::FileUploadBlockReq>::Create(channel_.get(), rpcmethod_uploadBlock_, context, response);
}

void FileService::Stub::async::uploadBlock(::grpc::ClientContext* context, ::file::FileUploadBlockRspList* response, ::grpc::ClientWriteReactor< ::file::FileUploadBlockReq>* reactor) {
  ::grpc::internal::ClientCallbackWriterFactory< ::file::FileUploadBlockReq>::Create(stub_->channel_.get(), stub_->rpcmethod_uploadBlock_, context, response, reactor);
}

::grpc::ClientAsyncWriter< ::file::FileUploadBlockReq>* FileService::Stub::AsyncuploadBlockRaw(::grpc::ClientContext* context, ::file::FileUploadBlockRspList* response, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncWriterFactory< ::file::FileUploadBlockReq>::Create(channel_.get(), cq, rpcmethod_uploadBlock_, context, response, true, tag);
}

::grpc::ClientAsyncWriter< ::file::FileUploadBlockReq>* FileService::Stub::PrepareAsyncuploadBlockRaw(::grpc::ClientContext* context, ::file::FileUploadBlockRspList* response, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncWriterFactory< ::file::FileUploadBlockReq>::Create(channel_.get(), cq, rpcmethod_uploadBlock_, context, response, false, nullptr);
}

::grpc::Status FileService::Stub::download(::grpc::ClientContext* context, const ::file::FileDownloadReq& request, ::file::FileDownloadRsp* response) {
  return ::grpc::internal::BlockingUnaryCall< ::file::FileDownloadReq, ::file::FileDownloadRsp, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_download_, context, request, response);
}

void FileService::Stub::async::download(::grpc::ClientContext* context, const ::file::FileDownloadReq* request, ::file::FileDownloadRsp* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::file::FileDownloadReq, ::file::FileDownloadRsp, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_download_, context, request, response, std::move(f));
}

void FileService::Stub::async::download(::grpc::ClientContext* context, const ::file::FileDownloadReq* request, ::file::FileDownloadRsp* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_download_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::file::FileDownloadRsp>* FileService::Stub::PrepareAsyncdownloadRaw(::grpc::ClientContext* context, const ::file::FileDownloadReq& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::file::FileDownloadRsp, ::file::FileDownloadReq, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_download_, context, request);
}

::grpc::ClientAsyncResponseReader< ::file::FileDownloadRsp>* FileService::Stub::AsyncdownloadRaw(::grpc::ClientContext* context, const ::file::FileDownloadReq& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncdownloadRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::ClientReader< ::file::FileDownloadBlockRsp>* FileService::Stub::downloadBlockRaw(::grpc::ClientContext* context, const ::file::FileDownloadBlockReq& request) {
  return ::grpc::internal::ClientReaderFactory< ::file::FileDownloadBlockRsp>::Create(channel_.get(), rpcmethod_downloadBlock_, context, request);
}

void FileService::Stub::async::downloadBlock(::grpc::ClientContext* context, const ::file::FileDownloadBlockReq* request, ::grpc::ClientReadReactor< ::file::FileDownloadBlockRsp>* reactor) {
  ::grpc::internal::ClientCallbackReaderFactory< ::file::FileDownloadBlockRsp>::Create(stub_->channel_.get(), stub_->rpcmethod_downloadBlock_, context, request, reactor);
}

::grpc::ClientAsyncReader< ::file::FileDownloadBlockRsp>* FileService::Stub::AsyncdownloadBlockRaw(::grpc::ClientContext* context, const ::file::FileDownloadBlockReq& request, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::file::FileDownloadBlockRsp>::Create(channel_.get(), cq, rpcmethod_downloadBlock_, context, request, true, tag);
}

::grpc::ClientAsyncReader< ::file::FileDownloadBlockRsp>* FileService::Stub::PrepareAsyncdownloadBlockRaw(::grpc::ClientContext* context, const ::file::FileDownloadBlockReq& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::file::FileDownloadBlockRsp>::Create(channel_.get(), cq, rpcmethod_downloadBlock_, context, request, false, nullptr);
}

FileService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      FileService_method_names[0],
      ::grpc::internal::RpcMethod::CLIENT_STREAMING,
      new ::grpc::internal::ClientStreamingHandler< FileService::Service, ::file::FileUploadBlockReq, ::file::FileUploadBlockRspList>(
          [](FileService::Service* service,
             ::grpc::ServerContext* ctx,
             ::grpc::ServerReader<::file::FileUploadBlockReq>* reader,
             ::file::FileUploadBlockRspList* resp) {
               return service->uploadBlock(ctx, reader, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      FileService_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< FileService::Service, ::file::FileDownloadReq, ::file::FileDownloadRsp, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](FileService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::file::FileDownloadReq* req,
             ::file::FileDownloadRsp* resp) {
               return service->download(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      FileService_method_names[2],
      ::grpc::internal::RpcMethod::SERVER_STREAMING,
      new ::grpc::internal::ServerStreamingHandler< FileService::Service, ::file::FileDownloadBlockReq, ::file::FileDownloadBlockRsp>(
          [](FileService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::file::FileDownloadBlockReq* req,
             ::grpc::ServerWriter<::file::FileDownloadBlockRsp>* writer) {
               return service->downloadBlock(ctx, req, writer);
             }, this)));
}

FileService::Service::~Service() {
}

::grpc::Status FileService::Service::uploadBlock(::grpc::ServerContext* context, ::grpc::ServerReader< ::file::FileUploadBlockReq>* reader, ::file::FileUploadBlockRspList* response) {
  (void) context;
  (void) reader;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status FileService::Service::download(::grpc::ServerContext* context, const ::file::FileDownloadReq* request, ::file::FileDownloadRsp* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status FileService::Service::downloadBlock(::grpc::ServerContext* context, const ::file::FileDownloadBlockReq* request, ::grpc::ServerWriter< ::file::FileDownloadBlockRsp>* writer) {
  (void) context;
  (void) request;
  (void) writer;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace file
