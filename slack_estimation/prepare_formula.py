import glob
import os
import sys
import os
import shutil
import glob
import numpy as np

all_inputs = set()
incremental_inputs_file={}
input_delaration_map={}
constraints_map={}
individual_const_map={}

selected_plen_file_without_ext=[]

selected_constraints=[]

number_of_constraints = 0

def process_constraints(arg1, arg2, arg3, arg4):
    with open(arg1+"/"+"input_to_declaration", 'r') as file:
        lines = file.readlines()
        for i in range(int(len(lines)/2)):
            inp = lines[2*i].rstrip('\n')
            declare=lines[2*i+1].rstrip('\n')
            input_delaration_map[inp] = declare
    # print(input_delaration_map)
    whole_smt2_path = arg1+"/"+"whole_smt2"
    with open(whole_smt2_path, 'r') as file:
        lines = file.readlines()
        for i in range(int(len(lines)/2)):
            line_index = lines[i*2]
            line_parts = line_index.split()
            id=line_parts[0]
            index = line_parts.index("|")
            constraints_map[id]={}
            constraints_map[id]["inputs"]=set()
            constraints_map[id]["weight"]=0
            # print(constraints_map[id]["weight"])
            # print("--",index)
            for j in range(index+1,len(line_parts)):
                # if line_parts[j] not in all_inputs:
                all_inputs.add(line_parts[j])
                constraints_map[id]["inputs"].add(line_parts[j])

            line = lines[i*2+1]
            constraints_map[id]["constraint"]=line
            # Do something with line_parts
    
    # print(len(constraints_map),all_inputs)
    
    # smt2_map_list = glob.glob(arg1 + "/*.smt2_map")

    for m_file in selected_plen_file_without_ext:
        file_name_without_ext = m_file
        # selected_plen_file_without_ext.append(file_name_without_ext)
        individual_const_map[file_name_without_ext] = {}

        smt2_map_path = os.path.join(arg1, file_name_without_ext + ".smt2_map")
        with open(smt2_map_path, 'r') as file:
            lines = file.readlines()
            # print("ddd",file_name_without_ext,lines)
            for line in lines:
                line_parts = line.split()
                c_id = int(line_parts[0])
                index = line_parts[0]
                true_false = line_parts[1]
                individual_const_map[file_name_without_ext][index] = true_false #key is constraint_index, value is true_false
                # individual_const_map[file_name_without_ext]["true_false"] = true_false
    
    # print(individual_const_map)

    for f in individual_const_map.keys():
        # print("--",f, individual_const_map[f].keys())
        for k in individual_const_map[f].keys():
           
            if k in constraints_map.keys():
                constraints_map[k]["weight"] += 1

    top_K_constraints = sorted(constraints_map.items(), key=lambda x: x[1]["weight"], reverse=True)[:number_of_constraints]

    def get_input_index(inputs):
        s=sorted(inputs)[-1]
        s_all_inputs=sorted(all_inputs)
        for i in range(len(s_all_inputs)):
            if s_all_inputs[i] == s:
                return i
    for j in range(len(all_inputs)):
        incremental_inputs_file[j]=[]
    # print(top_K_constraints,individual_const_map.keys() )
    for constraint, data in top_K_constraints:
        current_to_write=[]
        current_to_write.append("(set-logic QF_AUFBV )")
        ii=get_input_index(data["inputs"])
        # print(ii, data["inputs"])
        # for inp in data["inputs"]:
        for inp in sorted(all_inputs):
            current_to_write.append(input_delaration_map[inp])
        # print(sorted(data["inputs"]))
        current_to_write.append(data["constraint"])
        current_to_write.append("(check-sat)")
        current_to_write.append("(exit)")
        with open(arg2+"/"+constraint+".smt2_single", "w") as file:
            file.write("\n".join(current_to_write))
            incremental_inputs_file[ii].append(arg2+"/"+constraint+".smt2_single")
            
        with open("/home/opdfi/opdfi/test/compile_results/slack_constraints"+"/"+constraint+".single_constraint", "w") as file:
            file.write("\n".join(current_to_write))
                   
    for k, data in incremental_inputs_file.items():
        with open(arg2 + "/" + str(k+1) + ".const_index", "w") as file:
            file.write("\n".join(data))
    
    # for constraint, data in top_K_constraints:
    #     print(f"Constraint: {constraint}")
    #     print(f"Weight: {data['weight']}")
    #     print(f"Inputs: {data['inputs']}")
    #     print(f"Constraint: {data['constraint']}")
    #     print()
        
        # for k in constraints_map.keys():
        #     print(constraints_map[k]["weight"])
    # for j in range(len(top_K_constraints)):
    #     print(top_K_constraints[j][0])
    #     print(top_K_constraints[j][1]["inputs"])
    #     print(top_K_constraints[j][1]["constraint"])
    #     print()

    # print(constraints_map.keys())

    current_inputs=[]
    for i,x in enumerate(sorted(all_inputs)):
        break
        current_inputs.append(x)
        current_to_write=[]
        current_to_write.append("(set-logic QF_AUFBV )")
        for constraint, data in top_K_constraints:
            if len(current_inputs) == len(data["inputs"]):
                whole_match=True
                for inp in data["inputs"]:
                    if inp not in current_inputs:
                        whole_match=False
                if whole_match:
                    # pass
                    current_to_write.append(input_delaration_map[inp])
            # if x not in data["inputs"]:
            #     data["constraint"] = data["constraint"].replace(x, "(not "+x+")")
            #     data["inputs"].add(x)
            # top_K_constraints[j][1]["inputs"].add(x)

        # with open(arg2+"/"+str(i)+".inputs_list", "w") as file:
            # print(x)
    for j in top_K_constraints:
        # print(j[1]['weight'])
        pass
    for j in constraints_map.keys():
        pass
        # print(constraints_map[j]["weight"])
    # print(top_K_constraints)

