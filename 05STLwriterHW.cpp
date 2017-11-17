#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
using namespace std;

constexpr double pi = 3.1415926535;
#define ONE_S "  "

class Vec3d {
private:
    double x, y, z;
public:
    explicit Vec3d(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}
    // print method for the vector, operator overloading not necessary
    void print() {
        cout << '<' << x << ", " << y << ", " << z << '>' << '\n';
    }
    void incX(int size) {
        x += size;
    }
    void incY(int size) {
        y+= size;
    }
    void incZ(int size) {
        z += size;
    }
    // dot product method
    double dot(const Vec3d& a) {
        return x*a.x + y*a.y + z*a.z;
    }
    // dist method
    double dist(const Vec3d& a) {
        return sqrt(pow(x-a.x, 2) + pow(y-a.y, 2) + pow(z-a.z, 2));
    }
    // scalar multiplication
    friend Vec3d operator * (const Vec3d& a, int b) {
        Vec3d result(a.x*b, a.y*b, a.z*b);
        return result;
    }
    friend Vec3d operator * (int b, const Vec3d& a) {
        return a * b;
    }
    friend Vec3d operator + (const Vec3d& a, const Vec3d& b) {
        Vec3d result(a.x+b.x, a.y+b.y, a.z+b.z);
        return result;
    }
    friend Vec3d operator - (const Vec3d& a, const Vec3d& b) {
        Vec3d result(a.x-b.x, a.y-b.y, a.z-b.z);
        return result;
    }
    friend ostream& operator << (ostream &s, const Vec3d& a) {
        s << a.x << ' ' << a.y << ' ' << a.z;
    }

};

class Face {
private:
    Vec3d a;
    Vec3d b;
    Vec3d c;
public:
    Face(Vec3d a, Vec3d b, Vec3d c) : a(a), b(b), c(c) {}
    void print() {
        cout << a << ' ' << b << ' ' << c << endl;
    }
    Vec3d getA() const {
        return a;
    }

    Vec3d getB() const {
        return b;
    }

    Vec3d getC() const {
        return c;
    }
};

class Shape {
protected: 
    string name;
    double x, y, z;
    vector<Vec3d> vertices;
    vector<Vec3d> normals;
    vector<Face> faces;
public:
    Shape (double x, double y, double z) : x(x), y(y), z(z), name("") {}
    void vertexInfo() const {
        for (auto z : vertices) {
            z.print();
        }
    }
    void faceInfo() const {
        for (auto z : faces) {
            z.print();
        }
    }
    void writeToSTL(string f) const {
        ofstream file;
        file.open(f ,ofstream::app);
        file << "solid " << name << '\n';
        for (int i = 0; i < faces.size(); i++) {
            file << ONE_S << "facet normal " << normals[i] << '\n';
            file << ONE_S << ONE_S << "outer loop" << '\n';
            file << ONE_S << ONE_S << ONE_S << "vertex " << faces[i].getA() << '\n';
            file << ONE_S << ONE_S << ONE_S << "vertex " << faces[i].getB() << '\n';
            file << ONE_S << ONE_S << ONE_S << "vertex " << faces[i].getC() << '\n';
            file << ONE_S << ONE_S << "endloop" << '\n';
            file << ONE_S << "endfacet" << '\n';
        }
        file << "endsolid " << name << '\n';
        file.close();
    }
    virtual void print() const = 0;
};

class Cylinder : public Shape{
private:
	double r, h;
	int facets;
public:
    Cylinder(double x, double y, double z, double r, double h, int facets) : Shape(x, y, z), r(r), h(h), facets(facets) {
        name = "Cylinder";
        double theta = 360.0 / (facets);
        // create top points
        for (int i = 0; i < facets; i++) {
            if (theta * i < 90) {
                // quadrant 1
                Shape::vertices.push_back(Vec3d(r * cos(theta * i * pi/180), r * sin(theta * i * pi/180), z + h));
            } else if(theta * i < 180 && theta * i > 90) {
                // quadrant 2
                Shape::vertices.push_back(Vec3d(-r * cos((180 - theta * i)* pi/180), r * sin((180 - theta * i)* pi/180), z + h));
            } else if(theta * i < 270 && theta * i > 180) {
                // quadrant 3
                Shape::vertices.push_back(Vec3d(-r * cos((theta * i - 180)* pi/180), -r * sin((theta * i - 180)* pi/180), z + h));
            } else {
                // quadrant 4
                Shape::vertices.push_back(Vec3d(r * cos((360 - theta * i)* pi/180), -r * sin((360 - theta * i)* pi/180), z + h));
            }
        }
        // create bottom points
        for (int i = 0; i < facets; i++) {
            if (theta * i < 90) {
                // quadrant 1
                Shape::vertices.push_back(Vec3d(r * cos(theta * i * pi/180), r * sin(theta * i * pi/180), z));
            } else if(theta * i < 180 && theta * i > 90) {
                // quadrant 2
                Shape::vertices.push_back(Vec3d(-r * cos((180 - theta * i)* pi/180), r * sin((180 - theta * i)* pi/180), z));
            } else if(theta * i < 270 && theta * i > 180) {
                // quadrant 3
                Shape::vertices.push_back(Vec3d(-r * cos((theta * i - 180)* pi/180), -r * sin((theta * i - 180)* pi/180), z));
            } else {
                // quadrant 4
                Shape::vertices.push_back(Vec3d(r * cos((360 - theta * i)* pi/180), -r * sin((360 - theta * i)* pi/180), z));
            }
        }

        int size = Shape::vertices.size()/2;
        
        // add top and bottom center points at size * 2 & size * 2 + 1
        Shape::vertices.push_back(Vec3d(x, y, z));
        Shape::vertices.push_back(Vec3d(x, y, z + h));
        for (int i = 0; i <= size - 1; i++) {
            Shape::normals.push_back(Vec3d(0, 0, 0));
            Shape::normals.push_back(Vec3d(0, 0, 0));
            if (i == size - 1) {
                Shape::faces.push_back(Face(Shape::vertices[i], Shape::vertices[0], Shape::vertices[size*2+1]));
                Shape::faces.push_back(Face(Shape::vertices[i + size - 1], Shape::vertices[0 + size], Shape::vertices[size*2]));
            } else {
                Shape::faces.push_back(Face(Shape::vertices[i], Shape::vertices[i + 1], Shape::vertices[size*2+1]));
                Shape::faces.push_back(Face(Shape::vertices[i + size - 1], Shape::vertices[i + size], Shape::vertices[size*2]));
            }
        }
        
        // make faces
        for (int i = 0; i <= size - 1; i++) {
            Shape::normals.push_back(Vec3d(0, 0, 0));
            Shape::normals.push_back(Vec3d(0, 0, 0));
            // if it is last point need to get first one
            if (i == size - 1) {
                Shape::faces.push_back(Face(Shape::vertices[i], Shape::vertices[0], Shape::vertices[size]));
                Shape::faces.push_back(Face(Shape::vertices[i], Shape::vertices[i + size], Shape::vertices[size]));
            } else {
                Shape::faces.push_back(Face(Shape::vertices[i], Shape::vertices[i + size], Shape::vertices[i + size + 1]));
                Shape::faces.push_back(Face(Shape::vertices[i], Shape::vertices[i + 1], Shape::vertices[i + size + 1]));
            }
        }
    }
    void print() const {
        cout << name << ", R: " << r << ", H: " << h << ", at (" << x << ',' << y << ',' << z << ")" << '\n';
    }
};

