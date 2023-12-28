#include "AStar.h"
#ifdef __linux__
#include <Raylib/raylib.h>
#include <Raylib/raymath.h>
#define WIN_MAX 1400
#else
#include <raylib.h>
#include <raymath.h>
#define WIN_MAX 100
#endif

//for testing 

#define LCL_DIST 50
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
	const int node_num = 2048;
	SetRandomSeed(time(0));
	TArray<node_t> nodes = {};
	for(int i =0; i<node_num; i++){
		node_t tmp ={.location = {(float)GetRandomValue(0, WIN_MAX), (float)GetRandomValue(0, WIN_MAX)},.edges = {}, .connected = false};
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
	int start = GetRandomValue(0,nodes.Num());
	int end = GetRandomValue(0,nodes.Num());
	if(start == end){
		end = (start+1)%node_num;
	}
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
	/*
	for(int i =0; i<path.Num(); i++){
		Active.Add(path[i]);
	}*/
	path.Add(end);
#ifdef profile
	for(int i =0; i<500; i++){
		start = GetRandomValue(0,nodes.Num());
		end = GetRandomValue(0,nodes.Num());
		if(start == end){
			end = (start+1)%node_num;
		}
		path = AStar(astarnodes, start, end);
	}
	return 0;
#endif
	int idx = 0;
	double tval =0;
	Color bg = {62,0,0,255};
	SetTraceLogLevel(LOG_FATAL);
	SetTargetFPS(60);
	InitWindow(WIN_MAX,WIN_MAX,"pathfinder");
	TArray<int> Active = {};
	while(!WindowShouldClose()){
		tval+= GetFrameTime();
		if(idx<path.Num() && tval>0){
			Active.Add(path[idx]);
			idx++;
			tval = 0;
		}
		else if(idx ==path.Num()){
restart:
			tval = 0;
			start = GetRandomValue(0,nodes.Num());
			end = GetRandomValue(0,nodes.Num());
			if(start == end){
				end = (start+1)%node_num;
			}
			Active = {};
			path = AStar(astarnodes,start,end);
			if(path.Num()<2){
				goto restart;
			}
			path.Add(end);
			idx = 0;
		}
		BeginDrawing();
		ClearBackground(BLACK);
		//draw non active
		for(int i = 0; i<nodes.Num(); i++){
			bool contains = TArrayContains(Active, i);
			if(!contains && !(i == start) && !(i == end)){
				DrawCircleV(nodes[i].location, 3,bg);
			}
			for(int j =0; j<nodes[i].edges.Num(); j++){
				bool contains2 = TArrayContains(Active, nodes[i].edges[j]);
				if(nodes[i].edges[j]>i){
					if (!contains && !contains2){
						DrawLineV(nodes[i].location, nodes[nodes[i].edges[j]].location, bg);
					}
				}
			}
		}
		//draw active
		for(int i = 0; i<nodes.Num(); i++){
			bool contains = TArrayContains(Active, i);
			if(i == start || i == end){
				DrawCircleV(nodes[i].location, 3,BLUE);
			}
			else if(contains){
				DrawCircleV(nodes[i].location, 3,GREEN);
			}
			for(int j =0; j<nodes[i].edges.Num(); j++){
				bool contains2 = TArrayContains(Active, nodes[i].edges[j]);
				if(nodes[i].edges[j]>i){
					if(contains && contains2){
						DrawLineV(nodes[i].location, nodes[nodes[i].edges[j]].location, GREEN);
					}
				}
			}
		}
		DrawFPS(WIN_MAX-100,100);
		EndDrawing();
	}
	return 0;
}
