//
// Created by Snehil Vishwakarma on 2/1/17.

#include <iostream>
#include <vector>
#include <sstream>
#include <math.h>
#include <sys/stat.h>
#include "SImage.h"
#include "SImageIO.h"

#define loop(i, n) for( ; i < n ; ++i )
#define loop0(i, n) for( i = 0 ; i < n ; ++i )
#define param3(t) t.r, t.g, t.b
#define sub(a, b) (a - b)
#define add(a, b) (a + b)
#define sqr(a) (a * a)

using namespace std;

struct rgb
{
    SDoublePlane r, g, b;
    rgb()
    {
        r = SDoublePlane(0, 0);
        g = SDoublePlane(0, 0);
        b = SDoublePlane(0, 0);
    }
    rgb(int rows, int cols)
    {
        r = SDoublePlane(rows, cols);
        g = SDoublePlane(rows, cols);
        b = SDoublePlane(rows, cols);
    }
    void getColorPlane(int k, SDoublePlane** t, int i)
    {
        switch (k)
        {
            case 0: t[i] = &r; break;
            case 1: t[i] = &g; break;
            case 2: t[i] = &b; break;
            default: t[i] = NULL; break;
        }
    }
    void add_padding(int max)
    {
        r.padding(max);
        g.padding(max);
        b.padding(max);
    }
};

struct gaussian
{
    double **val, sigma;
    int ksize, median;
    gaussian(double sig, int ker)
    {
        if (ker % 2 == 0 || ker < 1)
            ksize = -1;
        else
        {
            int i, j;
            double var = sig * sig * 2;
            double gconst = (var * M_PI);
            sigma = sig;
            ksize = ker;
            median = ksize/2;
            val = new double* [ksize];
	          loop0 (i, median+1)
            {
                val[i] = new double [ksize];
		            loop0 (j, median+1)
                {
                    val[i][j] = 1 / (gconst * exp (add(sqr(sub(i, median)), sqr(sub(j, median))) / var));
                    val[i][ksize-1-j] = val[i][j];
                }
                j = ksize-1-i;
                if(i != j)
                {
                    val[j] = new double[ksize];
                    memcpy(val[j], val[i], sizeof(double)*ksize);
                }
            }
            //this->print_gaussian();
        }
    }
    ~gaussian()
    {
        int i;
        loop0 (i, ksize)
            delete []val[i];
    }
    void print_gaussian()
    {
        int i, j;
        loop0 (i, ksize)
        {
            cout << endl;
            loop0 (j, ksize)
                cout << i << " " << j << " " << val[i][j] << endl;
        }
        cout << endl;
    }
};

int main(int argc, char **argv)
{
    try
    {
        if(argc != 3)
        {
            cout << "Incorrect number of arguments; Correct usage:" << endl;
            cout << "    p2 inputfile1 inputfile2" << endl;
            return -1;
        }
        
        int i, j, k, x, y, z, s, med, rcmax;
        double sig;
        vector<rgb> ii;
        SDoublePlane **temp = new SDoublePlane* [6];
        
        // Input and Output png files
        string in1(argv[1]), in2(argv[2]);
        string out = in1.substr(0, in1.length()-4) + " & " + in2.substr(0, in2.length()-4);
        string folder = "./morphed/"+out+"/";
        mkdir( folder.c_str(), 0777);
        folder = folder + "COLORED/";
        mkdir( folder.c_str(), 0777);
        
        // Copying png input values to SDoublePlanes
        ii.resize(2);
        loop0 (i, 2)
             SImageIO::read_png_file(argv[i+1], param3(ii[i]));
        
        // Finding max rows and max columns
        i = ii[0].r.rows(); 
        if (i < ii[0].r.cols())
            i = ii[0].r.cols();
        if (i < ii[1].r.rows())
            i = ii[1].r.rows();
        if (i < ii[1].r.cols())
            i = ii[1].r.cols();
        rcmax = 1;
        while (rcmax < i)
          rcmax <<= 1;
        loop0 (i, 2)
          if ( rcmax > ii[i].r.rows() || rcmax > ii[i].r.cols() )
            ii[i].add_padding(rcmax); 
        
        // Creating intermediate and Output SDoublePlanes
        rgb *t1;
        loop0 (i, 4)
        {
            t1 = new rgb(rcmax, rcmax);
            ii.push_back(*t1);
            t1 = NULL;
        }
        
        s = 11;
        med = 3;
        std::ostringstream ns;
        ns << med;
        folder = folder + "Kernel_" + ns.str() + "/";
        mkdir( folder.c_str(), 0777);
        // Calculating multiple output files for different gaussian parameters
        loop0 (z, 22)
        {
            sig = 3 * ( (double)z + ( (double)s /1.5 ) ) / ( 3 * (double)s );
            gaussian obj( sig, med);
            loop0 (k, 3)
            {
	              loop0 (i, 6)
	    	            ii[i].getColorPlane(k, temp, i);
                i = obj.median;
                loop (i, (rcmax-obj.median))
                {
            	      j = obj.median;
            	      loop (j, (rcmax-obj.median))
            	      {
		                    x = -(obj.median);
            	          loop (x, obj.median+1)
		                    {
            	              y = -(obj.median);
            	              loop (y, obj.median+1)
            	              {
                                (*(temp[2]))[i][j] += ( (obj.val)[x+obj.median][y+obj.median] * (*(temp[0]))[i-x][j-y] );
			                          (*(temp[3]))[i][j] += ( (obj.val)[x+obj.median][y+obj.median] * (*(temp[1]))[i-x][j-y] );     
            	              }
            	          }
		                    (*(temp[4]))[i][j] = ( (*(temp[2]))[i][j] + (*(temp[1]))[i][j] - (*(temp[3]))[i][j])/2;
            	          (*(temp[5]))[i][j] = ( (*(temp[3]))[i][j] + (*(temp[0]))[i][j] - (*(temp[2]))[i][j])/2;
            	      }
                }
            }
            std::ostringstream num;
            num << sig;
            cout << ("sig_" + num.str() + ".png") << " " << med << endl;
            cout << ("sig_" + num.str() + ".png") << " " << med << endl;
            
            SImageIO::write_png_file((folder + "sig_" + num.str() + ".png").c_str(), param3(ii[4]));
            SImageIO::write_png_file((folder + "sig_" + num.str() + ".png").c_str(), param3(ii[5]));
            loop0 (k, 4)
                ii[k+2] = rgb(rcmax, rcmax);
        }
        return 0;
    }
    catch(const string &err)
    {
        cerr << "Error: " << err << endl;
    }
}
