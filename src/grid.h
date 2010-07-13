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
#include <list>
#include "houghUtility.h"

using namespace std;

typedef struct{
    int i;
    int j;
    int count;
    list<pair<double, double> > values;

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
    void find_hoodless(char *writefile); //find cells without neighbours
    list<cell*> find_hoodless(int min); //find cells without <= min neighbours
    void grid_hough(char* writefile); // find lines using hough
    list<cell*> get_matched_cells(); //get a list with all cells in grid which have counter > 0
    list<cell*> get_hood(cell* c); //get a list of all neighbours of a cell
    list<cell*> get_hoodcells(int min); //get cells which have >= min neighbours
    void find_lines(); //try to find lines in grid
    cell* rek_line(cell* startcell, int direction); //rekursion following each neighbour of a given cell in a given direction to find last cell in line
    int which_hood(cell* firstcell, cell* secondcell); //returns direction from firstcell to secondcell interms of the 9 neighbours
    void hookers_on_the_grid(char* writefile);

    private:
    cell** gridcell;
    double max_x, max_y, min_x, min_y;
    double cellstep_x, cellstep_y;
    int resolution;
};
#endif
