#include "AStar.h"
#include <raylib.h>
#include <raymath.h>
//for testing 
#define WIN_MAX 900
#define LCL_DIST 25
struct node_t{
	Vector2 location;
	TArray<int> edges;
	bool connected;
};
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
	const int node_num = 8192;
	SetRandomSeed(time(0));
	TArray<node_t> nodes = {};
	for(int i =0; i<node_num; i++){
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
	int end = GetRandomValue(0,nodes.Num());
	if(start == end){
		end = (start+1)%node_num;
	}
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
