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

		Particle p = (*it);
		printf("Point @ test before change temperature: %d\n", (*it).GetTemperature());
		(*it).SetTemperature(3);
		printf("Point @ test after change temperature: %d\n", (*it).GetTemperature());
		Particle p1 = *it;
		printf("Point @ test dereference after change temperature: %d\n", (*it).GetTemperature());
		
		if((*it).GetTemperature() >= MIN_TEMPERATURE){ // I.e not a null particle
			printf("Point @ before calculating new position x: %f, y: %f, z: %f, vx: %f, vy: %f, vz: %f\n", (*it).GetX(), (*it).GetY(), (*it).GetZ(), (*it).GetVX(), (*it).GetVY(), (*it).GetVZ());
			(*it).calculateNewPosition();
			printf("Point @ after calculating new position x: %f, y: %f, z: %f, vx: %f, vy: %f, vz: %f\n", (*it).GetX(), (*it).GetY(), (*it).GetZ(), (*it).GetVX(), (*it).GetVY(), (*it).GetVZ());
			
			buffer(p.GetX(), p.GetY(), p.GetZ()) = p;
		}
	}
		
	for(std::vector<Particle>::iterator it = particles.begin(); it != particles.end(); it++){

		Particle p = *it;
		printf("Point @ second loop: %d\n", p.GetTemperature());
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
	
	for(std::vector<Particle>::iterator it = particles.begin(); it != particles.end(); it++){
		Particle p = *it;
		
		if(p.GetTemperature() >= MIN_TEMPERATURE){ // I.e not a null particle
			
			int x = p.GetX(), y = p.GetY(), z = p.GetZ();
			
			std::vector<Particle> neighbours;
			for(int i = std::max(0, x-NEIGHBOUR_DISTANCE); i <= std::min(OCTREE_SIZE-1, x+NEIGHBOUR_DISTANCE); i++){
				for(int i = std::max(0, x-NEIGHBOUR_DISTANCE); i <= std::min(OCTREE_SIZE-1, x+NEIGHBOUR_DISTANCE); i++){
					for(int i = std::max(0, x-NEIGHBOUR_DISTANCE); i <= std::min(OCTREE_SIZE-1, x+NEIGHBOUR_DISTANCE); i++){
						if(!(i == x && j == y && k == z) && buffer.at(i, j, k).GetTemperature()>=MIN_TEMPERATURE){
							neighbours[i][j][k] = buffer.at(x-1+i, y-1+j, z-1+z);
						}
					}
				}
			}
			printf("Point @ before calculating forces x: %f, y: %f, z: %f, vx: %f, vy: %f, vz: %f\n", p.GetX(), p.GetY(), p.GetZ(), p.GetVX(), p.GetVY(), p.GetVZ());
			(*it).calculateForces(neighbours, 3);
			printf("Point @ after calculating forces x: %f, y: %f, z: %f, vx: %f, vy: %f, vz: %f\n", p.GetX(), p.GetY(), p.GetZ(), p.GetVX(), p.GetVY(), p.GetVZ());
			
			
			//TODO calculate forces and velocities
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
	for(std::vector<Particle>::iterator it = particles.begin(); it != particles.end(); it++){
		Particle p = *it;
		
		printf("Point @ drawing x: %f, y: %f, z: %f, vx: %f, vy: %f, vz: %f\n", p.GetX(), p.GetY(), p.GetZ(), p.GetVX(), p.GetVY(), p.GetVZ());
		
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