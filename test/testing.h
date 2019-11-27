#pragma once
#include <repo.h>
#include <boost/filesystem.hpp>

namespace reggatad::testing {

void initTestingRepo(const boost::filesystem::path& repoRootDir, 
                     const boost::filesystem::path& repoMetaFile);

}