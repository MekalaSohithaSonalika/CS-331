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

int connectcosts1[4] = { 1,  0,  1, 999 };

struct distance_table 
{
  int costs[4][4];
} dt1;

void printdt1(struct distance_table *dtptr);

/* students to write the following two routines, and maybe some others */

void rtinit1() 
{
    int i, j;
    struct rtpkt pkt;

    // Initialize all entries to 999 (infinity)
    for(i = 0; i < 4; i++)
        for(j = 0; j < 4; j++)
            dt1.costs[i][j] = 999;

    // Initialize the known costs to neighbors
    dt1.costs[0][0] = 1;    // Cost to node 0
    dt1.costs[1][1] = 0;    // Cost to self is 0
    dt1.costs[2][2] = 1;    // Cost to node 2
    dt1.costs[3][3] = 999;  // No direct connection to node 3

    // Set minimum costs for direct paths
    dt1.costs[0][0] = 1;    // Cost to node 0 via direct path
    dt1.costs[1][0] = 0;    // Cost to self
    dt1.costs[2][2] = 1;    // Cost to node 2 via direct path
    dt1.costs[3][0] = 999;  // No known path to node 3 yet

    printf("At time t=%.3f, rtinit1() called\n", clocktime);
    printdt1(&dt1);

    // Send initial distance vector to neighbors
    pkt.sourceid = 1;
    pkt.mincost[0] = dt1.costs[0][0];
    pkt.mincost[1] = 0;
    pkt.mincost[2] = dt1.costs[2][2];
    pkt.mincost[3] = dt1.costs[3][0];

    // Send to neighbors (nodes 0 and 2)
    pkt.destid = 0;
    tolayer2(pkt);
    pkt.destid = 2;
    tolayer2(pkt);
}


void rtupdate1(struct rtpkt *rcvdpkt)
{
    int i, j;
    int changed = 0;
    int sourceid = rcvdpkt->sourceid;
    
    printf("At time t=%.3f, rtupdate1() called, received vector from %d\n", clocktime, sourceid);

    // Update the costs table for the source node's column
    for(i = 0; i < 4; i++) {
        if(sourceid == 0) {
            dt1.costs[i][0] = connectcosts1[0] + rcvdpkt->mincost[i];
        } else if(sourceid == 2) {
            dt1.costs[i][2] = connectcosts1[2] + rcvdpkt->mincost[i];
        }
    }

    // Check if any minimum costs have changed
    for(i = 0; i < 4; i++) {
        int min_cost = 999;
        // Find minimum cost to node i through any neighbor
        for(j = 0; j < 4; j++) {
            if(dt1.costs[i][j] < min_cost) {
                min_cost = dt1.costs[i][j];
            }
        }
        // If minimum cost to any destination has changed
        if(min_cost != dt1.costs[i][1]) {
            changed = 1;
            dt1.costs[i][1] = min_cost;
        }
    }

    printdt1(&dt1);

    // If costs have changed, send updates to neighbors
    if(changed) {
        struct rtpkt pkt;
        pkt.sourceid = 1;
        for(i = 0; i < 4; i++) {
            pkt.mincost[i] = dt1.costs[i][1];
        }

        // Send to neighbors (nodes 0 and 2)
        pkt.destid = 0;
        tolayer2(pkt);
        pkt.destid = 2;
        tolayer2(pkt);
        
        printf("At time t=%.3f, node 1 sends updated vectors to neighbors\n", clocktime);
    }
}


void printdt1(struct distance_table *dtptr)
{
  printf("\n===========================================\n");
  printf("FINAL DISTANCE TABLE FOR NODE 1\n");
  printf("===========================================\n");
  printf("                via     \n");
  printf("   D1 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);
  printf("     3|  %3d   %3d\n",dtptr->costs[3][0], dtptr->costs[3][2]);
  printf("===========================================\n\n");
}

void linkhandler1(int linkid, int newcost)   
{
}

