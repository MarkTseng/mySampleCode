#include <iostream>
#include <iomanip>
#include <ctime>
 
int main()
{
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::cout << "time: " << std::put_time(&tm, "%R") << '\n';
}

