rm -rf build-ios && mkdir build-ios && cd build-ios
cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/iOS.cmake -DNO_CLEW=1 -DNO_PTEX=1 -DNO_OMP=1 -DNO_TBB=1 -DNO_CUDA=1 -DNO_MAYA=1 -G Xcode ..
open OpenSubdiv.xcodeproj/