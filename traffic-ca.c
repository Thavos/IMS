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
#define CAR_COUNT 5
#define PROBABILITY 12 // PROBABILITY of 10 implementing rule number 3
#define SEED 532464521

#define PERCENTAGE 100
// depends on how accurate the probability should be
// can replace PERCENTAGE in rule number 3
// #define PROMILE 1000

// speed which will have new cars in the model
#define STARTING_SPEED 3
#define SPAWNING_CARS_CHANCE 30 // what is chane of spwaning car each iteration

#define NUMBER_OF_ROADS 4

// sum of those 3 has to be hundred
#define CHANCE_TO_RIGHT 33
#define CHANCE_TO_LEFT 33
#define CHANCE_TO_STRAIGHT 34

// turning to right when the light is red
#define TURNING_RIGHT_RED true

#define DIRECTION_OPTIONS 3

enum direction
{
    S,
    L,
    R,
};

/// it is simulation of intersectation of 4 roads which will be coming
/// from 4 directions NORTH, WEST, SOUTH, EAST
/// on the road with ids:
/// NORTH are cars moving from NORTH to SOUTH
/// WEST are cars moving from WEST to EAST
/// SOUTH are cars moving from SOUTH to NORTH
/// EAST are cars moving from EAST to WEST

int MAX_STATIONARY_DURATION = 0;

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
    // false until it is beyond field 53 after it is set to truth
    // count number of cars which passed the crossroad
    bool passed_crossroad;
    int stationary_counter;
} car;

typedef struct road
{
    unsigned int position;
    car *car_ptr;
    bool is_intersection;

} field;

// calculates average over iterations where car were stationary without movement
// variable stationary_sum is summing up averages of stationary cars in one iteration
void avegare_cars_are_stationary(field (*roads)[ROAD_LENGTH], float *stationary_sum)
{
    float stationary_cars = 0;
    float all_cars = 0;
    for (int i = 0; i < NUMBER_OF_ROADS; i++)
    {
        field *road = roads[i];
        for (int j = 0; j < ROAD_LENGTH; j++)
        {
            if (road[j].car_ptr != NULL)
            {
                all_cars++;
                if (road[j].car_ptr->speed == 0)
                {
                    stationary_cars++;
                }
            }
        }
    }

    // result * 100 = percentage of stationary cars in this iteration
    if (all_cars != 0)
    {
        float result = stationary_cars / all_cars;
        (*stationary_sum) = (*stationary_sum) + result;
    }
}

void get_average_speed_values(field (*roads)[ROAD_LENGTH], float *speed, float *measurement)
{
    for (int i = 0; i < NUMBER_OF_ROADS; i++)
    {
        field *road = roads[i];
        for (int j = 0; j < ROAD_LENGTH; j++)
        {
            if (road[j].car_ptr != NULL)
            {
                (*speed) = (*speed) + road[j].car_ptr->speed;
                (*measurement)++;
            }
        }
    }
}

void count_passed_cars(field (*roads)[ROAD_LENGTH], int *counter)
{
    for (int i = 0; i < NUMBER_OF_ROADS; i++)
    {
        field *road = roads[i];
        for (int j = 53; j < ROAD_LENGTH; j++)
        {
            if (road[j].car_ptr != NULL && road[j].car_ptr->passed_crossroad == false)
            {
                (*counter)++;
                road[j].car_ptr->passed_crossroad = true;
            }
        }
    }
}

enum direction choose_direction()
{
    int random_num = rand() % 100;

    if (random_num < CHANCE_TO_RIGHT)
    {
        return R; // Right
    }
    else if (random_num < CHANCE_TO_RIGHT + CHANCE_TO_LEFT)
    {
        return L; // Left
    }
    else
    {
        return S; // Straight
    }
}

// generating new cars
void spawn_cars(field (*roads)[ROAD_LENGTH])
{

    for (int i = 0; i < NUMBER_OF_ROADS; i++)
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
            new_car->speed = rand() % MAX_SPEED;
            ; // starting speed
            new_car->dir = choose_direction();
            new_car->passed_crossroad = false;
            new_car->stationary_counter = 0;
            road[0].car_ptr = new_car;
        }
    }
}

