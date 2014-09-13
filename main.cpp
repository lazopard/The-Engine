#include "V3.h"
#include "M33.h"

#include <fstream>
#include <iostream>

using namespace std;

int main() {

    V3 p, o, a;
    cout << "Choose point to rotate: ";
    cin >> p;

    cout << "Choose origin: ";
    cin >> o;
    cout << endl;

    cout << "Choose axis: ";
    cin >> a;
    cout << endl;

    ofstream xf, yf, zf, tf;
    tf.open("theta.csv");
    xf.open("x.csv");
    yf.open("y.csv");
    zf.open("z.csv");
    float theta = -10;
    V3 new_p;
    for(auto i = 0; i < 361; i++) {
        theta += 10;
        new_p = p.rotatePoint(o, a, theta);
        tf << theta << endl;
        xf << new_p[0] << endl;
        yf << new_p[1] << endl;
        zf << new_p[2] << endl;
    }
    xf.close(); yf.close(); zf.close();
}

