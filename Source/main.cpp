#include <main.h>

int inline __fastcall minus(int a, int b)
{
	int total;
	_asm sub ecx, edx
	_asm mov total, eax
	return total;
	//Naked removes return
	//Fastcall put arguments in the ecx and edx
}

double inline __declspec (naked) __fastcall my_sqrt(double n)
{
	//Naked removes return
	//Fastcall put arguments in the ecx and edx
	_asm fld qword ptr[esp + 4]
	_asm fsqrt
	_asm ret 8
}

int inline GetGridPos(vector2* pos)
{
	int pos_x = pos->x / ALIGNMENT;
	int pos_y = pos->y / ALIGNMENT;
	return (pos_x * (xSize / ALIGNMENT)) + pos_y;
}

bird_list* AddToList(bird_list* list, bird* value)
{
	if (list == 0)
	{
		bird_list* b = new bird_list();
		b->data = value;
		b->next = 0;
		return b;
	}
	else
	{
		bird_list* start = list;
		while (list->next != 0)
		{
			list = list->next;
		}

		bird_list* b= new bird_list();
		b->data = value;
		b->next = 0;

		list->next = b;
		return start;
	}
}

vector2 inline vector2minus(vector2* a, vector2* b)
{
	vector2 result;
	result.x = b->x - a->x;
	result.y = b->y - a->y;
	return result;
}

#define vector2_normalize(a) {float f = my_sqrt(((*a).x * (*a).x) + ((*a).y * (*a).y)); (*a).x = (*a).x / f; (*a).y = (*a).y / f;}
#define add_vector(a, b) (*a).x += (*b).x; (*a).y += (*b).y;

static vector2 temp;

float inline squared_dist(vector2* a, vector2* b)
{
	temp = vector2minus( a, b);
	return my_sqrt((temp.x * temp.x) + (temp.y * temp.y));
}


float inline unsquared_dist(vector2* a, vector2* b)
{
	temp = vector2minus( a, b);
	return ((temp.x * temp.x) + (temp.y * temp.y));
}

