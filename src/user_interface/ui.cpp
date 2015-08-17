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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ui.h"
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

const char command_message[] = "\
TASPA is an image processing program using arbitrary boundary detection\n\
algorithms to generate an all-pairs shortest paths matrix in the ground\n\
space of a bitmap.\n\
\n\
TASPA is designed to demonstrate a vertex thinning algorithm, using word\n\
combinatorics to find the minimum set of convex vertices. It then applies\n\
Thorup's SSSP algorithm from each vertex to generate a matrix whose entries\n\
[a_ij] are the next edge to traverse from vertex i to vertex j.\n\n";

const char brief_usage[] = "Brief USAGE: \n\
	taspa [-l <log_file>] [-r <report_file>] [-d <distiller_name>] \n\
	      [-w] [-s] [-v] [-h] [-p] [--] <input_image> <output_image>\n\n";


const char extended_usage[] = "Where: \n\
\n\
   -l <log_file>        Append log events to [log_file].\n\
   -r <report_file>     Append report to [report_file].\n\
   -d <distiller_name>  Specify a distiller.\n\
   -w  Consider boundary words as log events (needs -l).\n\
   -s                   Send log events to stdout.\n\
   -v                   Print details to stdout.\n\
   --                   Ignores further arguments.\n\
   -h                   Displays usage information and exits.\n\
   -p                   Save all pairs path matrix to <output_image>.path\n\
   <input_image>        (required)  Input image filename.\n\
   <output_image>       (required)  Output image filename, no extension.\n\
	\n\n";
   

void PrintSyntax(char* argv0, char opt) {
	
	std::string briefUsage(brief_usage);

	switch (opt) {
		case '?' : std::cerr << briefUsage; break;
		case 'h' : std::cerr 
			<< brief_usage << extended_usage << command_message; break;
	}
	
	std::cerr << "Version 1.2.0\n";
	
    #ifdef HAVE_SDL_IMAGE
	fprintf(stderr, "This build of taspa uses the SDL_image library.\n\n"); 
	#else
	fprintf(stderr, 
		"This build of taspa does not use the SDL_image library.\n\n");
	#endif
}


bool GetCommandLineInput ( std::string& inFilename, std::string& outFilename, 
		std::string& logFilename, std::string& reportFilename, std::string&
		distillerName,
		bool& appendToLog, bool& verbose, bool& logToStdout, bool& saveLots, 
		bool& saveReport, bool& savePaths, int argc, char* argv[] ) {
	
	////////////////////////////////////////////////////////////
	/* Get command line arguments */

	char c;
	opterr = 0;

	while ((c = getopt (argc, argv, "l:r:d:pvswh")) != -1)
	 switch (c)
	   {
	   case 'v': verbose = true;              break;
	   case 's': logToStdout = true;          break;
	   case 'w': saveLots = true;             break;
      case 'p': savePaths = true;            break;
	   case 'l': logFilename = optarg;        break;
	   case 'r': reportFilename = optarg;     break;
	   case 'd': distillerName = optarg;      break;
	   case 'h': PrintSyntax(argv[0],c); return false;
	   case '?':
		 if (optopt == 'l' || optopt == 'r' || optopt == 'd')
		   fprintf (stderr, "Option -%c requires an argument.\n", optopt);
		 
		 else if (isprint (optopt))
		   fprintf (stderr, "Unknown option `-%c'.\n", optopt);
		 
		 else
		   fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
	     
	     PrintSyntax(argv[0],c);
		 return false;
	   
	   default:
	     PrintSyntax(argv[0],'h');
		 return false;
	   }

	if (argc - optind >= 2 && argc > 2) {
		inFilename      = argv[optind];
		outFilename     = argv[optind+1];
		
		if (outFilename.substr(outFilename.length()-3, outFilename.length()-1)
                != "bmp") {
          fprintf(stderr, "Output file type must be a 24-bit .bmp bitmap.\n\n");
          return false;
        }
		appendToLog = logFilename != "";
		saveReport  = reportFilename != "";
	}
	
	else {
		fprintf(stderr, "Please supply an input and an output image name.\n\n");		
		PrintSyntax(argv[0],'?');
		return false;
	}
	
	return true;
}

