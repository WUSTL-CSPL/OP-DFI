using namespace std;
#include <stdlib.h>
#include <stdio.h>
#define WCET 100000                             // WCET of the task
#define FORMULA_FOLDER "/tmp/processed_formula" // folder to store the formula
#define FINAL_RESULT_FILE "./runtime_results/runtime_result.txt"

#include <vector>
#include <string>
#include <cstring>
#include <map>
#include "online_estimator.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

int iteration_active = 0;

int current_slack = 0;

int slack_level = 0;
int code_version_id=0;
int previous_slack_level = 0;
std::vector<string> path_formulas;
std::vector<int> path_lens;
std::vector<std::pair<int, int>> recorded_slacks;
std::vector<std::pair<int, int>> code_versions_info;
int current_iteration_number = 0;
// std::vector<int> recorded_slacks;

int opdfi_current_inputs[100] = {0};
int opdfi_current_num = 0;
int opdfi_current_used_input_num = 0;

int is_slack_estimaton_init = 0;

#define MAX_INPUT_NUM 100
int slack_num_input = 0;
std::map<int, std::vector<std::string>> inputnum_to_filename;
std::map<std::string, int> const_filename_to_number;
int current_estimate_num = 0;
std::map<int, int> const_eval_result;

std::map<std::string, std::vector<std::pair<int, int>>> path_to_const_map;

void opdfi_obtain_input(int *inputs, int num)
{
    for (int i = 0; i < num; i++)
    {
        opdfi_current_inputs[opdfi_current_num] = inputs[i];
        opdfi_current_num++;
    }
    // slack_estimate(opdfi_current_inputs,opdfi_current_num);
    // opdfi_slack_estimate();
}
int opdfi_read_input(int id)
{
    if (id < opdfi_current_num)
    {
        return opdfi_current_inputs[id];
    }
    else
    {
        return -1;
    }
}

#define FORMULA_FOLDER "/tmp/processed_formula/" // folder to store the formula
int init_slack_estimation()
{
    if (is_slack_estimaton_init)
    {
        return is_slack_estimaton_init;
    }
    else
    {
        {
            // printf("init slack estimation\n");
            init();
            is_slack_estimaton_init = 1;

            for (int i = 1;; i++)
            {
                std::string filePath = FORMULA_FOLDER + std::to_string(i) + ".const_index"; // Specify your file path here

                FILE *file = fopen(filePath.c_str(), "r");
                // printf("file path: %s\n", filePath.c_str());
                // Check if the file exists
                if (file != NULL)
                {

                    inputnum_to_filename[i] = std::vector<std::string>();
                    char buffer[1024]; // Buffer to store file contents
                    while (fgets(buffer, sizeof(buffer), file) != NULL)
                    {
                        // Output the line, or process it
                        buffer[strcspn(buffer, "\n")] = '\0'; // Remove the '\n' character at the end
                        inputnum_to_filename[i].push_back(buffer);
                        // printf("%d - %s \n", i, buffer);
                        std::string tmp_full(buffer);
                        std::string baseName = tmp_full.substr(tmp_full.find_last_of("/") + 1);
                        baseName = baseName.substr(0, baseName.find_last_of("."));
                        const_filename_to_number[tmp_full] = stoi(baseName);
                        // printf(" %s: %d \n", buffer, stoi(baseName));
                    }
                }
                else
                {
                    // std::cout << "total inputs is " << i - 1 << ".\n";
                    slack_num_input = i - 1;
                    break;
                }
            }
        }

        {
            // printf("path_formulas.size(): %d\n", path_formulas.size());
            for (auto p : path_formulas)
            {
                path_to_const_map[p] = std::vector<std::pair<int, int>>();
                std::string filePath = FORMULA_FOLDER + p + ".smt2_map"; // Specify your file path here

                FILE *file = fopen(filePath.c_str(), "r");
                // printf("file-smt2_map path: %s\n", filePath.c_str());
                // Check if the file exists
                if (file != NULL)
                {

                    char line[256]; // Buffer to store file contents
                    while (fgets(line, sizeof(line), file) != NULL)
                    {

                        int tmp_line_idx = strcspn(line, "\n");
                        line[tmp_line_idx] = '\0';

                        char *token = strtok(line, " ");

                        // printf("ddd : %p\n ",token);
                        int num_constr = 0;

                        num_constr = atoi(token);

                        token = strtok(NULL, " ");
                        if (strlen(token) > 0)
                        {
                            if (strcmp(token, "1") == 0)
                            {
                                std::pair<int, int> tmp_pair = std::make_pair(num_constr, 1);
                                path_to_const_map[p].push_back(tmp_pair);
                            }
                            else if (strcmp(token, "0") == 0)
                            {
                                std::pair<int, int> tmp_pair = std::make_pair(num_constr, 0);
                                path_to_const_map[p].push_back(tmp_pair);
                                //  printf("%s :---%d, (%d,%d) \n", p.c_str(), num_constr, tmp_pair.first,tmp_pair.second);
                            }
                            else
                            {
                                printf("error: %s\n", token);
                            }
                        }
                        // printf("%s,%d : (%d,%d) \n", p.c_str(), num_constr, path_to_const_map[p][path_to_const_map[p].size()-1].first,path_to_const_map[p][path_to_const_map[p].size()-1].second);
                    }
                }
            }
        }

        return is_slack_estimaton_init;
    }
}

