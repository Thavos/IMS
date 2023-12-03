#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define ITERATIONS 1000
#define ROAD_LENGTH 100
#define MAX_SPEED 5
#define CAR_COUNT 20

void init(int seed, int road[], int new_road[], int empty_road[]){
    // Init random gen
    srand(seed); //time(NULL));

    // Init roads   
    for(int i = 0; i < ROAD_LENGTH; i++){
        road[i] = -1;
        new_road[i] = -1;
        empty_road[i] = -1;
    }

    // Init cars
    int pos;
    for(int i = 0; i < CAR_COUNT; i++){
        do{
            pos = rand() % ROAD_LENGTH + 1;
        } while(road[pos] != -1);
        
        int speed = 0; //rand() % MAX_SPEED + 1;
        road[pos] = speed;
        new_road[pos] = speed;
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

void update_road(int road[], int new_road[]){
    int speed = 0;
    for(int i = 0; i < ROAD_LENGTH; i++){
        if(road[i] != -1){
            speed = road[i];
            
            // Rule 1: Acceleration
            if(speed < MAX_SPEED){
                speed++;
            }

            // Rule 2: Breaking
            for(int j = 1; j <= speed; j++){
                int k = (i+j) % ROAD_LENGTH;
                if(road[k] != -1){
                    speed = j - 1;
                    break;
                }
            }

            // Rule 3: Random factor
            if(speed > 0 && rand() % 10 < 3)
                speed--;
            
            // Rule 4: Movement
            new_road[(i + speed) % ROAD_LENGTH] = speed; // Move car to new position
        }
    }

    memcpy(road, new_road, ROAD_LENGTH * sizeof(int));
}

int main(){
    int road[ROAD_LENGTH],
        new_road[ROAD_LENGTH],
        empty_road[ROAD_LENGTH];
    int seed = 1232387932;

    init(seed, road, new_road, empty_road);

    // Start simulation
    for(int t = 1; t < ITERATIONS; t++){
        printf("Iteration %d:\t", t);
        print_road(new_road);
        memcpy(new_road, empty_road, ROAD_LENGTH * sizeof(int)); // Clear new road before next step
        update_road(road, new_road);
    }
}