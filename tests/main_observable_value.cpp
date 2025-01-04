//
// Created by Simon Cros on 12/31/24.
//

#include "observable_value.hpp"

#include <cassert>
#include <iostream>

struct MyType
{
    MyType() { std::cout << "Default constructor" << std::endl; }
    explicit MyType(int x) { std::cout << "Single int constructor" << std::endl; }
    explicit MyType(std::initializer_list<int> il) { std::cout << "Initializer list constructor" << std::endl; }
    MyType(MyType&& my) noexcept { std::cout << "Move constructor" << std::endl; }
    MyType(const MyType& my) { std::cout << "Copy constructor" << std::endl; }
};

int main()
{
    {
        std::cout << "\nOptional\n" << std::endl;

        std::optional<MyType> op1(std::in_place);
        std::optional<MyType> op2(std::move(op1));
        std::optional<MyType> op3(std::in_place, 1);
        std::optional<MyType> op4(std::in_place, {1, 2, 3});
        std::optional<MyType> op5(4);
        std::optional<MyType> op6(MyType(4));
    }

    {
        std::cout << "\nObservableValue\n" << std::endl;

        ObservableValue<MyType> ov1; //                                 (1)
        ObservableValue<MyType> ov2(std::move(ov1)); //                 (2)
        ObservableValue<MyType> ov3(std::in_place, 1); //               (3)
        ObservableValue<MyType> ov4(std::in_place, {1, 2, 3}); //   (4)
        ObservableValue<MyType> ov5(4); //                              (5)
        ObservableValue<MyType> ov6(MyType(4)); //                    (5)
    }

    {
        std::cout << "\nListeners\n" << std::endl;

        auto simpleListener = [](const int& i) -> void { std::cout << "SL " << i << std::endl; };

        ObservableValue<int> ov = 42;
        ov.addListener([](const int& i) -> void { std::cout << "L1 " << i << std::endl; });
        ov.addListener([](const int& i) -> void { std::cout << "L2 " << i << std::endl; });
        ov = 44;
        ov = 0;
#if __cpp_lib_containers_ranges >= 202202L
        ov.addListeners((decltype(simpleListener)[]){simpleListener, simpleListener, simpleListener});
        ov = 42;
#endif

        ObservableValue<int> moved = std::move(ov);
        moved = 0;

        assert(*moved == 0);
        assert(*(moved.operator->()) == 0);
    }

    return 0;
}
