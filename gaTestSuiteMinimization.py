import time
import random

from tqdm import tqdm

N_INDIVIDUALS=100
MUTATION_RATE=0.01
CROSSOVER_PROBABILITY=0.9
INPUT_FILE="./input/matrix_94_3647_1.txt"
MAX_GENERATIONS=100

# Global Variables
currentGeneration = [] #list of list
generationFitness = []
totalOfFitnesses = 0
numberEvaluations = 0

reqMatrix = [] #list of list
numberOfTests = 0
numberOfReqs = 0

#functions

def run():

    initializePopulation()  

    for generations in tqdm(range(MAX_GENERATIONS)):
        evaluatePopulation()
        nextGen()
    
    return generations

def initializePopulation():
    error=False
    global numberOfReqs
    numberOfReqs = int(INPUT_FILE.split("_")[1])+1
    global numberOfTests
    numberOfTests = int(INPUT_FILE.split("_")[2])

    with open(INPUT_FILE, "r") as file:
        Lines = file.readlines()
    global reqMatrix
    reqMatrix =[]
    for line in Lines:
        reqMatrix.append(line)
    global currentGeneration
    for index in range(N_INDIVIDUALS):
        currentGeneration.append([])
        indexing = [False for i in range(numberOfTests)]

        for gene in range(numberOfTests):
            aux = random.randint(0,numberOfTests-1)
            while indexing[aux]:
                aux = random.randint(0,numberOfTests-1)
            indexing[aux] = True
            currentGeneration[index].append(aux)

def evaluatePopulation():
    global generationFitness
    for i in range(N_INDIVIDUALS):
        generationFitness.append(eval(currentGeneration[i]))
    
def nextGen():
    global currentGeneration
    global generationFitness
    for individual in range(N_INDIVIDUALS) :

        #Selection
        parent1 = selection()
        parent2 = selection()
        while parent1 == parent2:
            parent2 = selection()

        #Crossover
        if random.random() <= CROSSOVER_PROBABILITY:
            offspring = crossover(currentGeneration[parent1], currentGeneration[parent2]);

            #Mutation
            mutate(offspring)

            #Evaluation
            fitnessOffspring = eval(offspring)

            #Recombination 
            if fitnessOffspring <= generationFitness[individual]:
                #delete currentGeneration[individual]
                currentGeneration[individual] = offspring
                generationFitness[individual] = fitnessOffspring

            else:
                pass
                #delete [] offspring;

def selection():
    global generationFitness
    i1 = random.randint(0,N_INDIVIDUALS-1) 
    i2 = random.randint(0,N_INDIVIDUALS-1) 
    while i1 == i2:
        i2 = random.randint(0,N_INDIVIDUALS-1)  

    if generationFitness[i1] < generationFitness[i2]:
        return i1
    else:
        return i2

def getBestSolution():
    global numberOfTests
    global generationFitness
    minFitness = numberOfTests

    for i in range(N_INDIVIDUALS):
        if generationFitness[i] < minFitness:
            minFitness = generationFitness[i]

    return minFitness
def getAverageSolution():
    avgFitness = 0

    for i in range(N_INDIVIDUALS):
        global generationFitness
        avgFitness += generationFitness[i]
        avgFitness /= 1.0*N_INDIVIDUALS

    return avgFitness

def eval(individual):
    fitness = 0
    global numberOfReqs
    satisfiedRequirements = [False for i in range(numberOfReqs)]
    fitness = 0
    for fit in range(numberOfTests):
        fitness = fit
        for req in range(numberOfReqs):
            if reqMatrix[individual[fit]][req] == "1":
                satisfiedRequirements[req] = True
        
        finish = True
        for i in range(numberOfReqs):
            if not satisfiedRequirements[i]:
                finish = False
                break

        if finish:
            break

    return fitness + 1;
def mutate(individual):
    for i in range(numberOfTests):
        if random.random() <= MUTATION_RATE:
            point1 = random.randint(0,numberOfTests-1)
            point2 = random.randint(0,numberOfTests-1)
            while point1 == point2:
                point2 = random.randint(0,numberOfTests-1)

            aux = individual[point1];
            individual[point1] = individual[point2]
            individual[point2] = aux

def crossover(parent1, parent2):
    if len(parent1) != len(set(parent1)):
        print(parent1)
        raise ValueError("parent1 error")
    if len(parent2) != len(set(parent2)):
        print(parent2)
        raise ValueError("parent2 error")
    point1 = random.randint(0, numberOfTests-1)
    point2 = random.randint(point1, numberOfTests-1)

    offspring1=list(parent1)
    add_list=[]
    remove_list=[]

    for i in range(point1, point2+1):
        ind1=parent1.index(parent2[i])
        ind2=parent2.index(parent1[i])
        if (ind1 < point1 or ind1 > point2):
            remove_list.append(parent1[ind1])
        if (ind2 < point1 or ind2 > point2):
            add_list.append(parent2[ind2])
        offspring1[i] = parent2[i]

    for i in range(len(add_list)):
        offspring1[parent1.index(remove_list[i])]=add_list[i]

    offspring2=list(parent2)
    add_list=[]
    remove_list=[]

    for i in range(point1, point2+1):
        ind1=parent2.index(parent1[i])
        ind2=parent1.index(parent2[i])
        if (ind1 < point1 or ind1 > point2):
            remove_list.append(parent2[ind1])
        if (ind2 < point1 or ind2 > point2):
            add_list.append(parent1[ind2])
        offspring2[i] = parent1[i]

    for i in range(len(add_list)):
        offspring2[parent1.index(remove_list[i])]=add_list[i]

    if len(offspring1) != len(set(offspring1)):
        print(offspring1)
        raise ValueError("Offspring1 error")
    if len(offspring2) != len(set(offspring2)):
        print(offspring2)
        raise ValueError("Offspring2 error")

    offFitness1 = eval(offspring1)
    offFitness2 = eval(offspring2)

    if offFitness1 < offFitness2:
        return offspring1
    else:
        return offspring2


start = time.time()
finalGeneration = run()
end = time.time()
print("--- %s seconds ---" % (end - start))
print("\nThe best solution was found in generation "+str(finalGeneration)+", with fitness: "+str(getBestSolution()))
print("Time: "+str(end-start)+" seconds") 

#print(crossover([1,2,3,4,5,6,7,8,9,10],[1,1,1,1,1,1,1,1,1,1]))