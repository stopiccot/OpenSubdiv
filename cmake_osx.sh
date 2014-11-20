rm -rf build-osx && mkdir build-osx && cd build-osx
cmake -DNO_CLEW=1 -DNO_PTEX=1 -DNO_OMP=1 -DNO_TBB=1 -DNO_CUDA=1 -DNO_MAYA=1 -G Xcode ..
open OpenSubdiv.xcodeproj/