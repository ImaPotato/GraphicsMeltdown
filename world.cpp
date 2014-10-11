/*
 * world.cpp
 *
 *  Created on: 21/09/2014
 *      Author: riddelbenj
 */

#include "world.h"
#include "array2d.h"
#include <stdio.h>
#include <iostream>
#include "define.h"

// const Particle /*NULL_PARTICLE = Particle(0,0,0,-500,0,0,0)*/;
// static int OCTREE_SIZE = 16;

//int activeModel = 0; // Which model is actively being drawn. Can be 1 or 2
Octree<Particle> model(OCTREE_SIZE, NULL_PARTICLE);

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
	for (int z = 0; z < 4; z++){		
		for(int y = 0; y < 4; y++){
			for(int x = 0; x < 4; x++){
				//model(x,y,z) = Particle(x-OCTREE_SIZE/2,y-OCTREE_SIZE/2,z-OCTREE_SIZE/2,DEF_TEMPERATURE,0,0,0);
				model(x,y,z) = Particle(x, y, z,DEF_TEMPERATURE,0,0,0);
			}
		}
	}
	
	model(OCTREE_SIZE-1, OCTREE_SIZE-1, OCTREE_SIZE-1) = Particle(OCTREE_SIZE-1, OCTREE_SIZE-1, OCTREE_SIZE-1,DEF_TEMPERATURE,0,-1,0);
	
	printf("Model Loaded\n");
}

// UPDATING MODEL
void World::UpdateModel(){
	Octree<Particle> buffer(OCTREE_SIZE, NULL_PARTICLE);	
	
	for (int z = 0; z < model.size(); z++){		
		for(int y = 0; y < model.size(); y++){
			for(int x = 0; x < model.size(); x++){
				Particle p = model.at(x,y,z);
				if(p.GetTemperature() >= MIN_TEMPERATURE){ // I.e not a null particle
					p.calculateNewPosition();
					buffer(p.GetX(), p.GetY(), p.GetZ()) = p;
				}
			}
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
	
	for (int z = 0; z < model.size(); z++){		
		for(int y = 0; y < model.size(); y++){
			for(int x = 0; x < model.size(); x++){
				Particle p = model.at(x,y,z);
				if(p.GetTemperature() >= MIN_TEMPERATURE){ // I.e not a null particle
					
					Particle ***neighbours = new Particle**[3];
					for(int i = 0; i < 3; i++){
						neighbours[i] = new Particle*[3];
						for(int j = 0; j < 3; j++){
							neighbours[i][j] = new Particle[3];
							for(int k = 0; k < 3; k++){
								if(!(x-1+i <0 || y-1+j < 0 || z-1+z < 0
									|| x-1+i >= OCTREE_SIZE || y-1+j >= OCTREE_SIZE || z-1+z >= OCTREE_SIZE 
									|| (x-1+i == x || y-1+j == j || z-1+z == z))){
									neighbours[i][j][k] = buffer.at(x-1+i, y-1+j, z-1+z);
								}else
								{
									neighbours[i][j][k] = NULL_PARTICLE;
								}
							}
						}
					}
					
					p.calculateForces(neighbours, 3);
					
					
					//TODO calculate forces and velocities
					
// 					buffer(p.GetX(), p.GetY(), p.GetZ()) = p;
				}
			}
		}		
	}
	
	model = buffer;
}

// DRAWING TO SCREEN
void World::Draw(){
	UpdateModel();
	//printf("Drawing Model%d\n",activeModel);
	DrawModel(); 
}

// NOTE: This version does not use z sclices so is not optimized but works
void World::DrawModel(){
	for (int z = 0; z < model.size(); z++){		
		for(int y = 0; y < model.size(); y++){
			for(int x = 0; x < model.size(); x++){
				Particle p = model.at(x,y,z);
				
				if(p.GetTemperature() >= MIN_TEMPERATURE){ // I.e not a null particle
					//printf("Drawing point x: %f, y: %f, z:%f\n", p.GetX(), p.GetY(), p.GetZ());
					p.Draw();
				}
				else{
					//printf("Not drawing point x: %f, y: %f, z:%f\n", p.GetX(), p.GetY(), p.GetZ());
				}
			}
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