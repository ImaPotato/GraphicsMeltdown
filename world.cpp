/*
 * world.cpp
 *
 *  Created on: 21/09/2014
 *      Author: riddelbenj
 */

#include <ctime>
#include "world.h"
#include "array2d.h"
#include <stdio.h>
#include <iostream>
#include "define.h"
#include <vector>
#include <algorithm>

// const Particle /*NULL_PARTICLE = Particle(0,0,0,-500,0,0,0)*/;
// static int OCTREE_SIZE = 16;

//int activeModel = 0; // Which model is actively being drawn. Can be 1 or 2
Octree<Particle> model(OCTREE_SIZE, NULL_PARTICLE);
std::vector<Particle> particles;
std::vector<Particle> outsideParticles;


World::World() {
	printf("World Created\n");
 }

 void World::LoadModel(char* fileName){
	 (void)fileName;
	 
 //activeModel = 0;
	// TODO load up a file and fill octree
}

void World::LoadDefaultModel(){	
	//activeModel = 0;
// 	for (int z = 0; z < 4; z++){		
// 		for(int y = 0; y < 4; y++){
// 			for(int x = 0; x < 4; x++){
// 				//model(x,y,z) = Particle(x-OCTREE_SIZE/2,y-OCTREE_SIZE/2,z-OCTREE_SIZE/2,DEF_TEMPERATURE,0,0,0);
// 				model(x,y,z) = Particle(x, y, z,DEF_TEMPERATURE,0,0,0);
// 				particles.insert(particles.end(), model(x, y, z));
// 			}
// 		}
// 	}
	
	for(int i = 0; i < OCTREE_SIZE; i++){
		model(i, i, i) = Particle(i, i, i,DEF_TEMPERATURE,0,-1,0);
		particles.insert(particles.end(), model.at(i, i, i));
	}
	printf("Model Loaded\n");
}

// UPDATING MODEL
void World::UpdateModel(){
	printf("\n\n\n\nBegin UpdateModel\n");
	
	Octree<Particle> buffer(OCTREE_SIZE, NULL_PARTICLE);
	
	for(std::vector<Particle>::iterator it = particles.begin(); it != particles.end(); it++){		
		if((*it).GetTemperature() >= MIN_TEMPERATURE){ // I.e not a null particle
			(*it).calculateNewPosition();
			
			printf("Point @ x: %f, y: %f, z: %f\n", (*it).GetX(), (*it).GetY(), (*it).GetZ());
			
			buffer((*it).GetX(), (*it).GetY(), (*it).GetZ()) = (*it);
		}
	}
	
// 	for (int z = 0; z < model.size(); z++){		
// 		for(int y = 0; y < model.size(); y++){
// 			for(int x = 0; x < model.size(); x++){
// 				Particle p = model.at(x,y,z);
// 				if(p.GetTemperature() >= MIN_TEMPERATURE){ // I.e not a null particle
// 					p.calculateNewPosition();
// 					
// 					//TODO calculate temperatures
// 					
// 					buffer(p.GetX(), p.GetY(), p.GetZ()) = p;
// 				}
// 			}
// 		}		
// 	}
	
	outsideParticles.clear();
	
	for(std::vector<Particle>::iterator it = particles.begin(); it != particles.end(); it++){
		Particle p = *it;
		
		if(p.GetTemperature() >= MIN_TEMPERATURE){ // I.e not a null particle
			std::vector<Particle> neighbours;
			bool isOnOutside = false;
			for(int z = std::max(0, int((*it).GetZ() - NEIGHBOUR_DISTANCE)); z <= std::min(OCTREE_SIZE-1, int((*it).GetZ() + NEIGHBOUR_DISTANCE)); z++){
				for(int y = std::max(0, int((*it).GetY() - NEIGHBOUR_DISTANCE)); y <= std::min(OCTREE_SIZE-1, int((*it).GetY() + NEIGHBOUR_DISTANCE)); y++){
					for(int x = std::max(0, int((*it).GetX() - NEIGHBOUR_DISTANCE)); x <= std::min(OCTREE_SIZE-1, int((*it).GetX() + NEIGHBOUR_DISTANCE)); x++){
						if(!((*it).GetX() == x && (*it).GetY() == y && (*it).GetZ() == z)){
							if(p.GetTemperature() >= float(MIN_TEMPERATURE)){
								neighbours.insert(neighbours.end(), buffer.at(x, y, z));
							}
							else{
								isOnOutside = true;
							}
						}
					}
				}
			}
			(*it).calculateForces(neighbours);
			if(isOnOutside){
				outsideParticles.insert(outsideParticles.end(), *it);
			}
		}
	}
		
	
	model = buffer;
	printf("End UpdateModel\n\n\n");
}

// DRAWING TO SCREEN
void World::Draw(){
	UpdateModel();
	//printf("Drawing Model%d\n",activeModel);
	DrawModel(); 
}

// NOTE: This version does not use z sclices so is not optimized but works
void World::DrawModel(){
// 	for(int z = 0; z < OCTREE_SIZE; z++){
// 		for(int y = 0; y < OCTREE_SIZE; y++){
// 			for(int x = 0; x < OCTREE_SIZE; x++){
// 				
// // 				printf("Drawing point @ x: %d, y: %d, z: %d\n", x, y, z);
// 				Particle p = model.at(x, y, z);
// // 				printf("Drawing point with values x: %f, y: %f, z: %f\n", p.GetX(), p.GetY(), p.GetZ());
// 				if(p.GetTemperature() >= MIN_TEMPERATURE){
// 					p.Draw();
// 				}
// 			}
// 		}
// 	}
	
	for(std::vector<Particle>::iterator it = particles.begin(); it != particles.end(); it++){
		Particle p = *it;
		if(p.GetTemperature() >= MIN_TEMPERATURE){ // I.e not a null particle
			//printf("Drawing point x: %f, y: %f, z:%f\n", p.GetX(), p.GetY(), p.GetZ());
			p.Draw();
		}
		else{
			//printf("Not drawing point x: %f, y: %f, z:%f\n", p.GetX(), p.GetY(), p.GetZ());
		}
	}
}

// NOTE: This is theoretically same as above code converted to use z sclices... but doesn't work
/*
void World::DrawModel(Octree<Particle> model){
	for (int z = 0; z < model.size(); z++){
		Array2D<Particle> tmp = model.zSlice(z);
			for(int y = 0; y < model.size(); y++){
				for(int x = 0; x < model.size(); x++){
					Particle p = (tmp(x,y));
					if(p.GetTemperature() >= MIN_TEMPERATURE){ // I.e not a null particle
						p.Draw();
				}					
			}
		}		
	}
}
*/