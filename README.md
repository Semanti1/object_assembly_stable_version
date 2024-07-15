In causal_furniture.cpp, uncomment one of lines 23-28 depending on which object we are running the experiment for. Same for lines 193-198 in isValid function. This step needs to be done BEFORE running any experiment. 


To compile:

g++ *.cpp  -lboost_program_options -g

To run (sample command):

 ./a.out --problem causal_furniture --furniture "wall" --number 4 --causal true --causalfilename Wall (Final)_planner_embeddings.txt --disabletree 0 --usetransforms 1 --runs 5 --mindoubles 14 --maxdoubles 14 --conf 0.9 --outputfile test_troubleshooting/wall_expert_vis_
Descriptions:

--furniture - furniture name

--number - number of parts in the furniture

--causal - true if using causal graph, false otherwise

--causalfilename the file path which contains user causal models. One entry per line. Files named like "Flash (Final)_planner_embeddings" contain user causal models.

--disabletree - 0 if using POMCP, 1 if using RolloutSearch

--runs - number of runs per # of simulations, reward will be the average across all runs

--mindoubles - minimum # of monte carlo simulations in the experiment

--maxdoubles - maximum # of simulations in the experiment.

--confidence - value between 0 and 1. It denotes the level of confidence that causal link implies physical link in assembly.A value of 0.5 denotes random assignment, i.e., no causal knowledge. Ideally value should be set to more than 0.5. 

--outputfile - Path to output file.


In experiment.cpp -> comment out lines 58 and 80 if you don't need to see the state. This will speed things up. In mcts.cpp, comment out lines 42 and 83 if you don't want to display belief state. This will also speed things up.