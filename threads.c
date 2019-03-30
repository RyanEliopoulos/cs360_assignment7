#include<unistd.h>
#include<stdlib.h>
#include<math.h>
#include<stdio.h>
#include<pthread.h>



struct philosopher {
    int id;             // philosopher number 0-4
    int left_chopstick; // index values for sems
    int right_chopstick;
    pthread_mutex_t *sems; // array of mutexes
} philosopher;    


int randomGaussian(int, int);
void setTable (pthread_mutex_t *, struct philosopher *);
void beginBanquet (pthread_mutex_t *, struct philosopher *);
void dine (struct philosopher *phil);

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
        pthread_create(threads+i, NULL, (void *) dine, (void *) (phils+i));
    }


    


    // wait for all the threads to print before program terminates
    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }
}

// logic for an individual philosopher
void dine(struct philosopher *phil) {

   
    int think = randomGaussian(4, 8);     
    think = (think < 0) ? 0: think;

    printf("philosopher %d is thinking for %d seconds\n", phil->id, think);
    sleep(think);
    
    printf("philosopher %d wants chopsticks %d and %d\n", phil->id, phil->left_chopstick, phil->right_chopstick);
    
      
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


