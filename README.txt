To build, install, and run:
Navigate to the directory containing this README file.
Replace $INSTALL_DIR in the commands below with the directory to which you want to 
    install the program to (or create an environemnt variable for it).
Then use the following commands:
    cmake -H. -Btmp -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR
    cmake --build tmp --clean-first --target install
    $INSTALL_DIR/bin/demo



Note on running environemnt:
-This game works best when running on the lab machines in person or running on the VM.
-Works with less slowdowns on the VM if SSH is used on the host machine to run the program on the VM.
-The collision detection does not work when using SSH to run the game on 
    the lab machines because the game is slowed down too much (at least using my internet and laptop).
    Which means when the snake eats 2 or more apples, the player loses (because the snake's head
    moves so slowly that the its body keeps catching up to it and a collision occurs).