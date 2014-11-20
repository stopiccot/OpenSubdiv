mkdir build-windows-x86
cd build-windows-x86
cmake -DNO_OPENCL=1 -DNO_CLEW=1 -DNO_PTEX=1 -DNO_OMP=1 -DNO_TBB=1 -DNO_CUDA=1 -DNO_MAYA=1 -G "Visual Studio 12" ..
pause