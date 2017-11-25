/*
 * banker.cpp
 *
 * Student Name:Matthew Wojcik  
 * Student Number:10151233
 *
 * Class: CPSC 457 Spring 2017
 * Instructor: Pavol Federl
 *
 * Copyright 2017 University of Calgary. All rights reserved.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <algorithm>

using namespace std;

class Banker
{
private:
    int numProc;      // the number of processes
    int numResources; // the number of resources
    int * available;  // number of available instances of each resource
    int ** max;       // the max demand of each process, e.g., max[i][j] = k
                      // means process i needs at most k instances of resource j
    int ** allocation;// the number of resource instances already allocated
    int ** need;      // the number of resource isntances needed by each process

public:

    /* Initializing the vectors and matrixes for the Banker's Algorithm. Takes ownership of
     * all arrays.
     * @param avail  The available vector
     * @param m      The max demand matrix
     * @param alloc  The allocation matrix
     * @param p      The number of processes
     * @param r      The number of resources
     */
    Banker (int * avail, int ** m, int ** alloc, int p, int r) {
        numProc = p;
        numResources = r;

        // Setup the available vector, the max matrix, and the allocation matrix
        available = avail;
        max = m;
        allocation = alloc;

        // Initialize the need matrix
        need = new int*[numProc];
        for (int i = 0; i < numProc; i++)
            need[i] = new int[numResources];
    }

    /* Destroy the vectors and matrixes
     */
    ~Banker() {
        numProc = 0;
        numResources = 0;

        // Free all allocated memory space
        delete[] available;
        for (int i = 0; i < numProc; i++)
        {
            delete[] need[i];
            delete[] max[i];
            delete[] allocation[i];
        }
        delete[] need;
        delete[] max;
        delete[] allocation;
    }

    /* Check whether it is safe to grant the request
     * @param pid    The process that is making the request
     * @param req    The request
     * @param sequenceOrReason  The safe execution sequence returned by the algorithm
     * @return Whether granting the request will lead to a safe state.
     */
    bool isSafe (int pid, int * req, string & sequenceOrReason) {
      int procsDone = 0;
      int i,j;
      //check if resources desired is more than the process said they would use
      for(i = 0;i< numResources;i++){
        if(((max[pid][i]) - (allocation[pid][i])) < req[i]){
            sequenceOrReason = "Invalid request (exceeded max allocation)";
            return false;
          }
        }
      //check if you have enough resources
      for(i = 0;i< numResources;i++){
        if(available[i] < req[i]){
            sequenceOrReason = "Not enough resources for this request";
            return false;
          }
        }
      
      //build the need matrix
      for(i = 0;i< numProc;i++){
        for(j = 0;j< numResources;j++){
          need[i][j] = max[i][j] - allocation[i][j];}}
      
      //check if its a safe state or not
      
      
      
      sequenceOrReason = "";
Step1:      
      int *work = new int[numResources];
      copy(available, available + numResources, work);
      
      //see how many resources youll have to work with if the request goes through
      for(i=0;i<numResources;i++){
        work[i] = work[i] - req[i];
        allocation[pid][i] = allocation[pid][i] + req[i];
        need[pid][i] = need[pid][i] - req[i];
        }
        
      bool *Finish = new bool[numProc];
      //instantiate the finished array
      for(i =0; i<numProc;i++){
        Finish[i] = false;
      }
      
      int ok = 0; //ok used to see if any of the resources were too little
Step2:

      for(i =0; i<numProc;i++){
        ok = 0;
        //if theyre not finished
        if(Finish[i] == false){
          //check if it can run
          for(j = 0;j < numResources;j++){
            if(need[i][j] > work[j]){
              ok--;
            }
          }
          //see if all the resources were okay, then we run the process
          if(ok == 0) goto Step3;
      }
    }
    goto Step4;

    //update what you have to work with
Step3: 
      for(j = 0; j<numResources;j++){
          work[j] = work[j] + allocation[i][j];
      }
      //cout << work[0] << work[1] << work[2] << ".\n"; //testing
      Finish[i] = true;
      procsDone++; //for printing
      //build the output string
      if(procsDone < numProc){
            sequenceOrReason.append("P" + to_string(i) + ", ");
          }
      else{
            sequenceOrReason.append("P" + to_string(i));
          }
          
      goto Step2;
      
       //see if anything was un- finished to decide if its safe or not  
Step4:      
      for(i=0;i < numProc;i++){
        if(Finish[i] != true){
          sequenceOrReason = "This would result in an unsafe state";
          return false;
        }
      }             
      return true;
    }
};

