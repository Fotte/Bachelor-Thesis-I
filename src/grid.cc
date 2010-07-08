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
        cerr << "Cannot read file" << endl;
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
    file.close();
    file.open(datafile);

    cellstep_x = (fabs(min_x) + fabs(max_x)) / resolution;
    cellstep_y = (fabs(min_y) + fabs(max_y)) / resolution;
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
    
    cout << "erzeuge grid" << endl;
    cout << "resoluition: " << resolution << endl;
    cout << "max_x, min_x " << max_x << " " << min_x << endl;
    cout << "max_y, min_y " << max_y << " " << min_y << endl;
    cout << "cellstep x,y " << cellstep_x << " " << cellstep_y << endl;
    
    while(file >> x >> y)
    {
        get_cell(x, y)->count++;
        get_cell(x, y)->values.push_back(make_pair(x, y));
    }
    cout << "created grid\n";
}


cell* grid::get_cell(double x, double y){
   return &gridcell[(int)((y + fabs(min_y)) / cellstep_y)][(int)((x + fabs(min_x)) / cellstep_x)];
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
    
    cout << "printing grid to file: " << writefile << endl;
    cout << resolution + 1 << " * " << resolution + 1 << " cells" << endl;
    
    for(int i = resolution; i >= 0; i--){
        for(int j = 0; j <= resolution; j++){
            //target << gridcell[i][j].count << "\t";
            if(gridcell[i][j].count > 0)
                target << j << " " << i << endl;
                //target << "#";
            //else target << " ";
        }
        target << endl;
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
=======
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
