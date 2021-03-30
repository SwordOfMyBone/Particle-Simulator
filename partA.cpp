#include <iostream>
#include <ompt.h>
#include <string>

// to adjust schedule type set the OMP_SCHEDULE env variable.


int main(){


#pragma omp parallel for schedule(runtime) // Creates threadpool and runs for loop with env var.
    for(int i = 0; i < 1000; ++i ){ // count to 1000 
        std::cout << i << std::endl;
    }
}
