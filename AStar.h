#pragma once
#include <vector>
#include <iostream>
#include <time.h>
//these macros are to make this algorythm compatable with unreal c++ using std::vector and standard std::vector operations should work fine
#define TArray std::vector
#define Add push_back
#define Num size
//send this to A star as the external implementation of A*
struct AStarNode_t{
	//the indices of the edges in the array
	TArray<int> edges;
	//the distances between each edge and the ndoe
	TArray<double> distances;
	//the euclidian distance between the node and the en
	double euc_distance;
};
//implemtation of A* using sets, create an array of nodes and it will return an array of indices that connect the two points
//it will return an empty set if there is not viable path s
//source is https://en.wikipedia.org/wiki/A*_search_algorithm
TArray<int> AStar(TArray<AStarNode_t> in_nodes, int start, int end);