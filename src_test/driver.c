
#include "../slack_estimation/online_estimator.h"
#include "../code_switch/code_switch.h"

// extern "C" {
// void invoke_by_external_v1(int a, int b);
// void invoke_by_external(int a, int b);
// }
#include <stdlib.h>
#include <stdio.h>

#define TEST_SIZE 10000

extern void (*function_ptr)(int, int);

void opdfi_entry();
void invoke_by_external(int now, int len);
int read_test_inputs(int input_bag[TEST_SIZE][2]);
void save_final_result();
int main()
{

  // init();
  // int  input_bags[][2]

  int input_bag[TEST_SIZE][2];
  int count_inputs = 0;
  count_inputs=read_test_inputs(input_bag);

 
  for (int c = 0; c < count_inputs; c++)
  {
    
    int inputs[10];
    inputs[0] = input_bag[c][0];
    inputs[1] = input_bag[c][1];
    opdfi_obtain_input(inputs, 2);

    start_iteration();
    // function_ptr(a,  b);
    // opdfi_entry();
    invoke_by_external(inputs[0], inputs[1]);

    printf("iteration: %d, ", c);
    reset_iteration();
  }
  save_final_result();
  return 0;
}

__attribute__((noinline)) void opdfi_entry()
{

  function_ptr(opdfi_read_input(0), opdfi_read_input(1));
}

int read_test_inputs(int input_bag[TEST_SIZE][2]){
  FILE *file = fopen("/home/opdfi/opdfi/test/test_inputs.txt", "r");
  if (file == NULL) {
    printf("Failed to open the file.\n");
    return 1;
  }

  char line[100];
  int count_inputs = 0;
  while (fgets(line, sizeof(line), file)) {
    int input1, input2;
    if (sscanf(line, "%d %d", &input1, &input2) == 2) {
      input_bag[count_inputs][0] = input1;
      input_bag[count_inputs][1] = input2;
      // printf("input1: %d, input2: %d\n", input1, input2);
      count_inputs++;
    }
  }

  fclose(file);
  // printf("num_test_inputs: %d\n", count_inputs);
  return count_inputs;

}