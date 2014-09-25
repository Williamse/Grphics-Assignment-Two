//
//  Rasterizer.cpp
//
//
//  Created by Joe Geigel on 11/30/11.
//  Copyright 2011 Rochester Institute of Technology. All rights reserved.
//



#include "Rasterizer.h"
#include "simpleCanvas.h"

#include <iostream>
using namespace std;

/**
 *
 * Simple class that performs rasterization algorithms
 *
 */


/**
 * Constructor
 *
 * @param n - number of scanlines
 *
 */
Rasterizer::Rasterizer (int n) : n_scanlines (n)
{
}

/**
*Construct the edge table
*/
void Rasterizer::BuildEdgeTable(vector<AllEdge>& EmptyEdge, int n, int x[], int y[])
{
	int num;
	std::cout << "Building Edge Table....." << endl;
	std::cout << ".....Edge Table Contents Should be empty currently: " << EmptyEdge.size() << std::endl;
	std::cout << ".....Vertacies: " << n << std::endl;
	
	//For each vertacie
	for (int vertacie = 0; vertacie < n; vertacie++)
	{
		std::cout << "........Edge " << vertacie << std::endl;
		//Get the first and second coordinate
		int first = vertacie;
		int second = (vertacie  < n - 1) ? vertacie + 1 : 0;
	
		//Build Edge
		AllEdge edge = AllEdge();
		edge.MaxY = this->Max(y[first], y[second]);
		edge.MinY = this->Min(y[first], y[second]);
		edge.X_OfMinY = (edge.MinY == y[first]) ? x[first] : x[second];
		edge.EdgeSlope = this->slope(y[first], y[second], x[first], x[second]);

		//FOR DEBUGGING PURPOSES ONLY DELETE LATER
		edge.x0 = x[first];
		edge.y0 = y[first];
		edge.x1 = x[second];
		edge.y1 = y[second];
		///////////////////////////////////////////

		std::cout <<".........MAX Y: " << edge.MaxY << std::endl;
		std::cout <<".........MIN Y: " << edge.MinY << std::endl;
		std::cout <<".........MIN X: " << edge.X_OfMinY << std::endl;
		std::cout <<".........Slope: " << edge.EdgeSlope << std::endl;
		//Add edge
		EmptyEdge.push_back(edge);
	}
}

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
void Rasterizer::drawPolygon(int n, int x[], int y[], simpleCanvas &C)
{
	//Build the all edge table
	vector<AllEdge> all_edge;
	BuildEdgeTable(all_edge, n, x, y);

	//Build the global edge table
	vector<AllEdge> global_edge;

	for (int all_e = 0; all_e <= all_edge.size(); all_e++)
	{
		//Add initial edge
		if (global_edge.size() == 0 && all_edge.at(all_e).EdgeSlope != 0)
		{
			all_edge.at(all_e).EdgeSlope = (1 / all_edge.at(all_e).EdgeSlope);
			global_edge.push_back(all_edge.at(all_e));
		}
		else
		{
			//GRRRR too tired to figure this shit out and its making me angry
			int global_edge_size = global_edge.size();
			for (int start = 0; start < global_edge_size; start++)
			{
				AllEdge cur_edge = all_edge.at(0);
				if (start == all_edge.size()
					||(cur_edge.MinY <= global_edge.at(start).MinY 
					&& cur_edge.X_OfMinY <= global_edge.at(start).X_OfMinY))
				{
					if ()
					all_edge.at(all_e).EdgeSlope = (1 / all_edge.at(all_e).EdgeSlope);
					global_edge.push_back(all_edge.at(all_e));
					break;
				}

				//Increase index
			}
		}
	}

	//

	std::cout << "Edges : " << all_edge.size() << std::endl;

}

//Helpers
int Rasterizer::Max(int one, int two)
{
	return (one >= two) ? one : two;
}
int Rasterizer::Min(int one, int two)
{
	return (one >= two) ? two : one;
}
//slope of line
double Rasterizer::slope(int y0, int y1, int x0, int x1)
{
	double slope;
	if (y0 == y1){ slope = 0; }
	else if(x0 == x1){ slope = NULL; }
	else { slope = (y0 - y1) / (x0 - x1); }
	return slope;
}