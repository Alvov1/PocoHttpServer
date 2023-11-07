#include <iostream>
#include <iomanip>

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServer.h>

class MyRequestHandler final: public Poco::Net::HTTPRequestHandler {
    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) override {
        std::time_t t = std::time(nullptr);
        std::cout << std::put_time(std::localtime(&t), "[%c %Z] GET ") << request.clientAddress() << std::endl;

        const auto data = Poco::format(R"({ "Host": "%s", "Method": "%s", "URI": "%s"})",
                                       request.getHost(),
                                       request.getMethod(),
                                       request.getURI());

        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK); response.setContentType("text/html");
        (response.send() << data).flush();
    }
};

class MyRequestHandlersFactory final : public Poco::Net::HTTPRequestHandlerFactory {
    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request) override {
        return new MyRequestHandler();
    }
};

struct MyServerApplication final : public Poco::Util::ServerApplication {
    int main(const std::vector<std::string>& argv) override {
        if(argv.size() != 1)
            throw std::invalid_argument("Please specify the host address: " + std::string(argv[0]) + " 127.0.0.1:8000");

        Poco::Net::HTTPServerParams::Ptr params = new Poco::Net::HTTPServerParams;
        params->setMaxThreads(20); params->setMaxQueued(100); params->setTimeout(1000);

        const Poco::Net::SocketAddress address(argv[0]);
        const Poco::Net::ServerSocket socket(address);
        const MyRequestHandlersFactory::Ptr factory = new MyRequestHandlersFactory();

        Poco::Net::HTTPServer server(factory, socket, params);
        server.start();
        std::cout << "Endpoint is started on the address " << address << std::endl;

        waitForTerminationRequest(); server.stopAll(); return 0;
    }
};

POCO_SERVER_MAIN(MyServerApplication)