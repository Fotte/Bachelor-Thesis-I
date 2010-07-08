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
    gridcell = new cell*[resolution];
    for(int i = 0; i < resolution; ++i)
    {
        gridcell[i] = new cell[resolution];
        for(int j = 0; j < resolution; ++j)
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
        get_cell(x-1, y-1)->count++;

    }
}


cell* grid::get_cell(double x, double y){
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
    
    for(int i = resolution -1; i >= 0; i--){
    //for(int i = 0; i < resolution; i++){
        for(int j = 0; j < resolution; j++){
        //for(int j = resolution -1; j >=0; j--){
            //target << gridcell[i][j].count << "\t";
            if(gridcell[i][j].count > 0)
                target << "# ";
            else target << "  ";
        }
        target << endl;
    }
    target.close();
}

void grid::free()
{
    for(int i = 0; i < resolution; ++i)
    {
        delete gridcell[i];
    }
    delete[] gridcell;
}
