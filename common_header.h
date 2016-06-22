/*
 * common_header.h
 *
 *  Created on: Feb 7, 2013
 *      Author: rashid
 */

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
extern "C" {
#include "CL/cl.h"
}
;
#endif

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <limits>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <process.h>
#include <time.h>
#include <Psapi.h>
#else
#include <sys/time.h>
#endif

#ifndef COMMON_HEADER_H_
#define COMMON_HEADER_H_


namespace Galois {
namespace OpenCL {

#ifdef _GOPT_DEBUG
#define DEBUG_CODE(X) {X}
#define INFO_CODE(X) {X}
#else
#define DEBUG_CODE(X) {}
#define INFO_CODE(X) {}
#endif

//long long inline get_time_in_microseconds() {
//   struct timeval tv;
//   gettimeofday(&tv, NULL);
//   return tv.tv_sec * 1000000 + tv.tv_usec;
//}
///////////////////////////////////////////////////////////////////////////////////
/*
 * A simple timer class, use this to make measurements so they are easily comparable.
 * */


///////////////////////////////////////////////////////////////////////////////////
template<typename EType, typename NType>
class DIMACS_GR_Challenge9_Format {
public:
   static bool is_comment(std::string & str) {
      return str.c_str()[0] == 'c';
   }
   static std::pair<size_t, size_t> parse_header(std::string & s) {
      if (s.c_str()[0] == 'p') {
         char buff[256];
         strcpy(buff, s.substr(1, s.size()).c_str());
         char * tok = strtok(buff, " ");
         tok = strtok(NULL, " "); // Ignore problem name for challenge9 formats.
         size_t num_nodes = atoi(tok) - 1; // edges start from zero.
         tok = strtok(NULL, " ");
         size_t num_edges = atoi(tok);
         return std::pair<size_t, size_t>(num_nodes, num_edges);
      }
      return std::pair<size_t, size_t>((size_t) -1, (size_t) -1);
   }
   static std::pair<NType, std::pair<NType, EType> > parse_edge_pair(std::string & s) {
      if (s.c_str()[0] == 'a') {
         char buff[256];
         strcpy(buff, s.substr(1, s.size()).c_str());
         char * tok = strtok(buff, " ");
         NType src_node = atoi(tok) - 1;
         tok = strtok(NULL, " ");
         NType dst_node = atoi(tok) - 1;
         tok = strtok(NULL, " ");
         EType edge_wt = atoi(tok);
         return std::pair<NType, std::pair<NType, EType> >(src_node, std::pair<NType, EType>(dst_node, edge_wt));
      }
      return std::pair<NType, std::pair<NType, EType> >(-1, std::pair<NType, EType>(-1, -1));
   }
};

/////////////////////////////
}
}  //End namespaces

#endif /* COMMON_HEADER_H_ */
