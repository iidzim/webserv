#include <string>
#include <iostream>
#include <sstream>
#include <map>

int main(void){

    std::map<int, int>::iterator it;
    std::map<int, int> m;
    m.insert(std::pair<int, int>(1, 1));
    m.insert(std::pair<int, int>(3, 3));
    m.insert(std::pair<int, int>(2, 2));

    it = m.begin();
    std::cout << it->first << " " << it->second << std::endl;
    it++;
    it++;
    it++;
    std::cout << it->first << " " << it->second << std::endl;
    // std::stringstream ss;
    // std::string

    // ss << 100;// << ' ' << 200;
    // int foo,bar;
    // ss >> foo >> bar;
    // std::cout << "foo: " << foo << '\n';
    // std::cout << "bar: " << bar << '\n';
    // return (0);
}
