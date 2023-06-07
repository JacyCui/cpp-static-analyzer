/**
 * @class LiveVar
 * @brief Test case for live variable analysis.
 */
class LiveVar {

    int ifElse(int m, int n, int k) {
        int x = m;
        if (n > 0) {
            return x + n;
        } else {
            return k + n;
        }
    }

    int loop(int x) {
        int y = 0;
        for (int i = 0; i < x; ++i) {
            y += 20;
        }
        return y;
    }

    int loopBranch(int m, int n, int k) {
        int a;
        for (int i = m - 1; i < k; i++) {
            if (i >= n) {
                a = n;
            }
            a = a + i;
        }
        return a;
    }

    void branchLoop(int c, bool d) {
        int x, y, z;
        x = 1;
        y = 2;
        if (c > 0) {
            do {
                x = y + 1;
                y = 2 * z;
                if (d) {
                    x = y + z;
                }
                z = 1;
            } while (c < 20);
        }
        z = x;
    }

    void unaryOperator() {
        int a = 1;
        a++;
        a = 2;
        int* c;
        c = &a;
    }

};
