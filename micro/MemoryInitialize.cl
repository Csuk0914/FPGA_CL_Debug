__kernel void initialize_memory(__global int * location){
      location[get_global_id(0)]=get_global_id(0);
}
