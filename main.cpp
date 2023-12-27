#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <iostream>
#include <time.h>
//these macros are to make this algorythm compatable with unreal c++ using std::vector and standard std::vector operations should work fine
#define TArray std::vector
#define Add push_back
#define Num size
//for testing 
#define WIN_MAX 900
#define LCL_DIST 200
struct node_t{
	Vector2 location;
	TArray<int> edges;
	bool connected;
};
// actual pathfinding
//send this to A star as the external implementation of A*
struct AStarNode_t{
	//the indices of the edges in the array
	TArray<int> edges;
	//the distances between each edge and the ndoe
	TArray<double> distances;
	//the euclidian distance between the node and the en
	double euc_distance;
};
struct AStarInternalNode_t{
	int idx;
	TArray<int> edges;
	TArray<double> edge_distances;
	//heuristic function
	double h;
	//cost from the start of the path to the node
	double g;
	double f(){
		return h+g;
	}
	bool operator ==(AStarInternalNode_t other){
		return other.idx == this->idx;
	}
};
int set_min(TArray<AStarInternalNode_t> set){
	const double inf = 10e15;
	double minval = inf*2;
	double minidx = -1;
	//printf("set size: %zu\n", set.Num());
	for(int i =0; i<set.Num(); i++){
		if(set[i].f()<minval){
			minval = set[i].f();
			minidx =i;
		}
	}
	if(minidx == -1){
		exit(-1);
	}
	return minidx;
}
TArray<int> reconstruct_path(TArray<int> camefrom, int current){
	TArray<int> out = {};
	while(camefrom[current] != -1){
		out.Add(current);
		current = camefrom[current];

	}
	return out;
}
TArray<int> SetRemove(TArray<int> set, int index){
	TArray<int> out = {};
	for(int i =0; i<set.Num(); i++){
		out.Add(set[i]);
	}
	out.erase(out.begin()+(index));
	return out;
}
bool SetContains(TArray<int> set, int value){
	if(set.Num() == 0){
		return false;
	}
	for(int i =0; i<set.Num(); i++){
		if(set[i] == value){
			//printf("%d = %d\n", set[i].idx, value.idx);
			return true;
		}
	}
	//printf("set doesn't contain: %d\n",value.idx);
	return false;
}

//https://en.wikipedia.org/wiki/A*_search_algorithm
TArray<int> AStar(TArray<AStarNode_t> in_nodes, int start, int end){
	TArray<AStarInternalNode_t> nodes = {};
	//set up
	const double inf = 10e15;
	for(int i =0; i<in_nodes.Num(); i++){
		AStarInternalNode_t node;
		node.idx = i;
		node.edges = in_nodes[i].edges;
		node.edge_distances = in_nodes[i].distances;
		node.g = inf;
		node.h = in_nodes[i].euc_distance;
		nodes.Add(node);
	}
	nodes[start].g = 0;
	//end set up 
	TArray<int> openset = {start};
	TArray<int> camefrom = {};
	//camefrom set up;
	for(int i =0; i<nodes.Num(); i++){
		camefrom.Add(-1);
	}
	//end camefrom set up
	TArray<double> fscore = {};
	//fscore set up
	for(int i = 0; i<nodes.Num(); i++){
		fscore.Add(inf);
	}
	fscore[start] = nodes[start].h;
	//end fscore set up
	while(openset.Num()>0){
		TArray<AStarInternalNode_t> tmp = {};
		for(int i =0; i<openset.Num(); i++){
			printf("%d\n",nodes[openset[i]].idx);
			tmp.Add(nodes[openset[i]]);
		}
		int crtmp = set_min(tmp);
		int current = openset[crtmp];
		printf("start: %d, current:%d end: %d\n",start, current, end);
		if(current == end){
			return reconstruct_path(camefrom, current);
		}
		openset = SetRemove(openset, crtmp);
		for(int i =0; i<nodes[current].edges.Num(); i++){
			int neighbor = nodes[current].edges[i];
			double tmp_g_score = nodes[current].g+nodes[current].edge_distances[i];
			//printf("tmp g score: %f, g score: %f\n", tmp_g_score, nodes[neighbor].g);
			if(tmp_g_score<nodes[neighbor].g){
				camefrom[neighbor] = current;
				nodes[neighbor].g = tmp_g_score;
				if(!SetContains(openset, neighbor)){
					//printf("called\n");
					openset.Add(neighbor);
				}
			}
		}
	}
	//end fscore set up
	printf("returned empty set\n");
	exit(0);
	return {};
}
// end actual pathfinding 
bool TArrayContains(TArray<int> &vec, int value){
	bool contains = false;
	for(int i =0; i<vec.Num(); i++){
		if(vec[i] == value){
			contains = true;
		}
	}
	return contains;
}
int main(){
	SetRandomSeed(time(0));
	TArray<node_t> nodes = {};
	for(int i =0; i<32; i++){
		node_t tmp ={.location = {(float)GetRandomValue(0, WIN_MAX), (float)GetRandomValue(0, WIN_MAX)}, .connected = false, .edges = {}};
		nodes.Add(tmp);
	}
	nodes[0].connected = true;
	for(int i =0; i<nodes.Num(); i++){
		for(int j =i+1; j<nodes.Num(); j++){
			float dist = Vector2Distance(nodes[i].location, nodes[j].location);
			if(((dist<LCL_DIST) || ( (!nodes[i].connected || !nodes[j].connected) && dist<LCL_DIST*2))){
				nodes[i].edges.Add(j);
				nodes[j].edges.Add(i);
				nodes[j].connected = true;
			}
		}
	}
	SetTraceLogLevel(LOG_FATAL);
	InitWindow(900,900,"pathfinder");
	TArray<int> Active = {};
	int start = GetRandomValue(0,nodes.Num());
	int end = GetRandomValue(start,nodes.Num());
	Active.Add(start);
	Active.Add(end);
	TArray<AStarNode_t> astarnodes = {};
	for(int i =0; i<nodes.Num(); i++){
		AStarNode_t node;
		node.distances ={};
		for(int j =0; j<nodes[i].edges.Num(); j++){
			int k = nodes[i].edges[j];
			double dist = Vector2Distance(nodes[i].location,nodes[k].location);
			node.distances.Add(dist);
		}
		node.edges = nodes[i].edges;
		node.euc_distance = Vector2Distance(nodes[i].location, nodes[end].location);
		astarnodes.Add(node);
	}
	TArray<int> path = AStar(astarnodes, start, end);
	for(int i =0; i<path.Num(); i++){
		Active.Add(path[i]);
	}
	while(!WindowShouldClose()){
		BeginDrawing();
		ClearBackground(BLACK);
		for(int i = 0; i<nodes.Num(); i++){
			bool contains = TArrayContains(Active, i);
			if(contains){
				DrawCircleV(nodes[i].location, 6,GREEN);
			}
			else{
				DrawCircleV(nodes[i].location, 6,RED);
			}
			for(int j =0; j<nodes[i].edges.Num(); j++){
				bool contains2 = TArrayContains(Active, nodes[i].edges[j]);
				if(nodes[i].edges[j]>i){
					if(contains && contains2){
						DrawLineV(nodes[i].location, nodes[nodes[i].edges[j]].location, GREEN);
					}
					else{
						DrawLineV(nodes[i].location, nodes[nodes[i].edges[j]].location, RED);
					}
				}
			}
		}
		EndDrawing();
	}
	return 0;
}