class Cube: public Shape {
private:
	double size;
public:
    Cube(double x, double y, double z, double size) : Shape(x, y, z), size(size) {
        name = "Cube";

        // initialize all the points
        Shape::vertices.push_back(Vec3d(x + size, y, z + size));
        Shape::vertices.push_back(Vec3d(x, y + size, z + size));
        Shape::vertices.push_back(Vec3d(x + size, y + size, z + size));
        Shape::vertices.push_back(Vec3d(x, y, z + size));
        Shape::vertices.push_back(Vec3d(x, y, z));
        Shape::vertices.push_back(Vec3d(x + size, y + size, z));
        Shape::vertices.push_back(Vec3d(x, y + size, z));
        Shape::vertices.push_back(Vec3d(x + size, y, z));

        // create the 12 faces
        Shape::faces.push_back(Face(Shape::vertices[0], Shape::vertices[1], Shape::vertices[2]));
        Shape::faces.push_back(Face(Shape::vertices[1], Shape::vertices[0], Shape::vertices[3]));
        Shape::faces.push_back(Face(Shape::vertices[4], Shape::vertices[5], Shape::vertices[6]));
        Shape::faces.push_back(Face(Shape::vertices[5], Shape::vertices[4], Shape::vertices[7]));
        Shape::faces.push_back(Face(Shape::vertices[4], Shape::vertices[1], Shape::vertices[3]));
        Shape::faces.push_back(Face(Shape::vertices[1], Shape::vertices[4], Shape::vertices[6]));
        Shape::faces.push_back(Face(Shape::vertices[1], Shape::vertices[5], Shape::vertices[2]));
        Shape::faces.push_back(Face(Shape::vertices[5], Shape::vertices[1], Shape::vertices[6]));
        Shape::faces.push_back(Face(Shape::vertices[5], Shape::vertices[0], Shape::vertices[2]));
        Shape::faces.push_back(Face(Shape::vertices[0], Shape::vertices[5], Shape::vertices[7]));
        Shape::faces.push_back(Face(Shape::vertices[4], Shape::vertices[0], Shape::vertices[7]));
        Shape::faces.push_back(Face(Shape::vertices[0], Shape::vertices[4], Shape::vertices[3]));

        // initialize the normals
        Shape::normals.push_back(Vec3d(0, 0, 1));
        Shape::normals.push_back(Vec3d(0, 0, 1));
        Shape::normals.push_back(Vec3d(0, 0, -1));
        Shape::normals.push_back(Vec3d(0, 0, -1));
        Shape::normals.push_back(Vec3d(0, -1, 0));
        Shape::normals.push_back(Vec3d(0, -1, 0));
        Shape::normals.push_back(Vec3d(1, 0, 0));
        Shape::normals.push_back(Vec3d(1, 0, 0));
        Shape::normals.push_back(Vec3d(0, 1, 0));
        Shape::normals.push_back(Vec3d(0, 1, 0));
        Shape::normals.push_back(Vec3d(-1, 0, 0));
        Shape::normals.push_back(Vec3d(-1, 0, 0));
    }
    void print() const {
        cout << name << ", Size: " << size << ", at (" << x << ',' << y << ',' << z << ")" << '\n';        
    }
};

class CAD {
private:
    vector<Shape*> shapes;
public:
    CAD() : shapes() {}
    ~CAD() {
        for (auto s : shapes) {
            delete s;
        }
    }

    void add(Shape* s) {
        shapes.push_back(s);
    }

    void write(string s) {
        for (auto x : shapes) {
            x->print();
            x->writeToSTL(s);
            x->vertexInfo();
            x->faceInfo();
        }
    }
};

//https://www.stratasysdirect.com/resources/how-to-prepare-stl-files/
//https://www.viewstl.com/
int main() {
	CAD c;
	c.add(new Cube(0,0,0,   5));
	c.add(new Cylinder(100,0,0,    3, 10, 10));
    c.write("test.stl");
}
