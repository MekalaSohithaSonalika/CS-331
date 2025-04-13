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
} dt2;

void printdt2(struct distance_table *dtptr);

/* students to write the following two routines, and maybe some others */

void rtinit2() 
{
    int i, j;
    struct rtpkt pkt;

    // Initialize all entries to 999 (infinity)
    for(i = 0; i < 4; i++)
        for(j = 0; j < 4; j++)
            dt2.costs[i][j] = 999;

    // Initialize the known costs to neighbors
    dt2.costs[0][0] = 3;    // Cost to node 0
    dt2.costs[1][1] = 1;    // Cost to node 1
    dt2.costs[2][2] = 0;    // Cost to self is 0
    dt2.costs[3][3] = 2;    // Cost to node 3

    // Set minimum costs for direct paths
    dt2.costs[0][0] = 3;    // Cost to node 0 via direct path
    dt2.costs[1][1] = 1;    // Cost to node 1 via direct path
    dt2.costs[2][2] = 0;    // Cost to self
    dt2.costs[3][3] = 2;    // Cost to node 3 via direct path

    printf("At time t=%.3f, rtinit2() called\n", clocktime);
    printdt2(&dt2);

    // Send initial distance vector to neighbors
    pkt.sourceid = 2;
    pkt.mincost[0] = dt2.costs[0][0];
    pkt.mincost[1] = dt2.costs[1][1];
    pkt.mincost[2] = 0;
    pkt.mincost[3] = dt2.costs[3][3];

    // Send to neighbors (nodes 0, 1, and 3)
    pkt.destid = 0;
    tolayer2(pkt);
    pkt.destid = 1;
    tolayer2(pkt);
    pkt.destid = 3;
    tolayer2(pkt);
}


void rtupdate2(struct rtpkt *rcvdpkt)
{
    int i, j;
    int changed = 0;
    int sourceid = rcvdpkt->sourceid;
    
    printf("At time t=%.3f, rtupdate2() called, received vector from %d\n", clocktime, sourceid);

    // Update the costs table for the source node's column
    for(i = 0; i < 4; i++) {
        if(sourceid == 0) {
            dt2.costs[i][0] = dt2.costs[0][0] + rcvdpkt->mincost[i];
        } else if(sourceid == 1) {
            dt2.costs[i][1] = dt2.costs[1][1] + rcvdpkt->mincost[i];
        } else if(sourceid == 3) {
            dt2.costs[i][3] = dt2.costs[3][3] + rcvdpkt->mincost[i];
        }
    }

    // Check if any minimum costs have changed
    for(i = 0; i < 4; i++) {
        int min_cost = 999;
        // Find minimum cost to node i through any neighbor
        for(j = 0; j < 4; j++) {
            if(dt2.costs[i][j] < min_cost) {
                min_cost = dt2.costs[i][j];
            }
        }
        // If minimum cost to any destination has changed
        if(min_cost != dt2.costs[i][2]) {
            changed = 1;
            dt2.costs[i][2] = min_cost;
        }
    }

    printdt2(&dt2);

    // If costs have changed, send updates to neighbors
    if(changed) {
        struct rtpkt pkt;
        pkt.sourceid = 2;
        for(i = 0; i < 4; i++) {
            pkt.mincost[i] = dt2.costs[i][2];
        }

        // Send to neighbors (nodes 0, 1, and 3)
        pkt.destid = 0;
        tolayer2(pkt);
        pkt.destid = 1;
        tolayer2(pkt);
        pkt.destid = 3;
        tolayer2(pkt);
        
        printf("At time t=%.3f, node 2 sends updated vectors to neighbors\n", clocktime);
    }
}


void printdt2(struct distance_table *dtptr)
{
  printf("\n===========================================\n");
  printf("FINAL DISTANCE TABLE FOR NODE 2\n");
  printf("===========================================\n");
  printf("                via     \n");
  printf("   D2 |    0     1    3 \n");
  printf("  ----|-----------------\n");
  printf("     0|  %3d   %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][1],
	 dtptr->costs[0][3]);
  printf("dest 1|  %3d   %3d   %3d\n",dtptr->costs[1][0], dtptr->costs[1][1],
	 dtptr->costs[1][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][0], dtptr->costs[3][1],
	 dtptr->costs[3][3]);
  printf("===========================================\n\n");
}






