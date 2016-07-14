__kernel void test_add_different_uncoal(__global int * location,__global int  * addr,  int val){
//   atomic_add(&location[addr[get_global_id(0)]],val);
      location[addr[get_global_id(0)]]+= val;
}
