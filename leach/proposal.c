#include <stdio.h>
#include "policy.h"
#include "../include/common.h"

extern struct node nodes[NUMBER_OF_NODES];

extern int cluster_head[NUMBER_OF_CLUSTER*4];
extern int base_station;
extern int number_of_cluster;
extern int now_time;

double max(double a, double b) {
    return (a>b)?a:b;
}

void da_policy() {
    printf("Transmission manager\n");

    int tx_count = 0;
    int expired = 0;
    int measurements = 0;
    int maxR = -1, maxRindex;

    // N = ROUND - #hops
    int N = ROUND;

    for(int n=0; n<number_of_cluster; n++) {
        for(int m=0; m<nodes[cluster_head[n]].cluster_length; m++) {
            int A[ROUND], R[ROUND];
            int current_node = nodes[cluster_head[n]].clusters[m];

            for(int i=0; i<ROUND; i++) R[i] = 0;

            for(int i=now_time; i<N; i++) {
                R[i] = 0;
                for(int j=0; j<nodes[current_node].tm_length; j++) {
                    int tmp = nodes[current_node].tm[j].deadline - i;
                    if(tmp < 0) tmp = 0;
                    R[i] += tmp;
                }
                for(int j=0; j<i-1; j++) {
                    R[i] += (nodes[current_node].tm[j].deadline-j)*1;
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
                    rb[now_time-1] = 1.0;
                    rn[now_time-1] = 0.0;

                    for(int i=now_time-2; i>=k; i--) {
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
                for(int i=0; i<nodes[current_node].tm_length; i++) {
                    if(nodes[current_node].tm[i].deadline > now_time) expired++;
                }

                tx_count += 1;
                measurements += nodes[current_node].tm_length;
            }
        }
    }

    printf("expired/total/tx count = %d/%d/%d\n", expired, measurements, tx_count);

    return;
}

