
#include "code_switch.h"
#include <stdio.h>
#include <vector>
#define DECLARE_INVOKE_BY_EXTERNAL(N)                          \
    extern "C"                                                 \
    {                                                          \
        void invoke_by_external_##N##_version_opdfi(int, int); \
    }

extern "C"
{
    void invoke_by_external(int, int);
}

// #include "/tmp/code_version_header.h"
extern std::vector<std::pair<int, int>> recorded_slacks;
extern int current_iteration_number;

void (*function_ptr)(int, int);
// Add missing function declarations

// extern void invoke_by_external_v1(int, int);
// extern void invoke_by_external_v2(int, int);
// extern void invoke_by_external_v3(int, int);

void code_switching()
{
    // function_ptr = function_ptr_table[slack_level]; // GLOBAL variable
    // // to do
    // printf("switching to version: %p\n", function_ptr);
    // Remove incorrect function invocations
    // invoke_by_external_v
    // invoke_by_external_v1
}

extern int previous_slack_level;
void opdfi_code_switching()
{
    // return;
    // function_ptr = function_ptr_table[slack_level];
    // printf("switching to %d-th version: %p\n", slack_level, function_ptr);

    // return;
    // if (previous_slack_level == slack_level)
    // {
    //     // printf("redddd eck_level: %d\n",slack_level);
    //     if (function_ptr != function_ptr_table[slack_level])
    //     {
    //         printf("something goes wrong %p : %p\n", function_ptr, function_ptr_table[slack_level]);
    //     }
    //     return;
    // }
    // else
    // {
    //     // printf("dddd e--slack_level: %d\n",slack_level);
    //     previous_slack_level = slack_level;
    // }
    // printf("before %p: after %p\n", function_ptr, function_ptr_table[6]);
    // // function_ptr=function_ptr_table[slack_level]; //GLOBAL variable

    // function_ptr = function_ptr_table[slack_level];
    // // to do
    // printf("switching to %d-th version: %p\n", slack_level, function_ptr);
    // Remove incorrect function invocations
    // invoke_by_external_v
    // invoke_by_external_v1
}


extern int iteration_active;

void opdfi_instrumented_code_switching(void **ptr_to_funptr, void **func_ptr_array)
{

    // printf("\n");
    if (!iteration_active)
    {

        return;
    }

    // return;
    if (previous_slack_level == slack_level)
    {
        // printf("retrun slack_level: %d\n",slack_level);
        return;
    }
    else
    {
        //  printf("continue slack_level: %d\n",slack_level);
        // previous_slack_level=slack_level;
    }
    // printf("hrrt\n");
    void *before = *ptr_to_funptr;

    // *ptr_to_funptr = func_ptr_array[slack_level]; // to fixed
    *ptr_to_funptr = func_ptr_array[code_version_id]; // to fixed
    
    // }

    //  *ptr_to_funptr=func_ptr_array[9];
    // *ptr_to_funptr=before;
    void *after = *ptr_to_funptr;
    if (slack_level != previous_slack_level)
    {
        // printf("switching from %d-th: %p, to %d-th : %p\n", previous_slack_level, before, slack_level, func_ptr_array[slack_level]);
        // printf("switching from %d-th to %d-th version\n",previous_slack_level,slack_level);
    }
    previous_slack_level = slack_level;
    if (recorded_slacks.size()<current_iteration_number){
        // float tjk =current_slack * 100;// / WCET;
        recorded_slacks.push_back(std::make_pair(0, code_version_id));
    }else{
        recorded_slacks[current_iteration_number-1].second = code_version_id;
    
    }
    
}
