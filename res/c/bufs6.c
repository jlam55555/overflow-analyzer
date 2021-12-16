int f();

int main() {
        char *p1, *p2, *p3;

        while (1) {
                if (f()) {
                        p1 = p2;
                } else {
                        p2 = p3;
                }
        }
}
