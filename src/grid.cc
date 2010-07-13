#include "houghUtility.h"
#include <cstring>
#include <sstream>
#include "grid.h"


grid::grid(char* datafile, int resolution)
{
    max_x = max_y = min_x = min_y = cellstep_x = cellstep_y  = 0.0;
    this->resolution = resolution;
    ifstream file(datafile);
    
    if(!file)
    {
        cerr << "Cannot read file! File " << __FILE__ << " Line: " << __LINE__ << endl;
        exit(-1);
    }
    double x, y;
    while(file >> x >> y)
    {
        if(x > max_x)
            max_x = x;
        if(x < min_x)
            min_x = x;
        if(y > max_y)
            max_y = y;
        if(y < min_y)
            min_y = y;
    }

    cellstep_x = (fabs(min_x) + fabs(max_x)) / resolution;
    cellstep_y = (fabs(min_y) + fabs(max_y)) / resolution;
    
   // #ifdef M_DB
    cout << "\tmin_x: " << min_x << endl;
    cout << "\tmin_y: " << min_y << endl;
    cout << "\tmax_x: " << max_x << endl;
    cout << "\tmax_y: " << max_y << endl;
    cout << "\tresolution: " << resolution << endl;
    cout << "\tcellstep_x: " << cellstep_x << "\tcellstep_y: "<< cellstep_y << endl;
    //#endif

    file.close();
    file.open(datafile);

    gridcell = new cell*[resolution + 1];
    for(int i = 0; i <= resolution; ++i)
    {
        gridcell[i] = new cell[resolution + 1];
        for(int j = 0; j <= resolution; ++j)
        {
            gridcell[i][j].i = i;
            gridcell[i][j].j = j;
            gridcell[i][j].count = 0;
        }
    }
    
    while(file >> x >> y)
    {
        /*Note: catesian: 2 / 0 is 0/2 in array!
        */
        gridcell[(int)((y + fabs(min_y)) / cellstep_y)][(int)((x + fabs(min_x)) / cellstep_x)].count++;
        gridcell[(int)((y + fabs(min_y)) / cellstep_y)][(int)((x + fabs(min_x)) / cellstep_x)].values.push_back(make_pair(y, x));
    }
    
}


cell* grid::get_cell(double x, double y){
    //assert(cellstep_x > 0 && cellstep_y > 0);
   return &gridcell[(int)((x + fabs(min_x)) / cellstep_x)][(int)((y + fabs(min_y)) / cellstep_y)];
}

cell* grid::get_cell(int i, int j){
    return &gridcell[i][j];
}

void grid::increase_cell_count(int i, int j){
    gridcell[i][j].count++;
}

void increase_cell_count(cell *c){
    c->count++;
}
    
void grid::print_grid(char *writefile){
    ofstream target;
    target.open(writefile);
    for(int i = 0; i <= resolution; ++i){
        for(int j = 0; j <= resolution; ++j){
            if(gridcell[i][j].count > 0)
                target << j << " " << i << endl;
        }
    }
    target.close();
}

void grid::free()
{
    for(int i = 0; i <= resolution; ++i)
    {
        delete[] gridcell[i];
    }
    delete[] gridcell;
}

void grid::find_hoodless(char *writefile){
    ofstream target;
    target.open(writefile);
    list<cell*> hoodless;
    list<cell*> hoods;
    list<cell*> matched;
    list<cell*>::iterator it;

    //get all cells which have count > 0
    matched = get_matched_cells();

    for(it = matched.begin(); it != matched.end(); it++){ 
        //get neidhborhood of current cell
        hoods = get_hood((*it));
        //if there are not enough neighbours add cell to hoodless list
        if (hoods.size() < 2) hoodless.push_back((*it));
    }
    
    for(it = hoodless.begin(); it != hoodless.end(); it++){
        target << (*it)->j << " " << (*it)->i << endl;
    }
    target.close();
}