int main (int argc, char * const argv[])
{
    ifstream config;       // Configuration file
    string conffile;       // The configuration file name
    int numProc;           // The number of processes
    int numResources;      // The number of resources
    string sequenceOrReason;       // The execution sequence returned by the Banker's Algorithm
    int i, j, index;       // Indices for the vectors and matrixes
    int pid;               // The ID of the process that is making the request
    string reqStr;         // The request vector in string format

    // Read in the config file name from the commanda-line arguments
    if (argc < 2)
    {
        cout << "Usage: banker <config file>\n";
        return 0;
    }
    else
    {
        conffile = argv[1];
    }

    // Open the file
    config.open(conffile.c_str());

    // Get the number of process and the number of resources
    string line, var, equal;    // strings for parsing a line in the config file
    getline(config, line);
    istringstream iss(line);
    iss >> var >> equal >> numProc;     // Get the number of processes
    iss.clear();

    getline(config, line);
    iss.str(line);
    iss >> var >> equal >> numResources;    // Get the number of resources
    iss.clear();

    // Create the available vector, the max matrix, and the allocation matrix
    // according to the number of processes and the number of resources
    int * available = new int[numResources];
    int ** max = new int*[numProc];
    int ** allocation = new int*[numProc];
    for (int i = 0; i < numProc; i++)
    {
        max[i] = new int[numResources];
        allocation[i] = new int[numResources];
    }

    // Get the available vector
    getline(config, line);
    replace(line.begin(), line.end(), '<', ' ');  // Remove "<" and ">"
    replace(line.begin(), line.end(), '>', ' ');
    iss.str(line);
    iss >> var >> equal;
    for (j = 0; j < numResources; j++)        // Read in the "available" vector
        iss >> available[j];
    iss.clear();

    // Get the max matrix and the allocation matrix
    for (i = 0; i < numProc; i++)
    {
        getline(config, line);
        replace(line.begin(), line.end(), '<', ' ');
        replace(line.begin(), line.end(), '>', ' ');
        iss.str(line);
        iss >> var;
        index = atoi(&var.at(1));            // Get the process ID
        if (index < 0 || index >= numProc)
        {
            cerr << "Invalid process ID: " << var << endl;
            return 0;
        }

        // Get the number of resources allocated to process "index".
        for (j = 0; j < numResources; j++)
            iss >> allocation[index][j];

        // Get the max allocation to process "index".
        for (j = 0; j < numResources; j++)
            iss >> max[index][j];

        iss.clear();
    }

    // Get the request vector
    int * request = new int[numResources];
    getline(config, line);
    reqStr = line.substr(line.find('<'), line.find('>') - line.find('<') + 1);
    replace(line.begin(), line.end(), '<', ' ');
    replace(line.begin(), line.end(), '>', ' ');
    iss.str(line);
    iss >> var >> pid >> equal;
    for (j = 0; j < numResources; j++)          // Read in the "request" vector
        iss >> request[j];
    iss.clear();

    // Check the request using the Banker's algorithm.
    Banker * banker = new Banker(available, max, allocation, numProc, numResources);
    if (banker -> isSafe(pid, request, sequenceOrReason))
        cout << "Grant request " << reqStr << " from P" << pid << ".\n"
             << "Sequence: " << sequenceOrReason << ".\n";
    else
        cout << "Reject request " << reqStr << " from P" << pid << ".\n"
             << "Reason: " << sequenceOrReason << "\n";
}
