#include "graph.h"
//					 0	1	X	D	Db
int ANDG[5][5] ={	{0,	0,	0,	0,	0 },		//0
					{0,	1,	X,	D,	Db},		//1
					{0,	X,	X,	X,	X },		//X
					{0,	D,	X,	D,	0 },		//D
					{0,	Db,	X,	0,	Db}};		//Db
					
//					 0	1	X	D	Db
int ORG[5][5] = {	{0,	1,	X,	D,	Db},		//0
					{1,	1,	1,	1,	1 },		//1
					{X,	1,	X,	X,	X },		//X
					{D, 1,	X,	D,	1 },		//D
					{Db,1,	X,	1,	Db}};		//Db
					
//					 0	1	X	D	Db					
int NANDG[5][5] ={	{1,	1,	1,	1,	1 },		//0
					{1,	0,	X,	Db,	D },		//1
					{1,	X,	X,	X,	X },		//X
					{1,	Db,	X,	Db,	1 },		//D
					{1,	D,	X,	1,	D}};		//Db
					
//					 0	1	X	D	Db
int NORG[5][5] = {	{1,	0,	X,	Db,	D },		//0
					{0,	0,	0,	0,	0 },		//1
					{X,	0,	X,	X,	X },		//X
					{Db, 0,	X,	Db,	0 },		//D
					{D,0,	X,	0,	D}};		//Db	
					
//					 0	1	X	D	Db
int NOTG[5] = 		{1,	0,	X, 	Db,	D }; 
//	stuck at value	 0	1	X			//correct value
int FAULTG[3][3] ={	{0,	Db,	X},			//0
					{D, 1 ,	X},			//1
					{0, 1 ,	X}};		//X


// Read the number of primary inputs
/**
 * Calculates the number of nodes Inputs (Type=1) and Outputs (Po=1) in the given graph.
 *
 * @param graph The graph represented as an array of NODE structures.
 * @param Max The maximum index of the graph array.
 * @return An integer array containing the number of primary inputs and outputs.
 */
int* PrimaryInputOutputNumber (NODE *graph, int Max)
{
	int i;
	static int in_out[] ={0,0};
	for (i=0; i<=Max; i++)
	{
		if (graph[i].Type == 1)
			in_out[0]++;
		if (graph[i].Po ==1)
			in_out[1]++;
	}
	return in_out;
}

/*****************************************************************************************************************************
 Routine to Print the Input Patterns(.vec files)
*****************************************************************************************************************************/

/**
 * Prints the elements of a vector of patterns.
 *
 * @param vector The vector of patterns.
 * @param Total The total number of patterns in the vector.
 * @param Npi The number of elements in each pattern.(which is equal to the number of primary inputs Npi)
 */
void PrintVector (PATTERN *vector, int Total, int Npi)
{
	int i, j;
	printf("Pattern\n");
	for (i = 0; i < Total; i++)
	{
		for (j = 0; j < Npi; j++)
			printf("%d", vector[i].pattern[j]);
		printf("\n");
	}
}

/**
 * Reads patterns from a file and stores them in a vector.
 *
 * This function reads patterns from a file line by line and stores them in a vector.
 * Each character in the file is checked and mapped to a corresponding value in the vector.
 * The mapping rules are as follows:
 * - 'x' or 'X' is mapped to X
 * - '1' is mapped to 1
 * - '0' is mapped to 0
 * - '\n' is ignored
 * Any other character encountered in the file is considered an invalid pattern and the program exits.
 *
 * @param fvec The file pointer to the input file.
 * @param vector The array of structures where the patterns will be stored.
 * @param Mpi The maximum number of characters to read from each line of the file.
 * @return The number of patterns successfully read and stored in the vector.
 */
