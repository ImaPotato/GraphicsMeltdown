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

const Particle NULL_PARTICLE = Particle(0,0,0,-500,0,0,0);
int octreeSize = 16;

//int activeModel = 0; // Which model is actively being drawn. Can be 1 or 2
Octree<Particle> model(octreeSize, NULL_PARTICLE);

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
				//model(x,y,z) = Particle(x-octreeSize/2,y-octreeSize/2,z-octreeSize/2,DEF_TEMPERATURE,0,0,0);
				model(x,y,z) = Particle(x, y, z,DEF_TEMPERATURE,0,0,0);
			}
		}
	}
	
	model(octreeSize-1, octreeSize-1, octreeSize-1) = Particle(octreeSize-1, octreeSize-1, octreeSize-1,DEF_TEMPERATURE,0,0,0);
	
	printf("Model Loaded\n");
}

// UPDATING MODEL
void World::UpdateModel(){
	Octree<Particle> nextFrame(octreeSize, NULL_PARTICLE);	
	
	CalculatePositions(&nextFrame);		
	CalculateTemperatures(&nextFrame);
	CalculateVelocities(&nextFrame);		
	
	model = nextFrame;
}

void World::CalculatePositions(Octree<Particle>* nextFrame){
	// TODO
	for (int z = 0; z < model.size(); z++){		
		for(int y = 0; y < model.size(); y++){
			for(int x = 0; x < model.size(); x++){
				Particle p = model.at(x,y,z);
				if(p.GetTemperature() >= MIN_TEMPERATURE){ // I.e not a null particle
					p.calculateNewPosition();

					(*nextFrame)(p.GetX(), p.GetY(), p.GetZ()) = p;
				}
			}
		}		
	}
}

void World::CalculateTemperatures(Octree<Particle>* nextFrame){
	
	(void)nextFrame;
	
	
	//do things
	// TODO
}

void World::CalculateVelocities(Octree<Particle>* nextFrame){
	(void)nextFrame;
	//calculate things	
	// TODO
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