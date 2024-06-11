

# import numpy as np  
import sys
import random

def write_to_test_file(filename):
    tosave=[]
    for k in range(2):
        for i in range(3):
            for j in range(7):
                tosave.append([i,j])
    
    for i in range(2):
        for j in range(95,97):
            tosave.append([i,j])

           
    random.shuffle(tosave)
                
    # Write to filename
    with open(filename, 'w') as f:
        for data in tosave:
            f.write(f'{data[0]} {data[1]}\n')
if __name__ == '__main__':
    # Generate random input data
    if len(sys.argv) > 1:
        filename = sys.argv[1]
    else:
        filename = '/home/opdfi/opdfi/src_test/test_inputs.txt'

    write_to_test_file(filename)
    print(f'Input data saved to {filename}')