/**
 * @class ConstPropagation
 * @brief Test case for constant propagation analysis.
 */
class ConstPropagation {

    int dummy() {
        int x;
        int y;
        x = 1;
        y = x;
        return y;
    }

    int ifElse(int n) {
        int x, y, z, a;
        if (n > 0) {
            x = 1;
            y = 3;
        } else {
            x = 2;
            y = 3;
        }
        z = x + y;
        return z;
    }

    int binaryOp(int n) {
        int x = 15, y = 2;
        int neg = -x;
        int add = x + y;
        int sub = x - y;
        int mul = x * y;
        int div = x / y;
        int mod = x % y;
        int And = x & y;
        int Or = x | y;
        int Xor = x ^ y;
        int LShift = x << y;
        int RShift = x >> y;
        int zero = x - div * y - mod;
        n /= zero;
        return n;
    }
};
