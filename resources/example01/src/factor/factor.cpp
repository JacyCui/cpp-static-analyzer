#include "factor.h"

namespace example01 {

    int Factor::factor(int i) {
        int res = 1;
        while (i > 1) {
            res *= i;
            i--;
        }
        return res;
    }

    Factor::Factor(int n) {
        this->n = n;
    }

    int Factor::getNum() {
        return n;
    }

}
