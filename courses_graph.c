#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 

// COLOR MACROS
#define WHITE 0
#define GREY 1
#define BLACK 2

// SIZE MACROS 
#define NAME_SZ 31
#define LINE_SZ 1001

struct Course{

	char name[NAME_SZ]; 
	char * pre_req_ptr; 	
	int color; 
	int original_idx; 

}; 

int countLines(FILE** fp);
void buildA(int N, int A[N][N], struct Course courses[N]);
void printMatrix(int N, int A[N][N]); 
int topoSort(int N, int A[N][N], struct Course courses[N]); 
int DFS(int N, int A[N][N], struct Course courses[N], struct Course sorted[N], int * sorted_idx);
int DFS_visit(int N, int A[N][N], int idx, struct Course courses[N], struct Course sorted[N], int * sorted_idx);

int main(){


	// print instructions 
	printf("\nThis program will read, from a file, ");
	printf("a list of courses and their prerequisites and will print the list in which to take courses.\n"); 	

	// get filename and check if its valid 
	FILE* fp = NULL; 
	char fileName[NAME_SZ]; 
	printf("Enter filename: "); 
	scanf("%31s", fileName); 	
	if((fp = fopen(fileName, "r")) == NULL){
		printf("Could not open file %s. Exit.\n\nFailed to read from file. Program will terminate.\n\n", fileName); 
		exit(1); 
	} 

	// get number of lines = num of courses and init courses array 
	int num_of_courses = countLines(&fp); 

	// build array of courses
	struct Course courses[num_of_courses]; 	
	
	// reset file pointer to beginning 
	rewind(fp); 

	// read and store courses, record prereqs
	int idx = 0, line_idx = num_of_courses; 
	char line[LINE_SZ] = "\0"; 
	char * ptr = NULL; 
	while(line_idx){			// continue until line_idx == 0

		// get whole lines
		fgets(line, LINE_SZ - 1, fp); 

		// strtok
		// get ptr to course name
		ptr = strtok(line, " \n"); 
		
		// add course name 
		int i; 
		for(i = 0; i < NAME_SZ; ++i){ courses[num_of_courses - line_idx].name[i] = ptr[i]; } 

		// get ptr to string with prereqs
		ptr = strtok(NULL, "\n"); 

		// store pre reqs on heap, NULL if none
		if(ptr == NULL) courses[num_of_courses-line_idx].pre_req_ptr = ptr; 
		else{
			courses[num_of_courses-line_idx].pre_req_ptr = (char*) malloc(LINE_SZ*sizeof(char)); 
			for(i = 0; ptr[i] != '\0'; ++i){ courses[num_of_courses-line_idx].pre_req_ptr[i] = ptr[i]; } 
		}

		// increment to next course 	
		line_idx--; 

	}


	// print num of vertices 
	printf("Number of vertices in built graph: N = %d\n", num_of_courses); 


	// print each vertex by index 
	int i; 
	printf("Vertex - coursename correspondence\n"); 
	for(i = 0; i < num_of_courses; ++i) printf("%d - %s\n", i, courses[i].name); 


	// build and print adjacency matrix, parse prereq strings for data 
	int A[num_of_courses][num_of_courses];		 // construct matrix 
	buildA(num_of_courses, A, courses); 	 	 // fill out matrix with pre req data 

	// print adjacency matrix
	printMatrix(num_of_courses, A); 

	// topological sort, check for cycles 
	int cycles = topoSort(num_of_courses, A, courses); 


	// print courses in order if successful 
	if(!cycles){
		printf("Order in which to take courses:\n"); 
		int order = 1; 
		for(i = num_of_courses-1; i >= 0; --i){
			// print courses 
			printf("%d. - %s (corresponds to graph vertex %d)\n", order++, courses[i].name, courses[i].original_idx); 
		}
	}
	else{
		printf("There was at least one cycle. There is no possible ordering of the courses.\n"); 
	}

	printf("\n"); 
	

	// close files
	if(fclose(fp) == EOF) printf("Could not close file\n"); 

	// free pre req memory 
	for(i = 0; i < num_of_courses; ++i) free(courses[i].pre_req_ptr); 

	return 0; 
}


// Returns num of lines in file 
int countLines(FILE** fp){

	
	int num_of_courses = 0; 
	char c = getc(*fp); 
	while(c != EOF){

		if(c == '\n') num_of_courses++; 
		c = getc(*fp); 

	}

	return num_of_courses; 

}