list<cell*> grid::find_hoodless(int min){
    list<cell*> matched;
    list<cell*> hoodless;
    list<cell*> hoods;
    list<cell*>::iterator it;
    cell* c;

    matched = get_matched_cells();

    for(it = matched.begin(); it != matched.end(); it++){
        hoods = get_hood(*it);
        if(hoods.size() == min) hoodless.push_back(*it);
    }
    return hoodless;
}

list<cell*> grid::get_matched_cells(){
    list <cell*> matched;
    for(int i = 0; i <= resolution; i++){
        for(int j = 0; j <= resolution; j++){
            if(gridcell[i][j].count > 0) matched.push_back(&gridcell[i][j]);
        }
    }
    return matched;
}

void
grid::grid_hough(char *writefile)
{
    ofstream file(writefile);
    for(int i = 0; i <= resolution; ++i)
    {
        for(int j = 0; j <= resolution; ++j)
        {
            if(gridcell[i][j].count > 0){
                stringstream in;
                stringstream out;
                in << "sgrid_hough_" << i << '_' << j << ".txt";
                out << "tgrid_hough_"<< i << '_' << j << ".txt";

                ofstream source(in.str().c_str());
                
                list<pair<double, double> >::iterator list_iter;
                
                for(list_iter = gridcell[i][j].values.begin(); list_iter != gridcell[i][j].values.end(); ++list_iter)
                {
                    source << list_iter->first << " " << list_iter->second << endl;

                }
                source.close();
                hough(in.str().c_str(), out.str().c_str(), 180);
                remove(in.str().c_str());
            }
        }
    }
}

list<cell*> grid::get_hood(cell* c){
    list<cell*> hood;
    cell* tmp;
    for(int k = -1; k <= 1; k++){
        for(int l = -1; l <= 1; l++){
            //check boundaries
            if(c->i + k >= 0 && c->i + k <= resolution)
                if(c->j + l >= 0 && c->j + l <= resolution)
                {
                    tmp = &gridcell[c->i + k][c->j + l];
                    //its in da hood if not itself and count > 0
                    if(!(k == 0 && l == 0) && tmp->count > 0) hood.push_back(tmp);
                }
        }
    }
    return hood;
}

list<cell*> grid::get_hoodcells(int min){
    list<cell*> matched;
    list<cell*> hoodcells;
    list<cell*> hoods;
    list<cell*>::iterator it;
    cell* c;

    matched = get_matched_cells();

    for(it = matched.begin(); it != matched.end(); it++){
        hoods = get_hood(*it);
        if(hoods.size() >= min) hoodcells.push_back(*it);
    }
    return hoodcells;
}

void grid::find_lines(){
    list<cell*> hoodcells;
    list<cell*> linestart;
    list<cell*>::iterator it;
    list<cell*>::iterator it2;
    cell *neighbour = 0;
    cell *last = 0;


    ofstream target;
    target.open("grid_lines.txt");

    //points with one neighbour are possible line starting points
    //linestart = find_hoodless(1);
    linestart = find_hoodless(3);

    for(it = linestart.begin(); it != linestart.end(); it++){ 
        hoodcells = get_hood(*it);
        for(it2 = hoodcells.begin(); it2 != hoodcells.end(); it2++){
        //get the only neighbour
        //neighbour = get_hood(*it).front();
        neighbour = *it2;
        //get last cell which follows in direction of the neighbour
        last = rek_line(neighbour, which_hood(*it, neighbour));
        
        //cout << "starting line search for " << (*it)->i << " " << (*it)->j << endl;
        //cout << "found line end " << last->i << " " << last->j << endl;
        
        //never mind why we have to switch i and j ... FOK!!
        target << (*it)->j << " " << (*it)->i << endl;
        target << last->j << " " << last->i << endl << endl;
    }
    }
    target.close();
}

cell* grid::rek_line(cell* c, int direction){
    list<cell*>::iterator it;
    list<cell*> hood;
    int count = 0;

    hood = get_hood(c);
    for(it = hood.begin(); it != hood.end(); it++){
        if(which_hood(c, *it) == direction) {
            return rek_line(*it, direction);
            count++;
        }
    }
    if (count == 0)
        return c;
    
    return NULL;
}

