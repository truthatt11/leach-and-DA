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
    int A[ROUND], R[ROUND];
    int maxR = -1, maxRindex;
    // N = ROUND - #hops
    int N = ROUND;

    for(int i=0; i<ROUND; i++) R[i] = 0;
    if(now->clusterHead >= 0) {
        for(int i=0; i<now->tm_length; i++)
            now->tm[i].N = now->tm[i].deadline-1;
    }else {
        for(int i=0; i<now->tm_length; i++)
            now->tm[i].N = now->tm[i].deadline;
    }


    for(int i=now_time; i<N; i++) {
        int avg = 0;
        for(int j=0; j<now->tm_length; j++) {
            int tmp = now->tm[j].N - i;
            if(tmp < 0) tmp = 0;
            R[i] += tmp;
        }
        for(int j=0; j<now->tm_length; j++)
            avg += now->tm[j].N;
        if(now->tm_length != 0)
            avg /= now->tm_length;
        for(int j=0; j<i-1; j++) {
            R[i] += (avg-j)*1;
        }
    }

    for(int i=0; i<N; i++) {
        if(R[i] > maxR) {
            maxR = R[i];
            maxRindex = i;
        }
    }

    // 1=wait, 2=transmit
    for(int k=N-2; k>=0; k--) {
        if(k != maxRindex)
            A[k] = 1;
        else {
            double rb[ROUND], rn[ROUND];
            rb[ROUND-1] = 1.0;
            rn[ROUND-1] = 0.0;

            for(int i=ROUND-2; i>=k; i--) {
                rn[i] = max(0.0, i*rn[i+1]/(i+1) + rb[i+1]/(i+1));
                rb[i] = max(R[i], rn[i]);
            }

            if(R[k] >= rb[k+1])
                A[k] = 2;
            else
                A[k] = 1;
        }
    }

    if(A[now_time] > 1) {
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

    return;
}

void da_policy() {
    printf("Transmission manager - epoch %d\n", now_time);

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

