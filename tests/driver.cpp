#include "int2048.h"
#include <climits>
int main() {
    std::string op;
    sjtu::int2048 a,b;
    while (std::cin >> op >> a >> b) {
        if (op == "+") std::cout << a+b;
        if (op == "-") std::cout << a-b;
        if (op == "*") std::cout << a*b;
        if (op == "/") std::cout << a/b;
        if (op == "%") std::cout << a%b;
        if (op == "cmp") std::cout << (a<b) << (a==b) << (a>b);
        if (op == "self") { a += a; a *= a; a /= a; std::cout << a; }
        if (op == "min") std::cout << sjtu::int2048(LLONG_MIN);
        std::cout << '\n';
    }
}
