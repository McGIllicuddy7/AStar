#include "AStar.h"
#include <chrono>
#ifdef __linux__
#include <Raylib/raylib.h>
#include <Raylib/raymath.h>
#define WIN_MAX 1400
#else
#include <raylib.h>
#include <raymath.h>
#define WIN_MAX 1000
#endif

//for testing 
#define DRAW_INACTIVE
#undef DRAW_INACTIVE
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
	int num_count = 10;
	long long average_time_a_star = 0;
	for(int i =0; i<num_count; i++){
		start = GetRandomValue(0,nodes.Num());
		end = GetRandomValue(0,nodes.Num());
		if(start == end){
			end = (start+1)%node_num;
		}
		auto t1 =std::chrono::high_resolution_clock::now();
		path = AStar(astarnodes, start, end);
		auto t2 = std::chrono::high_resolution_clock::now()-t1;
		long long elapsed = std::chrono::duration_cast<std::chrono::microseconds>(t2).count();
		printf("case %d took %f seconds for %zu nodes\n",i,(double)elapsed/10e5, path.Num());
		average_time_a_star += elapsed;
	}
	printf("%f average time\n", ((double)average_time_a_star/10e5)/num_count);
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
#ifdef DRAW_INACTIVE
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
#endif
		//draw active
		for(int i = 0; i<nodes.Num(); i++){
			bool contains = TArrayContains(Active, i);
			if(i == start){
				DrawCircleV(nodes[i].location, 3,BLUE);
			}
			else if(i == end){
				DrawCircleV(nodes[i].location, 3,PURPLE);
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
