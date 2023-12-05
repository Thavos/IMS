#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#define ITERATIONS 50

// has to be a even number
#define ROAD_LENGTH 100
#define POSITION_BEFORE_CROOSROAD ROAD_LENGTH / 2 - 1

#define MAX_SPEED 5
#define CAR_COUNT 20
#define PROBABILITY 10 // PROBABILITY of 10 implementing rule number 3
#define SEED 1232387932

#define PERCENTAGE 100
// depends on how accurate the probability should be
// can replace PERCENTAGE in rule number 3
// #define PROMILE 1000

// speed which will have new cars in the model
#define STARTING_SPEED 3
#define SPAWNING_CARS_CHANCE 20 // what is chane of spwaning car each iteration

#define NUMBER_OF_ROADS 4

#define DIRECTION_OPTIONS 3
enum direction
{
    R,
    L,
    S
};

/// it is simulation of intersectation of 4 roads which will be coming
/// from 4 directions NORTH, WEST, SOUTH, EAST
/// on the road with ids:
/// NORTH are cars moving from NORTH to SOUTH
/// WEST are cars moving from WEST to EAST
/// SOUTH are cars moving from SOUTH to NORTH
/// EAST are cars moving from EAST to WEST

enum road_dir
{
    NORTH,
    WEST,
    SOUTH,
    EAST
};

typedef struct sem
{
    bool green;
    short int timer;
} semaphor;

typedef struct cars
{
    unsigned int speed;
    short unsigned int dir;
    // unsigned int position;

} car;

typedef struct road
{
    unsigned int position;
    car *car_ptr;
    bool is_intersection;
} field;

// generating new cars
void spawn_cars(field (*roads)[ROAD_LENGTH])
{

    for (int i = 0; i < 4; i++)
    {
        if ((rand() % PERCENTAGE < SPAWNING_CARS_CHANCE))
        {

            field *road = roads[i];

            if (road[0].car_ptr != NULL)
            {
                return;
            }

            car *new_car = (car *)malloc(sizeof(car));
            if (new_car == NULL)
            {
                fprintf(stderr, "ERROR: Allocating cars failed");
                exit(1);
            }
            new_car->speed = 3; // starting speed
            new_car->dir = rand() % DIRECTION_OPTIONS;

            road[0].car_ptr = new_car;
        }
    }
}

void init(int seed, field road[])
{
    // Init random gen
    srand(seed); //

    // int warn = seed;
    // warn++;
    // srand(time(NULL));

    static int test = 0; /// TODO odstran debug only

    // Init roads
    for (int i = 0; i < ROAD_LENGTH; i++)
    {
        road[i].position = i;
        road[i].car_ptr = NULL;
        // TODO
        road[i].is_intersection = false;
    }

    // if (test != 0)
    // {
    //     test++;
    //     return;
    // }

    // Init cars
    int pos = 0;
    for (int i = 0; i < CAR_COUNT; i++)
    {

        do
        {
            pos = (rand() % ROAD_LENGTH);
        } while (road[pos].car_ptr != NULL ||
                 // insures that starting cars will not spawn in the middle of crossroad or near proximity to it
                 (pos > ROAD_LENGTH / 2 - 5 && pos < ROAD_LENGTH / 2 + 5));

        car *new_car = (car *)malloc(sizeof(car));
        if (new_car == NULL)
        {
            fprintf(stderr, "ERROR: Allocating cars failed");
            exit(1);
        }
        int speed = rand() % MAX_SPEED; // rand() % MAX_SPEED ;
        new_car->speed = speed;
        new_car->dir = rand() % DIRECTION_OPTIONS;
        // if (pos < 49)
        // {
        //     new_car->dir = R;
        // }
        // else
        // {
        //     new_car->dir = S;
        // }

        road[pos].car_ptr = new_car;
    }
    test++;
}

void print_road(field road[])
{
    for (int i = 0; i < ROAD_LENGTH; i++)
    {
        if (road[i].car_ptr == NULL)
            printf("-");
        else
            printf("%d", road[i].car_ptr->speed);
    }
    // printf("\n");
}

