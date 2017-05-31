#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "../mt64.h"
#include "../include/common.h"
#include "policy.h"

struct node nodes[NUMBER_OF_NODES];

int cluster_head[NUMBER_OF_CLUSTER*4];
int base_station;
int number_of_cluster;
int now_time;

// Build cluster by choosing the closest one

void cluster_build() {
    if(number_of_cluster == 0) return;
    for(int i=0; i<NUMBER_OF_NODES; i++)
        nodes[i].cluster_length = 0;
    for(int i=0; i<NUMBER_OF_NODES; i++) {
        int dist, min_dist=2147483647;
        int min_cluster = 0;

        // nodes out of energy are considered dead
        if(nodes[i].energy < 0.1) continue;
        for(int j=0; j<number_of_cluster; j++) {
            if(cluster_head[j] < 0) continue;
            int dx = GET_X(nodes[i].position) - GET_X(nodes[cluster_head[j]].position);
            int dy = GET_Y(nodes[i].position) - GET_Y(nodes[cluster_head[j]].position);

            dist = dx*dx + dy*dy;
            if(dist < min_dist) {
                min_dist = dist;
                min_cluster = cluster_head[j];
            }
        }
        if(nodes[i].clusterHead < 0) continue;
        nodes[min_cluster].clusters[nodes[min_cluster].cluster_length] = i;
        nodes[min_cluster].cluster_length++;
        nodes[i].clusterHead = min_cluster;
    }

    return;
}


// radio model

double etx(int src, int dest) {
    double e_elec = 5e-8;
    double e_tr = 1e-10;
    double d;
    int dx = GET_X(src) - GET_X(dest);
    int dy = GET_Y(src) - GET_Y(dest);

    d = dx*dx+dy*dy;

    return e_elec + e_tr*d;
}

double erx() {
    double e_elec = 5e-8;

    return e_elec;
}

void decrease_energy() {
    for(int i=0; i<number_of_cluster; i++) {
        for(int j=0; j<nodes[cluster_head[i]].cluster_length; j++) {
            // src id: clusters[i][j], dest id: cluster_head[i]
            int child = nodes[cluster_head[i]].clusters[j];
            nodes[child].energy -= SENSOR_DATA * etx(nodes[child].position, nodes[cluster_head[i]].position);
            nodes[cluster_head[i]].energy -= SENSOR_DATA * erx();
//            node_energy[cluster_head[i]] -= SENSOR_DATA * AGGREGATION_RATE * etx(node_position[cluster_head[i]], base_station);
        }
        nodes[cluster_head[i]].energy -= SENSOR_DATA * etx(nodes[cluster_head[i]].position, base_station);
    }
}

void debug_info() {
    printf("cluster heads:");
    for(int i=0; i<number_of_cluster; i++) {
        printf(" %d", cluster_head[i]);
    }
    printf("\n");

    for(int i=0; i<number_of_cluster; i++) {
        printf("%d (%d):", cluster_head[i], nodes[cluster_head[i]].cluster_length);
        for(int j=0; j<nodes[cluster_head[i]].cluster_length; j++) {
            printf(" %d", nodes[cluster_head[i]].clusters[j]);
        }
        printf("\n");
    }
}

void generate_measurements() {
    for(int i=0; i<NUMBER_OF_NODES; i++) {
        int tmlength = nodes[i].tm_length;
        nodes[i].tm[tmlength].deadline = 10+now_time;
        nodes[i].tm[tmlength].N = 10+now_time;
        nodes[i].tm[tmlength].size = SENSOR_DATA;
        nodes[i].tm_length++;
        nodes[i].self_gen++;
        nodes[i].states[now_time] = nodes[i].tm_length;
    }
}

int main(int argc, char* argv[]) {
    int cluster_dist[NUMBER_OF_CLUSTER*4];
    int alive_count, temp1, temp2;
    FILE* nodefile = fopen(argv[1], "r");
    double p = (double)NUMBER_OF_CLUSTER/(double)NUMBER_OF_NODES;

    init_genrand64(time(0));

    if(nodefile == NULL) {
        printf("%s not found!\n", argv[1]);
        return 0;
    }

    // initialize energy and position
    fscanf(nodefile, "%d", &base_station);
    fscanf(nodefile, "%d", &temp2);
    base_station = (base_station<<MASK_BIT) | temp2;
    for(int i=0; i<NUMBER_OF_NODES; i++) {
        fscanf(nodefile, "%d", &temp1);
        fscanf(nodefile, "%d", &temp2);
        nodes[i].position = (temp1<<MASK_BIT) | temp2;
    }
    for(int i=0; i<NUMBER_OF_NODES; i++)
        nodes[i].energy = 1.0;
    for(int i=0; i<NUMBER_OF_CLUSTER*4; i++)
        cluster_head[i] = -1;
    for(int i=0; i<NUMBER_OF_NODES; i++)
        nodes[i].tm_length = 0;

//    printf("base_station_x = %d\n", GET_X(base_station));
//    printf("base_station_y = %d\n", GET_Y(base_station));

    // start LEACH algorithm
    for(now_time=0; now_time<ROUND; now_time++) {
        double energy_left = 0.0;
        number_of_cluster = 0;

        if(now_time%(NUMBER_OF_NODES/NUMBER_OF_CLUSTER) == 0)
            for(int i=0; i<NUMBER_OF_NODES; i++)
                nodes[i].is_clustered = 0;

        // select next-now_time CH
        for(int i=0; i<NUMBER_OF_NODES; i++) {
            double prob = genrand64_real1();
            double t = p/(1-p*(now_time%(NUMBER_OF_NODES/NUMBER_OF_CLUSTER)));

            if(prob <= t && !nodes[i].is_clustered && nodes[i].energy > 0.1) {
                cluster_head[number_of_cluster++] = i;
                nodes[i].clusterHead = -1;
                nodes[i].is_clustered = 1;
            }
            else
                nodes[i].clusterHead = 0;
        }

        cluster_build();
//        debug_info();
//        decrease_energy();
        generate_measurements();
        da_policy();

        alive_count = 0;
//        printf("Node energy, Round %d\n", now_time);
        for(int i=0; i<NUMBER_OF_NODES; i++) {
//            printf("%d: %f\n", i, node_energy[i]);
            energy_left += nodes[i].energy;
            if(nodes[i].energy > 0.1) alive_count++;
        }
    }

    return 0;
}

