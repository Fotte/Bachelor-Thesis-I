#include "houghUtility.h"

using namespace std;

houghPoint2D::houghPoint2D():
s_x(0.0), s_y(0.0), e_x(0.0), e_y(0.0), count(0)
{
}

houghPoint2D::houghPoint2D(double start_x, double start_y, double end_x, double end_y):
s_x(start_x), s_y(start_y), e_x(end_x), e_y(end_y), count(0)
{
}

houghPoint2D::houghPoint2D(double start_x, double start_y):
s_x(start_x), s_y(start_y), e_x(0.0), e_y(0.0), count(0)
{
}

void 
houghPoint2D::set_start(double x, double y)
{
    s_x = x;
    s_y = y;
}

void  
houghPoint2D::set_end(double x, double y)
{
    e_x = x;
    e_y = y;
}


void 
houghPoint2D::set_points(double start_x, double start_y, double end_x, double end_y)
{
    s_x = start_x;
    s_y = start_y;
    e_x = end_x;
    e_y = end_y;
}

int  
houghPoint2D::get_count()
{
    return count;
}

std::pair<double, double> houghPoint2D::get_start()
{
    return std::make_pair(s_x, s_y);
}

std::pair<double, double> houghPoint2D::get_end()
{
    return std::make_pair(e_x, e_y);
}

houghPoint2D houghPoint2D::operator++(int unused)
{
    houghPoint2D tmp = *this;
    ++count;
    return tmp;
}

houghPoint2D& houghPoint2D::operator++()
{
    ++count;
    return *this;
}

houghLine::houghLine()
{ 
    phi = 0.0; 
    r = 0.0; 
    sx = 0.0; 
    sy = 0.0;
    ex = 0.0;
    ey = 0.0;
    count = 0;
}


houghLine::houghLine(double phi_in, double r_in)
{ 
    phi = phi_in; 
    r = r_in; 
    sx = 0.0; 
    sy = 0.0;
    ex = 0.0;
    ey = 0.0;
    count = 0;
}


houghLine::houghLine(double phi_in, double r_in, double s1, double s2)
{ 
    phi = phi_in; 
    r = r_in; 
    sx = s1; 
    sy = s2;
    ex = 0.0;
    ey = 0.0;
    count = 0;
}

    
bool 
houghLine::operator==(const houghLine& other) const 
{
    return (fabs(this->phi - other.phi) < delta_equal && fabs(this->r - other.r) < delta_equal); 
}
    
std::ostream& 
operator<<(std::ostream& os, const houghLine &a)
{
    os << a.sx << " " << a.sy << "\n";
    os << a.ex << " " << a.ey <<"\n\n";
    return os;
}

std::string 
houghLine::toString()
{
    stringstream os;
    os << sx << " " << sy << " ";
    os << ex << " " << ey << "\t phi: " << phi << "\t r: " << r;
    return os.str();
}
    
houghLine 
houghLine::operator++(int unused)
{ 
    houghLine tmp = *this; 
    ++count; 
    return tmp;
}
    
houghLine& 
houghLine::operator++()
{
    ++count; 
    return *this;
}

void
houghLine::increase() const
{
    count++;
}

void
houghLine::set_start(double e1, double e2)
{
    sx = e1;
    sy = e2;
}



void
houghLine::set_end(double e1, double e2)
{
    ex = e1;
    ey = e2;
}

void
houghLine::set_parameters(double phi_in, double r_in)
{
    phi = phi_in;
    r = r_in;
}

double
houghLine::dist_to(double nx, double ny)
{
    return sqrt(pow(sx - nx, 2) + pow(sy - ny, 2));
}

void
houghLine::set_count(int count_in)
{
    count = count_in;
}


int
houghLine::value() const
{
    return count;
}

houghLine*
houghLine::find(std::set<houghLine> &hset, houghLine &e)
{
    std::set<houghLine>::iterator votingIterator;

    for (votingIterator = hset.begin(); votingIterator != hset.end(); votingIterator++)
    {
        if ((*votingIterator) == e)
        {
            return const_cast<houghLine*>(&(*votingIterator));
        }
    }
    return NULL;
}


int hough_hash(double x, double min, int accuracy)
{
    if (x <0) return static_cast<int>(-x * accuracy - 1); //you will never understand
    else return static_cast<int>((x + fabs(min)) * accuracy);
}

void  hough_get_min_max(char *in, int resolution, double &min, double &max)
{
    double x, y, theta, dist;
    ifstream source(in, std::ios::in);

    max = 0;
    min = 0;
    while(source >> x >> y)
    {
        for(int i = 0; i < resolution; ++i)
        {
            theta = (float)i * M_PI / 180.0;
            dist  = x * cos(theta) + y * sin(theta);
            if(dist > max)
            {
                max = dist;
            }
            if(dist < min)
            {
                min = dist;
            }
        }

    }
}

double hough_unhash(int hash)
{
    double acurracy = 1000.0;
    return (hash/acurracy);

}

void hough(char* in, char* out, int resolution)
{
    double max,min;
    int accuracy = 10; // hough accuray

    hough_get_min_max(in, resolution, min, max);
    double x, y, theta, dist;
    cout<<"Pre-Array1 initialized"<<endl;
    houghLine **votingMatrix = new houghLine*[resolution]; 
    int hough_size = (fabs(min) + max) * accuracy;//size of the array
    cout<<"Pre-Array2 initialized"<<endl;
    cout << "max " << max << " min "<< min <<  "\tresoution: " << resolution << endl;
    cout << "arraysize: " << hough_size << endl;
    
    
    for(int i = 0; i < resolution; ++i)
    {
        votingMatrix[i] = new houghLine[hough_size]; 
        for(int j = 0; j < hough_size; ++j)
        {
            votingMatrix[i][j].set_start(0.0, 0.0);
            votingMatrix[i][j].set_end(0.0, 0.0);
            votingMatrix[i][j].set_parameters(0.0, 0.0);
            votingMatrix[i][j].set_count(-1);
        }
    }
    cout<<"Array initialized"<<endl;
    
    ifstream source(in, std::ios::in);
    ofstream target(out, std::ios::out);

    while(source >> x >> y)
    {
        for(int i = 0; i < resolution; i=i+1)
        {
            theta = (float)i * M_PI / 180.0;
            dist  = x * cos(theta) + y * sin(theta);
            if(votingMatrix[i][hough_hash(dist, min, accuracy)].value() < 0)
            {
                votingMatrix[i][hough_hash(dist, min, accuracy)].set_start(x,y);
                votingMatrix[i][hough_hash(dist, min, accuracy)].set_parameters(theta, dist);
                votingMatrix[i][hough_hash(dist, min, accuracy)].increase();
            } else
            {
//                if(votingMatrix[i][hough_hash(dist, min, accuracy)].dist_to(x, y) < 150)
  //              {// attention magic number here ;)
                    votingMatrix[i][hough_hash(dist, min, accuracy)].set_end(x,y);
                    votingMatrix[i][hough_hash(dist, min, accuracy)].increase();
    //            }
            }
        }

    }
    cout<<"Array filled"<<endl;
    for(int i = 0; i < resolution; ++i)
    {
        for(int j = 0; j < hough_size; ++j)
        {
            if(votingMatrix[i][j].value() > 10)
                target << votingMatrix[i][j];
        }
        delete votingMatrix[i];
    }
    delete [] votingMatrix;
}
