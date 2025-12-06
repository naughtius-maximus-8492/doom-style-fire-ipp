if [[ $1 == "clean" ]] ; then
	echo "Cleaning build directory..."
	rm -rf build/
fi
cmake -B build-linux/ -DCMAKE_C_COMPILER=icx -DCMAKE_CXX_COMPILER=icpx
cmake --build build-linux/ -j
