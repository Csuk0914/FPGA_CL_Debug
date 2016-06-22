#include "cl_common.cl"
typedef struct _NodeData {
   int nout;
   float value;
} NodeData;

typedef struct _GraphType {
   uint _num_nodes;
   uint _num_edges;
   uint _node_data_size;
   uint _edge_data_size;
   __global NodeData *_node_data;
   __global uint *_out_index;
   __global uint *_out_neighbors;
   __global uint *_out_edge_data;
} Graph;

#define ALPHA 0.15f
#define TOLERANCE 0.1f

void init(__local Graph * g, __global uint * data) {
   g->_num_nodes = data[0];
   g->_num_edges = data[1];
   g->_node_data_size = data[2];
   g->_edge_data_size = data[3];
   g->_node_data= (__global NodeData *) &data[4];
   int offset = 4 + (g->_num_nodes * g->_node_data_size);
   g->_out_index = &data[offset];
   offset += g->_num_nodes+1;
   g->_out_neighbors = & data[offset];
   offset += g->_num_edges;

}

//Initialize
__kernel void initialize(__global uint * _g) {
   __local Graph gp; // = (__global Graph *)(_g);
   init(&gp, _g);
   __local Graph * g = &gp;
   uint src = get_global_id(0);
   if(src < g->_num_nodes) {
      __global NodeData * sdata = &g->_node_data[src];
      sdata->value = 1.0 - ALPHA;
      //for(int nbr = g->_out_index[src]; nbr != g->_out_index[src+1]; ++nbr) {
      //   int dst = g->_out_neighbors[nbr];
      //   __global NodeData * ddata = &g->_node_data[dst];
      //   atomic_add(&ddata->nout, 1);
      // }
   } //End if src < num_nodes
}

//PageRank - pull
__kernel void pageRank(__global int * _g) {
   __local Graph gp; // = (__global Graph *)(_g);
   init(&gp, _g);
   __local Graph * g = &gp;
   uint src = get_global_id(0);
   if(src < g->_num_nodes) {
      __global NodeData * sdata = &g->_node_data[src];
      float sum = 0;
     // for(int nbr = g->_out_index[src]; nbr != g->_out_index[src+1]; ++nbr) {
     //    int dst = g->_out_neighbors[nbr];
     //    __global NodeData * ddata = &g->_node_data[dst];
     //    unsigned dnout = ddata->nout;
     //    if(ddata->nout > 0) {
     //       sum += ddata->value/dnout;
     //    }
     // }
      float pr_value = sum*(1.0 - ALPHA) + ALPHA;
      float diff = fabs(pr_value - sdata->value);
      //if(diff > TOLERANCE)
      {
         sdata->value = pr_value;
      }

   } //End if src < num_nodes

}
