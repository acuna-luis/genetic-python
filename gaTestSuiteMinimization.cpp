
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <cstdlib>
#include <ctime>

#define N_INDIVIDUALS 100
#define MUTATION_RATE    0.01
#define CROSSOVER_PROBABILITY 0.9
#define INPUT_FILE "./input/matrix_94_3647_1.txt"
#define MAX_GENERATIONS 100

// Global Variables
int **currentGeneration;
int *generationFitness;
int totalOfFitnesses;
int numberEvaluations = 0;

int** reqMatrix;
int numberOfTests;
int numberOfReqs;

// Functions declarations
void allocateMemory(void);
int run(void);
void initializePopulation(void);
void evaluatePopulation(void);
void nextGen(void);
int selection(void);
int getBestSolution(void);
int getAverageSolution(void);
int eval(int*);
void mutate(int*);
int* crossover(int*, int*);
void displayMatrix(void);


// Allocate memory for our individuals
void allocateMemory(void){
    currentGeneration = new int*[N_INDIVIDUALS];

    generationFitness = new int[N_INDIVIDUALS];

    for(int index=0; index<N_INDIVIDUALS; ++index)
        currentGeneration[index] = new int[numberOfTests];
}


// Set random values to the individuals of the population
void initializePopulation(void){
    std::ifstream matrixFile(INPUT_FILE);
    std::string filename(INPUT_FILE);

    bool error = false;

    if(matrixFile.is_open()){

        numberOfReqs = stoi(filename.substr(filename.find("_", 0) + 1, filename.find("_", filename.find("_", 0) + 1))) + 1;
        numberOfTests = stoi(filename.substr(filename.find("_", filename.find("_", 0) + 1) + 1,
         filename.find("_", filename.find("_", filename.find("_", 0) + 1))));

        std::cout<<numberOfReqs<<":"<<numberOfTests<<std::endl;

        reqMatrix = new int*[numberOfTests];

        int i = 0;
        for (std::string line; std::getline(matrixFile, line); ){
            reqMatrix[i] = new int[numberOfReqs];

            for(int j = 0; j < numberOfReqs; ++j)
                reqMatrix[i][j] = line[j] - '0';

            ++i;
        }
    }
    else
        error = true;

    if(!error){

        // Allocate memory for our population
        allocateMemory();

        bool indexing[numberOfTests];
         
        for(int index=0; index<N_INDIVIDUALS; ++index){

            for(int i = 0; i < numberOfTests; ++i)
                indexing[i] = false;
            
            for(int gene=0; gene<numberOfTests; ++gene){
                int aux;

                do{
                    aux = rand() % numberOfTests;
                }while (indexing[aux]);

                indexing[aux] = true;

                currentGeneration[index][gene] = aux;
            }
        }
                
    }

}

// Evaluation of a individual
int eval(int* individual){
    int fitness = 0;

    bool satisfiedRequirements[numberOfReqs];
    for(int i = 0; i < numberOfReqs; ++i)
        satisfiedRequirements[i] = false;

    for(fitness = 0; fitness < numberOfTests; ++fitness){
        for(int j = 0; j < numberOfReqs; ++j)
            if(reqMatrix[individual[fitness]][j] == 1)
                satisfiedRequirements[j] = true;
        
        bool finish = true;
        for(int i = 0; i < numberOfReqs; ++i)
            if(!satisfiedRequirements[i]){
                finish = false;
                break;
            }

        if (finish)
            break;
    }

    numberEvaluations++;
    return fitness + 1;
}

// Returns the best fitness value
int getBestSolution(void) {
    float minFitness = numberOfTests;

    for (int i=0; i < N_INDIVIDUALS; ++i)
        if (generationFitness[i] < minFitness)
            minFitness = generationFitness[i];

    return minFitness;
}

// Returns the average fitness value
int getAverageSolution() {
  int avgFitness = 0;

  for (int i=0; i < N_INDIVIDUALS; ++i)
    avgFitness += generationFitness[i];

  avgFitness /= (double)N_INDIVIDUALS;

  return avgFitness;
}

// Evaluate all individuals
void evaluatePopulation(void){
  for(int i = 0; i < N_INDIVIDUALS; ++i)
    generationFitness[i] = eval(currentGeneration[i]);
}

// Binary Tournament Selection Operator
int selection(void){
    int i1 = rand() % (N_INDIVIDUALS);
    int i2;
    do{
        i2 = rand() % (N_INDIVIDUALS);
    }while(i1 == i2);

    if(generationFitness[i1] < generationFitness[i2])
        return i1;
    else
        return i2;
}

