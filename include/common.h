#ifndef COMMON

#define COMMON

#define NUMBER_OF_NODES 100
#define SIZE_OF_TM 2000
#define NUMBER_OF_CLUSTER 5
#define ROUND 600
#define MASK_BIT 10
#define MASK ((1<<MASK_BIT)-1)
#define GET_X(a) (a>>MASK_BIT)
#define GET_Y(a) (a&MASK)
#define SENSOR_DATA 1000

struct measurement{
    int deadline;
    int N;
    int size;
};

struct node{
    int clusters[NUMBER_OF_NODES];
    struct measurement tm[SIZE_OF_TM];
    int cluster_length, tm_length, self_gen;
    int clusterHead;
    int position;
    int is_clustered;
    double energy;

    // for proposal usage
    int states[ROUND];
};

#endif