void drawSquares()
{
	int i;
	#define tri_count 4
	vector2 v[tri_count * BIRD_COUNT];

	
	bird* tmp = birds;
	for (i = 0; i < BIRD_COUNT; i++)
	{
		v[(i * tri_count) + 0].x = tmp->position.x; v[(i * tri_count) + 0].y = tmp->position.y;
		v[(i * tri_count) + 1].x = tmp->position.x + DRAW_SIZE; v[(i * tri_count) + 1].y = tmp->position.y;
		v[(i * tri_count) + 2].x = tmp->position.x + DRAW_SIZE; v[(i * tri_count) + 2].y = tmp->position.y + DRAW_SIZE;
		v[(i * tri_count) + 3].x = tmp->position.x; v[(i * tri_count) + 3].y = tmp->position.y + DRAW_SIZE;
		tmp++;
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, &v);
	glColor3f(1.0, 1.0, 0);
	glDrawArrays(GL_QUADS, 0, tri_count * BIRD_COUNT);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void drawMap()
{
	int count = 0;
	for (int x = 0; x < xSize / MAP_SIZE; x++)
	{
		for (int y = 0; y < ySize / MAP_SIZE;y++)
		{
			if (map[x][y] == 1)
			{
				count++;
			}
		}
	}
	vector2* v = new vector2[tri_count * count];
	int i = 0;
	for (int x = 0; x < xSize / MAP_SIZE; x++)
	{
		for (int y = 0; y < ySize / MAP_SIZE;y++)
		{
			if (map[x][y] == 1)
			{
				v[(i*tri_count) + 0].x = (x * MAP_SIZE); v[(i*tri_count) + 0].y = (y * MAP_SIZE);
				v[(i*tri_count) + 1].x = (x * MAP_SIZE) + MAP_SIZE; v[(i*tri_count) + 1].y = (y * MAP_SIZE);
				v[(i*tri_count) + 2].x = (x * MAP_SIZE) + MAP_SIZE; v[(i*tri_count) + 2].y = (y * MAP_SIZE) + MAP_SIZE;
				v[(i*tri_count) + 3].x = (x * MAP_SIZE); v[(i*tri_count) + 3].y = (y * MAP_SIZE) + MAP_SIZE;
				i++;
			}
		}
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, v);
	glColor3f(0.3, 0.7, 0.5);
	glDrawArrays(GL_QUADS, 0, tri_count * count);
	glDisableClientState(GL_VERTEX_ARRAY);

	delete[] v;

}

void update()
{
	if (GetAsyncKeyState(VK_ESCAPE))
	{
		exit(0);
	}

	long true_time_passed = clock() - last_clock;
	if ((true_time_passed / CLOCKS_PER_SEC) > 1)
	{
		last_clock = clock();
		old_frame_rate = frame_rate;
		frame_rate = 0;
		std::cout << old_frame_rate << std::endl;
	}

	//Put in a wall check


#ifdef UPDATE_BIRDS
	long time_passed, now;

	now = clock();
	time_passed = now - last;
	frame_rate++;

	bird* tmp = birds;
	for (int i = 0; i < BIRD_COUNT; i++)
	{
		int before_move = GetGridPos(&tmp->position);

		for (int ii = 0; ii < 6; ii++)
		{
			bird_list* target;
			int target_no = -1;
			switch (ii)
			{
				case 0: target_no = before_move; break;
				case 1: target_no = before_move - 1; break;
				case 2: target_no = before_move + 1; break;
				case 3: target_no = before_move - ((xSize) / ALIGNMENT); break;
				case 4: target_no = before_move + ((xSize) / ALIGNMENT); break;
				case 5:	target_no = (before_move - 1) - ((xSize) / ALIGNMENT); break;
				case 6: target_no = (before_move + 1) - ((xSize) / ALIGNMENT); break;
				case 7: target_no = (before_move - 1) + ((xSize) / ALIGNMENT); break;
				case 9: target_no = (before_move + 1) + ((xSize) / ALIGNMENT); break;
			}

			if (target_no > 0 && target_no < (xSize / ALIGNMENT) * (ySize / ALIGNMENT))
				target = bird_grid[target_no];
			else
				target = 0;

			while (target != 0)
			{
				if (target->data != tmp)
				{
					vector2 temp_v;
					temp_v.x = target->data->position.x - tmp->position.x;
					temp_v.y = target->data->position.y - tmp->position.y;

					float dist = (temp_v.x * temp_v.x) + (temp_v.y * temp_v.y);

					if (dist < (ALIGNMENT * ALIGNMENT) && dist > (SEPERATION * SEPERATION)) //ALIGNMENT
					{
						vector2 theirs = target->data->heading;
						theirs.x *= ATTRACTION_STRENGTH; theirs.y *= ATTRACTION_STRENGTH;
						add_vector(&tmp->heading, &theirs);
					}
					if (dist < (SEPERATION * SEPERATION)) //Seaporation
					{
						//Work out vector between the two
						vector2 v = vector2minus( &target->data->position, &tmp->position);
						if (dist != 0)
						{
							v.x /= dist;
							v.y /= dist;
						}
						add_vector(&tmp->heading, &v);
					}
				}
				target = target->next;
			}
		}

		//Freewill
		float addX = (float)((rand() % SPEED_MAX + SPEED_MIN)) * HUMANITY;
		float addY = (float)((rand() % SPEED_MAX + SPEED_MIN)) * HUMANITY;
		if ((rand() % 2 + 0) == 1) addX = -addX;
		if ((rand() % 2 + 0) == 1) addY = -addY;
		tmp->heading.x += addX;
		tmp->heading.y += addY;
		vector2_normalize(&tmp->heading);

		//Move the bird
		vector2 future_pos;
		future_pos.x = (tmp->heading.x * tmp->speed) * (time_passed * 0.10);
		future_pos.y = (tmp->heading.y * tmp->speed) * (time_passed * 0.10);
		add_vector(&future_pos, &tmp->position);

		//Check for boundaries
		if (future_pos.x > (xSize - DRAW_SIZE))
		{
			tmp->position.x = xSize - DRAW_SIZE;
			tmp->heading.x = -tmp->heading.x;
		}
		else if (future_pos.x < 0)
		{
			tmp->position.x = 0;
			tmp->heading.x = -tmp->heading.x;
		}
		else if (future_pos.y > (ySize - DRAW_SIZE))
		{
			tmp->position.y = ySize - DRAW_SIZE;
			tmp->heading.y = -tmp->heading.y;
		}
		else if (future_pos.y < 0)
		{
			tmp->position.y = 0;
			tmp->heading.y = -tmp->heading.y;
		}
		else
		{
			int map_x = future_pos.x / MAP_SIZE;
			int map_y = future_pos.y / MAP_SIZE;
			if (map[map_x][map_y] == 1)
			{
				tmp->heading.x = -tmp->heading.x;
				tmp->heading.y = -tmp->heading.y;
			}
			else
			{
				tmp->position = future_pos;
			}
		}

		//1.Separation - avoid crowding neighbors (short range repulsion)
		//2.ALIGNMENT - steer towards average heading of neighbors
		//3.Cohesion - steer towards average position of neighbors (long range attraction)

		int after_move = GetGridPos(&tmp->position);

		if (before_move != after_move)
		{
			bird_list* grid_tmp = bird_grid[before_move];

			bird_list* previous = 0;
			while (grid_tmp->data != tmp)
			{
				previous = grid_tmp;
				grid_tmp = grid_tmp->next;
			}
			bird_list* next = grid_tmp->next;
			if (previous != 0)
				previous->next = next;
			else
				bird_grid[before_move] = next;

			delete grid_tmp;

			grid_tmp = bird_grid[after_move];
			if (grid_tmp != 0)
			{
				while (grid_tmp->next != 0)
				{
					grid_tmp = grid_tmp->next;
				}
				bird_list* l = new bird_list();
				l->data = tmp;
				l->next = 0;
				grid_tmp->next = l;
			}
			else
			{
				bird_list* l = new bird_list();
				l->data = tmp;
				l->next = 0;
				bird_grid[after_move] = l;
			}
		}

		tmp++;
	}
	last = now;
#endif

#if OUTPUT_LIST_SIZE
	//work out largets list
	int max = 0;
	for (int i=0;i<(xSize / ALIGNMENT) * (ySize / ALIGNMENT);i++)
	{
		bird_list * tmp = bird_grid[i];
		int count = 0;
		while (tmp != 0)
		{
			count++;
			tmp = tmp->next;
		}
		if (count > max) max = count;
	}
	printf("Largest list = %d\n", max);
#endif
}

void changeSize(int w, int h)
{
	xSize = w;
	ySize = h;
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, xSize, ySize, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
}

void display()
{
	update();
	glClearColor(0.2, 0.8, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	drawSquares();
	drawMap();

	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
}

void on_exit()
{
	delete[] birds;

	for (int i = 0;i < xSize/MAP_SIZE; i++)
	{
		delete[] map[i];
	}
	delete[] map;

	for (int i = 0;i < (xSize / ALIGNMENT) * (ySize / ALIGNMENT); i++)
	{
		delete bird_grid[i];
	}
	delete[] bird_grid;
}

#define random_number(min, max) rand() % (max) + min;

int main(int argc, char* argv[])
{
	int i;

	atexit(on_exit);

	xSize = 800;
	ySize = 800;

	glutInit(&argc, argv);
	glutInitWindowSize(xSize, ySize);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Efficient Flocking");

	glutReshapeFunc(changeSize);
	glutDisplayFunc(display);
	
	srand(clock());

	//Generate Map
	//-----------------------------------------------------------------------
	map = new unsigned char*[xSize / MAP_SIZE];
	for (int i = 0;i < xSize/MAP_SIZE; i++)
	{
		map[i] = new unsigned char[ySize / MAP_SIZE];
	}
	for (int i=0;i<BUILDING_COUNT;i++)
	{
		vector2 size;
		size.x = random_number(1,BUILDING_SIZE);
		size.y = random_number(1,BUILDING_SIZE);
		vector2 start_pos;
		start_pos.x = random_number(0, (int)((xSize / MAP_SIZE) - size.x));
		start_pos.y = random_number(0, (int)((ySize / MAP_SIZE) - size.y));
		for (int x = start_pos.x; x < start_pos.x + size.x; x++)
		{
			for (int y = start_pos.y; y < start_pos.y + size.y ; y++)
			{
				map[x][y] = 1;
			}
		}
	}
	//-----------------------------------------------------------------------
	//Generate Birds
	//-----------------------------------------------------------------------
	bird_grid = new bird_list*[(xSize / ALIGNMENT) * (ySize / ALIGNMENT)];
	printf("Creating %d lists\n", (xSize / ALIGNMENT) * (ySize / ALIGNMENT));

	for (int i = 0;i < (xSize / ALIGNMENT) * (ySize / ALIGNMENT); i++)
	{
		bird_grid[i] = 0;
	}

	birds = new bird[BIRD_COUNT];

	for (i = 0; i < BIRD_COUNT; i++)
	{
		do
		{
			birds[i].position.x = rand() % (xSize - 5) + 5;
			birds[i].position.y = rand() % (xSize - 5) + 5;
		}
		while (map[(int)birds[i].position.x/MAP_SIZE][(int)birds[i].position.y/MAP_SIZE] == 1);
		
		birds[i].heading.x = (rand() % SPEED_MAX + SPEED_MIN);
		birds[i].heading.y = (rand() % SPEED_MAX + SPEED_MIN);
		birds[i].speed = 1;

		if ((rand() % 2 + 0) == 1) birds[i].heading.x = -birds[i].heading.x;
		if ((rand() % 2 + 0) == 1) birds[i].heading.y = -birds[i].heading.y;

		int grid_pos = GetGridPos(&birds[i].position);
		bird_grid[grid_pos] = AddToList(bird_grid[grid_pos], &birds[i]);
	}
	//-----------------------------------------------------------------------
	//Generate Matricies
	//-----------------------------------------------------------------------
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, xSize, ySize, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	//-----------------------------------------------------------------------
	//Start Game
	//-----------------------------------------------------------------------
	glutMainLoop();
	//-----------------------------------------------------------------------
}


