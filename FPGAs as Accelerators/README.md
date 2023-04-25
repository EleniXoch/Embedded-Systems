# Embedded-Systems-FPGA-Acceleration

This project is a part of ECE 340 Embedded Systems course at University of Thessaly

We present 2 versions of the C code for the same [algorithm](https://github.com/EleniXoch/Embedded-Systems/blob/main/FPGAs%20as%20Accelerators/README.md#explaining-the-algorithm). The [software implementation](https://github.com/EleniXoch/Embedded-Systems/blob/main/FPGAs%20as%20Accelerators/lsal_sw.c) was profiled with CPU performance in mind, while the [hardware implementation](https://github.com/EleniXoch/Embedded-Systems/blob/main/FPGAs%20as%20Accelerators/lsal_hw.cpp) was run in Xilinx Zedboard FPGA. 

An OpenCL-based host unit was responsible for all the data management and the configuring the hardware accelerator. It was also used to verify the correct algorithm execution by blocking, waiting and checking the result. 

Below we explain our train of thought and basic conclusions after experimenting with different versions and techniques

# Optimization techniques

## Padding

- Left and Right padding: Extra row and column in order to avoid costly if/else statements

![square drawio](https://user-images.githubusercontent.com/123579658/234270519-529e75f1-d4f0-45bd-808b-39f6ab8e2516.png)


- N-padding: Padding focused on dependencies of our algorithm

We fill the matrix with random A-C-T-G nucleobases while the padding is filled with the letter 'W' to show the parts of the padded matrix. 

## Memory Handling 

- Anti-diagonals: 
Turning antidiagonals into rows taking into consideration that we need 2 anti diagonals to calculate the third one. Each cell/computation now depends on the upper 2 rows as the arrows show in the diagram below. 

<img align="middle" src="https://user-images.githubusercontent.com/123579658/234271914-274b4295-f92b-4d4f-9556-8cbb1eac9a49.png" width=30% height=30%>

- Iter:
There is a memory limit in FPGA and therefore we load our matrices with multiple iterations depending on the size/shape of the matrix. This can be achieved by using the iter parameter to load blocks of memory and taking care of the modulo upfront.

<img src="https://user-images.githubusercontent.com/123579658/234336887-7dbe1bc5-200f-4113-a8af-6ccb36152897.png" width=20% height=20%>


- Directives: 
The vitis HLS tool allowed us to easily implement HLS directives. In the loops we found it was most beneficial we used UNROLL to transform our loops.

# Metrics and results


As a result of the above optimizations for hardware performance, we can see that our final version (version 3) is slower when run in a CPU than our initial implementation profiled specifically for CPU(version 1). 

<img src="https://user-images.githubusercontent.com/123579658/234267498-cf17cab7-bf9b-4b67-8bc4-7a06bee66f74.png" width=40% height=40%>





# Explaining the Algorithm 
This smith-waterman algorithm is used to compare two strings of A-C-T-G nucleobases (Query/N and Database/M)
and searches for the longest local sequence alignment. The algorithm works as follows, one letter from the first string
(Query) is compared to another letter from the second string (Database). In order for it to compute the value of this
comparison, it creates 3 temporary variables which contain the values of the west (cell on the left of the current one), the
north (cell above the current one) and the northwest(cell on the up-left of the current one) cells.
<figure>
  <img align="middle" src="https://user-images.githubusercontent.com/123579658/234262882-114b36b3-6f3b-4ca2-84aa-776f2debbdc0.png">
</figure>


If the letters of the strings are the same we have a ’match’ and the temporary variable for the northwest cell , will be
augmented by 2. If the letters are not the same (’no match’), then this value will be reduced by 1. The values of west and
north cells will be always reduced by one. The algorithm picks the largest of these numbers and gives it to the cell
that corresponds to the letters compared. If all the numbers are below zero, the cell’s value will be zero. The first version
(lsal full) we created runs in this way, calculating row by row the elements until the matrix is filled.
