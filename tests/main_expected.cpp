//
// Created by Simon Cros on 1/4/25.
//

#include "expected.hpp"

#include <cassert>
#include <iostream>
#include <string>

struct MyType
{
    constexpr MyType() { std::cout << "Default constructor" << std::endl; }
    constexpr MyType(int x) { std::cout << "Single int constructor" << std::endl; }
    constexpr MyType(std::initializer_list<int> il) { std::cout << "Initializer list constructor" << std::endl; }
    constexpr MyType(MyType&& my) noexcept { std::cout << "Move constructor" << std::endl; }
    constexpr MyType(const MyType& my) { std::cout << "Copy constructor" << std::endl; }
};

int main()
{
    {
        std::cout << "Overload test" << std::endl;

        Expected<int, std::string> tmp1(5);
        Unexpected<std::string> tmp2("42");

        Expected<MyType, std::string> ex01; //                          (1)
        Expected<MyType, std::string> ex02(ex01); //                    (2)
        Expected<MyType, std::string> ex03(std::move(ex01)); //         (3)
        Expected<MyType, std::string> ex04(tmp1); //                    (4)
        Expected<MyType, std::string> ex05(std::move(tmp1)); //    (5)
        Expected<MyType, std::string> ex06(4); //                     (6)
        Expected<MyType, std::string> ex07(tmp2); //                    (7)
        Expected<MyType, std::string> ex08(std::move(tmp2)); //       (8)
        Expected<MyType, std::string> ex09(std::in_place, 42); //       (9)
        Expected<MyType, std::string> ex10(std::in_place, {4, 2}); // (10)
        Expected<MyType, std::string> ex11(unexpect, "42"); //          (11)
        Expected<MyType, std::string> ex12(unexpect, {'4', '2'}); //  (12)
    }

    return 0;
}
