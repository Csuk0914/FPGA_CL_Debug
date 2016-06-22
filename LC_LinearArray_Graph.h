/*
 * LC_LinearArray_Graph.h
 *
 *  Created on: Feb 27, 2013
 *  Single array representation, has incoming and outgoing edges.
 *      Author: rashid
 */

#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <limits>

#ifdef _WIN32
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#else
#include <unistd.h>
#include <sys/mman.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#ifdef _WIN32
#  define le64toh(x) (x) // OSSwapLittleToHostInt64(x)
#  define le32toh(x) (x) //  OSSwapLittleToHostInt32(x)
#endif

#ifdef __APPLE__
#include <libkern/OSByteOrder.h>
#  define le64toh(x) (x) // OSSwapLittleToHostInt64(x)
#  define le32toh(x) (x) //  OSSwapLittleToHostInt32(x)
#elif __FreeBSD__
#  include <sys/endian.h>
#elif __linux__
#  include <endian.h>
#  ifndef le64toh
#    if __BYTE_ORDER == __LITTLE_ENDIAN
#      define le64toh(x) (x)
#      define le32toh(x) (x)
#    else
#      define le64toh(x) __bswap_64 (x)
#      define le32toh(x) __bswap_32 (x)
#    endif
#  endif
#endif

#ifndef LC_LinearArray_Graph_H_
#define LC_LinearArray_Graph_H_

