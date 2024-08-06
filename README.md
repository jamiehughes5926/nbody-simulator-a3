## README

### Project Description
This program simulates the motion of a number of bodies (planets, particles, etc.) with an attractive force between each pair of particles. The simulation employs parallelization using multi-threading to enhance performance.

### Performance Improvements
A loader was added to the program due to the long execution time of the non-multi-threaded version with 10,000 particles, which took approximately 45 minutes. The loader ensures that progress is being made.

### System Configuration
- **Computer:** MacBook Pro M1
- **Cores:** 10

### Language and Multi-Threaded API
- **Programming Language:** C++
- **Multi-Threaded API:** C++ Standard Library std::thread (C++11)
- **Compiler:** g++ or clang++
- **Compilation Flags:** -std=c++11 -O3 -pthread

### Performance Comparison
#### Small: 1,000 Particles
- **Single Thread Implementation:** 28.7027 seconds
- **Multi Threaded Implementation:** 0.284372 seconds

#### Large: 5,000 Particles
- **Single Thread Implementation:** 744.033 seconds
- **Multi Threaded Implementation:** 5.64824 seconds

#### Massive: 10,000 Particles
- **Single Thread Implementation:** 2949.54 seconds
- **Multi Threaded Implementation:** 22.4863 seconds

### Compilation and Execution
To compile the program, use the following command:
```bash
g++ -std=c++11 -O3 -pthread -o nbody_simulator main.cpp
