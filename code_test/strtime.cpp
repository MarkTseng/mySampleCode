#include <ctime>
#include <iostream>

int main()
{
    std::time_t t = std::time(NULL);
    char mbstr[100];
    if (std::strftime(mbstr, sizeof(mbstr), "%H:%M", std::localtime(&t))) {
        std::cout << mbstr << '\n';
    }
}