void opdfi_slack_estimate()
{
    if (!iteration_active)
    {
        return;
    }

    if (opdfi_current_used_input_num < opdfi_current_num)
    {
        opdfi_current_used_input_num++;
        //    printf("opdfi_current_used_input_num: %d\n",opdfi_current_used_input_num);
    }
    else
    {
        return;
    }
    init_slack_estimation();

    // slack_estimate(opdfi_current_inputs, opdfi_current_num); //This is the full evaluation
    slack_estimate(opdfi_current_inputs, opdfi_current_used_input_num);
}

void slack_estimate(int *inputs, int _num)
{
    // printf("inputs arguments: %d\n", _num);
    // return;
    // _num=2;//opdfi_comment
    for (int num = current_estimate_num + 1; num <= _num; num++)
    {
        // printf("num-input to evaluate: %d\n", num);
        std::vector<std::string> to_eval = inputnum_to_filename[num];
        for (int i = 0; i < to_eval.size(); i++)
        {
            // printf("to_eval: %s\n", to_eval[i].c_str());
            char command[500];
            sprintf(command, "python3 ~/opdfi/slack_estimation/process_formula.py %s /tmp/output2 %d ", to_eval[i].c_str(), num);

            for (int j = 0; j < num; j++)
            {
                char temp[30];
                sprintf(temp, " %d", inputs[j]);
                strcat(command, temp);
                // memset(temp, 0, sizeof(temp)); // Clear the contents of temp
            }
            // sprintf(command, "python3 ~/opdfi/slack_estimation/process_formula.py /tmp/processed_formula/%s.smt2 /tmp/output2 %d %d %d",path_formulas[i].c_str(), inputs[0], inputs[1],num);
            system(command);
            //  std::cout<<"command: "<<command<<std::endl;
            // printf("command: %s\n", command);
            system("z3 /tmp/output2  >/tmp/z3_output");

            // slack_level = i+1;
            FILE *fp = fopen("/tmp/z3_output", "r");

            if (fp != NULL)
            {
                char line[256];
                if (fgets(line, sizeof(line), fp) != NULL)
                {
                    // printf("line: %s\n", line);
                    if (std::strncmp(line, "sat", 3) == 0)
                    {
                        // printf("%d-th num_inputs: %s\n", const_filename_to_number[to_eval[i]], line);
                        const_eval_result[const_filename_to_number[to_eval[i]]] = 1;
                        // slack_level = i + 1;
                        // current_slack = WCET - path_lens[i]; // TODO, read the corresponding slack, feel free to choose any WCET tool.
                    }
                    else
                    {
                        // printf(" %d-th num_inputs: %s\n", const_filename_to_number[to_eval[i]], line);
                        const_eval_result[const_filename_to_number[to_eval[i]]] = 0;
                    }
                }
                fclose(fp);
            }
        }

        for (auto fg : const_eval_result)
        {
            // printf("const_eval_result: %d, %d\n", fg.first, fg.second);
        }
    }
    current_estimate_num = _num;
    // return;

    // char command[500];
    int i = slack_level;
    for (; i < path_formulas.size(); i++)
    {
        std::vector<std::pair<int, int>> pmap = path_to_const_map[path_formulas[i]];
        // printf("path_formulas[i]: %s\n", path_formulas[i].c_str());
        int is_feasible = 0;
        // slack_level = i+1;
        int j = 0;
        for (; j < pmap.size(); j++)
        {
            // printf("identify infeasible %d-th, %d:   %d VS %d \n", j, pmap[j].first, pmap[j].second, const_eval_result[pmap[j].first]);
            // printf("first %d,const_eval_result.count(pmap[j].first): %d\n", pmap[j].first, const_eval_result.count(pmap[j].first));
            if (const_eval_result.count(pmap[j].first) == 0)
            {
                is_feasible = 1;
                continue;
            }
            if (const_eval_result[pmap[j].first] == pmap[j].second)
            {
                is_feasible = 1;
            }
            else
            {

                // printf("infeasible at %d-th, %d:   %d VS %d \n", j, pmap[j].first, pmap[j].second, const_eval_result[pmap[j].first]);
                is_feasible = 0;
                // break;
                // printf("identify infeasible %d-th, %d:   %d VS %d \n",j, pmap[j].first,pmap[j].second,const_eval_result[pmap[j].first]);
                break;
            }
        }
        if (!is_feasible)
        {
            for(int j=0;j<code_versions_info.size();j++){
                if(code_versions_info[j].first<=i){
                    // recorded_slacks.push_back(std::make_pair(current_slack, code_versions_info[j].second));
                    // break;
                    code_version_id = j;
                }
            }
            slack_level = i;
            // slack_level = 0; // tochange
            // current_slack = WCET - path_lens[i];
            current_slack = path_lens[i];
            // printf("infeasible at %d : slack %d\n",i,current_slack);
            // break;
        }
        else
        {
            for(int j=0;j<code_versions_info.size();j++){
                if(code_versions_info[j].first<=i){
                    // recorded_slacks.push_back(std::make_pair(current_slack, code_versions_info[j].second));
                    // break;
                    code_version_id = j;
                }
            }

            slack_level = i;
            // slack_level = 0; // tochange
            // current_slack = WCET - path_lens[i];
            current_slack = path_lens[i];
            break;
        }
    }
    if (i == path_formulas.size())
    {
        slack_level = path_formulas.size();
        code_version_id = code_versions_info.size()-1;
        // for(int j=0;j<code_versions_info.size();j++){
        //         if(code_versions_info[j].first<=i){
        //             // recorded_slacks.push_back(std::make_pair(current_slack, code_versions_info[j].second));
        //             // break;
        //             code_version_id = j;
        //         }
        //     }
        // slack_level = 0; // tochange
        current_slack = path_lens[i];
    }

    // printf("current slack: %d at level %d\n", current_slack, slack_level);
    // printf("current slack: %d %\n", current_slack * 100 / WCET);
    // printf("Estimated slack = %d % of WCET\n", current_slack );
    if (recorded_slacks.size() < current_iteration_number)
    {
        // float tjk =current_slack * 100;// / WCET;
        // recorded_slacks.push_back(std::make_pair(current_slack * 100 / WCET, 0));
        recorded_slacks.push_back(std::make_pair(current_slack, 0));
    }
    else
    {
        // recorded_slacks[current_iteration_number-1].first = current_slack * 100 / WCET;
        recorded_slacks[current_iteration_number - 1].first = current_slack;
    }
}
void v0_slack_estimate(int *inputs, int num)
{

    // char command[500];
    int i = slack_level;
    for (; i < path_formulas.size(); i++)
    {
        char command[500];

        sprintf(command, "python3 ~/opdfi/slack_estimation/process_formula.py /tmp/processed_formula/%s.smt2 /tmp/output2 %d ", path_formulas[i].c_str(), num);

        for (int j = 0; j < num; j++)
        {
            char temp[30];
            sprintf(temp, " %d", inputs[j]);
            strcat(command, temp);
            // memset(temp, 0, sizeof(temp)); // Clear the contents of temp
        }
        // sprintf(command, "python3 ~/opdfi/slack_estimation/process_formula.py /tmp/processed_formula/%s.smt2 /tmp/output2 %d %d %d",path_formulas[i].c_str(), inputs[0], inputs[1],num);
        system(command);
        //  std::cout<<"command: "<<command<<std::endl;
        // printf("command: %s\n", command);
        system("z3 /tmp/output2  >/tmp/z3_output");

        int is_feasible = 0;
        // slack_level = i+1;
        FILE *fp = fopen("/tmp/z3_output", "r");

        if (fp != NULL)
        {
            char line[256];
            if (fgets(line, sizeof(line), fp) != NULL)
            {
                if (std::strncmp(line, "sat", 3) == 0)
                {
                    is_feasible = 1;
                    // printf("Test %d-th path constraints: %s", i, line);
                    slack_level = i + 1;
                    // current_slack = WCET - path_lens[i]; // TODO, read the corresponding slack, feel free to choose any WCET tool.
                    current_slack = path_lens[i];
                }
            }
            fclose(fp);
        }
        if (is_feasible)
        {
            break;
        }
    }
    if (i == path_formulas.size())
    {
        slack_level = path_formulas.size();
    }

    printf("current slack: %d at level %d\n", current_slack, slack_level);
}

