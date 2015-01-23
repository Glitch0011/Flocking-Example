#include <Windows.h>
#include <gl\GL.h>
#include "glut.h"
#include <time.h>
#include <iostream>

#define BIRD_COUNT 8000
#define DRAW_SIZE 3
#define HALF_DRAW_SIZE 1.5
#define SEPERATION 10
#define ALIGNMENT 40
#define ATTRACTION_STRENGTH 0.005

#define MAP_SIZE 20

#define BUILDING_SIZE 5
#define BUILDING_COUNT 30

#define OUTPUT_LIST_SIZE FALSE
#define UPDATE_BIRDS

#define SPEED_MAX 30
#define SPEED_MIN 10

//High = more random
#define HUMANITY 0.009

int xSize;
int ySize;
long last;

long last_clock;
int frame_rate;
int old_frame_rate;

unsigned char** map;

typedef struct
{
	float x;
	float y;
} vector2;

struct bird
{
	vector2 position;
	vector2 heading;
	int speed;
};
bird* birds;

struct bird_list
{
	bird* data;
	bird_list* next;
};
bird_list ** bird_grid;