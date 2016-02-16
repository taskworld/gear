!#/bin/sh

mkdir -p out/lib/include/httpclient
mkdir -p out/ios/include/httpclient

#For Mac OSX
cmake .
make

mv lib_gear.a out/lib
cp httpclient/*.hpp out/lib/include/httpclient

#For iOS
cmake . -DCMAKE_TOOLCHAIN_FILE=iOS.cmake -DIOS_PLATFORM=OS
make

mv lib_ios_gear.a out/ios/ios_os_gear.a

cmake . -DCMAKE_TOOLCHAIN_FILE=iOS.cmake -DIOS_PLATFORM=SIMULATOR
make

mv lib_ios_gear.a out/ios/ios_sim_gear.a

lipo -create "out/ios/ios_sim_gear.a" "out/ios/ios_os_gear.a" -output "out/ios/lib_gear.a"
