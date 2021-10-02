int printf(const char* fmt,...);
int main() {
    printf ("hello world\n");
    for(int i=0;i<10;i++){
        if(i==9)continue;
        i=12;
    }
    int k=0;
    int j=10;
    // i=12;
    switch (j) {
        case 1:
            if(k>9)break;
        case 3-1:
            break;

    }
    // continue;
    return 0;
}