int ReadVec(FILE *fvec, PATTERN *vector)
{
	char fvec_content[Mpi];
	int i, j = 0;
	while (fgets(fvec_content, Mpi, fvec))	//Read one line at a time from the .vec file
	{
		i =0;
		while (fvec_content[i] != '\0')
		{
			if (fvec_content[i]=='x' || fvec_content[i] == 'X')
				vector[j].pattern[i] = X;
			else if (fvec_content[i]=='1')
				vector[j].pattern[i] = 1;
			else if (fvec_content[i]=='0')
				vector[j].pattern[i] = 0;
			else if ((fvec_content[i]=='\n'))
				;
			else
			{
				printf("Invalid Pattern\n");
				exit(0);
			}
			i++;
		}
		j++;
	}
	return j;
}

/**
 * Performs logic simulation on a given circuit using a set of input patterns.
 *
 * @param graph The circuit graph represented as an array of NODE structures.
 * @param vector The array of input patterns.
 * @param Max The maximum number of nodes in the circuit.
 * @param Npi The number of primary inputs in the circuit.
 * @param Total The total number of input patterns.
 * @param fres The file pointer to write the simulation results.
 */
void LogicSimulation(NODE *graph, PATTERN *vector, int Max, int Npi, int Total, FILE *fres)
{
	int i,k;
	// Perform logic simulation for each input pattern
	for (i = 0; i < Total; i++)
	{
		int j = 0; // Index for input pattern bits
		// Iterate through the nodes in the circuit
		for (k = 1; k <= Max; k++)
		{
			if (graph[k].Type != 0)
			{
				if (graph[k].Type == INPT) // Primary input node
				{
					graph[k].Cval = vector[i].pattern[j];
					j++;
				}
				else
				{
					Gate(graph, k); // Gate simulation
				}
			}
		}
		StoreOutput(graph, Max, fres); // Store the output values
	}
}

/**
 * Simulates fault injection in a circuit using given patterns and detects faults.
 *
 * @param graph The circuit graph represented as an array of NODE structures.
 * @param vector The array of patterns to be applied to the circuit.
 * @param Max The maximum number of nodes in the circuit.
 * @param Npi The number of primary inputs in the circuit.
 * @param Total The total number of patterns to be applied.
 * @param ffau_res The file pointer to the output file for storing the simulation results.
 * @param flag The flag variable used for detecting faults.
 * @param stuck The array of faults to be injected into the circuit.
 * @param Tfs The total number of faults to be injected.
 */
void FaultSimulation(NODE *graph, PATTERN *vector,int Max, int Npi, int Total, FILE *ffau_res, int flag, FAULT *stuck, int Tfs)
{
	int i,j, k,l;
	
	for(l = 0; l< Tfs; l++)
	{
		//printf("%d/%d\t", stuck[l].gate, stuck[l].fault);
		fprintf(ffau_res,"%d /%d\n", stuck[l].gate, stuck[l].fault);
		for (i=0; i <Total; i++)	//go through the total number of patterns
		{
			flag = 0;
			j = 0;		//go through each bit in the input pattern
			for (k=0; k<=Max; k++)		//go througn the nodes in the circuit
			{
				if (graph[k].Type != 0)
				{ 
					if (graph[k].Type==INPT) // if it is primary input
					{
						graph[k].Cval = vector[i].pattern[j];
						if (vector[i].pattern[j]==1)
							fputc('1',ffau_res);
						else if (vector[i].pattern[j]==0)
							fputc('0',ffau_res);
						else if (vector[i].pattern[j]==2)
							fputc('x',ffau_res);
						j++;
					}
					else 
						Gate(graph,k);//gate simulation;
					if (k == stuck[l].gate)
					{
						graph[k].Cval = FAULTG[graph[k].Cval][stuck[l].fault];
					}
					if (graph[k].Po ==1)
					{
						if (graph[k].Cval == D || graph[k].Cval == Db)
							flag = 1;
					}		
				}

			}
			fputs("\t", ffau_res);
			if (flag)
				fputs("Detected\n", ffau_res);
			else
				fputs("Not Detected\n", ffau_res);
		}
	}
}

