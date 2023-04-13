int factor(int i) {
    int res = 1;
    while (i > 1) {
        res *= i;
        i--;
    }
    return res;
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

int main() {
    int x = factor(3);
    int y = fib(4);
    return x - y;
}
