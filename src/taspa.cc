/* 
 * Copyright 2009, 2010, Jake Askeland, jake(dot)askeland(at)gmail(dot)com
 * 
 *  * This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *  * This file is part of Topological all shortest paths automatique' (TASPA).
 * 
 *     TASPA is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     TASPA is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with TASPA.  If not, see <http://www.gnu.org/licenses/>.
 */
 
/* Image processing support */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_SDL_IMAGE
#include <SDL/SDL_image.h>           // Include before bmp.h for Jpeg support
#endif

#include "bitmap/bmp.h"		         // Basic image processing via bitmap object

/* User interface support */
#include "user_interface/ui.h"

/* Word combinatorics and graph manipulators */
#include "region/region.h"	                // Region detection support
#include "polygon/polygon.hpp"              // Polygon object
#include "polygon/AdjacencyMatrix.h"        // Matrix representation of graphs
#include "thorup/thorup.h"                  // Integer weight pathfinding

/* Misc. utilities */
#include "stopwatch/Stopwatch.h"            // For run time analysis
#include <fstream>                          // For standard and file I/O
#include <time.h>                           // rand() support
#include <stdlib.h>                         // rand() support
#include <sstream>			                // String stream support
#include "std_extensions/stream_objects.h"  // For onullstream object

extern onullstream null_ostream;            // For non-verbose (null) output

