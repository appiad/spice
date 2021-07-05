# spice
Spice is an implementation of features from a SPICE ("Simulation Program with Integrated Circuit Emphasis") program. The features I've implemented allow for static analysis of a resistive circuit composed of resistors, voltage sources, and current sources.
  
## Requirements
This project requires at least C++11 as well as [Eigen3](https://eigen.tuxfamily.org/index.php?title=Main_Page), [Catch2](https://github.com/catchorg/Catch2/tree/v2.x), and [spdlog](https://github.com/gabime/spdlog). 
Installation instructions can be found at their respective pages but the steps below can be followed to install from source:
- Eigen
```
git clone https://gitlab.com/libeigen/eigen.git
mkdir build && cd build
cmake ..
make install 
```

- Catch2
```
git clone https://github.com/catchorg/Catch2.git
cmake -Bbuild -H. -DBUILD_TESTING=OFF
sudo cmake --build build/ --target install
```

- spdlog
```
git clone https://github.com/gabime/spdlog.git
cd spdlog && mkdir build && cd build
cmake .. && make -j
make install 
```
Note that ```sudo make install``` may have to be used instead of ```make install```.
## Installation 
To install, just clone the repository:

``` git clone https://github.com/appiad/spice/```
## Building
## Usage
### Netlists
## License 