// Fill out adjacency matrix 
void buildA(int N, int A[N][N], struct Course courses[N]){

	// initialize matrix elements to 0
	int i,j;
	for(i = 0; i < N; ++i){
		for(j = 0; j < N; ++j){
			A[i][j] = 0; 
		}
	}

	
	// mark adjacent relationships in matrix 
	char * ptr = NULL; 
	for(i = 0; i < N; ++i){
		
		// idx# = vertex#
		// for each course, parse pre req string 
		ptr = strtok(courses[i].pre_req_ptr, " \n"); 
		while(ptr != NULL){ 				// keep checking while there are still pre reqs 

			// check name against list of courses 			
			for(j = 0; j < N; ++j){
				if(j != i){			// don't check current course 

					// if there is a match > mark current courses col in the row of match as 1
					if(strcmp(courses[j].name, ptr) == 0){
						A[j][i] = 1; 
					} 
				}
			}
			
			ptr = strtok(NULL, " \n"); 
		}
	}
}

// Print adjacency matrix
//Adjacency matrix:
//    |   0   1   2
//-----------------
//   0|   0   1   1
//   1|   0   0   0
//   2|   1   1   0
void printMatrix(int N, int A[N][N]){

	printf("\nAdajacency Matrix:\n"); 

	int i, j, k;
	for(i = 0; i < N + 1; ++i){
		for(j = 0; j < N; ++j){
			
			// 0th row needs space and col index 
			if(i == 0){

				if(j == 0){
					// print space
					printf("    |"); 

					// print col idx
					printf("   %d", j);
				}
				else{
					// print col idx
					printf("   %d", j);
				}
	
				// print separation line ----- at end of row
				if(j == (N - 1)){
					printf("\n"); 
					for(k = 0; k < N; ++k){
						if(k == 0) printf("---------"); 
						else printf("----");
					}
				}
			}			
			// otherwise print row index then matrix contents 
			else{

				if(j == 0) printf("   %d|", i-1); 	// print row index first
				printf("   %d", A[i-1][j]);

			}
		}
		printf("\n"); 
	}
	printf("\n"); 
} 

int DFS_visit(int N, int A[N][N], int idx, struct Course courses[N], struct Course sorted[N], int * sorted_idx){
	int cycle = 0; 

	// set color of visited node to grey 
	courses[idx].color = GREY; 

	// iteraterate to its adjacenct values, call function recursively 
	int i; 
	for(i = 0; i < N; ++i){

		if(A[idx][i] == 1 && courses[i].color == WHITE){	// call for each adj course 
			cycle = DFS_visit(N, A, i, courses, sorted, sorted_idx); 
		}
		else if(A[idx][i] == 1 && courses[i].color == GREY){	// cycle exists, exit
			cycle = 1; 
		}

		if(cycle) break; 
	}	


	if(!cycle){
		// all adjacents are visited, place in sorted vector
		// mark as black 
		courses[idx].color = BLACK;				// set color to black 
		for(i = 0; courses[idx].name[i] != '\0'; ++i){
			sorted[*sorted_idx].name[i] = courses[idx].name[i]; // name
		}
		sorted[*sorted_idx].name[i] = '\0'; 	// NULL
		sorted[*sorted_idx].original_idx = idx; 		// original index 
		(*sorted_idx)++;					// increment to next sorted spot  
	}

	return cycle; 
}

int DFS(int N, int A[N][N], struct Course courses[N], struct Course sorted[N], int * sorted_idx){

	int cycle = 0; 

	// set all courses colors to white 
	int i; 
	for(i = 0; i < N; ++i) courses[i].color = WHITE; 
	
	// visit each node in courses as long as its white 
	for(i = 0; i < N; ++i){
		if(courses[i].color == WHITE) cycle = DFS_visit(N, A, i, courses, sorted, sorted_idx); 
		if(cycle) break; 
	}

	return cycle; 
}

// Topologically sort the courses in increasing order of dependency 
// Exit of the course nodes contain a cycle 
// returns 1 if cycle is found, 0 otherwise 
int topoSort(int N, int A[N][N], struct Course courses[N]){

	// create array to hold sorted courses 
	struct Course sorted[N]; 
	int sorted_idx = 0; 	// keeps track of next available sorted spot

	// DFS
	int cycle = DFS(N, A, courses, sorted, &sorted_idx); 

	// Only swap if there is no cycle found 
	if(!cycle){

		// place sorted courses in original array 
		// only swaps names 
		int i, j; 
		for(i = 0; i < N; ++i){ 
			for(j = 0; sorted[i].name[j] != '\0'; ++j){
				courses[i].name[j] = sorted[i].name[j]; // name
			}
			courses[i].name[j] = '\0'; // NULL
			courses[i].original_idx = sorted[i].original_idx; 
		}

	}

	return cycle; 
} 
