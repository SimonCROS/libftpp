//
// Created by scros on 12/31/24.
//

#include <observable_value.hpp>

#include <cassert>
#include <iostream>

#include "thread_safe_iostream.hpp"

struct MyType
{
    MyType() = default;
    MyType(int x) { std::cout << "Single int constructor\n"; }
    MyType(std::initializer_list<int> il) { std::cout << "Initializer list constructor\n"; }
};

int main()
{
    ObservableValue<MyType> ov1{}; //                               (1)
    ObservableValue<MyType> ov2{std::move(ov1)}; //             (2)
    ObservableValue<MyType> ov3{std::in_place, 1}; //               (3)
    ObservableValue<MyType> ov4{std::in_place, {1, 2, 3}}; //   (4)
    ObservableValue<MyType> ov5{{1, 2, 3}}; //                   (5)
    ObservableValue<MyType> ov6{MyType(4)}; //                    (5)

    return 0;
}