namespace Galois {
namespace OpenCL {


template<typename GraphType>
unsigned inline readFromGR(GraphType & g, const char * file) {
   DEBUG_CODE(std::cout<<"Reading binary .gr file " << file << "\n";);
   std::ifstream cfile;
   cfile.open(file);

   // copied from GaloisCpp/trunk/src/FileGraph.h
   int masterFD = open(file, O_RDONLY);
   if (masterFD == -1) {
      printf("FileGraph::structureFromFile: unable to open %s.\n", file);
      abort();
   }

   struct stat buf;
   int f = fstat(masterFD, &buf);
   if (f == -1) {
      printf("FileGraph::structureFromFile: unable to stat %s.\n", file);
      abort();
   }
   size_t masterLength = buf.st_size;

   int _MAP_BASE = MAP_PRIVATE;
//#ifdef MAP_POPULATE
//  _MAP_BASE  |= MAP_POPULATE;
//#endif

   void* m = mmap(0, masterLength, PROT_READ, _MAP_BASE, masterFD, 0);
   if (m == MAP_FAILED ) {
      m = 0;
      printf("FileGraph::structureFromFile: mmap failed.\n");
      abort();
   }

   //parse file
   uint64_t* fptr = (uint64_t*) m;
   __attribute__((unused))      uint64_t version = le64toh(*fptr++);
   assert(version == 1);
   __attribute__((unused))    uint64_t sizeEdgeTy = le64toh(*fptr++);
   uint64_t numNodes = le64toh(*fptr++);
   uint64_t numEdges = le64toh(*fptr++);
   uint64_t *outIdx = fptr;
   fptr += numNodes;
   uint32_t *fptr32 = (uint32_t*) fptr;
   uint32_t *outs = fptr32;
   fptr32 += numEdges;
   if (numEdges % 2)
      fptr32 += 1;
   unsigned *edgeData = (unsigned *) fptr32;

   g._num_nodes = numNodes;
   g._num_edges = numEdges;
   DEBUG_CODE(std::cout<<"Sizes read from file :: Edge (" << sizeEdgeTy << ")\n";);
   g.init(g._num_nodes, g._num_edges);
   //node_data
   memset(g.node_data(), 0, sizeof(unsigned int) * g._num_nodes);
   for (unsigned int i = 0; i < g._num_edges; ++i) {
      g.out_neighbors()[i] = *le64toh(outs+i);
   }
   g.outgoing_index()[0] = 0;
   for (unsigned int i = 0; i < g._num_nodes; ++i) {
      g.outgoing_index()[i + 1] = *le64toh(outIdx+i);
   }
   unsigned int start = 0;
   unsigned int displacement = 0;
   for (unsigned int i = 0; i < g._num_nodes; ++i) {
      unsigned int end = *le64toh(outIdx+i);
      for (unsigned int idx = start; idx < end; ++idx) {
         //node i's idx neighbor is to be populated here.
         g.out_edge_data()[displacement] = *le64toh(edgeData+idx);
         g.out_neighbors()[displacement] = *le64toh(outs+idx);
         displacement++;
      }
      start = end;
   }
/*   for (size_t i = 0; i < g._num_nodes; ++i)
      g.node_data()[i] = std::numeric_limits<unsigned int>::max() / 2;*/
   cfile.close();
   g.update_in_neighbors();
   return 0;
}



static const char * _str_LC_LinearArray_Graph = "typedef struct _GraphType { \n"
      "uint _num_nodes;\n"
      "uint _num_edges;\n "
      "uint node_data_size;\n "
      "uint edge_data_size;\n "
      "__global uint *in_index;\n "
      "__global uint *in_neighbors;\n "
      "__global EdgeData *in_edge_data;\n "
      "__global NodeData*node_data;\n "
      "__global uint *out_index;\n "
      "__global uint *out_neighbors;\n "
      "__global EdgeData *out_edge_data;\n }GraphType;\n"
      "uint in_neighbors_begin(__local GraphType * graph, uint node){ \n return 0;\n}\n"
      "uint in_neighbors_end(__local GraphType * graph, uint node){ \n return graph->in_index[node+1]-graph->in_index[node];\n}\n"
      "uint in_neighbors_next(__local GraphType * graph, uint node){ \n return 1;\n}\n"
      "uint out_neighbors_begin(__local GraphType * graph, uint node){ \n return 0;\n}\n"
      "uint out_neighbors_end(__local GraphType * graph, uint node){ \n return graph->out_index[node+1]-graph->out_index[node];\n}\n"
      "uint out_neighbors_next(__local GraphType * graph, uint node){ \n return 1;\n}\n"
      "uint in_neighbors(__local GraphType * graph, uint node, uint nbr){ \n return graph->in_neighbors[graph->in_index[node]+nbr];\n}\n"
      "uint out_neighbors(__local GraphType * graph,uint node,  uint nbr){ \n return graph->out_neighbors[graph->out_index[node]+nbr];\n}\n"
      "__global NodeData * node_data(__local GraphType * graph, uint node){ \n return &graph->node_data[node];\n}\n"
      "__global EdgeData * in_edge_data(__local GraphType * graph, uint node, uint nbr){ \n return &graph->in_edge_data[graph->in_index[node]+nbr];\n}\n"
      "__global EdgeData * out_edge_data(__local GraphType * graph,uint node,  uint nbr){ \n return &graph->out_edge_data[graph->out_index[node]+nbr];\n}\n"
      "void initialize(__local GraphType * graph, __global uint *mem_pool){\nuint offset =4;\n graph->_num_nodes=mem_pool[0];\n"
      "graph->_num_edges=mem_pool[1];\n graph->node_data_size =mem_pool[2];\n graph->edge_data_size=mem_pool[3];\n"
      "graph->node_data= (__global NodeData *)&mem_pool[offset];\n offset +=graph->_num_nodes* graph->node_data_size;\n"
      "graph->out_index=&mem_pool[offset];\n offset +=graph->_num_nodes + 1;\n graph->out_neighbors=&mem_pool[offset];\n"
      "offset +=graph->_num_edges;\n"
      "graph->out_edge_data=(__global EdgeData*)&mem_pool[offset];\n"
      "offset +=graph->_num_edges*graph->edge_data_size;\n"
      "graph->in_index=& mem_pool[offset];\n offset +=graph->_num_nodes+1;\n graph->in_neighbors=& mem_pool[offset];\n offset +=graph->_num_edges;\n"
      "graph->in_edge_data=& mem_pool[offset];\n}\n";

template<typename NodeDataTy, typename EdgeDataTy>

struct LC_LinearArray_Graph {
   typedef Array<unsigned int> GPUType;
   typedef Array<unsigned int>::HostPtrType HostPtrType;
   typedef Array<unsigned int>::DevicePtrType DevicePtrType;
   typedef NodeDataTy NodeDataType;
   typedef EdgeDataTy EdgeDataType;
   typedef unsigned int NodeIDType;
   typedef unsigned int EdgeIDType;
   size_t _num_nodes;
   size_t _num_edges;
   unsigned int _max_degree;
   const size_t SizeEdgeData;
   const size_t SizeNodeData;
   GPUType * gpu_graph;
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
   LC_LinearArray_Graph() :
         SizeEdgeData(sizeof(EdgeDataType) / sizeof(unsigned int)), SizeNodeData(sizeof(NodeDataType) / sizeof(unsigned int)) {
      fprintf(stderr, "Created LC_LinearArray_Graph with %d node %d edge data. ", (int) SizeNodeData, (int) SizeEdgeData);
//      std::cout << "Created LC_LinearArray_Graph with " << SizeNodeData << " node " << SizeEdgeData << " edge data. ";
      _max_degree = _num_nodes = _num_edges = 0;
      gpu_graph = 0;
   }
   ~LC_LinearArray_Graph() {
      deallocate();
   }
   void read(const char * filename) {
      readFromGR(*this, filename);
   }
   size_t size() {
      return gpu_graph->size();
   }
   NodeDataType * node_data() {
      return (NodeDataType*) (gpu_graph->host_data + 4);
   }
   unsigned int * outgoing_index() {
      return (unsigned int*) (node_data()) + _num_nodes * SizeNodeData;
   }
   unsigned int num_neighbors(NodeIDType node) {
      return outgoing_index()[node + 1] - outgoing_index()[node];
   }
   unsigned int * out_neighbors() {
      return (unsigned int *) outgoing_index() + _num_nodes + 1;
   }
   unsigned int & out_neighbors(NodeIDType node, unsigned int idx) {
      return out_neighbors()[node + idx];
   }

