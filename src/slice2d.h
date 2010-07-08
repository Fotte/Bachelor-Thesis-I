typedef struct {
    double phi;
    double r;
} PointPolar;

typedef struct {
    double x;
    double y;
} PointCartesian;

bool operator<(const PointCartesian &p1, const PointCartesian &p2)
{
    return (p1.x < p2.x && p1.y < p2.y); 
};

class slice2d {
    public:
            static void slice_3d_map(double from, double to, char *file);
            static void captn_hough(char *file); // hough transformation for line extraction from point clouds
            static void delta_method(char *file, double delta); // find holes in data 
			static void scan_sort_polar(char *readfile, char *writefile); //reading input scan, sorting data among angle and writing output into file
			static void scan_sort_cart(char *readfile, char *writefile); //reading input scan, sorting data among first coords and writing output into file
            static bool compare_polar_points(PointPolar &a, PointPolar &b);
            static bool compare_cart_points(PointCartesian &a, PointCartesian &b);
            static void find_lines(char *file);
            static void find_lines_captnHough(char *file);
            static double dist_polar(double phi1, double r1, double phi2, double r2);
            static double dist_polar(PointPolar &p1, PointPolar &p2);
            static double dist_cart(double x1, double y1, double x2, double y2);
            static double dist_cart(PointCartesian &p1, PointCartesian &p2);
            static void find_circuit(char *readfile, char *writefile);
};
