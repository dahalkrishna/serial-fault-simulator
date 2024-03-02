#include "graph.h"
/***************************************************************************************************************************
 Main Function(Serial and Parallel Fault Simulation)
****************************************************************************************************************************/
void main(int argc,char **argv)
{
FILE *fisc,*fvec,*ffau,*fres, *ffau_res, *fdet;             //file pointers used for .isc file, .vec file, .faults file and resultfile
int Max,Opt,Npi,Npo,Total,Tfs, after_fault;              //maxnode id,option,tot no of PIs,tot no of Pos,Tot no of input patterns& faults in.vec in.faults
int *in_out;
NODE graph[Mnod];                         //structure used to store the ckt information in .isc file 
PATTERN vector[Mpt];                      //structure used to store the input vectors information in .vec file 
LIST *temp = NULL;
FAULT stuck[Mft];                      //structure used to store the faults information in .faults file
int a,b,c,d;                             //random variables
int flag;

//Read the .isc file and store the information in graph structure
fisc=fopen(argv[1],"r");                           //file pointer to open .isc file 
Max=0; 
Max=ReadIsc(fisc,graph);                 //read .isc file and return index of last node in graph formed
fclose(fisc);                            //close file pointer for .isc file
//PrintCircuit(graph,Max);                 //print all members of graph structure
in_out = PrimaryInputOutputNumber (graph, Max);
Npi = in_out[0];    //Total number of Primary Inputs
Npo = in_out[1];    //Total number of Primary Outputs
//Read the .vec file and store the information in vector structure
fvec=fopen(argv[2], "r");
Total = ReadVec(fvec, vector);		//returns the total number of input patteren to be supplied
fclose(fvec);
// PrintVector(vector, Total, Npi); 
fres=fopen(argv[3],"w");                           //file pointer to open .res file for printing results
//Perform Logic Simulation for each Input vector and print the Pos .val in output file 

LogicSimulation(graph, vector,Max, Npi, Total, fres);		//simulates the "*.isc" circuit based on the given input paterns
fclose(fres);                                                  //close file pointer for .out file
// Read the .faults file and store the information in fault structure
ffau = fopen(argv[4], "r");
Tfs = ReadFaults(ffau, stuck);
fclose(fvec);

ffau_res = fopen(argv[5], "w");
FaultSimulation(graph, vector, Max,  Npi,  Total, ffau_res,  flag, stuck, Tfs);     //simulates the "*.isc" circuit based on the given input paterns and injected faults
fclose(ffau_res);
ClearCircuit(graph,Mnod);
                                      //clear memeory for all members of graph
for(a=0;a<Total;a++){ bzero(vector[a].pattern,Mpi); }                //clear memeory for all members of vector
return;
}//end of main
/****************************************************************************************************************************/