int main( int argc, char* argv[] ) {
	
	////////////////////////////////////////////////////////////////
	// Initialize preferred ostream object to stdout
    //std::ostream out = std::cout;
	
	////////////////////////////////////////////////////////////////
	// Initialize random number generator and timer
	srand(time(NULL));
	Stopwatch watch;

	////////////////////////////////////////////////////////////////
	// Initialize colors for output marker
	const rgba rgbPurple ( 200,   0, 200 );
	const rgba rgbYellow (   0, 223, 255 );
	const rgba rgbOrange (  14, 148, 241 );
	const rgba rgbRed    (   0,   0, 255 );
	const rgba rgbOffRed ( 127, 127, 255 );
	const rgba rgbBlue   ( 255,   0,  32 );
	const rgba rgbGreen  (   0, 255,   0 );
	const rgba rgbAqua   ( 180, 221,  64 );


	////////////////////////////////////////////////////////////////
	// Initialize command argument -> distiller function pointer map
	std::map<std::string, Distiller> DistillerNames;
	DistillerNames.insert(std::make_pair("luminance",       &Luminance));
	DistillerNames.insert(std::make_pair("avg_luminance",   &AvgLuminance));
	DistillerNames.insert(std::make_pair("color_variation", &ColorVariation));
	DistillerNames.insert(std::make_pair("lum_diff",        &LumDiff));
	DistillerNames.insert(std::make_pair("footprint",       &Footprint));


	////////////////////////////////////////////////////////////////
	// Initialize various vertex counting variables
	size_t boundaryTileCount    = 0;

	size_t m_0    = 0;      // Pre-partitioning vertex count
	size_t n_0    = 0;      // Pre-partitioning edge count
	float  rho_0  = 0.0;    // Pre-partitioning graph density

	size_t m_1    = 0;      // Post-partitioning vertex count
	size_t n_1    = 0;      // Post-partitioning edge count
	float  rho_1  = 0.0;    // Post-partitioning graph density


	////////////////////////////////////////////////////////////////
	// Initialize various time counting variables
	float partTime  = 0.0;  // Partitioning time
	float pathTime  = 0.0;  // Path-finding time


	////////////////////////////////////////////////////////////////
	// Declare command line input (cli) variables
	std::string inFilename;
	std::string outFilename;
	std::string logFilename;
	std::string reportFilename;
	std::string distillerName;
	
	bool appendToLog = false;
	bool verbose	 = false;
	bool logToStdout = false;
	bool saveLots	 = false;
	bool saveReport  = false;
	bool savePaths   = false;
	
	////////////////////////////////////////////////////////////////
	// Fill command line input variables from argv

	if ( GetCommandLineInput (inFilename, outFilename, logFilename, 
		reportFilename, distillerName, 
		appendToLog, verbose, logToStdout, saveLots, saveReport, savePaths,
		argc, argv) == false ) return 1;
    
    //if (verbose) out = std::cout;


	////////////////////////////////////////////////////////////////
    // If we can open file outFilename, it already exists and we 
    // may attempt to allow to use the input file as a mask for this
    // already existing output file.

    bool attemptMask = false;
    std::string nonMaskFilename = inFilename;
    {
        std::ifstream outfileTest(outFilename.c_str());
        if (outfileTest) {
            outfileTest.close();
            attemptMask = true;
            nonMaskFilename = outFilename;
        }
    }
    
    
	////////////////////////////////////////////////////////////////
    // Remove output filename extension, ensure a "." as last char

    size_t dot = outFilename.find_last_of(".");
    if (dot == std::string::npos) {
        outFilename.append(".");
        dot = outFilename.find_last_of(".");
    }
    outFilename = outFilename.substr(0,dot+1);


	////////////////////////////////////////////////////////////////
	// Open log file
	std::ofstream logfile;
	if (appendToLog) { 
		logfile.open(logFilename.c_str(), std::ios::app);
		if (!logfile) { std::cerr << "Can't open " << logFilename << "\n"; }
	}

	////////////////////////////////////////////////////////////////
	// Open report file
	std::ofstream reportFile;
	if (saveReport) { 
		reportFile.open(reportFilename.c_str(), std::ios::app);
		if (!reportFile) { std::cerr << "Can't open " 
			<< reportFilename << "\n"; }
	}	
	

	////////////////////////////////////////////////////////////////
	// Initialize bitmap objects

	// Create bitmap object
	bitmap* inputBmp  = 0;
	bitmap* outputBmp = 0;
	
	if (verbose) std::cout << "Loading " << inFilename << "...\n";
	if (appendToLog) logfile << "Loading " << inFilename << "...\n";

	try { 
		if (DistillerNames.find(distillerName) == DistillerNames.end()) {
			distillerName = "luminance";
		}
			
		inputBmp  = OpenBitmap(inFilename, DistillerNames[distillerName]); 
		outputBmp = OpenBitmap(nonMaskFilename, DistillerNames["luminance"]); 

        // If we're attempting a mask and the input and output bitmaps
        // are a different size, abort the attempt and use a copy of the 
        // input bitmap for output.
        if (attemptMask && outputBmp->max() != inputBmp->max()) {
            delete outputBmp;
            outputBmp = OpenBitmap(inFilename, DistillerNames["luminance"]); 
        }
	
	} catch (catch_all_exception e) { std::cerr << e.what() << std::endl; }
	

	////////////////////////////////////////////////////////////////
	// Extract boundaries from bitmap object

	if (verbose) std::cout << "Extracting boundaries from bitmap object...\n";
	
	// Append timestamp to log file and/or stdout
	if (appendToLog) {
		logfile << TimeStamp() << " :: Begin boundary detection.\n";
	}
	watch.Start();

    SquareLatticeWalker walker;
    if (verbose) walker.initialize(inputBmp, std::cout);
    else walker.initialize(inputBmp, null_ostream);

	if (appendToLog) {
		logfile << TimeStamp() << " :: End boundary detection :: "
		<< watch.Lap() << std::endl;
	}

	// Store the number of boundary tiles
	boundaryTileCount = walker.size();
	
	
	////////////////////////////////////////////////////////////////
	// Extract vertices from boundaries
	
	location::Set linV;		// Set of all linear vertices found
	location::Set cvxV;     // Set of all convex vertices found
	region::List rvList;	// List of each set of boundary-seperated vertices
	location::Vector cvx;
	location::Vector ccv;
	location::Vector straight;

	if (verbose) 
        std::cout << "Parsing boundary tiles for convex curvature...\n";
	if (appendToLog) {
		logfile << TimeStamp() << " :: Begin convex curvature detection.\n";
	}
	watch.Start();
	
	bool storeWords = appendToLog && saveLots;

	try { 
        if (verbose) {
            ParseBoundary(linV, cvxV, rvList, inputBmp, walker, cvx, ccv,
                straight, std::cout, storeWords, logfile); 
        }
        else {
            ParseBoundary(linV, cvxV, rvList, inputBmp, walker, cvx, ccv,
                straight, null_ostream, storeWords, logfile); 
        }
	} catch (catch_all_exception e) { std::cerr << e.what() << std::endl; }	
		
	if (appendToLog) {
		logfile << TimeStamp() << " :: End convex curvature detection :: "
			<< watch.Lap() << std::endl;
	}

	////////////////////////////////////////////////////////////////
	// mark the vertices to be used in path-finding in purple

    std::string outputLineFilename = outFilename + "cnvx.bmp";

    {
        location::Vector lv(cvxV.begin(), cvxV.end());
        outputBmp->MarkVertices(ccv, rgbBlue);
        outputBmp->MarkVertices(cvx, rgbRed);
        outputBmp->MarkVertices(lv, rgbPurple);
        outputBmp->MarkVertices(straight, rgbOrange);

        // Write output bitmap
        if (verbose) std::cout 
            << "Marking convex vertices to "
            << outputLineFilename << "\n";
        try { outputBmp->WriteBitmapFile(outputLineFilename); }
        catch (catch_all_exception e) { std::cerr << e.what() << std::endl; }	
    }


	////////////////////////////////////////////////////////////////
	// Open output file in gimp
//    std::string openGimp = "gimp " + outputLineFilename;
//    system(openGimp.c_str());
//    return 0;
    

	////////////////////////////////////////////////////////////////
	// Construct adjacency matrix
	
	if (verbose) std::cout << "Determining edge weights...\n"; 

	if (appendToLog) {
		logfile << TimeStamp() << " :: Begin adjacency matrix construction.\n";
	}

    // Initialize adjacency matrix
	AdjacencyMatrix M;
	if (verbose) M.initialize(cvxV,rvList,inputBmp,std::cout);
	else M.initialize(cvxV,rvList,inputBmp,null_ostream);

	m_0     = M.TotalEdgeCount();
	n_0     = cvxV.size();
	rho_0   = (float)m_0 / (float)n_0;

	if (appendToLog) {
		logfile << TimeStamp() << " :: End adjacency matrix construction :: "
			<< watch.Lap() << std::endl;		
	}
	
	location::Vector mv;


	////////////////////////////////////////////////////////////////
    // Fill mv with the approximate minimum set of vertices required for
    // pathfinding.

    // Create a copy of M such that only convex vertices are used.
    //M.RemoveConcaveVertices();
    mv = M.GetConvexVertices();
    AdjacencyMatrix A(M);    
    assert(A.GetVertices() == mv);

	m_1    = A.TotalEdgeCount();
	n_1    = mv.size();
	rho_1  = (float)m_1 / (float)n_1;
	if (verbose) {
	    std::cout << n_1 << " vertices and " << m_1 << " edges.\n";
	    long t = m_1*n_1 + n_1*n_1;
	    float l = log10f(t);
	    std::cout << "Op count will be on the order of 10^" << l << "\n";
	}


	////////////////////////////////////////////////////////////////
	// Test runtime of pathfinding with polygon analysis
    /*  choose a couple vertices and find the shortest  */
    /*  path from one to the other */

    // create a matrix with entries leading one through a path from
    // the i'th entry to the j'th entry.
    PathMatrix P(mv, inputBmp);

	{
        if (verbose) std::cout 
            << "Control-C to quit. All output to this point is saved.\n"
            << "Generating all-pairs shortest paths' lookup table...\n"; 

        if (appendToLog) {
            logfile << TimeStamp() 
                << " :: Begin making all-pairs shortest paths' lookup table.\n";
        }
        watch.Start();

        if (verbose) {
            // Thorup's method  O(n^2 + nm)	
            ThorupPaths(P,A,std::cout);
        }
        
        else {
            // Thorup's method  O(n^2 + nm)
            ThorupPaths(P,A,null_ostream);
        }

        pathTime = watch.Lap();
        
        if (appendToLog) {
            logfile << TimeStamp() 
                << " :: End making all-pairs shortest paths' lookup table :: "
                << pathTime << std::endl;		
        }

        bool savePathLines = appendToLog && saveLots;

        for (int i = 0; i < 1; i ++)
            MarkAPath(P, inputBmp, outputBmp, mv, savePathLines, logfile);

        if (savePaths) {
    
            std::string pathMatrixFilename = 
                outFilename + "path";
            
            if (verbose) std::cout << "Writing path matrix to "
                << pathMatrixFilename << "\n";
  
            
            std::ofstream pathMatrixFile
                (pathMatrixFilename.c_str(), std::ios::binary);
            
            if (!pathMatrixFile) {
                std::cerr << "Path matrix file i/o error.";
            }
            
            else {
                P.SaveToDisk(pathMatrixFile);
                pathMatrixFile.close();
            }
        }
	}


	////////////////////////////////////////////////////////////////
	// mark the vertices to be used in path-finding in green

	outputBmp->MarkVertices(mv, rgbGreen);
	

	////////////////////////////////////////////////////////////////
	// Begin cleanup

	if (appendToLog) logfile.close();
	if (appendToLog) { 
		logfile.open(logFilename.c_str(), std::ios::app);
		if (!logfile) { std::cerr << "Cannot open " << logFilename << "\n"; }
	}
	
	// Write output bitmap
    std::string outPathFilename = 
        outFilename + "path.bmp";
	if (verbose) std::cout <<"Writing image with a path to "
        << outPathFilename << "\n";
	try { 
		outputBmp->WriteBitmapFile(outPathFilename); 
	} catch (catch_all_exception e) { std::cerr << e.what() << std::endl; }	
	

	////////////////////////////////////////////////////////////////
	// Finish cleaning up

	if (appendToLog) {
		logfile << "\n\n";
		logfile.close();
	}
	if (saveReport) {
        
        std::streampos begin = 0;
        if (reportFile.tellp() == begin) {
            reportFile << "timestamp,filename,n_0,m_0,rho_0,n_1,m_1,rho_1,"
                    << "partition_time,path_time\n";
        }
        
        reportFile << TimeStamp() << ',' << inFilename << ',' 
            << n_0 << ',' << m_0 << ',' << rho_0 << ','
            << n_1 << ',' << m_1 << ',' << rho_1 << ','
            << partTime << ',' << pathTime << '\n';
		
		reportFile.close();
	}
	
	delete inputBmp;
	delete outputBmp;


	////////////////////////////////////////////////////////////////
	// Open output file in gimp
//    std::string openGimp = "gimp " + outPathFilename;
//    system(openGimp.c_str());

	
	////////////////////////////////////////////////////////////////
	// Exit with success
	if (verbose) std::cout << "Completed successfully.\n";
	return 0;

	////////////////////////////////////////////////////////////////
}
