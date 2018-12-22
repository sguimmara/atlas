#pragma once
#ifndef ATLAS_TEST_COMMON_HPP
#define ATLAS_TEST_COMMON_HPP

#include "lest/lest.hpp"
#define CASE( name ) lest_CASE( specification(), name )
extern lest::tests & specification();

#endif // ATLAS_TEST_COMMON_HPP