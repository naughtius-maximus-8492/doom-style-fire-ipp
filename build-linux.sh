if [[ $1 == "clean" ]] ; then
	echo "Cleaning build directory..."
	rm -rf build/
    cmake -B build-linux/ -DCMAKE_C_COMPILER=icx -DCMAKE_CXX_COMPILER=icpx cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON   
fi
cmake --build build-linux/ -j
