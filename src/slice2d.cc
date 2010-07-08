#include <map>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <list>
#include <vector>
#include <utility>

#include "houghUtility.h"
#include "slice2d.h"
#include "grid.h"

using namespace std;

void 
slice2d::slice_3d_map(double from, double to, char* file){
    int i;
    double x_val, y_val, z_val;
    ifstream source;
    source.open(file);
    ofstream target;
    target.open("2d_map_sliced.txt");
    if(!target.good() || !source.good())
    {
        cerr<<"Line "<<__LINE__<<" File: "<<__FILE__<<". Unable to open/read file."<<'\n';
        exit(-1);
    }
    if(source.peek() == EOF)
    {
        cerr<<"Line "<<__LINE__<<" File: "<<__FILE__<<". Empty File:"<< file <<'\n';
        exit(-1);
    }

    while(source.good()){
        source >> x_val >> y_val >> z_val;
        if (y_val > from && y_val < to){
            target << x_val << " " << z_val << '\n';
        }
    }
    source.close();
    target.close();
    cout<<"Done Slicing!\n";
}

void
slice2d::delta_method(char *file, double delta)
{
    double phi1, phi2, r1, r2, d;
    ifstream source;
    ofstream target;
    source.open(file);
    target.open("delta_method.txt");
    if(!target.good() || !source.good())
    {
        cerr<<"Line "<<__LINE__<<" File: "<<__FILE__<<". Unable to open/read file."<<'\n';
        exit(-1);
    }
    if(source.peek() == EOF)
    {
        cerr<<"Line "<<__LINE__<<" File: "<<__FILE__<<". Empty File:"<< file <<'\n';
        exit(-1);
    }
    
    if (source.good()) source >> phi1 >> r1;
    while(source.good()){
        source >> phi2 >> r2;
        //distance in polar coords
        d = sqrt(r1*r1 + r2*r2 - 2*r1*r2*cos(abs(phi2-phi1)));
        
        if(d > delta){
            target << r1*cos(phi1) << " " << r1*sin(phi1) << " " << "\n"; 
            target << r2*cos(phi2) << " " << r2*sin(phi2) << " " << "\n\n";
        }
        phi1 = phi2;
        r1 = r2;
        //else
            //delta = (delta + abs(d1-d2)) / 2;
    }
    source.close();
    target.close();
}

bool slice2d::compare_polar_points(PointPolar &a, PointPolar &b){
    return (a.phi < b.phi);
}


void slice2d::scan_sort_polar(char *readfile, char *writefile){
    double x,y,r,phi;
    list<PointPolar> liste;
    list<PointPolar>::iterator it;
    PointPolar p;
    ifstream source;
    ofstream target;
    source.open(readfile);
    target.open(writefile);
    if(!target.good() || !source.good())
    {
        cerr<<"Line "<<__LINE__<<" File: "<<__FILE__<<". Unable to open/read file."<<'\n';
        exit(-1);
    }
    if(source.peek() == EOF)
    {
        cerr<<"Line "<<__LINE__<<" File: "<<__FILE__<<". Empty File:"<< readfile <<'\n';
        exit(-1);
    }
    while (source.good()){
        source >> x >> y;
        p.r = sqrt(x*x + y*y);
        p.phi = atan2(y, x);
        liste.insert(liste.end(), p);
    }
    liste.sort(compare_polar_points);    
    for(it = liste.begin(); it != liste.end(); it++){
        target << (*it).phi << " " << (*it).r << "\n";
    }
}

bool slice2d::compare_cart_points(PointCartesian &a, PointCartesian &b){
    return (a.x < b.x);
}

