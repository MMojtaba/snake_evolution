# Info
This project was made for Dr. Michael D. Adams' SENG 475 course at UVic.  
This Virtual Machien Disk that contains the tooks to run this application can be found at: https://www.ece.uvic.ca/~frodo/cppbook/#vm_disk_images.  
OpenGL 2.1 and GLSL 1.2 were used, due to the limitation of the school's lab machines.  
A demo video for this project can be found at: https://youtu.be/KYwRsbAx9Rw  

## To build, install, and run:
-Navigate to the directory containing this README file.  
-Replace $INSTALL_DIR in the commands below with the directory to which you want to install the program to (or create an environemnt variable for it).  
-Then use the following commands:  
    cmake -H. -Btmp -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR  
    cmake --build tmp --clean-first --target install  
    $INSTALL_DIR/bin/demo  