// return values for direction from c1 to c2
// 1  2   3
// 4 #c1# 6
// 7  8   9
int grid::which_hood(cell* c1, cell* c2){
    if(c1->i - 1 == c2-> i && c1->j + 1 == c2->j) return 1;
    if(c1->i == c2-> i && c1->j + 1 == c2->j) return 2;
    if(c1->i + 1 == c2-> i && c1->j + 1 == c2->j) return 3;
    
    if(c1->i - 1 == c2-> i && c1->j == c2->j) return 4;
    if(c1->i == c2-> i && c1->j == c2->j) return 5;
    if(c1->i + 1 == c2-> i && c1->j == c2->j) return 6;
    
    if(c1->i - 1 == c2-> i && c1->j - 1 == c2->j) return 7;
    if(c1->i == c2-> i && c1->j - 1 == c2->j) return 8;
    if(c1->i + 1 == c2-> i && c1->j - 1 == c2->j) return 9;

    cout << "no neighbours" << endl;
    return 0;
}

void
grid::hookers_on_the_grid(char *writefile)
{
    double theta;
    double dist;
    int x, y;
    
    list<cell*> hoodcells;// = (1);
    list<cell*>::iterator list_iter = hoodcells.begin(); 
    // initialize our voting matrix
    houghLine **votingMatrix = new houghLine*[3]; 
    for(int i = 0; i < 3 ; ++i){
        votingMatrix[i] = new houghLine[resolution*3]; 
        for(int j = 0; j <resolution*3; ++j){
            votingMatrix[i][j].set_start(0.0, 0.0);
            votingMatrix[i][j].set_end(0.0, 0.0);
            votingMatrix[i][j].set_parameters(0.0, 0.0);
            votingMatrix[i][j].set_count(0);
        }
    }
    // fill the voting matrix with neighbours
    for(; list_iter != hoodcells.end(); ++list_iter){
        x = (*list_iter)->j; // NOTE: ATTENTION: REVERSE: BACKWARDS: FOK!
        y = (*list_iter)->i;

        // zero angle
        theta = 0.0; 
        dist  = x * cos(theta) + y * sin(theta);
        if(votingMatrix[0][(int)dist + resolution].value() >= 1){
            votingMatrix[0][(int)dist + resolution].set_end(x, y);
        } else {
            votingMatrix[0][(int)dist + resolution].set_start(x, y);
            votingMatrix[0][(int)dist + resolution].set_parameters(theta, dist);
        }
        votingMatrix[0][(int)dist + resolution].increase();
        
       
        // angle 45 degree
        theta = 45.0; 
        dist  = x * cos(theta) + y * sin(theta);
        if(votingMatrix[1][(int)dist + resolution].value() >= 1){
            votingMatrix[1][(int)dist + resolution].set_end(x, y);
        } else {
            votingMatrix[1][(int)dist + resolution].set_start(x, y);
            votingMatrix[1][(int)dist + resolution].set_parameters(theta, dist);
        }
        votingMatrix[1][(int)dist + resolution].increase();
        
        
        // angle 90 degree
        theta = 90.0;     
        dist  = x * cos(theta) + y * sin(theta);
        if(votingMatrix[2][(int)dist + resolution].value() >= 1){
            votingMatrix[2][(int)dist + resolution].set_end(x, y);
        } else {
            votingMatrix[2][(int)dist + resolution].set_start(x, y);
            votingMatrix[2][(int)dist + resolution].set_parameters(theta, dist);
        } 
        votingMatrix[2][(int)dist + resolution].increase();
    }
    ofstream target(writefile);
    for(int i = 0; i < 3; ++i){
        for(int j = 0; j < resolution*3; ++j){
            if(votingMatrix[i][j].value() > 3) // print only lines with at least 5 hits!
                target << votingMatrix[i][j];
        }
    }
    target.close();

    for(int i = 0; i < 3; ++i){
        delete[] votingMatrix[i];
    }
    delete[] votingMatrix;
}
