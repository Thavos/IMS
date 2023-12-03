#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define ITERATIONS  100
#define ROAD_LENGTH 100
#define MAX_SPEED   5

typedef struct {
    int position_id,
        state,
        timer;
} light;

void init(int seed, int road[], int new_road[], int empty_road[]);
void print_road(int road[]);
void update_road(int road[], int new_road[], light *my_light);

int main(){
    int road[ROAD_LENGTH],
        new_road[ROAD_LENGTH],
        empty_road[ROAD_LENGTH];

    light my_light = {49, 1, 20};

    init(149632231, road, new_road, empty_road);
    for(int t = 0; t < ITERATIONS; t++){
        if(rand() % 10 < 10)
            road[0] = 1;
        printf("Iteration %d:\t", t);
        print_road(road);
        memcpy(new_road, empty_road, ROAD_LENGTH * sizeof(int)); // Clear new road before next step
        update_road(road, new_road, &my_light);
    }

    return 0;
}

void init(int seed, int road[], int new_road[], int empty_road[]){
    // Init roads
    srand(seed);

    // Init road
    for(int i = 0; i < ROAD_LENGTH; i++){
        road[i] = new_road[i] = empty_road[i] = -1;
    }
}

void print_road(int road[]){
    for(int i = 0; i < ROAD_LENGTH; i++){
        if(road[i] == -1)
            printf("-");
        else
            printf("%d", road[i]);
    }
    printf("\n");
}

void update_road(int road[], int new_road[], light *my_light){
    int speed = 0;
    for(int i = ROAD_LENGTH - 1; i >= 0; i--){
        if(i == my_light->position_id){
            my_light->timer--;
            if(my_light->timer == 0){
                if(my_light->state == 1){
                    my_light->state = 0;
                    road[i] = 10;
                }else{
                    my_light->state = 1;
                    road[i] = -1;
                }
                my_light->timer = 20;
            }
        }
        if(road[i] != -1){
            speed = road[i];
            if(speed == 10)
                continue;
            
            // Rule 1: Acceleration
            if(speed < MAX_SPEED){
                speed++;
            }

            // Rule 2: Breaking
            for(int j = 1; j <= speed; j++){
                if(i + j >= ROAD_LENGTH)
                    break;
                
                if(new_road[i + j] != -1){
                    speed = j - 1;
                    break;
                }
            }

            // Rule 3: Random factor
            if(speed > 0 && rand() % 10 < 3)
                speed--;
            
            if(i + speed < ROAD_LENGTH)
                new_road[(i + speed)] = speed; // Move car to new position
        }
    }

    memcpy(road, new_road, ROAD_LENGTH * sizeof(int));
}