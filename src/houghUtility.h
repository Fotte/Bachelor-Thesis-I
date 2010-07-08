#ifndef HOUGHUTILITY
#define HOUGHUTILITY

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <fstream>
#include <utility>
#include <sstream>


void hough(char* in, char* out, int resolution);
int  hough_hash(double x, double min, int accuracy);
void  hough_get_min_max(char *in, int resolution, double &min, double &max);
double hough_unhash(int hash);


class houghPoint2D {
    public:
    houghPoint2D();
    houghPoint2D(double start_x, double start_y, double end_x, double end_y);
    houghPoint2D(double start_x, double start_y);

    void set_start(double x, double y);
    void set_end(double x, double y);
    void set_points(double start_x, double start_y, double end_x, double end_y);

    int get_count();
    std::pair<double, double> get_start();
    std::pair<double, double> get_end();

    houghPoint2D operator++(int unused); // postfix
    houghPoint2D& operator++();           // prefix

    private:
    double s_x, s_y, e_x, e_y;
    unsigned int count;

};


class houghLine {

    static const double delta_equal = 0.2;
    
    public:
    houghLine();
    houghLine(double phi_in, double r_in);
    houghLine(double phi_in, double r_in, double s1, double s2);

    /*
     * careful: < operator designed for set:
     * < is only true iff lhs != rhs and lhs < rhs!
     */
    friend bool operator<(const houghLine& lhs, const houghLine& rhs)  
    { 
        return ((lhs.phi - rhs.phi) < 0 && (lhs.r - rhs.r) < 0);
    };
    
    bool operator==(const houghLine& other) const;
    friend std::ostream& operator<<(std::ostream& os, const houghLine &a);    
    double dist_to(double nx, double ny);
    
    std::string toString();

    houghLine operator++(int unused);
    houghLine& operator++();

    void increase() const;
    void set_start(double s1, double s2);
    void set_end(double e1, double e2);
    void set_parameters(double phi, double r);
    void set_count(int count);
    int value() const;
    
    static houghLine *find(std::set<houghLine> &houghset, houghLine &e);

    private:
    double phi;  // angle in houghroom
    double r;    // distance in houghroom

    double sx; // end x 
    double sy; // end y

    mutable double ex; // end x
    mutable double ey; // end y

    mutable int count;
};

#endif
