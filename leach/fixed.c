#include <stdio.h>
#include "policy.h"
#include "../include/common.h"

extern struct node nodes[NUMBER_OF_NODES];

extern int cluster_head[NUMBER_OF_CLUSTER*4];
extern int base_station;
extern int number_of_cluster;
extern int now_time;

void da_policy() {
    // for every node, maintain their policy model
    // how measurements are corporated in the policy

    // After cluster_build -> generate measurements* -> calculate score?
    //                                               -> 
    // generate measurements: size and deadline ((application?

    // as for fixed, send measurements every round
    // compare point: transmission count?
    
    printf("FIXED POLICY\n");

    int expired = 0;
    int measurements = 0;
    int tx_count = 0;

    for(int i=0; i<number_of_cluster; i++) {
        for(int j=0; j<nodes[cluster_head[i]].cluster_length; j++) {
            int current_node = nodes[cluster_head[i]].clusters[j];
            for(int k=0; k<nodes[current_node].tm_length; k++) {
                // calc how many measurements expired?
                // if not cluster head, deadline decrease by one
                if(nodes[current_node].tm[k].deadline < now_time) expired++;
                measurements++;
            }
        }

        for(int j=0; j<nodes[cluster_head[i]].tm_length; j++) {
            if(nodes[cluster_head[i]].tm[j].deadline < now_time) expired++;
            measurements++;
        }
    }

    for(int i=0; i<number_of_cluster; i++) {
        for(int j=0; j<nodes[cluster_head[i]].cluster_length; j++) {
            tx_count++;
        }
        tx_count++;
    }

    for(int i=0; i<NUMBER_OF_NODES; i++) nodes[i].tm_length = 0;

/*
    for(int i=0; i<number_of_cluster; i++) {
        for(int j=0; j<cluster_length[i]; j++) {
            node_energy[clusters[i][j]] -= SENSOR_DATA*etx(node_position[clusters[i][j]], node_position[cluster_head[i]]);
            node_energy[cluster_head[i]] -= SENSOR_DATA*erx();
            node_energy[cluster_head[i]] -= SENSOR_DATA*etx(node_position[cluster_head[i]], base_station);
        }
    }
*/

    printf("expired/total/tx count = %d/%d/%d\n", expired, measurements, tx_count);

    return;
}

