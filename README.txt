To build, install, and run:
Navigate to the directory containing this README file.
Let $INSTALL_DIR be the directory to which the program will be installed.
Then use the following commands:
    cmake -H. -Btmp -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR
    cmake --build tmp --clean-first --target install
    $INSTALL_DIR/bin/demo