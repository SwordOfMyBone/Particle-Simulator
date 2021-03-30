#include <vector>

// Particle class reperesents a single object in the problem
class particle{
    int x,y,z; // coord to be stored as int

    public:
    particle(int iX, int iY, int iZ): x{iX}, y{iY}, z{iZ} // constructor takes in a particle
    {

    }

    void moveParticle(int i){
        x += i;
        y += i;
    }
};

class pSet{
    int size;
    std::vector<particle> pV;

    public:
    pSet(){


    }
};

