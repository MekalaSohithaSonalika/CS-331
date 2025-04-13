#include <stdio.h>
#include <stdlib.h>

/* Include the struct definition */
struct rtpkt {
  int sourceid;       /* id of sending router sending this pkt */
  int destid;         /* id of router to which pkt being sent 
                         (must be an immediate neighbor) */
  int mincost[4];    /* min cost to node 0 ... 3 */
};

extern int TRACE;
extern int YES;
extern int NO;
extern float clocktime;

void tolayer2(struct rtpkt packet);

struct distance_table 
{
  int costs[4][4];
} dt3;

void printdt3(struct distance_table *dtptr);

/* students to write the following two routines, and maybe some others */

void rtinit3() 
{
    int i, j;
    struct rtpkt pkt;

    // Initialize all entries to 999 (infinity)
    for(i = 0; i < 4; i++)
        for(j = 0; j < 4; j++)
            dt3.costs[i][j] = 999;

    // Initialize the known costs to neighbors
    dt3.costs[0][0] = 7;    // Cost to node 0
    dt3.costs[1][1] = 999;  // No direct connection to node 1
    dt3.costs[2][2] = 2;    // Cost to node 2
    dt3.costs[3][3] = 0;    // Cost to self is 0

    // Set minimum costs for direct paths
    dt3.costs[0][0] = 7;    // Cost to node 0 via direct path
    dt3.costs[1][0] = 999;  // No known path to node 1 yet
    dt3.costs[2][2] = 2;    // Cost to node 2 via direct path
    dt3.costs[3][3] = 0;    // Cost to self

    printf("At time t=%.3f, rtinit3() called\n", clocktime);
    printdt3(&dt3);

    // Send initial distance vector to neighbors
    pkt.sourceid = 3;
    pkt.mincost[0] = dt3.costs[0][0];
    pkt.mincost[1] = dt3.costs[1][0];
    pkt.mincost[2] = dt3.costs[2][2];
    pkt.mincost[3] = 0;

    // Send to neighbors (nodes 0 and 2)
    pkt.destid = 0;
    tolayer2(pkt);
    pkt.destid = 2;
    tolayer2(pkt);
}


void rtupdate3(struct rtpkt *rcvdpkt)
{
    int i, j;
    int changed = 0;
    int sourceid = rcvdpkt->sourceid;
    
    printf("At time t=%.3f, rtupdate3() called, received vector from %d\n", clocktime, sourceid);

    // Update the costs table for the source node's column
    for(i = 0; i < 4; i++) {
        if(sourceid == 0) {
            dt3.costs[i][0] = dt3.costs[0][0] + rcvdpkt->mincost[i];
        } else if(sourceid == 2) {
            dt3.costs[i][2] = dt3.costs[2][2] + rcvdpkt->mincost[i];
        }
    }

    // Check if any minimum costs have changed
    for(i = 0; i < 4; i++) {
        int min_cost = 999;
        // Find minimum cost to node i through any neighbor
        for(j = 0; j < 4; j++) {
            if(dt3.costs[i][j] < min_cost) {
                min_cost = dt3.costs[i][j];
            }
        }
        // If minimum cost to any destination has changed
        if(min_cost != dt3.costs[i][3]) {
            changed = 1;
            dt3.costs[i][3] = min_cost;
        }
    }

    printdt3(&dt3);

    // If costs have changed, send updates to neighbors
    if(changed) {
        struct rtpkt pkt;
        pkt.sourceid = 3;
        for(i = 0; i < 4; i++) {
            pkt.mincost[i] = dt3.costs[i][3];
        }

        // Send to neighbors (nodes 0 and 2)
        pkt.destid = 0;
        tolayer2(pkt);
        pkt.destid = 2;
        tolayer2(pkt);
        
        printf("At time t=%.3f, node 3 sends updated vectors to neighbors\n", clocktime);
    }
}


void printdt3(struct distance_table *dtptr)
{
  printf("\n===========================================\n");
  printf("FINAL DISTANCE TABLE FOR NODE 3\n");
  printf("===========================================\n");
  printf("                via     \n");
  printf("   D3 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 1|  %3d   %3d\n",dtptr->costs[1][0], dtptr->costs[1][2]);
  printf("     2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);
  printf("===========================================\n\n");
}






