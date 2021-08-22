#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include "helloworld.grpc.pb.h"
#include "landing.grpc.pb.h"
#include <glog/logging.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using helloworld::Greeter;
using helloworld::HelloReply;
using helloworld::HelloRequest;
using org::feuyeux::grpc::TalkRequest;
using org::feuyeux::grpc::TalkResponse;
using org::feuyeux::grpc::LandingService;
using org::feuyeux::grpc::TalkResult;
using org::feuyeux::grpc::ResultType;

class LandingClient {
public:
    LandingClient(std::shared_ptr<Channel> channel) : stub_(LandingService::NewStub(channel)) {}

    void Talk() {
        TalkRequest talkRequest;
        talkRequest.set_data("hello");
        talkRequest.set_meta("c++");

        TalkResponse talkResponse;
        ClientContext context;
        Status status = stub_->Talk(&context, talkRequest, &talkResponse);
        if (status.ok()) {
            std::cout << "Talk status:" << talkResponse.status() << std::endl;
            const TalkResult &talkResult = talkResponse.results(0);
            ResultType resultType = talkResult.type();
            std::cout << "Talk result:" << resultType << std::endl;
        } else {
            std::cout << "Talk error:" << status.error_code() << ": " << status.error_message()
                      << std::endl;
        }
    }

private:
    std::unique_ptr<LandingService::Stub> stub_;
};

class GreeterClient {
public:
    GreeterClient(std::shared_ptr<Channel> channel) : stub_(Greeter::NewStub(channel)) {}

    // Assembles the client's payload, sends it and presents the response back
    // from the server.
    std::string SayHello(const std::string &user) {
        // Data we are sending to the server.
        HelloRequest request;
        request.set_name(user);

        // Container for the data we expect from the server.
        HelloReply reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        // The actual RPC.
        Status status = stub_->SayHello(&context, request, &reply);

        // Act upon its status.
        if (status.ok()) {
            return reply.message();
        } else {
            std::cout << status.error_code() << ": " << status.error_message()
                      << std::endl;
            return "RPC failed";
        }
    }

private:
    std::unique_ptr<Greeter::Stub> stub_;
};

int main(int argc, char **argv) {
    /*日志文件名 <program name>.<host name>.<user name>.log.<Severity level>.<date>-<time>.<pid> */
    google::InitGoogleLogging(argv[0]);
    google::SetLogDestination(google::INFO, "/Users/han/hello_grpc/");
    FLAGS_colorlogtostderr=true;
    FLAGS_alsologtostderr = 1;

    LOG(INFO) << "Hello gRPC C++ Client is starting...";

    // Instantiate the client. It requires a channel, out of which the actual RPCs
    // are created. This channel models a connection to an endpoint specified by
    // the argument "--target=" which is the only expected argument.
    // We indicate that the channel isn't authenticated (use of
    // InsecureChannelCredentials()).
    std::string target_str;
    std::string arg_str("--target");
    if (argc > 1) {
        std::string arg_val = argv[1];
        size_t start_pos = arg_val.find(arg_str);
        if (start_pos != std::string::npos) {
            start_pos += arg_str.size();
            if (arg_val[start_pos] == '=') {
                target_str = arg_val.substr(start_pos + 1);
            } else {
                std::cout << "The only correct argument syntax is --target="
                          << std::endl;
                return 0;
            }
        } else {
            std::cout << "The only acceptable argument is --target=" << std::endl;
            return 0;
        }
    } else {
        target_str = "localhost:50051";
    }
    const std::shared_ptr<Channel> &channel = grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials());
    //
    GreeterClient greeter(channel);
    std::string user("world");
    std::string reply = greeter.SayHello(user);
    std::cout << "Greeter received: " << reply << std::endl;
    //
    LandingClient landingClient(channel);
    landingClient.Talk();
    LOG(WARNING) << "Hello gRPC C++ Client is stopping";
    google::ShutdownGoogleLogging();
    return 0;
}
