__kernel void test_atomic_different(__global int * location, int val){
   atomic_add(&location[get_global_id(0)], val);
//   location[get_global_id(0)]+= val;
}
