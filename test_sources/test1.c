int;
enum {a=sizeof(void)};
typedef struct A{int x;} _A;
struct A aa;
void f(){void* p1;
    void* p2=p1+1;}