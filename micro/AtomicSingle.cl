__kernel void test_atomic_single(__global int * location, int val){
   atomic_add(location, val);
}
