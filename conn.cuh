#ifndef CONN_CUH
#define CONN_CUH

namespace cc {
    struct ed{
        long long int x;
    };
    
    typedef struct ed edge;
    
    struct grp{
        int num_e,num_n;
        int**neigh,*deg;
    };
    
    typedef struct grp my_graph;

    void compute(int const, int const, edge *, int *);
}

#endif  // CONN_CUH
