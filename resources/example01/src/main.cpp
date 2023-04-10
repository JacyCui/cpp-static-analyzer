#include "factor.h"
#include "fib.h"

int main() {
    AbstractFactor* f1 = new Factor(10);
    int a = 8;
    int c = f1->factor(a);
    int d = f1->getNum();
    AbstractFib* f2 = new Fib(10);
    int e = 5;
    int f = f2->fib(e);
    int g = f2->getNum();
    return c + d + f + g;
}
