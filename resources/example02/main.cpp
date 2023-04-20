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
    int a;
    int b;
    int c;
    a = 1;
    if (a > 1) {
        b = 2;
    } else {
        b = 3;
    }
    c = b;
    return 0;
}
