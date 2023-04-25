#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>

const short GAP_i = -1;
const short GAP_d = -1;
const short MATCH = 2;
const short MISS_MATCH = -1;
const short CENTER = 0;
const short NORTH = 1;
const short NORTH_WEST = 2;
const short WEST = 3;

extern "C"{
    void compute_matrices(
	char *string1, char *string2,
	int *max_index, int *similarity_matrix, short *direction_matrix, int N, int M)
{
	int index = 0;
	int i = 0;
	int j = 0;
	int k = 0;
	int f = 0;

	int SIZE = 100;
	// Following values are used for the N, W, and NW values wrt. similarity_matrix[i]

	int north = 0;
	int west = 0;
	int northwest = 0;

	int max_value = 0;
	int max_cell = 0;//maximum for the whole array
	int max_cell_index = 0;//starting point

	int match;
	int test_val;
	int dir;
	int iter= 5; //temp memory database
	int mod_matrix = (M+2*N-2)%iter;
	int x = 0;

	char  string_query [70];
	char  string_database [200];

	int m =  M + 2*(N - 1);

	int a[SIZE];
	int b[SIZE];
	int c[SIZE];
	short direction[SIZE];

	memset(a, 0, sizeof(int) * SIZE);
	memset(b, 0, sizeof(int) * SIZE);
	memset(c, 0, sizeof(int) * SIZE);

	// Scan the N*M array row-wise starting from the second row.


	//n <- columns
	//m <- rows

	//(N-1)*N starting point because of padding
	int starting_point = (N-1)*N;

	//-----All Query-----//
	memcpy( string_query , string1, sizeof(char)*N);


	//-----Modulo database-----//
	//We load the modulo from whole padded matrix
	if(mod_matrix != 0){
		memcpy(string_database, string2, sizeof(char)*(mod_matrix));
	}

	//-----First Iter-----//	

	while((mod_matrix + iter*x) < (N )){ // for whole padding
		memcpy((string_database + mod_matrix + iter*x), (string2 + mod_matrix + iter*(x)), sizeof(char)*iter);
		x ++;
		
	}

	  for(i=0; i<= (M+N-2) ;i++){
			#pragma HLS UNROLL factor=4
	       for(j=0; j<=(N-1); j++){
			#pragma HLS UNROLL factor=8

	        	k = i +(N-1);

	        	index = k*N + j + j*N;

	        	//-----Iter Database-----//
	        	if((j == 0) && (((k-mod_matrix)%iter) == 0 )){
	        	//memcpy from string
	        		memcpy((string_database + iter*x + mod_matrix) , (string2+ iter*x + mod_matrix), sizeof(char)*iter);
	        		x ++;
	        	}

	        	if(j == 0){
					west = 0;
					northwest = 0;
					north = b[j] - 1;
				}
				else{
					north = b[j] - 1;
					west = b[j-1] - 1;
					northwest = a[j-1];

				}
				if(string_query[j] == string_database[k-j]){
					match = MATCH;
				}
				else{
					match = MISS_MATCH;
				}

				max_value = northwest + match;
				dir = NORTH_WEST;

				//search for maximum value[test_val, north, west, 0]
                //north
                if (max_value < north) {
                    max_value = north;
                    dir = NORTH;
                }
				//west
				if (max_value < west) {
					max_value = west;
					dir = WEST;
				}

				if (max_value <= 0) {
					max_value = 0;
					dir = CENTER;
				}
				//write
				c[j] = max_value;
				direction[j]= dir;

				if (max_cell < max_value) {
					max_cell = max_value;
					max_index[0] = index;
				}


	        }

	        memcpy(similarity_matrix +(i*(N)),  c , sizeof(int)*N);
	        memcpy(direction_matrix +(i*(N)),  direction , sizeof(int)*N);

	        memcpy(a, b, sizeof(int) * N);
	        memcpy(b, c, sizeof(int) * N);
	        memset(c, 0, sizeof(int) * N);
	        memset(direction, 0, sizeof(int) * N);

	    }





    }
}
/************************************************************************/
