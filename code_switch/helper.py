import glob
import os
import sys
import os
import shutil




def generage_include_header(arg1, arg2):
    lines = []
    num_versions = int(arg1)

    for i in range(num_versions):
        
        lines.append("DECLARE_INVOKE_BY_EXTERNAL("+str(i+1)+")")
    
    lines.append("void (*function_ptr_table["+str(num_versions+1)+"])(int, int) = {")
    lines.append("&invoke_by_external,")

    for i in range(num_versions):
        
        lines.append("&invoke_by_external_"+str(i+1)+"_version_opdfi"+",")
    lines.append("};")

    output_file="\n".join(lines)
    # print("\n".join(lines))

    with open(arg2, "w") as file:
        file.write(output_file)


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Please provide four arguments.")
        sys.exit(1)
    
    arg1 = sys.argv[1] # First argument is the number of the code version
    arg2 = sys.argv[2] # Second argument is the output header file name
  

    generage_include_header(arg1, arg2)