void reset_iteration()
{
    printf("slack = %d%% of WCET, protection = 0.%d\n", current_slack, code_versions_info[code_version_id].second);
    slack_level = 0;
    code_version_id = 0;

    previous_slack_level = 0;
    current_slack = 0;
    // int opdfi_current_inputs[100];
    opdfi_current_num = 0;
    opdfi_current_used_input_num = 0;
    current_estimate_num = 0;

    const_eval_result.clear();

    iteration_active = 0;
}
void save_final_result()
{
    std::ofstream file(FINAL_RESULT_FILE);
    int average_slack = 0;
    int average_coverage = 0;
    if (file.is_open())
    {

        file  << "estimated_slack protection_coverage\n";
        for (auto result : recorded_slacks)
        {
            average_slack += result.first;
            average_coverage += code_versions_info[result.second].second;
            file << result.first << "% 0." << code_versions_info[result.second].second << "\n";
        }
        file.close();
    }
    else
    {
        std::cout << "Failed to open file: " << FINAL_RESULT_FILE << std::endl;
    }
    std::cout << "Average estimaed slack: " << average_slack / recorded_slacks.size() << "% of WCET, average coverage: 0." << average_coverage / recorded_slacks.size() << "\n";
}

void start_iteration()
{
    iteration_active = 1;
    current_iteration_number++;
}

