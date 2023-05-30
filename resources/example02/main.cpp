int test1(int a, int b) {
    int c, d;
    c = a + b;
    d = a - b;
    if (c > d) {
        return c;
    } else {
        return d;
    }
}

int main(int argc, char* argv[]) {
    int a = 3;
    int b;
    int c;
    a = 1;
    if (a > 1) {
        b = 2;
    } else {
        b = 3;
    }
    c = b + a;
    int e = 1, d;
    return c;
}

int fib(int i) {
    int a = 0, b = 1;
    while (i > 0) {
        int tmp = b;
        b = a + b;
        a = tmp;
        i--;
    }
    return a;
}

int factor(int n) {
    int result = 1;
    for (int i = 1; i <= n; i++) {
        result = result * i;
    }
    return result;
}

void test2(int a, int b, int c, int d, int e) {
    a = 1;
    b = 2;
    while (1) {
        c = a + b;
        d = c - a;
        while (1) {
            d = b + d;
            if (1) break;
            d = a + b;
            e = e + 1;
        }
        b = a + b;
        e = c - a;
    }
    a = b * d;
    b = a - d;
}

