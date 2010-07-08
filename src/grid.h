#ifndef GRID
#define GRID

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <fstream>
#include <utility>
#include <sstream>

using namespace std;

typedef struct{
    int i;
    int j;
    int count;
} cell;

class grid{
    public:
    //constructors
    grid(char* datafile, int resolution);
    
    
    cell* get_cell(double x, double y);
    cell* get_cell(int i, int j);
    void increase_cell_count(int i, int j);
    void increase_cell_count(cell *c);
    void print_grid(char *writefile);
    void free();
    
    private:
    cell** gridcell;
    double max_x, max_y, min_x, min_y;
    double cellstep_x, cellstep_y;
    int resolution;
};
#endif
