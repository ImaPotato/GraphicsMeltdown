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

int activeModel = 0; // Which model is actively being drawn. Can be 1 or 2
Octree<Particle> model0(octreeSize, NULL_PARTICLE);
Octree<Particle> model1(octreeSize, NULL_PARTICLE);

World::World() {
	printf("World Created\n");
 }

void World::LoadModel(char* fileName){
	activeModel = 0;
	// TODO
}



void World::LoadDefaultModel(){
	activeModel = 0;
	for (int z = 0; z < octreeSize; z++){		
		for(int y = 0; y < octreeSize; y++){
			for(int x = 0; x < octreeSize; x++){
				model0(x,y,z) = Particle(x-octreeSize/2,y-octreeSize/2,z-octreeSize/2,DEF_TEMPERATURE,0,0,0);
				model1(x,y,z) = Particle(x-octreeSize/2,y-octreeSize/2,z-octreeSize/2,DEF_TEMPERATURE,0,0,0);
			}
		}
	}
	printf("Model Loaded\n");
}

// UPDATING MODEL
void World::UpdateModel(){
	activeModel = !activeModel;

	CalculatePositions();
	CalculateTemperatures();
	CalculateVelocities();
}

void World::CalculateTemperatures(){
	//do things
	// TODO
}

void World::CalculateVelocities(){
	//calculate things	
	// TODO
}

void World::CalculatePositions(){
	//move things
	// TODO
}

// DRAWING TO SCREEN
void World::Draw(){
	UpdateModel();
	printf("Drawing Model%d\n",activeModel);
	if(activeModel==1)
		DrawModel(model1);
	else
		DrawModel(model0); 
}

// NOTE: This version does not use z sclices so is not optimized but works
void World::DrawModel(Octree<Particle> model){
	for (int z = 0; z < model.size(); z++){		
		for(int y = 0; y < model.size(); y++){
			for(int x = 0; x < model.size(); x++){
				Particle p = model.at(x,y,z);
				if(p.GetTemperature() >= MIN_TEMPERATURE){ // I.e not a null particle
					p.Draw();
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