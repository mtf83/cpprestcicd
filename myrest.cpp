#include "../Release/include/cpprest/http_listener.h"
#include "../Release/include/cpprest/json.h"
#include <iostream>
#include <boost/asio/impl/src.hpp>
#include <boost/asio/ssl/impl/src.hpp>
#include <string>

using namespace std;
using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;

#define TRACE(msg)  cout << msg
#define TRACE_ACTION(a, k, v)   cout << a << L" (" << k << L", " << v << L")\n"

void handle_get(http_request request) {
    TRACE("\nhandle GET\n");
}

void handle_post(http_request request) {
    TRACE("\nhandle POST\n");
    std::cout << utility::conversions::to_utf8string(request.to_string()) << std::endl;
request
    .extract_json()
    .then([request](pplx::task<json::value> task) {
    try {
        auto const jvalue = task.get();
        string sTableName = utility::conversions::to_utf8string(jvalue.at(U("tableName")).as_string());
        auto array = jvalue.at(U("tableRecords")).as_array();
        auto name = array[0].at(U("name")).as_string();
        auto month = array[0].at(U("month")).as_string();
        auto category = array[0].at(U("category")).as_string();
        auto task = array[0].at(U("taskName")).as_string();
        auto effort = array[0].at(U("efforts")).as_string();
        auto efforts = stoi(effort);
        cout << " name : " << utility::conversions::to_utf8string(name) << endl;
        cout << " month : " << utility::conversions::to_utf8string(month) << endl;
        cout << " category : " << utility::conversions::to_utf8string(category) << endl;
        cout << " task : " << utility::conversions::to_utf8string(task) << endl;
        cout << " efforts : " << efforts << endl;
        return;
    }
    catch (http_exception& e) {
        std::cout << e.what() << std::endl;
        json::value jsonResponse;
        jsonResponse[U("error")] = json::value::string(U("record not found"));
        request.reply(status_codes::OK, jsonResponse);
    }
        }).wait();
        return;
}

void handle_put(http_request request) {
    TRACE("\nhandle PUT\n");
}

void handle_delete(http_request request) {
    TRACE("\nhandle DELETE\n");
}

int main() {
    http_listener_config listen_config;
    listen_config.set_ssl_context_callback([](ssl::context& ctx) {
        try {
            std::clog << "set_ssl_context_callback" << std::endl;
            ctx.set_options(ssl::context::default_workarounds |
                ssl::context::no_sslv2 | ssl::context::no_tlsv1 |
                ssl::context::no_tlsv1_1 |
                ssl::context::single_dh_use);
            ctx.set_password_callback(
                [](std::size_t,
                    ssl::context::password_purpose) {
                        return "6.CAtao";
                });
            //ctx.use_certificate_chain_file("c:\\ca-certificate.pem.txt");
            //ctx.use_private_key_file("c:\\ca-key.pem.txt", ssl::context::pem);
            ctx.use_certificate_file("/home/u/server.crt", ssl::context::pem);
            ctx.use_private_key_file("/home/u/server.key", ssl::context::pem);
            //ctx.use_certificate_chain_file("server.key");
            //ctx.use_private_key_file("server.key", ssl::context::pem);
            ctx.use_tmp_dh_file("/home/u/dh2048.pem");
            std::clog << "leave set_ssl_context_callback" << std::endl;
        }
        catch (std::exception const& e) {
            std::clog << "ERROR: " << e.what() << "\n";
        }
        });

    listen_config.set_timeout(utility::seconds(10));
    http_listener listener(U("https://localhost:10022")
        , listen_config
    ); // Server URL, Port .

    listener.support(methods::GET, [](http_request req) {
        std::clog << "enter handler" << std::endl;
        auto j = json::value::object();
        auto path = req.request_uri().path();
        std::clog << "path. " << std::endl;
        j[U("one")] =
            json::value::string(U("asdfasefasdfaefasdfasefasdfefasefasefaqf3wfs"
                "efasdfasefasdfzsfzdfaesfzsefzsdfzsef"));
        req.reply(status_codes::OK, j).wait();
        std::clog << "leave handler" << std::endl;
        });
    listener.support(methods::POST, [](http_request req) {handle_post(req); });
    listener.support(methods::PUT, [](http_request req) {handle_put(req); });
    listener.support(methods::DEL, [](http_request req) {handle_delete(req); });

    listener.open()
        .then([&listener] { std::clog << (U("\n start!!\n")); })
        .wait(); // Server open
    while (true)
        ;
    listener.close(); // huh
}