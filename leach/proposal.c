#include <stdio.h>
#include "policy.h"
#include "../include/common.h"

extern struct node nodes[NUMBER_OF_NODES];

extern int cluster_head[NUMBER_OF_CLUSTER*4];
extern int base_station;
extern int number_of_cluster;
extern int now_time;

static int tx_count = 0;
static int expired = 0;
static int measurements = 0;

double max(double a, double b) {
    return (a>b)?a:b;
}

void algorithm(struct node* now) {
    // anyway, calculate reward from TM buffer
    // transmit or not judge by the expected value

    // reward test by un-expired measurements
    int reward = 0, expected = 0;
    const int round = NUMBER_OF_NODES/NUMBER_OF_CLUSTER;
    const double p = (double)NUMBER_OF_CLUSTER/(double)NUMBER_OF_NODES;

    if(now->clusterHead >= 0) {
        for(int i=0; i<now->tm_length; i++)
            now->tm[i].N = now->tm[i].deadline-1;
    }else {
        for(int i=0; i<now->tm_length; i++)
            now->tm[i].N = now->tm[i].deadline;
    }

    for(int i=0; i<now->tm_length; i++) {
        if(now->tm[i].N >= now_time) reward++;
        if(now->tm[i].N > now_time) expected++;
    }

    if(!now->is_clustered) {
        // how to estimate the possibility of un-clustered node's reception of measurements
        // average cluster size * P(member tx) * E(member tm_length)
        expected += 1/(1-p*(now_time%round));
    }else {
        expected++;
    }

    // TX
    if(reward >= expected) {
        for(int i=0; i<now->tm_length; i++)
            if(now->tm[i].N < now_time)
                expired++;
        tx_count += 1;
        measurements += now->self_gen;

        if(now->clusterHead >= 0) {
            for(int i=0; i<now->tm_length; i++) {
                int len = nodes[now->clusterHead].tm_length + i;
                nodes[now->clusterHead].tm[len].deadline = now->tm[i].deadline;
                nodes[now->clusterHead].tm[len].N = now->tm[i].N;
                nodes[now->clusterHead].tm[len].size = now->tm[i].size;
            }

            nodes[now->clusterHead].tm_length += now->tm_length;
        }

        now->tm_length = now->self_gen = 0;
    }
}

void da_policy() {
    printf("Proposal - epoch %d\n", now_time);

    tx_count = 0;
    expired = 0;
    measurements = 0;

    for(int n=0; n<number_of_cluster; n++) {
        for(int m=0; m<nodes[cluster_head[n]].cluster_length; m++) {
            int current_node = nodes[cluster_head[n]].clusters[m];
            algorithm(&nodes[current_node]);
        }
        algorithm(&nodes[cluster_head[n]]);
    }

    printf("expired/total/tx count = %d/%d/%d\n", expired, measurements, tx_count);

    return;
}

