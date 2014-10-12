/*
 * particle.cpp
 *
 *  Created on: 21/09/2014
 *      Author: not ben
 */

#include "particle.h"

Particle::Particle() {
	Particle::x = 0.0;
	Particle::y = 0.0;
	Particle::z = 0.0;

	Particle::temperature = DEF_TEMPERATURE;
	
	Particle::vx = 0.0;
	Particle::vy = 0.0;
	Particle::vz = 0.0;
}

Particle::Particle(float x, float y, float z, float temperature, float vx, float vy, float vz){
	Particle::x = x;
	Particle::y = y;
	Particle::z = z;

	Particle::temperature = temperature;

	Particle::vx = vx;
	Particle::vy = vy;
	Particle::vz = vz;
}

void Particle::Draw(){
	glPushMatrix();
	glTranslatef(x,y,z);

	gluSphere(gluNewQuadric(), RADIUS, SPHERE_SLICES, SPHERE_STACKS ); //TODO Quadric??

	glPopMatrix();
}

// Setters
void Particle::SetTemperature(float t){
	if(t > MIN_TEMPERATURE)
		temperature = t;
	else
		temperature = MIN_TEMPERATURE;
}

// Getters
float Particle::GetTemperature(){
	return temperature;
}

float Particle::GetX(){
	return x;
}

float Particle::GetY(){
	return y;
}

float Particle::GetZ(){
	return z;
}

float Particle::GetVX(){
	return vx;
}

float Particle::GetVY(){
	return vy;
}

float Particle::GetVZ(){
	return vz;
}

void Particle::calculateNewPosition(){
	x+=vx;
	y+=vy;
	z+=vz;
	
	if(y<0){
		y=0;
		vy *= -0.99;
	}else if(y >= OCTREE_SIZE){
		y= OCTREE_SIZE - 1;
		vy *= -0.99;
	}
	if(x<0){
		x=0;
		vx *= -0.99;
	}else if(x >= OCTREE_SIZE){
		x= OCTREE_SIZE - 1;
		vx *= -0.99;
	}
	if(z<0){
		z=0;
		vz *= -0.99;
	}else if(z >= OCTREE_SIZE){
		z= OCTREE_SIZE - 1;
		vz *= -0.99;
	}
}

void Particle::calculateForces(std::vector<Particle> neighbours, int count){
	float fx = 0, fy = -0.1, fz = 0;
	
	for(std::vector<Particle>::iterator it = neighbours.begin(); it != neighbours.end(); it++){
		fx += x - (*it).GetX();
		fy += y - (*it).GetY();
		fz += z - (*it).GetZ();
	}
		
	vx += fx;
	vy += fy;
	vz += fz;
}