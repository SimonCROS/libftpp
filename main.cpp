//
// Created by scros on 11/10/24.
//

#include <iostream>
#include <memory>
#include <string>

#include "pool.hpp"

void printObjectString(Pool<std::string>::Object poolObject)
{
    std::cout << *poolObject << std::endl;
}

int main()
{
    Pool<std::string> test{4};

    auto s1 = std::make_shared<Pool<std::string>::Object>(test.acquire("One"));
    auto s2 = test.acquire("Two");

    test(s2);
}