   EdgeDataType * out_edge_data() {
      return (EdgeDataType *) (unsigned int *) (out_neighbors()) + _num_edges;
   }
   EdgeDataType &out_edge_data(NodeIDType node_id, int nbr_id) {
      return out_edge_data()[outgoing_index()[node_id] + nbr_id];
   }
   unsigned int * incoming_index() {
      return (unsigned int *) out_edge_data() + _num_edges * SizeEdgeData;
   }
   unsigned int * in_neighbors() {
      return (unsigned int *) incoming_index() + _num_nodes + 1;
   }
   EdgeDataType * in_edge_data() {
      return (EdgeDataType *) in_neighbors() + _num_edges;
   }
   unsigned int * last() {
      return (unsigned int *) in_edge_data() + _num_edges * SizeEdgeData;
   }
   GPUType * get_array_ptr(void) {
      return gpu_graph;
   }
   size_t num_nodes() {
      return _num_nodes;
   }
   size_t num_edges() {
      return _num_edges;
   }
   size_t max_degree() {
      return _max_degree;
   }
   void init(size_t n_n, size_t n_e) {
      _num_nodes = n_n;
      _num_edges = n_e;
      fprintf(stderr, "Allocating NN: :%d,  , NE %d :\n", (int) _num_nodes, (int) _num_edges);
//      std::cout << "Allocating NN: " << _num_nodes << " , NE :" << _num_edges << ". ";
      //Num_nodes, num_edges, [node_data] , [outgoing_index], [out_neighbors], [edge_data], [incoming_index] , [incoming_neighbors]
      gpu_graph = new GPUType(4 + _num_nodes * SizeNodeData + _num_nodes + 1 + _num_edges + _num_edges * SizeEdgeData + _num_edges + 1 + _num_edges + _num_edges * SizeEdgeData);
      (*gpu_graph)[0] = (int) _num_nodes;
      (*gpu_graph)[1] = (int) _num_edges;
      (*gpu_graph)[2] = (int) SizeNodeData;
      (*gpu_graph)[3] = (int) SizeEdgeData;
      //allocate_on_gpu();
   }
   /////////////////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////////////////
   void copy_to_device(void) {
      gpu_graph->copy_to_device();
   }
   /////////////////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////////////////
   void copy_to_host(void) {
      gpu_graph->copy_to_host();
   }
   DevicePtrType & device_ptr() {
      return gpu_graph->device_ptr();
   }
   HostPtrType & host_ptr(void) {
      return gpu_graph->host_ptr();
   }
   void print_header(void) {
      std::cout << "Header :: [";
      for (unsigned int i = 0; i < 6; ++i) {
         std::cout << gpu_graph->operator[](i) << ",";
      }
      std::cout << "\n";
      return;
   }
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
   void print_node(unsigned int idx, const char * post = "") {
      if (idx < _num_nodes) {
         std::cout << "N-" << idx << "(" << (node_data())[idx] << ")" << " :: [";
         for (size_t i = (outgoing_index())[idx]; i < (outgoing_index())[idx + 1]; ++i) {
            //std::cout << " " << (neighbors())[i] << "(" << (edge_data())[i] << "), ";
            std::cout << " " << (out_neighbors())[i] << "(" << (out_edge_data())[i] << "<" << node_data()[out_neighbors()[i]] << ">" << "), ";
         }
         std::cout << "]##[";
         for (size_t i = (incoming_index())[idx]; i < (incoming_index())[idx + 1]; ++i) {
            //std::cout << " " << (in_neighbors())[i] << "(" << (in_edge_data())[i] << "), ";
            std::cout << " " << (in_neighbors())[i] << "(" << (in_edge_data())[i] << "<" << node_data()[in_neighbors()[i]] << ">" << "), ";
         }
         std::cout << "]" << post;
      }
      return;
   }
   void print_node_nobuff(unsigned int idx, const char * post = "") {
      if (idx < _num_nodes) {
         fprintf(stderr, "N-%d(%d)::[", idx, (node_data())[idx]);
         for (size_t i = (outgoing_index())[idx]; i < (outgoing_index())[idx + 1]; ++i) {
            //std::cout << " " << (neighbors())[i] << "(" << (edge_data())[i] << "), ";
            fprintf(stderr, "%d ( < %d(%d) > ),  ", (out_neighbors())[i], (out_edge_data())[i], node_data()[out_neighbors()[i]]);
         }
         fprintf(stderr, "]##[");
         for (size_t i = (incoming_index())[idx]; i < (incoming_index())[idx + 1]; ++i) {
            //std::cout << " " << (in_neighbors())[i] << "(" << (in_edge_data())[i] << "), ";
            fprintf(stderr, "%d( %d <%d> ), ", (in_neighbors())[i], (in_edge_data())[i], node_data()[in_neighbors()[i]]);
         }
         fprintf(stderr, "]%s", post);
      }
      return;
   }

