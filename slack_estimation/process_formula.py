import sys
import struct




def process_template(template_file, placeholders):
    with open(template_file, "r") as file:
        lines = file.readlines()

    for i, line in enumerate(lines):
        for j, placeholder in enumerate(placeholders):
            lines[i] = lines[i].replace(placeholder, f"Value{j+1}")

    print(lines)

def int_to_twos_complement(num):
    # Convert the integer to a 4-byte two's complement representation
    twos_complement = struct.pack('>i', num)
    hex_twos_complement = [format(byte, '02x') for byte in twos_complement]
    hex_twos_complement.reverse()  # Reverse the order of the list
    # print(hex_twos_complement)
    return hex_twos_complement


def get_inserted_lines(num_inputs, inputs):
    template_files=[]
    for k in range(num_inputs):
        arg_2_complement = int_to_twos_complement(int(inputs[k]))
        template_files_arg_tmp = [f"(assert (= (select s{k} #x0000000{i}) #x{v}))\n" for i, v in enumerate(arg_2_complement)]
        template_files=template_files+template_files_arg_tmp
    # print(template_files)
    return template_files
    # arg3 = int(arg3)
    # arg4 = int(arg4)
    # arg3_2_complement = int_to_twos_complement(arg3)
    # template_files_arg3 = [f"(assert (= (select s1 #x0000000{i}) #x{v}))\n" for i, v in enumerate(arg3_2_complement)]

    # template_files_arg4=[]
    # if(num_symbol==2):
    #     arg4_2_complement = int_to_twos_complement(arg4)
    #     template_files_arg4 = [f"(assert (= (select s2 #x0000000{i}) #x{v}))\n" for i, v in enumerate(arg4_2_complement)]

    # # template_code = "\n".join(template_files_arg3 + template_files_arg4)

    # # print(template_code)
    # return (template_files_arg3 + template_files_arg4)


def process_formula(arg1, arg2, num_inputs, inputs):
    # Your code logic here
    # print(f"Argument 1: {arg1}")
    # print(f"Argument 2: {arg2}")
    # print(f"Argument 3: {arg3}")
    # print(f"Argument 4: {arg4}")

    file = open(arg1, "r")
    lines = file.readlines()
    file.close()
    inserted_lines = get_inserted_lines(num_inputs, inputs)

    integer_variable=1
    with open("/tmp/num_symbolic_input", "r") as file:
        integer_variable = int(file.read())
    lines[integer_variable+1:integer_variable+1] = inserted_lines  # Insert lines at index 3, Change this for different symbolic execution number
    tosave="".join(lines)
    with open(arg2, "w") as file:
        file.write(tosave)
    # print("".join(lines))
    
    

    


if __name__ == "__main__":
    # if len(sys.argv) != 5:
    #     print("Please provide four arguments.")
    #     sys.exit(1)
    
    arg1 = sys.argv[1] # First argument is the formula file name
    arg2 = sys.argv[2] # Second argument is the output file name
    num_inputs= int(sys.argv[3])
    inputs=sys.argv[4:4+num_inputs]
    # arg3 = sys.argv[3] # Third argument is the concrete parameter value of a
    # arg4 = sys.argv[4] # fourth argument is the concrete parameter value of b
    process_formula(arg1, arg2, num_inputs,inputs)