void slice2d::scan_sort_cart(char *readfile, char *writefile){
    list<PointCartesian> liste;
    list<PointCartesian>::iterator it, p_old;
    PointCartesian p;
    ifstream source;
    ofstream target;
    source.open(readfile);
    target.open(writefile);
    
    if(!target.good() || !source.good())
    {
        cerr<<"Line "<<__LINE__<<" File: "<<__FILE__<<". Unable to open/read file."<<'\n';
        exit(-1);
    }
    if(source.peek() == EOF)
    {
        cerr<<"Line "<<__LINE__<<" File: "<<__FILE__<<". Empty File:"<< readfile <<'\n';
        exit(-1);
    }
    
    while (source.good()){
        source >> p.x >> p.y;
        liste.push_back(p);
    }
    liste.sort(compare_cart_points);    
    for(it = liste.begin(); it != liste.end(); it++){
        if (it == liste.begin()){
            p_old = liste.begin(); //startwert fuer p_old
            target << (*it).x << " " << (*it).y << "\n";
        }
        else{
            //doppelte Punkte nicht verwenden
            //if(fabs((*it).x - (*p_old).x) > 0.001 || (fabs((*it).y - (*p_old).y) > 0.001))
                target << (*it).x << " " << (*it).y << "\n";
        }
        p_old = it;
    }
}

void slice2d::find_lines(char *file){
    ifstream source;
    ofstream target;
    source.open(file);
    target.open("2d_map_sliced_lines.txt");
    
    vector<PointCartesian> line;
    
    double phi, r, x, y, e1, e2, e3, tmp;
    PointCartesian p;

    double constr1, constr2, constr3; //constraints

    e1=e2=0.2;
    e3 = 30;
    
    while (source.good()){
//        source >> p.x >> p.y;
        source >> phi >> r;
        p.x = r * cos(phi);
        p.y = r * sin(phi);
        
        if(line.size() >= 3){
            
            tmp = 0;
            for(int i = 0; i < line.size() - 1; i++) tmp += dist_cart(line[i], line[i+1]);
            constr1 = dist_cart(line[0], p) / (tmp + dist_cart(line[line.size() -1], p));
            
            constr2 = dist_cart(line[line.size()-2], p) / 
                (dist_cart(line[line.size()-2],line[line.size()-1]) +
                 dist_cart(line[line.size()-1], p));
           
            constr3 = dist_cart(line[line.size()-1],p);

            //test if line ends
            if(1 >= constr1 && constr1 > 1 - e1 * line.size() && 1 >= constr2 && constr2 > 1 - e2 && constr3 < e3) 
                line.push_back(p);
            else {
                //write first and last point of line to file
                target << line[0].x << " " << line[0].y << "\n";
                target << line[line.size()-1].x << " " << line[line.size()-1].y << "\n\n";
                cout << "linie mit punkten " << line.size() << "\n"; 
                cout << constr1 << " " << constr2 << "\n\n";
                line.clear();
           }
        }
        else{
            //if (!line.empty()) cout << dist_cart(line[line.size()-1], p) << "\n";
            if (line.empty() || dist_cart(line[line.size()-1], p) < 30) line.push_back(p);
            else line.clear();
        }
    }
    source.close();
    target.close();
}

//function assumes polar ordered (phi) data for getting segments
void slice2d::find_circuit(char *readfile, char *writefile){
    ifstream source;
    ofstream target;
    source.open(readfile);
    target.open(writefile);

    list<PointPolar> segment; //set with small segments for dist comparison
    list<PointPolar> hoodless; //set with points which have no other points in neighbourhood
    list<PointPolar>::iterator it, it2;

    PointPolar p;

    while(source){
        //a) get new data in small segments
        for(int i = 0; i < 10; i++){
            if (source){
                source >> p.phi >> p.r;
                segment.push_back(p); //add element
            }
        }
        //b) delete some farer points from begin of the set
        while(segment.size() > 10){
            segment.pop_front(); //delete first point
        }

        //c) check if there are in neighbourhood other points
        for(it = segment.begin(); it != segment.end(); it++){
            for(it2 = segment.begin(); it2 != segment.end(); it2++){
                if(it != it2){
                    if(dist_polar((*it).phi, (*it).r, (*it2).phi, (*it2).r) < 10)
                        break;
                    if(&(*it2) == &segment.back()) //if after last element no neighbour is found add point to hoodless set
                        hoodless.push_back(*it);
                    
                }
            }
        }
    }

    for(it = hoodless.begin(); it != hoodless.end(); it++){
        target << (*it).r * cos((*it).phi) << " " << (*it).r * sin((*it).phi)<< endl;
    }
    target.close();
    source.close();
}



