int printf(const char* fmt,...);
enum C{x=10/2-(1-2)+(2>1)+1/(2-1)+(1&2),y=x+1};
enum {z=sizeof(enum C)};
int main(void) {
  return printf("hello %d\n",x);
}

