#include "session.h"

session::session(net::io_context& ioc) : resolver_(net::make_strand(ioc)), ws_(net::make_strand(ioc)), open(false) {}

void session::run(char const* host, char const* port) {
    // Save these for later
    host_ = host;

    // Look up the domain name
    resolver_.async_resolve(
        host,
        port,
        beast::bind_front_handler(
            &session::on_resolve,
            shared_from_this()));
}

void session::on_resolve(beast::error_code ec, tcp::resolver::results_type results) {
    if (ec)
        return fail(ec, "resolve");


    // Set the timeout for the operation
    beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

    // Make the connection on the IP address we get from a lookup
    beast::get_lowest_layer(ws_).async_connect(
        results,
        beast::bind_front_handler(
            &session::on_connect,
            shared_from_this()));

    if (resolveHandler != NULL) resolveHandler(ec, results);
}

void session::on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep) {
    if (ec)
        return fail(ec, "connect");

    // Turn off the timeout on the tcp_stream, because
// the websocket stream has its own timeout system.
    beast::get_lowest_layer(ws_).expires_never();

    // Set suggested timeout settings for the websocket
    ws_.set_option(
        websocket::stream_base::timeout::suggested(
            beast::role_type::client));

    // Set a decorator to change the User-Agent of the handshake
    ws_.set_option(websocket::stream_base::decorator(
        [](websocket::request_type& req)
        {
            req.set(http::field::user_agent,
                std::string(BOOST_BEAST_VERSION_STRING) +
                " websocket-client-async");
        }));

    // Update the host_ string. This will provide the value of the
    // Host HTTP header during the WebSocket handshake.
    // See https://tools.ietf.org/html/rfc7230#section-5.4
    host_ += ':' + std::to_string(ep.port());

    // Perform the websocket handshake
    ws_.async_handshake(host_, "/",
        beast::bind_front_handler(
            &session::on_handshake,
            shared_from_this()));

    if (connectHandler != NULL) connectHandler(ec, ep);
}

void session::on_handshake(beast::error_code ec) {
    if (ec)
        return fail(ec, "handshake");

    // Send the message
    //ws_.async_write(
    //    net::buffer(text_),
    //    beast::bind_front_handler(
    //        &session::on_write,
    //        shared_from_this()));
    open = true;
    ws_.async_read( // start the read loop
        buffer_,
        beast::bind_front_handler(
            &session::on_read,
            shared_from_this()));

    if (handshakeHandler != NULL) handshakeHandler(ec);
}

void session::fail(beast::error_code ec, char const* err) {
    if (failHandler != NULL) failHandler(ec, err);
    open = false;
}

void session::on_write(
    beast::error_code ec,
    std::size_t bytes_transferred)
{
    if (ec)
        return fail(ec, "write");

    //// Read a message into our buffer
    //ws_.async_read(
    //    buffer_,
    //    beast::bind_front_handler(
    //        &session::on_read,
    //        shared_from_this()));
    if (writeHandler != NULL) writeHandler(ec, bytes_transferred);
}

void session::on_read(
    beast::error_code ec,
    std::size_t bytes_transferred)
{
    // boost::ignore_unused(bytes_transferred);

    if (ec)
        return fail(ec, "read");

    //// Close the WebSocket connection
    //ws_.async_close(websocket::close_code::normal,
    //    beast::bind_front_handler(
    //        &session::on_close,
    //        shared_from_this()));
    if (readHandler != NULL) readHandler(ec, bytes_transferred, buffer_);

    buffer_.clear();

    // keep reading :D
    ws_.async_read(
        buffer_,
        beast::bind_front_handler(
            &session::on_read,
            shared_from_this()));
}

void session::on_close(beast::error_code ec)
{
    if (ec)
        return fail(ec, "close");

    // If we get here then the connection is closed gracefully

    // The make_printable() function helps print a ConstBufferSequence
    //std::cout << beast::make_printable(buffer_.data()) << std::endl;
    if (closeHandler != NULL) closeHandler(ec);
}

// public event handlers
void session::setResolveHandler(function<void(beast::error_code, tcp::resolver::results_type)> handler) {
    resolveHandler = handler;
}
void session::setConnectHandler(function<void(beast::error_code, tcp::resolver::results_type::endpoint_type)> handler) {
    connectHandler = handler;
}
void session::setHandshakeHandler(function<void(beast::error_code)> handler) {
    handshakeHandler = handler;
}
void session::setWriteHandler(function<void(beast::error_code, std::size_t)> handler) {
    writeHandler = handler;
}
void session::setReadHandler(function<void(beast::error_code, std::size_t, beast::flat_buffer)> handler) {
    readHandler = handler;
}
void session::setCloseHandler(function<void(beast::error_code)> handler) {
    closeHandler = handler;
}
void session::setFailHandler(function<void(beast::error_code, char const*)> handler) {
    failHandler = handler;
}

void session::write(string text) {
    beast::error_code ec;
    size_t bt = ws_.write(net::buffer(text), ec);

    on_write(ec, bt);
}

bool session::isClosed() {
    return !open;
}