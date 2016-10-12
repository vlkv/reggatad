#include "service.h"
#include "processor.h"

#include <gtest/gtest.h>

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <memory>
#include <iostream>

using boost::asio::ip::tcp;

class myTestFixture1: public testing::Test {
public:


	boost::asio::io_service _ios;
	boost::asio::ip::tcp::socket _socket;
	boost::thread _t;

	myTestFixture1() :
		_socket(_ios),
		_t(&myTestFixture1::startReggataD, this) {
		boost::this_thread::sleep(boost::posix_time::seconds(2));
	}

	void startReggataD() {
		std::cout << "startReggataD()" << std::endl;
		std::shared_ptr<Processor> _proc;
		std::unique_ptr<Service> _service;
		_proc = std::make_shared<Processor>();
		auto rootPath = std::string("/home/vitvlkv/Pictures");
		auto dbPath = std::string("/home/vitvlkv/Pictures/.reggata");
		_proc->openRepo(rootPath, dbPath);
		_service.reset(new Service(9100, _proc));
		_service->start();
	}

	void SetUp() {
		std::cout << "SetUp() called" << std::endl;
		boost::asio::ip::tcp::endpoint endpoint(
			boost::asio::ip::address::from_string("127.0.0.1"), 9100);
		std::cout << "Connecting..." << std::endl;
		_socket.connect(endpoint);
	}

	boost::system::error_code sendMsg(const std::string& message) {
		std::vector<char> buf;
		buf.resize(4 + message.size());
		uint32_t header = message.size();
		std::cout << "message size is " << header << std::endl;
		std::copy((char*)&header, (char*)&header+4, buf.begin());
		std::copy(message.begin(), message.end(), buf.begin()+4);
		boost::system::error_code error;
		_socket.write_some(boost::asio::buffer(buf), error);
		return error;
	}

	std::string recvMsg() {
		boost::array<char, 128> buf;
		boost::system::error_code error;
		size_t len = _socket.read_some(boost::asio::buffer(buf), error);
		std::string message(buf.c_array());
		return message;
	}


	void TearDown() {
		_socket.close();
	}

	~myTestFixture1() {
		// cleanup any pending stuff, but no exceptions allowed
	}
};

TEST_F (myTestFixture1, UnitTest1) {

	std::cout << "sendMsg" << std::endl;
	auto err = sendMsg("Hello World!\n");
	ASSERT_EQ(nullptr, err) << "sendMsg failed, error: " << err;
	boost::this_thread::sleep(boost::posix_time::seconds(2));
	auto resp = recvMsg();
	std::cout << "recvMsg resp: " << resp << std::endl;
}
