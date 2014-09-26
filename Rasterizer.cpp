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
		edge.EdgeSlope = this->one_over_slope(y[first], y[second], x[first], x[second]);
		edge.RealSlope = this->slope(y[first], y[second], x[first], x[second]);
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

std::vector<AllEdge> Rasterizer::BuildGlobalEdge(vector<AllEdge> all_edge)
{
	//Build the global edge table
	vector<AllEdge> global_edge;

	for (int all_e = 0; all_e < all_edge.size(); all_e++)
	{
		//Add initial edge
		if (global_edge.size() == 0 && all_edge.at(all_e).RealSlope != 0)
		{
			//all_edge.at(all_e).EdgeSlope = (1 / all_edge.at(all_e).EdgeSlope);
			global_edge.push_back(all_edge.at(all_e));
		}
		else if (global_edge.size() != 0 && all_edge.at(all_e).RealSlope != 0)
		{
			//GRRRR too tired to figure this shit out and its making me angry
			int global_edge_size = global_edge.size();
			for (int start = 0; start <= global_edge_size; start++)
			{

				AllEdge cur_edge = all_edge.at(all_e);
				if (start == global_edge_size ||
					cur_edge.MinY < global_edge.at(start).MinY ||
					(cur_edge.MinY == global_edge.at(start).MinY && cur_edge.X_OfMinY <= global_edge.at(start).X_OfMinY))
				{
					//all_edge.at(all_e).EdgeSlope = (1 / all_edge.at(all_e).EdgeSlope);
					global_edge.insert(global_edge.begin() + start, all_edge.at(all_e));
					break;
				}

				//Increase index
			}
		}
	}

	return global_edge;
}


//Build out the global edge table
vector<AllEdge>* Rasterizer::BuildActiveEdge(vector<AllEdge>& GlobalEdge, int scanline)
{
	vector<AllEdge>* active = new vector<AllEdge>();
	vector<AllEdge>*GlobalCopy = new vector<AllEdge>(GlobalEdge);

	int g_size = GlobalEdge.size();
	int deleted = 0;
	//std::vector<AllEdge> 
	for (int x = 0; x < g_size; x++)
	{
		if (GlobalCopy->at(x).MinY != scanline)
		{
			break;
		}
		else
		{
			active->push_back(GlobalCopy->at(x));
			GlobalEdge.erase(GlobalEdge.begin() + (x - deleted));
			deleted++;
		}
	}

	//Remove dynamic call
	delete GlobalCopy;

	return active;
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
	vector<AllEdge>* active_edge = this->BuildActiveEdge(global_edge, scanline);  //this->BuildActiveEdge(global_edge, scanline);

	for (int cur_scan = scanline; cur_scan < max_scanline; cur_scan++)
	{

		//Wrap each scanline change here
		bool parity_even = true;
		int cur_active_index = 0;

		//Go through the scan line ;
		for (int cur_x = active_edge->at(0).X_OfMinY; cur_x < active_edge->back().X_OfMinY; cur_x++)
		{
			//Are we at an edge
			if (active_edge->at(cur_active_index).X_OfMinY == cur_x)
			{
				//Swap parity value
				parity_even = (parity_even) ? false : true;
			}

			//If parity is odd draw
			if (!parity_even)
			{
				C.setPixel(cur_x, scanline);
			}
		}

		for (int edge = 0; edge < active_edge->size(); edge++)
		{
			active_edge->at(edge).X_OfMinY = active_edge->at(edge).X_OfMinY + active_edge->at(edge).EdgeSlope;
		}

		sort(active_edge->begin(), active_edge->end());
		cur_active_index++;
	}
	
	//Increase cur_active_index

	//Loop through all edges
	for (int edge_index = 0; edge_index < global_edge.size(); edge_index++)
	{
			
	}



	/*
	for (int all_e = 0; all_e < all_edge.size(); all_e++)
	{
		//Add initial edge
		if (global_edge.size() == 0 && all_edge.at(all_e).RealSlope != 0)
		{
			//all_edge.at(all_e).EdgeSlope = (1 / all_edge.at(all_e).EdgeSlope);
			global_edge.push_back(all_edge.at(all_e));
		}
		else if (global_edge.size() != 0 && all_edge.at(all_e).RealSlope != 0)
		{
			//GRRRR too tired to figure this shit out and its making me angry
			int global_edge_size = global_edge.size();
			for (int start = 0; start <= global_edge_size; start++)
			{
				
				AllEdge cur_edge = all_edge.at(all_e);
				if ( start == global_edge_size                   ||
					 cur_edge.MinY < global_edge.at(start).MinY  || 
				     (cur_edge.MinY == global_edge.at(start).MinY && cur_edge.X_OfMinY <= global_edge.at(start).X_OfMinY))
				{
					//all_edge.at(all_e).EdgeSlope = (1 / all_edge.at(all_e).EdgeSlope);
					global_edge.insert(global_edge.begin() + start, all_edge.at(all_e));
					break;
				}

				//Increase index
			}
		}
	}
	*/
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

double Rasterizer::slope(int y0, int y1, int x0, int x1)
{
	double slope;
	double top = y0 - y1;
	double bottom = x0 - x1;
	slope = top / bottom;
	return slope;
}