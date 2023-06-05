/**
 * @class ConstPropagation
 * @brief Test case for constant propagation analysis.
 */
class ConstPropagation {

    int dummy() {
        int x;
        int y;
        y = x = 1;
        return y;
    }

    void typeCast() {
        char a = 'a';
        int x = (int)(char)10000000;
        long long y = (long long)1000000000000000000;
        unsigned long long z = (unsigned long long)(unsigned int)-1;
    }

    int ifElse(int n) {
        int x, y, z, a;
        int u, v;
        if (n > 0) {
            x = 1;
            y = 3;
            if (n == 1) {
                u = 2;
                v = 2;
            }
        } else {
            x = 2;
            y = 3;
            if (n == 0) {
                v = 3;
            }
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
        int zero = x - (div * y) - mod;
        n /= zero;
        return n;
    }

     int loop() {
        int a = 10, b = 2, c;
        while (a > b) {
            c = b;
            --a;
        }
        return c;
    }

    void incDec() {
        int x = 0;
        int y = 0;
        x++;
        y--;
        int a = x++;
        int b = ++x;
        int c = y--;
        int d = --y;
    }

    int array() {
        int a[2][2];
        int i = 2, j = 3;
        return a[i][j];
    }

    int foo(int n) {
        return n;
    }

    int call() {
        int zero = 0;
        return foo(1 / zero);
    }

};