// PMX Crossover Operator
int * crossover(int* parent1, int* parent2){

    int point1 = rand() % numberOfTests;
    int point2;
    do{
        point2 = rand() % numberOfTests;
    }while(point1 == point2);

    int* offspring1 = new int[numberOfTests];
    int* offspring2 = new int[numberOfTests];

    std::vector<int> range;
    std::map<int, int> associations1;
    std::map<int, int> associations2;

    for(int i = point1 ; i < point2; i = (i + 1) % numberOfTests){
        range.push_back(i);

        associations1[parent1[i]] = parent2[i];
        associations2[parent2[i]] = parent1[i];

        offspring1[i] = parent2[i];
        offspring2[i] = parent1[i];
    }

    for(int i = 0; i < numberOfTests; ++i)
        if(std::find(range.begin(), range.end(), i) == range.end()){
            if(associations2.find(parent1[i]) != associations2.end()){
                int aux = parent1[i];
                do{
                    aux = associations2[aux];
                }while(associations2.find(aux) != associations2.end() && aux != parent1[i]);  
                offspring1[i] = aux;
            }
            else
                offspring1[i] = parent1[i];
            
            if(associations1.find(parent2[i]) != associations1.end()){
                int aux = parent2[i];
                do{
                    aux = associations1[aux];
                }while(associations1.find(aux) != associations1.end() && aux != parent2[i]);
                offspring2[i] = aux;
            }
            else
                offspring2[i] = parent2[i];

        } 

    int offFitness1 = eval(offspring1);
    int offFitness2 = eval(offspring2);

    if(offFitness1 < offFitness2){
        delete [] offspring2;
        return offspring1;
    }
    else{
        delete [] offspring1;
        return offspring2;
    }

}

// Mutation operator: random permutation of gene
void mutate(int* individual){
    for(int i=0; i<numberOfTests; ++i)
        if((rand()/(double)RAND_MAX) <= MUTATION_RATE) {
            int point1 = rand() % numberOfTests;
            int point2;
            do{
                point2 = rand() % numberOfTests;
            }while(point1 == point2);

            int aux = individual[point1];
            individual[point1] = individual[point2];
            individual[point2] = aux;
        }
}


// Evolve the population towards the next generation
void nextGen(void){
  int gene;
  int crossoverPoint;
  int mutateThisGene;

  for(int individual=0; individual<N_INDIVIDUALS; ++individual){

      // Selection
      int parent1 = selection();
      int parent2;
      do{
        parent2 = selection();
      }while(parent1 == parent2);

      // Crossover
      if( (rand()/(double)RAND_MAX) <= CROSSOVER_PROBABILITY){
        int* offspring = crossover(currentGeneration[parent1], currentGeneration[parent2]);

        // Mutation
        mutate(offspring);

        // Evaluation
        int fitnessOffspring = eval(offspring);

        // Recombination  
        if (fitnessOffspring <= generationFitness[individual]) { 
            delete [] currentGeneration[individual];
            currentGeneration[individual] = offspring;
            generationFitness[individual] = fitnessOffspring;
        }
        else
            delete [] offspring;
      }
    }

}

void displayMatrix(void){
    std::cout<<"#########################################"<<std::endl;
    for(int i = 0; i< N_INDIVIDUALS; ++i){
        for(int j = 0; j < numberOfTests; ++j)
            std::cout<<currentGeneration[i][j]<<",";
        std::cout<<std::endl;
    }
    std::cout<<"#########################################"<<std::endl;

    for(int i = 0; i< N_INDIVIDUALS; ++i){
        std::cout<<generationFitness[i];
        std::cout<<std::endl;
    }

    std::cout<<"#########################################"<<std::endl;

    for(int i = 0; i< numberOfTests; ++i){
        for(int j = 0; j < numberOfReqs; ++j)
            std::cout<<reqMatrix[i][j]<<",";
        std::cout<<std::endl;
    }

    std::cout<<"#########################################"<<std::endl;
}


// Performs the GA main loop: selection - recombination - mutation - replacement
int run(void){
    int generations = 0;

    initializePopulation();  

    // Iterate for a predefined maximum number of generations
    while(generations < MAX_GENERATIONS){
        evaluatePopulation();
        nextGen();
        //std::cout<<"\nGeneration "<<generations<<": Best solution: "<<getBestSolution()<<"; average fitness:"
            //<<getAverageSolution()<<std::endl;
        ++generations;

    }
    
    return generations;
}

int main(int argc, char *argv[]){
    int finalGeneration;
    time_t t, start, end;

    // Intialize random number generator 
    srand((unsigned) time(&t));

    time(& start);

    finalGeneration = run();

    time(& end);

    std::cout<<"\nThe best solution was found in generation "<<finalGeneration<<", with fitness: "
        <<getBestSolution()<<std::endl<<"Time: "<<difftime (end,start)<<" seconds"<<std::endl; 
}