void init(int seed, field road[])
{
    // Init random gen
    // srand(seed); //
    (void)seed;
    // int warn = seed;
    // warn++;
    srand(time(NULL));

    static int test = 0; /// TODO odstran debug only

    // Init roads
    for (int i = 0; i < ROAD_LENGTH; i++)
    {
        road[i].position = i;
        road[i].car_ptr = NULL;
        // TODO
        road[i].is_intersection = false;
    }

    // if (test != 0 && test != 3)
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
        new_car->stationary_counter = 0;
        if (pos > 49)
        {
            new_car->dir = S;
            new_car->passed_crossroad = true;
        }
        else
        {
            new_car->dir = choose_direction();
            new_car->passed_crossroad = false;
        }

        road[pos].car_ptr = new_car;
    }
    test++;
}

void print_road(field road[], short int road_code, semaphor N, semaphor W)
{
    for (int i = 0; i < ROAD_LENGTH; i++)
    {
        if (road[i].car_ptr == NULL)
            if ((road_code == NORTH || road_code == SOUTH) && N.green)
            {
                printf("\033[0;32m-\033[0m");
            }
            else if (((road_code == WEST || road_code == EAST) && W.green))
            {
                printf("\033[0;32m-\033[0m");
            }
            else
            {

                printf("\033[0;31m-\033[0m");
            }
        else
        {
            if (road[i].car_ptr->dir == S)
            {
                printf("%d", road[i].car_ptr->speed);
            }
            else if (road[i].car_ptr->dir == R)
            {
                printf("\033[0;34m%d\033[0m", road[i].car_ptr->speed);
            }
            else if (road[i].car_ptr->dir == L)
            {
                printf("\033[0;33m%d\033[0m", road[i].car_ptr->speed);
            }
        }
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

                        // will check if there are incoming cars from the direction whehe the green light is on if there are none
                        // speed will adjust to 2 ot 1 and turning right rule will take care of rest
                        bool check_turn = false;
                        if (TURNING_RIGHT_RED && road[i].car_ptr->dir == R)
                        {
                            check_turn = true;
                            int new_road_id = (j - 1) % 4;
                            if (new_road_id < 0)
                                new_road_id = 3;

                            field *new_road = roads[new_road_id];

                            for (int k = 0; k < 6; k++)
                            {
                                if (new_road[POSITION_BEFORE_CROOSROAD + 2 - k].car_ptr != NULL)
                                {
                                    if (new_road[POSITION_BEFORE_CROOSROAD + 2 - k].position + new_road[POSITION_BEFORE_CROOSROAD + 2 - k].car_ptr->speed >= POSITION_BEFORE_CROOSROAD + 1)
                                    {
                                        check_turn = false;
                                    }
                                }
                            }
                        }

                        if (!check_turn)
                        {
                            speed = 0;
                        }
                        else
                        {
                            // turn to right on red light is safe and speed is determined by car speed at the field 49
                            // in case it was 0 car will accelerate and move just by one othewise by 2
                            speed = road[i].car_ptr->speed ? 2 : 1;
                        }
                    }

                    else if (i + speed > POSITION_BEFORE_CROOSROAD && i < POSITION_BEFORE_CROOSROAD)
                    {
                        speed = POSITION_BEFORE_CROOSROAD - i;
                    }
                }

                else if ((j == WEST || j == EAST) && W.green == false)
                {
                    if (i == POSITION_BEFORE_CROOSROAD)
                    {
                        // will check if there are incoming cars from the direction whehe the green light is on if there are none
                        // speed will adjust to 2 ot 1 and turning right rule will take care of rest
                        bool check_turn = false;
                        if (TURNING_RIGHT_RED && road[i].car_ptr->dir == R)
                        {
                            check_turn = true;
                            int new_road_id = (j - 1) % 4;
                            if (new_road_id < 0)
                                new_road_id = 3;

                            field *new_road = roads[new_road_id];

                            for (int k = 0; k < 6; k++)
                            {
                                if (new_road[POSITION_BEFORE_CROOSROAD + 2 - k].car_ptr != NULL)
                                {
                                    if (new_road[POSITION_BEFORE_CROOSROAD + 2 - k].position + new_road[POSITION_BEFORE_CROOSROAD + 2 - k].car_ptr->speed >= POSITION_BEFORE_CROOSROAD + 1)
                                    {
                                        check_turn = false;
                                    }
                                }
                            }
                        }

                        if (!check_turn)
                        {
                            speed = 0;
                        }
                        else
                        {
                            // turn to right on red light is safe and speed is determined by car speed at the field 49
                            // in case it was 0 car will accelerate and move just by one othewise by 2
                            speed = road[i].car_ptr->speed ? 2 : 1;
                        }
                    }
                    else if (i + speed > POSITION_BEFORE_CROOSROAD && i < POSITION_BEFORE_CROOSROAD)
                    {
                        speed = POSITION_BEFORE_CROOSROAD - i;
                    }
                }

                // Rule 4: Movement
                if (speed != 0)
                {

                    if (road[i].car_ptr->dir != L)
                    {
                        road[i].car_ptr->stationary_counter = 0;
                    }

                    if (road[i].car_ptr->dir != S && i == POSITION_BEFORE_CROOSROAD && speed > 2)
                    {
                        speed = 2;
                    }

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
                        if (new_road[POSITION_BEFORE_CROOSROAD + 3].car_ptr == NULL)
                        {
                            new_road[POSITION_BEFORE_CROOSROAD + 3].car_ptr = road[i].car_ptr;
                            new_road[POSITION_BEFORE_CROOSROAD + 3].car_ptr->speed = speed;
                            new_road[POSITION_BEFORE_CROOSROAD + 3].car_ptr->dir = S;
                            road[i].car_ptr = NULL;
                            continue;
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
                        if (new_road[POSITION_BEFORE_CROOSROAD + 4].car_ptr == NULL)
                        {
                            new_road[POSITION_BEFORE_CROOSROAD + 4].car_ptr = road[i].car_ptr;
                            new_road[POSITION_BEFORE_CROOSROAD + 4].car_ptr->speed = speed;
                            new_road[POSITION_BEFORE_CROOSROAD + 4].car_ptr->dir = S;
                            road[i].car_ptr = NULL;
                            continue;
                        }
                        else
                        {
                            speed = 1;
                            new_road[POSITION_BEFORE_CROOSROAD + 3].car_ptr = road[i].car_ptr;
                            new_road[POSITION_BEFORE_CROOSROAD + 3].car_ptr->speed = speed;
                            new_road[POSITION_BEFORE_CROOSROAD + 3].car_ptr->dir = S;
                            road[i].car_ptr = NULL;
                            continue;
                        }
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
                            new_road[POSITION_BEFORE_CROOSROAD + 1].car_ptr = road[i].car_ptr;
                            new_road[POSITION_BEFORE_CROOSROAD + 1].car_ptr->speed = speed;
                            // car will appear on the opposite road
                            // C -> CAR       K -> part of crossroad    - -> ordinary field
                            //   --                                            --
                            // --KK--         ----> next iteration           --KC-- so from now on for the car can be applied rule turning right when in field 50
                            // -CKK--                                        --KK-- of the opposite side
                            //   --                                            --
                            new_road[POSITION_BEFORE_CROOSROAD + 1].car_ptr->dir = R; // TODO ----- asi zmatok ale vysvetlim preco na R
                            new_road[POSITION_BEFORE_CROOSROAD + 1].car_ptr->stationary_counter = 0;
                            road[i].car_ptr = NULL;
                            continue;
                        }
                    }

                    // in case that both cars on the opposide side of the road are staying on the field 50 and are trying to turn left

                    if (i == POSITION_BEFORE_CROOSROAD + 1 && road[i].car_ptr->dir == L)
                    {

                        int new_road_id = (j + 2) % 4;
                        field *new_road = roads[new_road_id];

                        // they will change positions at the same time
                        if (new_road[POSITION_BEFORE_CROOSROAD + 1].car_ptr != NULL && new_road[POSITION_BEFORE_CROOSROAD + 1].car_ptr->dir == L)
                        {
                            car *tmp_ptr = road[i].car_ptr;
                            road[i].car_ptr = new_road[POSITION_BEFORE_CROOSROAD + 1].car_ptr;
                            road[i].car_ptr->dir = R;
                            road[i].car_ptr->speed = 1;
                            road[i].car_ptr->stationary_counter = 0;
                            new_road[POSITION_BEFORE_CROOSROAD + 1].car_ptr = tmp_ptr;
                            new_road[POSITION_BEFORE_CROOSROAD + 1].car_ptr->dir = R;
                            new_road[POSITION_BEFORE_CROOSROAD + 1].car_ptr->speed = 1;
                            new_road[POSITION_BEFORE_CROOSROAD + 1].car_ptr->stationary_counter = 0;
                            continue;
                        }
                    }

                    if (road[i].car_ptr->dir == L && i == POSITION_BEFORE_CROOSROAD + 1 && speed == 1)
                    {

                        int new_road_id = (j + 2) % 4;
                        field *new_road = roads[new_road_id];
                        bool check = true;
                        // check if the opposite side is empty so car can turn left
                        for (int k = 0; k < 6; k++)
                        {
                            if (new_road[POSITION_BEFORE_CROOSROAD + 1 - k].car_ptr != NULL)
                            {
                                if (new_road[POSITION_BEFORE_CROOSROAD + 1 - k].position + new_road[POSITION_BEFORE_CROOSROAD + 1 - k].car_ptr->speed >= POSITION_BEFORE_CROOSROAD + 1)
                                {
                                    road[i].car_ptr->speed = 0;
                                    check = false;
                                    break;
                                }
                            }
                        }
                        if (check)
                        {
                            new_road[POSITION_BEFORE_CROOSROAD + 1].car_ptr = road[i].car_ptr;
                            new_road[POSITION_BEFORE_CROOSROAD + 1].car_ptr->speed = speed;
                            // same thing as above only now car is moving from starting position 50 by one field
                            new_road[POSITION_BEFORE_CROOSROAD + 1].car_ptr->dir = R;
                            new_road[POSITION_BEFORE_CROOSROAD + 1].car_ptr->stationary_counter = 0;
                            road[i].car_ptr = NULL;
                            continue;
                        }
                        else
                        {
                            // stopping
                            road[i].car_ptr->stationary_counter++;
                            if (road[i].car_ptr->stationary_counter > MAX_STATIONARY_DURATION)
                            {
                                MAX_STATIONARY_DURATION = road[i].car_ptr->stationary_counter;
                            }
                            continue;
                        }
                    }

                    // in this case car will be already on the road with correct direction
                    if (road[i].car_ptr->dir == L && i == POSITION_BEFORE_CROOSROAD + 1 && speed == 2)
                    {

                        int new_road_id = (j + 1) % 4;

                        field *new_road = roads[new_road_id];
                        new_road[POSITION_BEFORE_CROOSROAD + 3].car_ptr = road[i].car_ptr;
                        new_road[POSITION_BEFORE_CROOSROAD + 3].car_ptr->speed = speed;
                        new_road[POSITION_BEFORE_CROOSROAD + 3].car_ptr->dir = S;
                        new_road[POSITION_BEFORE_CROOSROAD + 3].car_ptr->stationary_counter = 0;
                        road[i].car_ptr = NULL;
                        continue;
                    }

                    // going straight
                    road[(i + speed)].car_ptr = road[i].car_ptr; // Move car to new position
                    road[(i + speed)].car_ptr->speed = speed;
                    road[i].car_ptr = NULL;
                }
                else
                {
                    road[i].car_ptr->speed = 0;
                    road[i].car_ptr->stationary_counter++;
                    if (road[i].car_ptr->stationary_counter > MAX_STATIONARY_DURATION)
                    {
                        MAX_STATIONARY_DURATION = road[i].car_ptr->stationary_counter;
                    }
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

    int car_passed = 0;
    float sum_speed = 0;
    float total_measurement = 0;
    // sums up the stationary cars over each iteration
    float stationary_cars_average = 0;

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

    printf("number represents car speed and colour of number the direction:\nWHITE --> STRAIGHT\t\033[0;34mBLUE --> RIGHT\t\033[0;33mYELLOW --> LEFT\033[0m\n");

    // Start simulation
    for (int t = 1; t < ITERATIONS; t++)
    {
        printf("Iteration %d:\n", t);

        if (NORTH_SOUTH.green)
        {
            printf("\033[0;32m%s \t\033[0m", dir_info[NORTH]);
            print_road(road[NORTH], NORTH, NORTH_SOUTH, WEST_EAST);
            printf("\033[0;32m\t %s \n\033[0m", dir_info[SOUTH]);
        }
        else
        {
            printf("\033[0;31m%s \t\033[0m", dir_info[NORTH]);
            print_road(road[NORTH], NORTH, NORTH_SOUTH, WEST_EAST);
            printf("\033[0;31m\t %s \n\033[0m", dir_info[SOUTH]);
        }

        if (WEST_EAST.green)
        {
            printf("\033[0;32m%s \t\033[0m", dir_info[WEST]);
            print_road(road[WEST], WEST, NORTH_SOUTH, WEST_EAST);
            printf("\033[0;32m\t %s \n\033[0m", dir_info[EAST]);
        }
        else
        {
            printf("\033[0;31m%s \t\033[0m", dir_info[WEST]);
            print_road(road[WEST], WEST, NORTH_SOUTH, WEST_EAST);
            printf("\033[0;31m\t %s \n\033[0m", dir_info[EAST]);
        }

        if (NORTH_SOUTH.green)
        {
            printf("\033[0;32m%s \t\033[0m", dir_info[SOUTH]);
            print_road(road[SOUTH], SOUTH, NORTH_SOUTH, WEST_EAST);
            printf("\033[0;32m\t %s \n\033[0m", dir_info[NORTH]);
        }
        else
        {
            printf("\033[0;31m%s \t\033[0m", dir_info[SOUTH]);
            print_road(road[SOUTH], SOUTH, NORTH_SOUTH, WEST_EAST);
            printf("\033[0;31m\t %s \n\033[0m", dir_info[NORTH]);
        }

        if (WEST_EAST.green)
        {
            printf("\033[0;32m%s \t\033[0m", dir_info[EAST]);
            print_road(road[EAST], EAST, NORTH_SOUTH, WEST_EAST);
            printf("\033[0;32m\t %s \n\033[0m", dir_info[WEST]);
        }
        else
        {
            printf("\033[0;31m%s \t\033[0m", dir_info[EAST]);
            print_road(road[EAST], EAST, NORTH_SOUTH, WEST_EAST);
            printf("\033[0;31m\t %s \n\033[0m", dir_info[WEST]);
        }

        count_passed_cars(road, &car_passed);
        get_average_speed_values(road, &sum_speed, &total_measurement);
        avegare_cars_are_stationary(road, &stationary_cars_average);

        for (int i = ROAD_LENGTH - 1; i >= 0; i--)
        {
            // printf("%i\n",i);
            // if (i == 49)
            // {
            //     printf("TU \n");
            // }
            update_road(road, i, NORTH_SOUTH, WEST_EAST);
        }

        update_semaphor(&NORTH_SOUTH, &WEST_EAST);
        spawn_cars(road);
    }

    printf("Number of cars which passed the crossroad: %i\n", car_passed);

    float average_speed = sum_speed / total_measurement;

    printf("Average speed of cars was: %f\n", average_speed);

    float cars_stationary = stationary_cars_average / (ITERATIONS - 1);

    printf("The average percentage of stationary cars over the simulation is: %f%%\n", cars_stationary * 100);

    printf("Maximum duration in which car was stationary %i\n", MAX_STATIONARY_DURATION);
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

//