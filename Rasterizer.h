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
class simpleCanvas;

typedef struct allEdge
{
	//
	//FOR DEBUG ONLY  REMOVE WHEN FINISHED
	//
	int x0, y0,x1,y1;


	int MinY;
	int MaxY;
	int X_OfMinY;
	double EdgeSlope;
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

	/**
	*Build the edge table 
	*/
	void BuildEdgeTable(std::vector<AllEdge>& EmptyEdge, int n, int x[], int y[]);

	//Helpers//
	int Max(int one, int two);
	int Min(int one, int two);
	double slope(int y0, int y1, int x0, int x1);


};


#endif
