#include "cmd.h"
#include "cmd_add_tags.h"
#include "cmd_remove_tags.h"
#include "reggata_exceptions.h"

#include <boost/log/trivial.hpp>

Cmd::Cmd() {
	// TODO Auto-generated constructor stub

}

std::unique_ptr<Cmd> Cmd::parse(const boost::property_tree::ptree& pt) {
	BOOST_LOG_TRIVIAL(debug) << "Cmd::parse";
	auto cmdStr = pt.get<std::string>("cmd");
	if (cmdStr == "add_tags") {
		return CmdAddTags::parse(pt);
	} else if (cmdStr == "remove_tags") {
		return CmdRemoveTags::parse(pt);
	} else {
		throw new ReggataException(std::string("Unexpected command: ") + cmdStr);
	}
}
