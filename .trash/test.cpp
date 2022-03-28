#include <string>
#include <iostream>
#include <sstream>

int main(void){

    std::stringstream ss;
    std::string

    ss << 100;// << ' ' << 200;
    int foo,bar;
    ss >> foo >> bar;
    std::cout << "foo: " << foo << '\n';
    std::cout << "bar: " << bar << '\n';
    return (0);
}
