// runner.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "bricks.h"

int main()
{
    std::cout << "All in all it is just a brick in the wall!\n";

    bricks_handle_t h = 0;
    bricks_open("bricks.xml", &h);

    bricks_handle_t* p = bricks_register_publisher("service1", "my.topic");

    bricks_handle_t* s = bricks_register_subscriber("service1", "my.topic", my_callback);

    while (true)
    {
        bricks_poll();

    }

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file