   static const char * get_graph_decl(std::string &res) {
      res.append(_str_LC_LinearArray_Graph);
      return _str_LC_LinearArray_Graph;
   }
   /////////////////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////////////////
   void allocate_on_gpu() {
      return;
   }
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
   void print_graph(void) {
      std::cout << "\n====Printing graph (" << _num_nodes << " , " << _num_edges << ")=====\n";
      for (size_t i = 0; i < _num_nodes; ++i) {
         print_node(i);
         std::cout << "\n";
      }
      return;
   }

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
   void print_graph_nobuff(void) {
      fprintf(stderr, "\n====Printing graph (%d ,%d )=====\n", (int) _num_nodes, (int) _num_edges);
      for (size_t i = 0; i < _num_nodes; ++i) {
         print_node_nobuff(i, "\n");
      }
      return;
   }

   /////////////////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////////////////
   void update_in_neighbors(void) {
      typedef std::pair<unsigned int, unsigned int> EDType;
      typedef std::vector<EDType> EdgeList;
      _max_degree = 0;
      EdgeDataType _max_weight = 0;
      std::vector<EdgeList> in_edges(_num_nodes);
      for (unsigned int node = 0; node < _num_nodes; ++node) {
         for (unsigned int nbr = outgoing_index()[node]; nbr < outgoing_index()[node + 1]; ++nbr) {
            unsigned int nbr_id = out_neighbors()[nbr];
            _max_weight = std::max(_max_weight, out_edge_data()[nbr]);
            in_edges[nbr_id].push_back(EDType(node, out_edge_data()[nbr]));
         }
         _max_degree = std::max(_max_degree, outgoing_index()[node + 1] - outgoing_index()[node]);
      }
      unsigned int count = 0;
      for (unsigned int node = 0; node < _num_nodes; ++node) {
         incoming_index()[node] = count;
         for (EdgeList::iterator it = in_edges[node].begin(); it != in_edges[node].end(); ++it) {
            in_neighbors()[count] = (*it).first;
            in_edge_data()[count] = (*it).second;
            count++;
         }
      }
      incoming_index()[_num_nodes] = count;
//      std::cout << "MaxDegree[" << _max_degree << "]. , Max weight:: " << _max_weight << " ";
      fprintf(stderr, "MaxDegree[%d]. , Max weight:: %d ", (int) _max_degree, (int) _max_weight);
      ;
   }
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
   void print_compact(void) {
      std::cout << "\nOut-index [";
      for (size_t i = 0; i < _num_nodes + 1; ++i) {
         std::cout << " " << outgoing_index()[i] << ",";
      }
      std::cout << "]\nNeigh[";
      for (size_t i = 0; i < _num_edges; ++i) {
         std::cout << " " << out_neighbors()[i] << ",";
      }
      std::cout << "]\nEData [";
      for (size_t i = 0; i < _num_edges; ++i) {
         std::cout << " " << out_edge_data()[i] << ",";
      }
      std::cout << "\n]In-index [";
      for (size_t i = 0; i < _num_nodes + 1; ++i) {
         std::cout << " " << incoming_index()[i] << ",";
      }
      std::cout << "]\nIn-Neigh[";
      for (size_t i = 0; i < _num_edges; ++i) {
         std::cout << " " << in_neighbors()[i] << ",";
      }
      std::cout << "]\nIn-EData [";
      for (size_t i = 0; i < _num_edges; ++i) {
         std::cout << " " << in_edge_data()[i] << ",";
      }
      std::cout << "]";
   }
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
   //TODO : Fix this to correctly read in-out neighbors.
   void read_graph(const char * filename) {
      std::ifstream in_file;
      in_file.open(filename);
      std::string buffer;
      std::pair<int, int> e_buffer;
      getline(in_file, buffer);
      while (DIMACS_GR_Challenge9_Format<unsigned int, unsigned int>::is_comment(buffer)) {
         getline(in_file, buffer);
      }
      e_buffer = DIMACS_GR_Challenge9_Format<unsigned int, unsigned int>::parse_header(buffer);
      //Read in number of nodes and vertices.
      //std::cout << "LC_LinearArray_Graph :: Reading .gr file " << filename << "\n";
      //std::cout << "Read header line " << buffer << " \n";
      //Undirected edges = twice directed edges!
      // Now the tricky part. We need to accumulate the edge-count for each
      // node, and then use those in a sorted manner to palce edges in the
      // neighbors and edge_data array.
      typedef std::map<unsigned int, unsigned int> EdgeListBuffer;
      typedef std::map<unsigned int, EdgeListBuffer *> GraphBuffer;
      GraphBuffer all_edges;
      size_t num_nodes_l = e_buffer.first + 1, num_edges_l = 0;
      //std::cout << "Number of nodes " << num_nodes_l << "\n";
      for (size_t i = 0; i < num_nodes_l; ++i) {
         all_edges[i] = new EdgeListBuffer();
      }
      std::pair<unsigned int, std::pair<unsigned int, unsigned int> > e_vec_buffer;
      do {
         getline(in_file, buffer);
         if (buffer.size() > 0 && DIMACS_GR_Challenge9_Format<unsigned int, unsigned int>::is_comment(buffer) == false) {
            e_vec_buffer = DIMACS_GR_Challenge9_Format<unsigned int, unsigned int>::parse_edge_pair(buffer);
            (*all_edges[e_vec_buffer.first])[e_vec_buffer.second.first] = e_vec_buffer.second.second;
            (*all_edges[e_vec_buffer.second.first])[e_vec_buffer.first] = e_vec_buffer.second.second;
            num_edges_l += 2;
         }
      } while (in_file.good());
      num_edges_l = 0;
      for (GraphBuffer::iterator it = all_edges.begin(); it != all_edges.end(); ++it) {
         num_edges_l += it->second->size();
      }
      ////Now we are going to populate the edges.
      //std::cout << "Number of edges" << num_edges_l << "\n";
      init(num_nodes_l, num_edges_l);
      size_t curr_edge_count = 0;
      size_t l_max_degree = 0;
      for (size_t i = 0; i < _num_nodes; ++i) {
         outgoing_index()[(i)] = curr_edge_count;
         node_data()[(i)] = (std::numeric_limits<unsigned int>::max()) / 2;
         if (all_edges[i]->size() > l_max_degree)
            l_max_degree = all_edges[i]->size();
         for (EdgeListBuffer::iterator it = all_edges[i]->begin(); it != all_edges[i]->end(); ++it) {
            out_neighbors()[curr_edge_count] = it->first;
            out_edge_data()[curr_edge_count] = it->second;
            ++curr_edge_count;
         }
      }
      //std::cout << "Max degree enountered is " << l_max_degree << "\n";
      outgoing_index()[_num_nodes] = _num_edges;
      //assert(num_edges == curr_edge_count);
      ///Clean-up : VERY VERY SLOW!!!!
      for (size_t i = 0; i < _num_nodes; ++i) {
         delete all_edges[i];
      }
   }
   ////////////##############################################################///////////
   ////////////##############################################################///////////
   unsigned int verify() {
      NodeDataTy * t_node_data = node_data();
      unsigned int * t_incoming_index = outgoing_index();
      unsigned int * t_neighbors = out_neighbors();
      EdgeDataType * t_in_edge_data = out_edge_data();
      unsigned int err_count = 0;
      for (unsigned int node_id = 0; node_id < _num_nodes; ++node_id) {
         NodeDataTy curr_distance = t_node_data[node_id];
         //Go over all the neighbors.
         for (unsigned int idx = t_incoming_index[node_id]; idx < t_incoming_index[node_id + 1]; ++idx) {
            NodeDataTy temp = t_node_data[t_neighbors[idx]];
            if (curr_distance + t_in_edge_data[idx] < temp) {
               if (err_count < 10) {
                  std::cout << "Error :: ";
                  print_node(node_id);
                  std::cout << " With :: ";
                  print_node(t_neighbors[idx]);
                  std::cout << "\n";
               }
               err_count++;
            }
         }
      } //End for
      return err_count;
   }
   ////////////##############################################################///////////
   ////////////##############################################################///////////
   unsigned int verify_in() {
      unsigned int * t_node_data = node_data();
      unsigned int * t_incoming_index = incoming_index();
      unsigned int * t_neighbors = in_neighbors();
      unsigned int * t_in_edge_data = in_edge_data();
      unsigned int err_count = 0;
      for (unsigned int node_id = 0; node_id < _num_nodes; ++node_id) {
         unsigned int curr_distance = t_node_data[node_id];
         //Go over all the neighbors.
         for (unsigned int idx = t_incoming_index[node_id]; idx < t_incoming_index[node_id + 1]; ++idx) {
            unsigned int temp = t_node_data[t_neighbors[idx]] + t_in_edge_data[idx];
            if (curr_distance > temp) {
               if (err_count < 10) {
                  std::cout << "Error :: ";
                  print_node(node_id);
                  std::cout << " With :: ";
                  print_node(t_neighbors[idx]);
                  std::cout << "\n";
               }
               err_count++;
            }
         }
      } //End for
      return err_count;
   }
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
   void deallocate(void) {
      delete gpu_graph;
   }
};
//End LC_Graph
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
}
} //End namespaces
#endif /* LC_LinearArray_Graph_H_ */
