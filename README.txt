To build, install, and run:
Navigate to the directory containing this README file.
Replace $INSTALL_DIR in the commands below with the directory to which you want to 
    install the program to (or create an environemnt variable for it).
Then use the following commands:
    cmake -H. -Btmp -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR
    cmake --build tmp --clean-first --target install
    $INSTALL_DIR/bin/demo