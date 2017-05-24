#ifndef COMMON

#define COMMON

#define NUMBER_OF_NODES 100
#define NUMBER_OF_CLUSTER 5
#define ROUND 1000
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
    struct measurement tm[NUMBER_OF_NODES];
    int cluster_length, tm_length;
    int is_cluster_head;
    int position;
    double energy;
};

#endif
