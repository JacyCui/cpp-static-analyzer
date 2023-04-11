#include "fib.h"

namespace example01 {

    int Fib::fib(int i) {
        int a = 0, b = 1;
        while (i > 0) {
            int tmp = b;
            b = a + b;
            a = tmp;
            i--;
        }
        return a;
    }

    Fib::Fib(int n) {
        this->n = n;
    }

    int Fib::getNum() {
        return n;
    }

}
