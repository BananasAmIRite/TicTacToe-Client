#include "session.h"
#include <future>


void run_ioc(net::io_context* ioc) {
	ioc->run();
}

//void start_read_handler(shared_ptr<session> wsSession) {
//	while (!wsSession->isClosed()) {
//		string line;
//		getline(cin, line);
//		wsSession->write(line);
//	}
//}

int main() {
	// start the websocket session
	net::io_context ioc;
	shared_ptr<session> wsSession = make_shared<session>(ioc);

	bool running = true; 
	bool running = true;

	// setup events
	wsSession->setFailHandler([](beast::error_code ec, const char* err) {
		cout << "error occurred: " << ec.message() << endl; 
		}); // lambda cuz I want to retain `this`
	wsSession->setReadHandler([](beast::error_code ec, size_t bt, boost::beast::flat_buffer buf) {
		// i dont know how to read the buffer :/
		cout << "message received: " << boost::beast::buffers_to_string(buf.data()) << endl;
	});

	wsSession->setHandshakeHandler([wsSession](beast::error_code ec) {
		cout << "connected!" << endl; 
		//future<void> readThread = async(&start_read_handler, wsSession); // i tried to do a hacky little thing here, dont worry about it
		}); 

	wsSession->run("localhost", "2000");

	future<void> e = async(&run_ioc, &ioc);

	while (true) {
		string line;
		getline(cin, line);
		wsSession->write(line);
	}

	//cout << "ioc run finished!" << endl; 


	return 0; 
}

	// setup events
	wsSession->setFailHandler([](beast::error_code ec, const char* err) {
		cout << "error occurred: " << ec.message() << endl;
		}); // lambda cuz I want to retain `this`
	wsSession->setReadHandler([](beast::error_code ec, size_t bt, boost::beast::flat_buffer buf) {
		// i dont know how to read the buffer :/
		cout << "message received: " << boost::beast::buffers_to_string(buf.data()) << endl;
		});

	wsSession->setHandshakeHandler([wsSession](beast::error_code ec) {
		cout << "connected!" << endl;
		//future<void> readThread = async(&start_read_handler, wsSession); // i tried to do a hacky little thing here, dont worry about it
		});

	wsSession->run("localhost", "2000");

	future<void> e = async(&run_ioc, &ioc);

	while (true) {
		string line;
		getline(cin, line);
		wsSession->write(line);
	}

	//cout << "ioc run finished!" << endl; 


	return 0;
}
