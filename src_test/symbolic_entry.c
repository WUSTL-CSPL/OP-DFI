 
 
 #include <klee/klee.h>
void invoke_by_external(int now, int len);


 void symbolic_execution_begin(){
    int now=1;
	int len=500;
	// int num_values=100;
	// int max_channels=10;

    klee_make_symbolic(&now, sizeof(now), "s0");
    klee_make_symbolic(&len, sizeof(len), "s1");
    // klee_make_symbolic(&num_values, sizeof(num_values), "s3");
    // klee_make_symbolic(&max_channels, sizeof(max_channels), "s4");

     klee_assume((now<20) & (len<100) );
     klee_assume((now>=0) & (len>=0) );
     invoke_by_external(now,len);
 }