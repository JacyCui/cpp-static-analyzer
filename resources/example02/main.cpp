int main() {
    int a, b, c, d;
    a = 2;
    if (a > 1) {
        b = 1;
    } else {
        b = 0;
    }
    c = a / b;
    switch (c) {
        case 1:
            d = 3;
            break;
        case 2:
            d = 4;
            break;
        default:
            d = 1;
    }
    int e[10][10] = {0};
    int f;
    f = e[3][3];
    return f;
}