void update_road(field (*roads)[ROAD_LENGTH], int i, semaphor N, semaphor W)
{
    int speed = 0;

    // j is representing all 4 roads -> enum road_dir
    for (int j = 0; j < 4; j++)
    {

        field *road = roads[j];

        if (road[i].car_ptr != NULL)
        {
            speed = road[i].car_ptr->speed;

            // Rule 1: Acceleration
            if (speed < MAX_SPEED)
            {
                speed++;
            }

            // Rule 2: Breaking
            for (int j = 1; j <= speed; j++)
            {
                int k = (i + j);
                // car is leaving the simulation road
                if (k > ROAD_LENGTH - 1)
                {
                    free(road[i].car_ptr);
                    road[i].car_ptr = NULL;
                    break;
                }

                if (road[k].car_ptr != NULL)
                {
                    speed = j - 1;
                    break;
                }
            }

            // check if car left the simulaton grid
            if (road[i].car_ptr != NULL)
            {

                // Rule 3: Random factor
                if (speed > 0 && (rand() % PERCENTAGE < PROBABILITY))
                {
                    speed--;
                }

                // if the red light is on and car is infront of crossroad then stop
                if ((j == NORTH || j == SOUTH) && N.green == false)
                {
                    // if it is standing before crossroad car does not move
                    if (i == POSITION_BEFORE_CROOSROAD)
                    {
                        speed = 0;
                    }
                    else if (i + speed > POSITION_BEFORE_CROOSROAD && i < POSITION_BEFORE_CROOSROAD)
                    {
                        speed = POSITION_BEFORE_CROOSROAD - i;
                    }
                }

                if ((j == WEST || j == EAST) && W.green == false)
                {
                    if (i == POSITION_BEFORE_CROOSROAD)
                    {
                        speed = 0;
                    }
                    else if (i + speed > POSITION_BEFORE_CROOSROAD && i < POSITION_BEFORE_CROOSROAD)
                    {
                        speed = POSITION_BEFORE_CROOSROAD - i;
                    }
                }

                // Rule 4: Movement
                if (speed != 0)
                {

                    // cars turning will always be on position POSITION_BEFORE_CROOSROAD so there is no need for checking additionaly if they can go or not the codnition above will
                    // take care of it

                    // make sure that cars going straight will not continue on red light into the middle of crossroad

                    // Rule: Turning
                    // closing to the crossroad and car has intend to change directions, it has to slow down to max speed 2
                    // constant POSITION_BEFORE_CROOSROAD field before crossroad
                    // turning car will always be at field POSITION_BEFORE_CROOSROAD
                    if (road[i].car_ptr->dir != S && speed + i > POSITION_BEFORE_CROOSROAD && i != POSITION_BEFORE_CROOSROAD && i <= POSITION_BEFORE_CROOSROAD)
                    {
                        speed = POSITION_BEFORE_CROOSROAD - i;
                    }

                    // turning right
                    // is placed on field POSITION_BEFORE_CROOSROAD and turning with speed 2
                    // this is case where car was not stopped at the crossroad

                    // TODO vzdy bude mat rychlost 2 nebere sa do uvahy random factor
                    if (road[i].car_ptr->dir == R && i == POSITION_BEFORE_CROOSROAD && speed == 2)
                    {
                        int new_road_id = (j - 1) % 4;
                        if (new_road_id < 0)
                            new_road_id = 3;

                        field *new_road = roads[new_road_id];

                        // POSITION_BEFORE_CROOSROAD + 3 TODO asi by malo fungovat vzdy
                        if (new_road[52].car_ptr == NULL)
                        {
                            new_road[52].car_ptr = road[i].car_ptr;
                            new_road[52].car_ptr->speed = speed;
                            new_road[52].car_ptr->dir = S;
                            road[i].car_ptr = NULL;
                            return;
                        }
                        else
                        {
                            speed = 1;
                        }
                    }

                    // case where car was stopped at the crossroad and moved into field 50 with speed 1

                    if (i == 50 && road[i].car_ptr->dir == R)
                    {
                        speed = 2;
                        int new_road_id = (j - 1) % 4;
                        if (new_road_id < 0)
                            new_road_id = 3;

                        field *new_road = roads[new_road_id];
                        // POSITION_BEFORE_CROOSROAD + 4 TODO asi by malo fungovat vzdy
                        new_road[53].car_ptr = road[i].car_ptr;
                        new_road[53].car_ptr->speed = speed;
                        new_road[53].car_ptr->dir = S;
                        road[i].car_ptr = NULL;
                        return;
                    }

                    // turning LEFT


                    if (road[i].car_ptr->dir == L && i == POSITION_BEFORE_CROOSROAD && speed == 2)
                    {
                        int new_road_id = (j + 2) % 4;
                        field *new_road = roads[new_road_id];
                        bool check = true;
                        // check if the opposite side is empty so car can turn left
                        for (int i = 0; i < 6; i++)
                        {
                            if (new_road[POSITION_BEFORE_CROOSROAD + 1 - i].car_ptr != NULL)
                            {
                                if (new_road[POSITION_BEFORE_CROOSROAD + 1 - i].position + new_road[POSITION_BEFORE_CROOSROAD + 1 - i].car_ptr->speed >= 50)
                                {
                                    check = false;
                                    speed = 1;
                                    break;
                                }
                            }
                        }
                        if (check)
                        {
                            new_road[50].car_ptr = road[i].car_ptr;
                            new_road[50].car_ptr->speed = speed;
                            // car will appear on the opposite road
                            // C -> CAR       K -> part of crossroad    - -> ordinary field
                            //   --                                            --
                            // --KK--         ----> next iteration           --KC-- so from now on for the car can be applied rule turning right when in field 50
                            // -CKK--                                        --KK-- of the opposite side
                            //   --                                            --
                            new_road[50].car_ptr->dir = R; // TODO ----- asi zmatok ale vysvetlim preco na R
                            road[i].car_ptr = NULL;
                            return;
                        }
                    }

                    // in case that both cars on the opposide side of the road are staying on the field 50 and are trying to turn left

                    if (i == 50 && road[i].car_ptr->dir == L)
                    {

                        int new_road_id = (j + 2) % 4;
                        field *new_road = roads[new_road_id];

                        // they will change positions at the same time 
                        if (new_road[50].car_ptr != NULL && new_road[50].car_ptr->dir == L){
                            car *tmp_ptr = road[i].car_ptr;
                            road[i].car_ptr = new_road[50].car_ptr;
                            road[i].car_ptr->dir = R;
                            new_road[50].car_ptr = tmp_ptr;
                            new_road[50].car_ptr->dir = R;
                        }
                    }

                    if (road[i].car_ptr->dir == L && i == 50 && speed == 1)
                    {

                        int new_road_id = (j + 2) % 4;
                        field *new_road = roads[new_road_id];

                        // check if the opposite side is empty so car can turn left
                        for (int i = 0; i < 6; i++)
                        {
                            if (new_road[POSITION_BEFORE_CROOSROAD + 1 - i].car_ptr != NULL)
                            {
                                if (new_road[POSITION_BEFORE_CROOSROAD + 1 - i].position + new_road[POSITION_BEFORE_CROOSROAD + 1 - i].car_ptr->speed >= 50)
                                {
                                    road[i].car_ptr->speed = 0;
                                    return;
                                }
                            }
                        }

                        new_road[50].car_ptr = road[i].car_ptr;
                        new_road[50].car_ptr->speed = speed;
                        // same thing as above only now car is moving from starting position 50 by one field
                        new_road[50].car_ptr->dir = R;
                        road[i].car_ptr = NULL;
                        return;
                    }

                    // in this case car will be already on the road with correct direction
                    if (road[i].car_ptr->dir == L && i == 50 && speed == 2)
                    {

                        int new_road_id = (j + 1) % 4;

                        field *new_road = roads[new_road_id];
                        new_road[52].car_ptr = road[i].car_ptr;
                        new_road[52].car_ptr->speed = speed;
                        new_road[52].car_ptr->dir = S;
                        road[i].car_ptr = NULL;
                        return;
                    }

                    // going straight
                    road[(i + speed)].car_ptr = road[i].car_ptr; // Move car to new position
                    road[(i + speed)].car_ptr->speed = speed;
                    road[i].car_ptr = NULL;
                }
                else
                {
                    road[i].car_ptr->speed = 0;
                }
            }
        }
    }
}

