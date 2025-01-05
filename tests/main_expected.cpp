//
// Created by Simon Cros on 1/4/25.
//

#include "expected.hpp"

#include <cassert>
#include <iostream>
#include <string>

struct MyType
{
    size_t dummy1;
    size_t dummy2;
    size_t dummy3;
    size_t dummy4;

    MyType() { std::cout << "Default constructor" << std::endl; }
    MyType(int x) { std::cout << "Single int constructor" << std::endl; }
    MyType(std::initializer_list<int> il) { std::cout << "Initializer list constructor" << std::endl; }
    MyType(MyType&& my) noexcept { std::cout << "Move constructor" << std::endl; }
    MyType(const MyType& my) { std::cout << "Copy constructor" << std::endl; }
};

int main()
{
#ifndef LIBFTPP_EXPECTED
    std::cout << "\033[33mUSING REAL std::expected\033[0m" << std::endl;
#endif

    {
        std::cout << "\n========== Overloads test ==========\n" << std::endl;

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

    {
        std::cout << "\n========== Size test ==========\n" << std::endl;

        std::cout << "Expected<MyType, std::string> size: " << sizeof(Expected<MyType, std::string>) << '\n'
            << "       MyType size: " << sizeof(MyType) << '\n'
            << "  std::string size: " << sizeof(std::string) << std::endl;
    }

    {
        std::cout << "\n========== Usage tests ==========\n" << std::endl;

        Expected<std::string, std::string> expected;
        std::string str;

        for (int i = 0; i < 4; ++i)
        {
            std::cout << "Type something: ";
            getline(std::cin, str);

            if (str.empty())
                expected = Unexpected("Nothing turned in :'(");
            else
                expected = str;

            if (expected.has_value())
                std::cout << "You entered " << expected->length() << " characters: `" << *expected << '`' << std::endl;
            else
                std::cout << "Error: " << expected.error() << std::endl;
        }
    }

    return 0;
}
