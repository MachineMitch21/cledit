sudo apt-get update
sudo apt-get install gcc nasm build-essential gdb ddd
sudo dpkg --add-architecture i386
sudo apt-get update
sudo apt-get install libc6:i386 libncurses5:i386 libstdc++6:i386
sudo apt-get install multiarch-support
sudo apt-get install gcc-multilib