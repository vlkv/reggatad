#include "application.h"
#include "client.h"
#include "json.hpp"
namespace json = nlohmann;

#include <gtest/gtest.h>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

#include <memory>
#include <iostream>
#include <sstream>


class TestFixture: public testing::Test {
public:
	const int PORT = 9100;
	boost::thread _t;

	fs::path HOME;
	fs::path REPO_ROOT;

	TestFixture() :
		_t(&TestFixture::startReggataApp, this),
		HOME("/home/vitvlkv/"),
		REPO_ROOT(HOME/"Pictures/") {
		boost::this_thread::sleep(boost::posix_time::seconds(1));
	}

	void startReggataApp() {
		Application app(PORT);
		app.openRepo(REPO_ROOT.string(), (REPO_ROOT/".reggata").string());
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
	json::json cmd = {
			{"id", "123"},
			{"cmd", "open_repo"},
			{"args", {
				{"root_dir", (HOME/"Downloads").c_str()},
				{"db_dir", (HOME/"Downloads/.reggata").c_str()},
				{"init_if_not_exists", true}
			}}
	};
	auto err = c.send(cmd.dump());
	ASSERT_EQ(nullptr, err) << "sendMsg failed, error: " << err;
	auto resp = c.recv();
	std::cout << "recvMsg resp: " << resp << std::endl;
}
