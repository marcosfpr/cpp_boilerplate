//
// Created by mfpr on 3/17/22.
//

#include <string>
#include <gtest/gtest.h>


#include "boilerplate_library/boilerplate_library.hpp"

TEST(DummyTest, DummyHello) {
    boilerplate::format::hello_world();
    ASSERT_TRUE(true);
}