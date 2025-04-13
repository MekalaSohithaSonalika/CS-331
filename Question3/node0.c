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
} dt0;

void printdt0(struct distance_table *dtptr);

/* students to write the following two routines, and maybe some others */

void rtinit0() 
{
    int i, j;
    struct rtpkt pkt;

    // Initialize all entries to 999 (infinity)
    for(i = 0; i < 4; i++)
        for(j = 0; j < 4; j++)
            dt0.costs[i][j] = 999;

    // Initialize the known costs to neighbors
    dt0.costs[0][0] = 0;    // Cost to self is 0
    dt0.costs[1][1] = 1;    // Cost to node 1
    dt0.costs[2][2] = 3;    // Cost to node 2
    dt0.costs[3][3] = 7;    // Cost to node 3

    // Set minimum costs for direct paths
    dt0.costs[1][0] = 1;    // Cost to node 1 via direct path
    dt0.costs[2][0] = 3;    // Cost to node 2 via direct path
    dt0.costs[3][0] = 7;    // Cost to node 3 via direct path

    printf("At time t=%.3f, rtinit0() called\n", clocktime);
    printdt0(&dt0);

    // Send initial distance vector to neighbors
    // Create packet with minimum costs to each destination
    pkt.sourceid = 0;
    pkt.mincost[0] = 0;
    pkt.mincost[1] = dt0.costs[1][0];
    pkt.mincost[2] = dt0.costs[2][0];
    pkt.mincost[3] = dt0.costs[3][0];

    // Send to each neighbor
    pkt.destid = 1;
    tolayer2(pkt);
    pkt.destid = 2;
    tolayer2(pkt);
    pkt.destid = 3;
    tolayer2(pkt);
}


void rtupdate0(struct rtpkt *rcvdpkt)
{
    int i, j;
    int changed = 0;
    int sourceid = rcvdpkt->sourceid;
    
    printf("At time t=%.3f, rtupdate0() called, received vector from %d\n", clocktime, sourceid);

    // Update the costs table for the source node's column
    for(i = 0; i < 4; i++) {
        dt0.costs[i][sourceid] = dt0.costs[sourceid][sourceid] + rcvdpkt->mincost[i];
    }

    // Check if any minimum costs have changed
    for(i = 0; i < 4; i++) {
        int min_cost = 999;
        // Find minimum cost to node i through any neighbor
        for(j = 0; j < 4; j++) {
            if(dt0.costs[i][j] < min_cost) {
                min_cost = dt0.costs[i][j];
            }
        }
        // If minimum cost to any destination has changed
        if(min_cost != dt0.costs[i][0]) {
            changed = 1;
            dt0.costs[i][0] = min_cost;
        }
    }

    printdt0(&dt0);

    // If costs have changed, send updates to neighbors
    if(changed) {
        struct rtpkt pkt;
        pkt.sourceid = 0;
        for(i = 0; i < 4; i++) {
            pkt.mincost[i] = dt0.costs[i][0];
        }

        // Send to each neighbor
        pkt.destid = 1;
        tolayer2(pkt);
        pkt.destid = 2;
        tolayer2(pkt);
        pkt.destid = 3;
        tolayer2(pkt);
        
        printf("At time t=%.3f, node 0 sends updated vectors to neighbors\n", clocktime);
    }
}


void printdt0(struct distance_table *dtptr)
{
  printf("\n===========================================\n");
  printf("FINAL DISTANCE TABLE FOR NODE 0\n");
  printf("===========================================\n");
  printf("                via     \n");
  printf("   D0 |    1     2    3 \n");
  printf("  ----|-----------------\n");
  printf("     1|  %3d   %3d   %3d\n",dtptr->costs[1][1],
	 dtptr->costs[1][2],dtptr->costs[1][3]);
  printf("dest 2|  %3d   %3d   %3d\n",dtptr->costs[2][1],
	 dtptr->costs[2][2],dtptr->costs[2][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][1],
	 dtptr->costs[3][2],dtptr->costs[3][3]);
  printf("===========================================\n\n");
}

void linkhandler0(int linkid, int newcost)   
{
}