def prepare_formula(arg1, arg2, arg3, arg4,arg5):
    num_of_version=int(arg5)
    smt2_file_list = glob.glob(arg1 + "/*.smt2")
    plen_file_list = glob.glob(arg1 + "/*.pathlen")
    
    # print(plen_file_list)
    pathfile_to_length={}
    def get_number_from_file(file_path):
        with open(file_path, 'r') as file:
            # pathfile_to_length[file_path]=int(file.read().strip())
            # print(int(file.read().strip()))
            result=int(file.read().strip())
            # print(result)
            file_name = os.path.basename(file_path)
            file_name_without_ext = os.path.splitext(file_name)[0]
            pathfile_to_length[file_name_without_ext]=result
            return result#int(file.read().strip())
    
    plen_file_list = sorted(plen_file_list, key=get_number_from_file)
    plen_file_list.reverse()
    # print(plen_file_list)

    all_path_to_length=[]
    for file_path in plen_file_list:
        file_name = os.path.basename(file_path)
        file_name_without_ext = os.path.splitext(file_name)[0]
        all_path_to_length.append([file_name_without_ext,get_number_from_file(file_path)])
        
    limit = int(arg3)
    if limit > len(plen_file_list):
        limit = len(plen_file_list)
    selected_plen_file=plen_file_list[:limit]
    # print(selected_plen_file)
    if os.path.exists(arg2):
            shutil.rmtree(arg2)
    os.makedirs(arg2)

    order_list=[]
    path_to_length=[]
    for file_path in selected_plen_file:
        file_name = os.path.basename(file_path)
        file_name_without_ext = os.path.splitext(file_name)[0]
        selected_plen_file_without_ext.append(file_name_without_ext)
        order_list.append(file_name_without_ext+" "+ str(get_number_from_file(file_path)))
        path_to_length.append([file_name_without_ext,get_number_from_file(file_path)])
        # Copy the file to arg2 folder
        shutil.copy(arg1+"/"+file_name_without_ext+".smt2", arg2+"/"+file_name_without_ext+".smt2")
        shutil.copy(arg1+"/"+file_name_without_ext+".pathlen", arg2+"/"+file_name_without_ext+".pathlen")
        shutil.copy(arg1+"/"+file_name_without_ext+".ktest", arg2+"/"+file_name_without_ext+".ktest")
        shutil.copy(arg1+"/"+file_name_without_ext+".smt2_map", arg2+"/"+file_name_without_ext+".smt2_map")
        
        
    tosave="\n".join(order_list)
    # print(tosave)
    with open(arg2+"/order_list", "w") as file:
        file.write(tosave)
    # def calculate_protection_rate(output_path):
    
    # for 
    # for p in or
    # with open(arg2+"/path_to_code_versions", "w") as file:
    # print(path_to_length)
    wcet=max(path_to_length,key=lambda x:x[1])
    # min_et=min(path_to_length,key=lambda x:x[1])
    min_et=min(all_path_to_length,key=lambda x:x[1])
    # print(wcet,min_et)
    code_version_coverage=[]
    dfi_scale=4.8
    # slack_scale=0.6
    for i in range(num_of_version):
        
        r=wcet[1]/path_to_length[int(len(path_to_length)/(num_of_version))*i][1]
        if r>dfi_scale:
            cover=1
        else:
            cover=r/dfi_scale
        if i==0:
            cover=0
        if i==num_of_version-1:
            code_version_coverage.append([int(len(path_to_length)-1),1])
        else:
            code_version_coverage.append([int(len(path_to_length)/(num_of_version))*i,cover])
        continue
        if i!=0 and i!=num_of_version-1:
            cover=1-(path_to_length[int(len(path_to_length)/(num_of_version))*i][1]-min_et[1])/(wcet[1]-dfi_scale*min_et[1])
            # cover=1-(path_to_length[int(len(path_to_length)/(num_of_version))*i][1])/(wcet[1]*dfi_scale)
            
            code_version_coverage.append([int(len(path_to_length)/(num_of_version))*i,cover])
        elif i==0:
            cover=0
            code_version_coverage.append([int(len(path_to_length)/(num_of_version))*i,cover])
        elif i==num_of_version-1:
            cover=1-(path_to_length[-1][1]-min_et[1])/(wcet[1]-dfi_scale*min_et[1])
            # cover=1-(path_to_length[-1][1])/(wcet[1]*dfi_scale)
            code_version_coverage.append([int(len(path_to_length)-1),cover])
        if cover>1:
            cover=1
        # code_version_coverage.append([int(len(path_to_length)/(num_of_version-1))*i,cover])
        # code_version_coverage.append(0)
    # path_to_coverage
    # print(wcet,code_version_coverage)
    with open(arg2+"/code_version_id_coverage", "w") as file:
        for cv in code_version_coverage:
            file.write(str(cv[0])+" "+str(int(cv[1]*1000))+"\n")
    with open("/home/opdfi/opdfi/test/compile_results/version_info", "w") as file:
        file.write("VersionID"+" "+"Coverage"+"\n")
        for cv in code_version_coverage:
            file.write(str(cv[0])+" "+str(int(cv[1]*1000)/1000)+"\n")
        # file.write(str(to_write))
    # print(pathfile_to_length)
        # file.write(str(num_of_version))
    path_order_list_scaled=[]
    
    path_order_list_scaled_num=[]
    slack_estimate_policy=[]
    for pl in path_to_length:
        
        with open(arg2+"/"+pl[0]+".pathlen_scale", "w") as file:
            # to_write=int((wcet[1]-pl[1])*100/(wcet[1]-min_et[1])) #Since this is execute in VM, the slack time is simulated. For real-hardware, use time derived from WCET analysis tool.
            to_write=int((wcet[1]-pl[1])*100/(wcet[1]))  #Since this is execute in VM, the slack time is simulated. For real-hardware, use time derived from WCET analysis tool.
            path_order_list_scaled.append(pl[0]+" "+ str(to_write))
            path_order_list_scaled_num.append([pl[0],to_write])
            file.write(str(to_write))
        
        # with open("/home/opdfi/opdfi/test/compile_results"+"/slack_estimate_policy", "a") as file:
            # eval_result=np.loadtxt(arg1+"/"+pl[0]+".smt2_map")
        eval_result = np.loadtxt(arg1 + "/" + pl[0] + ".smt2_map", delimiter=' ')
        to_save_string = "{"
        for eval_re in eval_result:
            to_save_string=to_save_string+" "+str(int(eval_re[0]))+":"+str(int(eval_re[1]))
        to_save_string=to_save_string+" }"
        # print(eval_result,to_save_string)
        slack_estimate_policy.append(to_save_string)
            # with open(arg1+"/"+pl[0]+".smt2_map") as readfile:
                
    tosave_path_order_list_scaled="\n".join(path_order_list_scaled)
    # print(tosave)
    with open(arg2+"/order_list_scaled", "w") as file:
        file.write(tosave_path_order_list_scaled)
    process_constraints(arg1, arg2, arg3, arg4)
    
    with open("/home/opdfi/opdfi/test/compile_results"+"/slack_estimate_policy", "w") as file:
        file.write("slack"+" "+"{ ith-const:eval_result }"+"\n")
        for i,pl in enumerate(path_order_list_scaled_num[:-1]):
            file.write(str(pl[1])+"% "+slack_estimate_policy[i]+"\n")

        
        
if __name__ == "__main__":
    if len(sys.argv) != 6:
        print("Please provide four arguments.")
        sys.exit(1)
    
    arg1 = sys.argv[1] # First argument is the klee output folders
    arg2 = sys.argv[2] # Second argument is the output folder
    arg3 = sys.argv[3] # Third argument is the path number limit
    arg4 = sys.argv[4] # Fourth argument is the constraints number limit
    arg5 = sys.argv[5] # Fifth argument is the number of code_versions
    # print(arg3,arg5)
    number_of_constraints=int(arg4)
    prepare_formula(arg1, arg2, arg3, arg4,arg5)