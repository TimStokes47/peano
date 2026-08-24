#include <iostream>
#include <cstring>
#include <cstdint>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <cstdlib>
#include <hip/hip_runtime.h>
#include "MLIRbridgeHIPfunctions.h"

// *** NOTE: These are the CUDA-based functions that the generated MLIR code call
//           to setup the streams, load and launch the kernels etc.
//           A similar MLIRbridgeROCM.cpp will be required for AMD ROCm support

extern "C" {
  typedef void* GPUModule;
  typedef void* GPUFunction;
  typedef void* GPUStream;
  
  GPUModule mgpuModuleLoad(const void* binary, size_t size) {
    if (!binary || size == 0) {
      std::cerr << "[CUDA ERROR] Invalid binary data!" << std::endl;
      return nullptr;
    }
    
    hipError_t res = hipInit(0);
    if (res != hipSuccess && res != 15) {
      std::cerr << "[CUDA ERROR] Failed to initialize CUDA driver: " << (int)res << std::endl;
    }
    
    hipDevice_t device = 0;

    res = hipDeviceGet(&device, 0);
    if (res != hipSuccess) {
      std::cerr << "[CUDA ERROR] Failed to get device: " << (int)res << std::endl;
      return nullptr;
    }
 
    // Load PTX as CUDA module
    hipModule_t cu_module = nullptr;
    res = hipModuleLoadData(&cu_module, binary);
    if (res != hipSuccess) {
      std::cerr << "[CUDA ERROR] Failed to load module: " << (int)res << std::endl;
      return nullptr;
    }
    return (GPUModule)cu_module;
  }

  void mgpuModuleUnload(GPUModule module) {
    if (module) {
      hipModuleUnload((hipModule_t)module);
    }
  }

  GPUFunction mgpuModuleGetFunction(GPUModule module, const char* name) {
    if (!module) {
      std::cerr << "[ERROR] Invalid module handle" << std::endl;
      return nullptr;
    }
    
    hipFunction_t cu_kernel = nullptr;
    hipError_t res = hipModuleGetFunction(&cu_kernel, (hipModule_t)module, name);
    if (res != hipSuccess) {
      std::cerr << "[CUDA ERROR] Failed to get kernel: " << (int)res << std::endl;
      return nullptr;
    }
    return (GPUFunction)cu_kernel;
  }

  // Stream management
  GPUStream mgpuStreamCreate() {
    hipStream_t stream = nullptr;

    hipError_t res = hipStreamCreateWithFlags(&stream, hipStreamDefault);
    if (res != hipSuccess) {
      std::cerr << "[CUDA ERROR] Failed to create stream: " << (int)res << std::endl;
      return nullptr;
    }
    return (GPUStream)stream;
  }

  void mgpuStreamDestroy(GPUStream stream) {
    if (stream) {
      hipError_t res = hipStreamDestroy((hipStream_t)stream);
      if (res != hipSuccess) {
        std::cerr << "[CUDA ERROR] Failed to destroy stream: " << (int)res << std::endl;
      }
    }
  }

  void mgpuStreamSynchronize(GPUStream stream) {
    if (stream) {
      hipError_t res = hipStreamSynchronize((hipStream_t)stream);
      if (res != hipSuccess) {
        std::cerr << "[CUDA ERROR] Failed to synchronize stream: " << (int)res << std::endl;
      }
    }
  }

  void mgpuLaunchKernel(GPUFunction func, 
                        int64_t gridDimX, int64_t gridDimY, int64_t gridDimZ,
                        int64_t blockDimX, int64_t blockDimY, int64_t blockDimZ,
                        int32_t smemSize,
                        GPUStream stream,
                        void** kernelParams, void** extraParams,
                        int64_t paramCount) {
    if (!func) {
      std::cerr << "[ERROR] Kernel function handle is null!" << std::endl;
      return;
    }

    void** gpu_params = (void**)kernelParams;
    
    hipError_t res = hipModuleLaunchKernel(
        (hipFunction_t)func,
        gridDimX, gridDimY, gridDimZ,
        blockDimX, blockDimY, blockDimZ,
        smemSize,
        (hipStream_t)stream,  
        gpu_params,
        nullptr
    );
    
    if (res != hipSuccess) {
      const char* errStr = nullptr;
      hipDrvGetErrorName(res, &errStr);
      std::cerr << "[ERROR] hipModuleLaunchKernel failed: " << errStr << std::endl;
      return;
    }
  }



  void* mgpuMemAlloc(int64_t size, void* stream, int8_t flags) { 
    void* ptr = nullptr;

    hipError_t res = hipMallocManaged(&ptr, size);
    if (res != hipSuccess) {
        std::cerr << "[CUDA ERROR] Failed to allocate GPU memory: " << hipGetErrorString(res) << std::endl;
        return nullptr;
    }
    return ptr;

  }

  int32_t mgpuMemFree(void* ptr, void* stream) {
      if (!ptr) return 0;

      hipPointerAttribute_t attrs;
      hipError_t attrRes = hipPointerGetAttributes(&attrs, ptr);
      hipGetLastError(); // clear error state regardless

      if (attrRes == hipSuccess && attrs.type == hipMemoryTypeManaged) {
          // Only free memory we actually allocated with cudaMallocManaged
          hipError_t res = hipFree(ptr);
          if (res != hipSuccess) {
              std::cerr << "[CUDA ERROR] Failed to free GPU memory: "
                        << hipGetErrorString(res) << std::endl;
              return -1;
          }
      } 
      return 0;
  }

}


void setupMLIRContextGPU(hipDevice_t targetDevice) {
    hipInit(0);
    hipDeviceGet(&targetDevice, 0);

    hipCtx_t context;
    hipDevicePrimaryCtxRetain(&context, targetDevice);
    hipCtxSetCurrent(context);
}

void releaseMLIRContextGPU(hipDevice_t targetDevice) {
    hipDevicePrimaryCtxRelease(targetDevice);
}

