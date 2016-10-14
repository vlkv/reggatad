#include "application.h"
#include "client.h"

#include <gtest/gtest.h>

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <memory>
#include <iostream>


class TestFixture: public testing::Test {
public:
	const int PORT = 9100;
	boost::thread _t;

	TestFixture() :
		_t(&TestFixture::startReggataApp, this) {
		boost::this_thread::sleep(boost::posix_time::seconds(1));
	}

	void startReggataApp() {
		Application app(PORT);
		app.openRepo(std::string("/home/vitvlkv/Pictures"), std::string("/home/vitvlkv/Pictures/.reggata"));
		app.start();
	}

	void SetUp() {
	}

	void TearDown() {
	}

	~TestFixture() {
		// cleanup any pending stuff, but no exceptions allowed
	}
};

TEST_F (TestFixture, UnitTest1) {
	Client c(PORT);
	std::cout << "sendMsg" << std::endl;
	auto err = c.send("{\"cmd\":\"add_tags\",\"file\":\"file.txt\"}");
	ASSERT_EQ(nullptr, err) << "sendMsg failed, error: " << err;
	auto resp = c.recv();
	std::cout << "recvMsg resp: " << resp << std::endl;
}
