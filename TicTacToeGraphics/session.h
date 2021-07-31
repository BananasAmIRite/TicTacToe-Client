#ifndef WSCLIENT_H
#define WSCLIENT_H

#include "wxPrecomp.h"
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

//------------------------------------------------------------------------------

// Sends a WebSocket message and prints the response
class session : public std::enable_shared_from_this<session>
{
    private:
        tcp::resolver resolver_;
        websocket::stream<beast::tcp_stream> ws_;
        beast::flat_buffer buffer_;
        std::string host_;

        // private event handlers that link to public listeners
        void on_resolve(beast::error_code, tcp::resolver::results_type); 
        void on_connect(beast::error_code, tcp::resolver::results_type::endpoint_type); 
        void on_handshake(beast::error_code); 
        void on_write(beast::error_code, std::size_t); 
        void on_read(beast::error_code, std::size_t); 
        void on_close(beast::error_code); 
        
        // error handler
        void fail(beast::error_code, char const*); 

        // public listeners
        function<void(beast::error_code, tcp::resolver::results_type)> resolveHandler; 
        function<void(beast::error_code, tcp::resolver::results_type::endpoint_type)> connectHandler; 
        function<void(beast::error_code)> handshakeHandler; 
        function<void(beast::error_code, std::size_t)> writeHandler; 
        function<void(beast::error_code, std::size_t, beast::flat_buffer)> readHandler;
        function<void(beast::error_code)> closeHandler;
        function<void(beast::error_code, char const*)> failHandler; 


    public:
        // Resolver and socket require an io_context
        session(net::io_context&); 
        // Start the asynchronous operation
        void run(char const*, char const*);

        void send(string);
        void setResolveHandler(function<void(beast::error_code, tcp::resolver::results_type)>); 
        void setConnectHandler(function<void(beast::error_code, tcp::resolver::results_type::endpoint_type)>); 
        void setHandshakeHandler(function<void(beast::error_code)>); 
        void setWriteHandler(function<void(beast::error_code, std::size_t)>); 
        void setReadHandler(function<void(beast::error_code, std::size_t, beast::flat_buffer)>); 
        void setCloseHandler(function<void(beast::error_code)>); 
        void setFailHandler(function<void(beast::error_code, char const*)>);
};

#endif