/**
 * @brief Performs gate operation on a given node in the graph.
 * 
 * This function takes a graph and a node as input and performs the gate operation on that node.
 * The gate operation depends on the type of gate associated with the node.
 * The function updates the Cval (current value) of the node based on the gate operation.
 * 
 * @param graph The graph containing the nodes.
 * @param node The node on which the gate operation is to be performed.
 */
void Gate (NODE *graph,int node)
{
	int op;
	LIST *fan_in;
	fan_in = graph[node].Fin;
	switch (graph[node].Type)
	{
		case NOT:
				graph[node].Cval = NOTG[graph[fan_in->id].Cval];	//the value of fanin to the not gate is inverted
		break;
		
		case FROM:
				graph[node].Cval = graph[fan_in->id].Cval;
		break;
		
		case BUFF:	
				graph[node].Cval = graph[fan_in->id].Cval;
		break;
		
		case AND:	
				op = graph[fan_in->id].Cval;
				fan_in = fan_in->next;
				while(fan_in != NULL)
				{
					op = ANDG[op][graph[fan_in->id].Cval];
					fan_in = fan_in->next;
				}
				graph[node].Cval = op;
		break;
		
		case NAND:
				op = graph[fan_in->id].Cval;
				fan_in = fan_in->next;
				while(fan_in != NULL)
				{
					op = ANDG[op][graph[fan_in->id].Cval];
					fan_in = fan_in->next;
				}
				graph[node].Cval = NOTG[op];
		break;
		
		case OR:	
				op = graph[fan_in->id].Cval;
				fan_in = fan_in->next;
				while(fan_in != NULL)
				{
					op = ORG[op][graph[fan_in->id].Cval];
					fan_in = fan_in->next;
				}
				graph[node].Cval = op;
		break;
		
		case NOR:	
				op = graph[fan_in->id].Cval;
				fan_in = fan_in->next;
				while(fan_in != NULL)
				{
					op = ORG[op][graph[fan_in->id].Cval];
					fan_in = fan_in->next;
				}
				graph[node].Cval = NOTG[op];
		break;
		case XOR:
				if(graph[fan_in->id].Cval == graph[fan_in->next->id].Cval)
					graph[node].Cval = 0;
				else 
					graph[node].Cval = 0;
		break;
		case XNOR:
				if(graph[fan_in->id].Cval == graph[fan_in->next->id].Cval)
					graph[node].Cval = 1;
				else 
					graph[node].Cval = 0;
		break;
		default:
			printf("Unknown gate type");
			exit(1);
		
	}
}

//Print output after simulation
/**
 * Stores the output of the graph in a file.
 * 
 * @param graph The graph containing the nodes.
 * @param Max The maximum number of nodes in the graph.
 * @param fres The file pointer to the output file.
 */
void StoreOutput(NODE *graph, int Max, FILE *fres)
{
	int i,j;
	j =0;
	for (i=0; i<=Max; i++)
	{
		if (graph[i].Type != 0)
		{
			if (graph[i].Po == 1)
			{
				if (graph[i].Cval ==1){
					// printf("1");
					fputc('1', fres);
				}
					
				else if (graph[i].Cval == 0){
					// printf("0");
					fputc('0', fres);
				}
				else if (graph[i].Cval == 2){
					fputc('x', fres);
				}
				else
					;
			}
		}
	}
	// printf("\n");
	fputc('\n', fres);
}

/**
 * Reads faults from a file and stores them in an array of FAULT structures.
 *
 * @param ffau The file pointer to the file containing the faults.
 * @param stuck The array of FAULT structures to store the read faults.
 * @return The number of faults read from the file.
 */
int ReadFaults(FILE *ffau, FAULT *stuck)
{
	char ffau_content[Mpi];
	int i = 0, j = 0;
	int gate, fault;
	while (fgets(ffau_content, Mpi, ffau))    //Read one line at a time from the .vec file
	{
		sscanf(ffau_content, "%d /%d", &gate, &fault); //because the faults are provided in the format GATE /STUCK
		stuck[i].gate = gate;
		stuck[i].fault = fault;
		i++;
	}
	return i;
}