void init()
{
    {
        FILE *file = fopen(FORMULA_FOLDER "/order_list_scaled", "r");
        if (file != NULL)
        {
            char line[256];
            while (fgets(line, sizeof(line), file) != NULL)
            {
                // Process each line here
                line[strcspn(line, "\n")] = '\0'; // Remove the '\n' character at the end
                char *token = strtok(line, " ");

                path_formulas.push_back(token);
                // printf("%s-", token);
                token = strtok(NULL, " ");
                // printf("%s-", token);
                path_lens.push_back(atoi(token)); // TODO, read the corresponding pWCET, feel free to choose any WCET analysis tool.
            }
            fclose(file);
        }
        else
        {
            printf("Failed to open file: %s\n", FORMULA_FOLDER "/order_list");
        }
    }

    {
        FILE *file = fopen(FORMULA_FOLDER "/code_version_id_coverage", "r");
        if (file != NULL)
        {
            char line[256];
            while (fgets(line, sizeof(line), file) != NULL)
            {
                // Process each line here
                line[strcspn(line, "\n")] = '\0'; // Remove the '\n' character at the end
                char *token = strtok(line, " ");

                int index=atoi(token);
                // path_formulas.push_back(token);
                // printf("%s-", token);
                token = strtok(NULL, " ");
                // printf("%s-", token);
                // path_lens.push_back(atoi(token)); // TODO, read the corresponding pWCET, feel free to choose any WCET analysis tool.
                int cover=atoi(token);
                code_versions_info.push_back(std::make_pair(index,cover));
            }
            fclose(file);

            for(auto p:code_versions_info){
                // printf("code_version_id_coverage: %d, %d\n",p.first,p.second);
            }
        }
        else
        {
            printf("Failed to open file: %s\n", FORMULA_FOLDER "/order_list");
        }
    }
}
