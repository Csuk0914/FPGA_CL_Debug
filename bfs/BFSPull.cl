//#include "cl_common.cl"
typedef  int NodeData;

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
   g->_out_edge_data= & data[offset];
}

//Initialize
__kernel void initialize(__global uint * _g) {
   __local Graph gp; // = (__global Graph *)(_g);
   init(&gp, _g);
   __local Graph * g = &gp;
   uint src = get_global_id(0);
   if(src < g->_num_nodes) {
      __global NodeData * sdata = &g->_node_data[src];
      *sdata = INT_MAX/4;
      if(src==0){
         *sdata=0;
      }
   } //End if src < num_nodes
}

//SSSP - pull
__kernel void sssp_kernel(__global int * _g) {
   __local Graph gp; // = (__global Graph *)(_g);
   init(&gp, _g);
   __local Graph * g = &gp;
   uint src = get_global_id(0);
   if(src < g->_num_nodes) {
      __global NodeData * sdata = &g->_node_data[src];
      for(int nbr = g->_out_index[src]; nbr != g->_out_index[src+1]; ++nbr) {
         int dst = g->_out_neighbors[nbr];
         int wt = g->_out_edge_data[nbr];
         __global NodeData * ddata = &g->_node_data[dst];
         if(*sdata > *ddata + wt){
            *sdata = *ddata+wt;
         }//end if

      }//end for nbr
         } //End if src < num_nodes
}
