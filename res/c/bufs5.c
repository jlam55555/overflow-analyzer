void f(char *d, char *s) {
        f(d, s);
}

int main() {
        char b1[512], b2[512];
        f(b1, b2);
}
