#include "ocl_header.h"
namespace Galois {
    namespace OpenCL {
        /*Last event. Should remove.*/
        cl_event OpenCL_Setup::event=nullptr;
        /*Global context*/
        cl_context OpenCL_Setup::context=nullptr;               // context
        /*Single command queue*/
        cl_command_queue OpenCL_Setup::queue=nullptr;           // command queue
        /*program to be compiled/executed*/
        cl_program OpenCL_Setup::program=nullptr;
        /*Has the runtime already been initialized*/
        bool OpenCL_Setup::initialized=false;
        /*Track memory usage */
        DeviceStats OpenCL_Setup::stats;
        /*The default device/platform is changed here.*/
        int  OpenCL_Setup::GOPT_CL_DEFAULT_PLATFORM_ID=1;
        int  OpenCL_Setup::GOPT_CL_DEFAULT_DEVICE_ID=0;
        std::vector<CL_Platform*>  OpenCL_Setup::platforms;
        std::string OpenCL_Setup::build_args;
    }

}