void update_semaphor(semaphor *N, semaphor *W)
{
    N->timer--;
    W->timer--;

    if (N->timer < 0)
    {
        N->timer = N->green ? 7 : 5; // if it is true turning light red
        N->green = !N->green;
    }

    if (W->timer < 0)
    {
        W->timer = W->green ? 7 : 5;
        W->green = !W->green;
    }
}

int main()
{

    field road[NUMBER_OF_ROADS][ROAD_LENGTH];
    // field new_road[ROAD_LENGTH];

    init(SEED, road[NORTH]);
    init(SEED, road[WEST]);
    init(SEED, road[SOUTH]);
    init(SEED, road[EAST]);

    // roads coming from north and east has green at the start of simulation
    semaphor NORTH_SOUTH = {.green = true, .timer = 5};
    semaphor WEST_EAST = {.green = false, .timer = 7};

    char dir_info[4][10] = {"NORTH", "WEST", "SOUTH", "EAST"};

    // Start simulation
    for (int t = 1; t < ITERATIONS; t++)
    {
        printf("Iteration %d:\n", t);
        printf("%s \t", dir_info[NORTH]);
        print_road(road[NORTH]);
        printf("\t %s \n", dir_info[SOUTH]);
        printf("%s \t", dir_info[WEST]);
        print_road(road[WEST]);
        printf("\t %s \n", dir_info[EAST]);
        printf("%s \t", dir_info[SOUTH]);
        print_road(road[SOUTH]);
        printf("\t %s \n", dir_info[NORTH]);
        printf("%s \t", dir_info[EAST]);
        print_road(road[EAST]);
        printf("\t %s \n", dir_info[WEST]);
        for (int i = ROAD_LENGTH - 1; i >= 0; i--)
        {
            // printf("%i\n",i);
            if (i == 49)
            {
                printf("TU \n");
            }
            update_road(road, i, NORTH_SOUTH, WEST_EAST);
        }
        update_semaphor(&NORTH_SOUTH, &WEST_EAST);
        spawn_cars(road);
    }

    for (int j = 0; j < 4; j++)
    {
        for (int i = 0; i < ROAD_LENGTH; i++)
        {
            if (road[j][i].car_ptr != NULL)
            {
                free(road[j][i].car_ptr);
                road[j][i].car_ptr = NULL;
            }
        }
    }
}