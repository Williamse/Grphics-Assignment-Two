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
*Constructs the Edge table 
*/
void Rasterizer::BuildEdgeTable(vector<AllEdge>& EmptyEdge, int n, int x[], int y[])
{
	//For each vertacie
	for (int vertacie = 0; vertacie < n; vertacie++)
	{
		//Get the first and second coordinate
		int first = vertacie;
		int second = (vertacie  < n - 1) ? vertacie + 1 : 0;
	
		//Build Edge
		AllEdge edge = AllEdge();
		edge.MaxY = this->Max(y[first], y[second]);
		edge.MinY = this->Min(y[first], y[second]);
		edge.X_OfMinY = (edge.MinY == y[first]) ? x[first] : x[second];
		edge.EdgeSlope = this->one_over_slope(y[first], y[second], x[first], x[second]);
		edge.RealSlope = this->slope(y[first], y[second], x[first], x[second]);

		//Add edge
		EmptyEdge.push_back(edge);
	}
}

/*
*Builds a Global Edge table
*/
std::vector<AllEdge> Rasterizer::BuildGlobalEdge(vector<AllEdge> all_edge)
{
	//Build the global edge table
	vector<AllEdge> global_edge;

	//For every edge
	for (int all_e = 0; all_e < all_edge.size(); all_e++)
	{
		//Add the first edge that does not have a slope of zero
		if (global_edge.size() == 0 && all_edge.at(all_e).RealSlope != 0)
		{
			global_edge.push_back(all_edge.at(all_e));
		}
		//After the first edge loop through the global edge table and add edges where applicable
		else if (global_edge.size() != 0 && all_edge.at(all_e).RealSlope != 0)
		{
			int global_edge_size = global_edge.size();
			for (int start = 0; start <= global_edge_size; start++)
			{
				AllEdge cur_edge = all_edge.at(all_e);
				if (start == global_edge_size ||
					cur_edge.MinY < global_edge.at(start).MinY ||
					(cur_edge.MinY == global_edge.at(start).MinY && cur_edge.X_OfMinY <= global_edge.at(start).X_OfMinY))
				{
					global_edge.insert(global_edge.begin() + start, all_edge.at(all_e));
					break;
				}
			}
		}
	}
	return global_edge;
}


/*
* Moves items from the global table to the active table
*
*  active      = Reference to an edge table
*  GlobalEdge  = Reference to the GlobalEdge table
*  Scanline    = The Scanline to use to determine what values should be moved to the active table
*/
void Rasterizer::BuildActiveEdge(vector<AllEdge>& active, vector<AllEdge>& GlobalEdge, int scanline)
{
	vector<AllEdge>*GlobalCopy = new vector<AllEdge>(GlobalEdge);
	int g_size = GlobalEdge.size();
	int deleted = 0;

	//std::vector<AllEdge> 
	for (int x = 0; x < g_size; x++)
	{
		//We have hit the scanline stop searching
		if (GlobalCopy->at(x).MinY != scanline)
		{
			break;
		}
		else
		{
			//Push the next value into the active table and erase from the global table.
			active.push_back(GlobalCopy->at(x));
			GlobalEdge.erase(GlobalEdge.begin() + (x - deleted));
			deleted++;
		}
	}

	//Remove dynamic table
	delete GlobalCopy;
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
	vector<AllEdge> global_edge = BuildGlobalEdge(all_edge);

	//get initial scanline 
	int scanline = global_edge.at(0).MinY;
	int max_scanline = global_edge.back().MaxY;
	int cur_x = 0;

	//active edge table
	vector<AllEdge>* active_edge = new vector<AllEdge>();
	this->BuildActiveEdge(*active_edge, global_edge, scanline);  //this->BuildActiveEdge(global_edge, scanline);
	
	//for every scanline
	for (int cur_scan = scanline; cur_scan < max_scanline; cur_scan++)
	{
		bool parity_even = true;
		int cur_active_index = 0;

		//Go through the scan line ;
		for (int cur_x = active_edge->at(0).X_OfMinY; cur_x <= active_edge->back().X_OfMinY; cur_x++)
		{
			//Are we at an edge
			if (round(active_edge->at(cur_active_index).X_OfMinY) == cur_x)
			{
				//Swap parity value
				parity_even = (parity_even) ? false : true;
				cur_active_index++;
			}

			//If parity is odd draw
			if (!parity_even)
			{
				C.setPixel(cur_x, cur_scan);
			}
		}

		//Update the X value 
		for (int edge = 0; edge < active_edge->size(); edge++)
		{
			active_edge->at(edge).X_OfMinY = active_edge->at(edge).X_OfMinY + active_edge->at(edge).EdgeSlope;
		}

		//Remove any Edges that are going to be at the next scanline
		int active_edge_deleted = 0;
		for (int edge = 0; edge - active_edge_deleted < active_edge->size(); edge++)
		{
			if (active_edge->at(edge - active_edge_deleted).MaxY == cur_scan + 1)
			{
				active_edge->erase(active_edge->begin() + (edge - active_edge_deleted));
				active_edge_deleted++;
			}
		}

		//Look for new values to add to the Edge Table
	    this->BuildActiveEdge(*active_edge ,global_edge, cur_scan + 1);

		//Sort the Active Edge table
		sort(active_edge->begin(), active_edge->end());
		
	}
}

/*
* Maximum of two integer values
*/
int Rasterizer::Max(int one, int two)
{
	return (one >= two) ? one : two;
}

/*
* Minimum Of two integer values
*/
int Rasterizer::Min(int one, int two)
{
	return (one >= two) ? two : one;
}

/*
* Given two points return the value 1/slope
*
* y0  = y zero 
* y1  = y one
* x0  = x zero
* x1  = x one
*/
double Rasterizer::one_over_slope(int y0, int y1, int x0, int x1)
{
	double slope;
	if (y0 == y1){ slope = DBL_MAX; }
	else if(x0 == x1){ slope = 0 ; }
	else 
	{ 
		slope = this->slope(y0,y1,x0,x1);
		slope = 1 / slope;
	}
	return slope;
}

/*
*Given two points returns slope
*
* y0 = Y zero
* y1 = y one 
* x0 = x zero
* x1 = x one
*/
double Rasterizer::slope(int y0, int y1, int x0, int x1)
{
	double slope;
	double top = y0 - y1;
	double bottom = x0 - x1;
	slope = top / bottom;
	return slope;
}