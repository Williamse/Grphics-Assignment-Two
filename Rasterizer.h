//
//  Rasterizer.h
//
//
//  Created by Joe Geigel on 11/30/11.
//  Copyright 2011 Rochester Institute of Technology. All rights reserved.
//

#ifndef _RASTERIZER_H
#define _RASTERIZER_H
#include <vector>
#include <math.h>
class simpleCanvas;

//A class that holds information about an edge
typedef struct allEdge
{
	//X and Y values for the edge
	int x0, y0,x1,y1;

	//Minimum y and X values
	int MinY;
	int MaxY;

	//x value at the minimum Y value
	double X_OfMinY;

	//Represents 1 / slope
	double EdgeSlope;

	//Slope of the value
	double RealSlope;

	//Overides the < operator (used when we sort the table after updating the x value)
	bool operator < (const allEdge& str) const
	{
		return (X_OfMinY < str.X_OfMinY);
	}

} AllEdge;

/**
 *
 * Simple class that performs rasterization algorithms
 *
 */
class Rasterizer {

public:
    /**
     * Constructor
     *
     * @param n - number of scanlines
     *
     */
    Rasterizer (int n);

    /**
     * Draw a filled polygon in the simpleCanvas C.
     *
     * The polygon has n distinct vertices.  The coordinates of the vertices
     * making up the polygon are stored in the x and y arrays.  The ith
     * vertex will have coordinate (x[i],y[i]).
     *
     * You are to add the implementation here using only calls
     * to C.setPixel()
     */
    void drawPolygon(int n, int x[], int y[], simpleCanvas &C);

private:
    /**
     * number of scanlines
     */
    int n_scanlines;

	//Builds the Global Edge Table
	std::vector<AllEdge> Rasterizer::BuildGlobalEdge(std::vector<AllEdge> all_edge);

	//Builds the Active Edge Table
	void Rasterizer::BuildActiveEdge(std::vector<AllEdge>& active, std::vector<AllEdge>& GlobalEdge, int scanline);

	/**
	*Build the edge table 
	*/
	void BuildEdgeTable(std::vector<AllEdge>& EmptyEdge, int n, int x[], int y[]);

	//Maximum of two ints
	int Max(int one, int two);
	
	//Minium of two ints
	int Min(int one, int two);

	//One over slope of a line
	double one_over_slope(int y0, int y1, int x0, int x1);

	//Slope of aline
	double slope(int y0, int y1, int x0, int x1);
};


#endif