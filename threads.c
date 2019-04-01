


#include<string.h>
#include<errno.h>
#include<error.h>
#include<unistd.h>
#include<stdlib.h>
#include<math.h>
#include<stdio.h>
#include<pthread.h>


#define EAT_TIME  100  // time spent eating before leaving the table

/* error codes */
#define THRD_CREAT 1 // error creating thread
#define MTX_UNLK 2   // error unlocking a mutex

struct philosopher {
    int id;             // philosopher number 0-4
    int left_chopstick; // index values for sems
    int right_chopstick;
    pthread_mutex_t *sems; // array of chopstick mutexes
} philosopher;    


// provided function for randomizing eat/think values
int randomGaussian(int, int);

// initialize the mutexes and philosophers
void setTable (pthread_mutex_t *, struct philosopher *);

// kind of a wrapper for dine. 
// spins up the threads that execute dine
void beginBanquet (pthread_mutex_t *, struct philosopher *);

// core eating/thinking/shared resource consumption logic
// each thread executes this function
void dine (struct philosopher *phil);

// attempts to lock both of the mutexes
// standing in as chopsticks
int getChopsticks(struct philosopher *);

// unlock the semaphores standing in as chopsticks
void returnChopsticks(struct philosopher *);

void main() {

    // instantiate dining variables - chopsticks and philosophers
    pthread_mutex_t sems[5];
    struct philosopher phils[5];

    // initialize dining variables
    setTable(sems, phils);

    // and feast!
    beginBanquet(sems, phils);    
}

void beginBanquet(pthread_mutex_t *sems, struct philosopher *phils) {

    pthread_t threads[5];

    // Spin up threads    
    for (int i = 0; i < 5; i++) {
        if ( pthread_create(threads+i, NULL, (void *) dine, (void *) (phils+i)) != 0 ) {
            fprintf(stderr, "Error spinning up thread %d\n %s\n", i, strerror(errno));
            exit(THRD_CREAT);
        }
    }
    
    // join philosopher threads before returning to main and terminating
    int ret;
    for (int i = 0; i < 5; i++) {
        ret = pthread_join(threads[i], NULL);
        if (ret != 0) {
            printf("thread %i returned with status: %s\n", i, strerror(ret));
        }
        else {
            printf("thread %d returned successfully\n", i);
        }
    }
}

// logic for an individual philosopher
void dine(struct philosopher *phil) {

    // instantiate our sims' food and thought meters
    int time_eating = 0;    // cumulative eat tim
    int time_thinking = 0;  // cumulative think time
    int think_time;         // interval think time
    int eat_time;           // interval eat time
    
    // and begin the thinking party 
    while (time_eating < EAT_TIME) {

        // get the thinking out of the way
        think_time  = randomGaussian(4, 8);     
        think_time = (think_time < 0) ? 0: think_time;
        printf("philosopher %d is thinking for %d seconds (%d)\n", phil->id, think_time, time_thinking);
        time_thinking += think_time;
        sleep(think_time);
        
        // now attempt to eat
        // loop continues until philosopher can gets both chopsticks
        printf("philosopher %d wants chopticks %d and %d\n", phil->id, phil->left_chopstick, phil->right_chopstick);
        while (getChopsticks(phil)) sleep(2); // 

        // have both chopsticks. now 'eat'  
        eat_time = randomGaussian(8, 8);
        eat_time = (eat_time < 0) ? 0: eat_time;
        printf("Philosopher %d is eating for %d seconds (%d)\n", phil->id, eat_time, time_eating);
        time_eating += eat_time;
        sleep(eat_time);

        // eating time done. Need to return the chopsticks
        returnChopsticks(phil);
    }
}


void returnChopsticks(struct philosopher *phil) {

    // return left chopstick
    int ret;
    if ( (ret = pthread_mutex_unlock(&phil->sems[phil->left_chopstick]) != 0)) {
        fprintf(stderr, "thread %d encountered error unlocking left chopstick mutex\n %s\n", phil->id, strerror(ret));
        exit(MTX_UNLK);
    }
    // return right chopstick
    if ( (ret = pthread_mutex_unlock(&phil->sems[phil->right_chopstick]) != 0)) {
        fprintf(stderr, "thread %d encountered error unlocking right chopstick mutex\n %s\n", phil->id, strerror(ret));
        exit(MTX_UNLK);
    }
}

int getChopsticks(struct philosopher *phil) {

    // try for left chopstick
    int ret;
    if ( (ret = pthread_mutex_trylock(&phil->sems[phil->left_chopstick])) == EBUSY) {  // check for a locked mutex
        return -1;
    } 
    else if (ret != 0) {  // check if terminal error occured attemptin to get mutex
        fprintf(stderr, "thread %d encountered error trylock-ing mutex %d\n %s\n", phil->id, phil->left_chopstick, strerror(ret));
        fprintf(stderr, "terminating\n");
        exit(MTX_UNLK); 
    }
    
    // have left chopstick. Try for the right
    if ( (ret = pthread_mutex_trylock(&phil->sems[phil->right_chopstick])) == EBUSY) { // check for a locked mutex
        ret = pthread_mutex_unlock(&phil->sems[phil->left_chopstick]);  // right chopstick was locked, so return the left one
        if (ret != 0) {  
            fprintf(stderr, "thread %d encountered error unlocking mutex %d\n %s\n", phil->id, phil->left_chopstick, strerror(ret));
            exit(MTX_UNLK);
        } 
        return -1;         
    }
    else if (ret != 0) {  // terminal error occcured attempting to et the right chopstick mutex
        fprintf(stderr, "thread %d encountered error trylock-ing mutex %d\n %s\n", phil->id, phil->left_chopstick, strerror(ret));
        exit(MTX_UNLK);
    }
    // have both chopsticks
    return 0;
}

// initializes dining variables - chopsticks and philosophers
void setTable (pthread_mutex_t *sems, struct philosopher *phils) {

    for (int i = 0; i < 5; i++) {
        pthread_mutex_init(sems + i, NULL);
        phils[i].id = i;
        phils[i].left_chopstick = i;
        phils[i].right_chopstick = (i + 1) % 5;
        phils[i].sems = sems;  
    }
}

// provides randomness to thinking/drinking
int randomGaussian(int mean, int stddev) {

	double mu = 0.5 + (double) mean;
	double sigma = fabs((double) stddev);
	double f1 = sqrt(-2.0 * log((double) rand() / (double) RAND_MAX));
	double f2 = 2.0 * 3.14159265359 * (double) rand() / (double) RAND_MAX;
	if (rand() & (1 << 5)) 
		return (int) floor(mu + sigma * cos(f2) * f1);
	else            
		return (int) floor(mu + sigma * sin(f2) * f1);
}


