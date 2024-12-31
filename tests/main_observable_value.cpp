//
// Created by scros on 12/31/24.
//

#include <observable_value.hpp>

#include <cassert>
#include <iostream>

#include "thread_safe_iostream.hpp"

struct MyType {
    MyType();
    MyType(int x) { std::cout << "Single int constructor\n"; }
    MyType(std::initializer_list<int> il) { std::cout << "Initializer list constructor\n"; }
};

int main()
{
    // Use std::optional to wrap MyType
    std::optional<MyType> opt1{1};
    // std::optional<MyType> opt2{{1, 2, 3}};

    ObservableValue<MyType> ov0;
    ObservableValue<MyType> ov1{std::in_place, 1};
    ObservableValue<MyType> ov2{std::in_place, {1, 2, 3}};
    ObservableValue<MyType> ov3{{1, 2, 3}};
    ObservableValue<MyType> ov4{MyType(4)};

    return 0;
}