void slice2d::find_lines_captnHough(char *file){
    ifstream source;
    ofstream target, debug;
    source.open(file);
    target.open("2d_map_sliced_lines_hook.txt");
    debug.open("mydebug.txt");
    
    set<houghLine> votingSet;
    set<houghLine>::iterator votingIterator;
    houghLine* hlp;

    double x, y, theta, dist;
    while(source >> x >> y)
    {
        cout << x << " " << y << endl;
        for(int i = 0; i < 360; i++)
        {
            theta = static_cast<double>(i) * M_PI / 180;
            dist   = x * cos(theta) + y * sin(theta);

            houghLine tmp(theta, dist, x, y);
            hlp = houghLine::find(votingSet, tmp);
            if(hlp != NULL)
            {
                hlp->increase();
                hlp->set_end(x, y);
            } else 
            {
                if(!(votingSet.insert(tmp).second))
                {
                      cout << "FUUUUUUUUUUUUUUUUUUUUUUUUUUUCK'n";
                }
            }
            
        }
    }
    //cout << "in: " << in << "\t out: " << out << endl;
    //cout << "VotingSet.size(): " << votingSet.size() << endl;    
    for(votingIterator = votingSet.begin(); votingIterator != votingSet.end(); ++votingIterator)
    {
        //cout << "Line: " << *votingIterator<<endl;
        if(votingIterator->value() > 1)
        {
            target << (*votingIterator) << std::endl << std::endl;
        }
    }


    debug.close();
    target.close();
    source.close();

}




double slice2d::dist_cart(double x1, double y1, double x2, double y2)
{
    return (sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)));
}

double slice2d::dist_cart(PointCartesian &p1, PointCartesian &p2){
    return (sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y)));
}

double slice2d::dist_polar(double phi1, double r1, double phi2, double r2)
{
    return (sqrt(r2*r2 + r1*r1 - 2*r1*r2*cos(phi2-phi1)));
}

double slice2d::dist_polar(PointPolar &p1, PointPolar &p2){
    return (sqrt(p1.r*p1.r + p2.r*p2.r - 2*p1.r*p2.r*cos(p2.phi-p1.phi)));
}


void usage(){
    cout<<"\tSlice: A Programm to cut slice 3d points clouds with a given size.\n";
    cout<<"\tUsage: ./slice [arg1] .. [arg4]\n";
    cout<<"\t\t [arg1] lower bound for slice, real numbered\n";
    cout<<"\t\t [arg2] upper bound for slice, real numbered\n";
    cout<<"\t\t [arg3] file to be sliced\n";
    cout<<"\t\t [arg4] maximum dist for delta_rule method\n";
}
int main(int argc, char *argv[]){
    if(argc < 4)
        usage();
    else{    
        slice2d::slice_3d_map(atof(argv[1]), atof(argv[2]), argv[3]);
        slice2d::scan_sort_cart("2d_map_sliced.txt", "2d_map_sliced_sort_cart.txt");
        slice2d::scan_sort_polar("2d_map_sliced.txt", "2d_map_sliced_sort_polar.txt");
        slice2d::find_circuit("2d_map_sliced_sort_polar.txt","hoods.txt");
        //hough("2d_map_sliced_sort_cart.txt", "hokedihook.txt", 180);
        //hough("hokedihook.txt", "hook.txt", 180);
        //hough("test.file", "2d_map_sliced_lines_hook.txt", 180);
        grid g("2d_map_sliced_sort_cart.txt", 50);
        //grid g("grid.test", 20);
        g.print_grid("grid.txt");
        g.grid_hough("grid.txt");
        g.free();
    }        
}
