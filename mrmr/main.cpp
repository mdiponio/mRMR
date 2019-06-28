/*
Copyright (C) 2018 by Ryan N. Lichtenwalter
Copyright (C) 2019 by Michael Diponio
Email: rlichtenwalter@gmail.com

This file is part of the Improved mRMR code base.

Improved mRMR is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Improved mRMR is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <cstdlib>
#include <cstring>

#include <errno.h>
#include <getopt.h>
#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>


#include "dataset.hpp"
#include "mrmr.hpp"
#include "utils.hpp"

void short_usage( char const * program ) {
	std::cout << "Usage: " << program << " [OPTION]... [FILE]                                     \n";
	std::cout << "Try '" << program << " --help' for more information.                            \n";
}

void usage( char const * program ) {
	std::cout << "Usage: " << program << " [OPTION]... [FILE]                                     \n";
	std::cout << "Compute mRMR values for attributes in data set, either taking input from        \n";
	std::cout << "standard input or from a file. Input from standard input, named pipes or process\n";
	std::cout << "substitution requires that the number of instances is specified in advance.     \n";
	std::cout << "                                                                                \n";
	std::cout << "  -c, --class=NUM           1-indexed class attribute selection;                \n";
	std::cout << "                            defaults to 1 if not provided                       \n";
	std::cout << "  -d, --discretize=VALUE    one of {round,floor,ceiling};                       \n";
	std::cout << "                            defaults to ceiling if not provided                 \n";
	std::cout << "  -n, --number=NUM          max number of attributes to compute                 \n";
	std::cout << "                            defaults to all attributes                          \n";
	std::cout << "  -l, --verbosity=VALUE     one of {0,1,2,quiet,info,debug};                    \n";
	std::cout << "                            defaults to 0=quiet if not provided                 \n";
	std::cout << "  -m,  --method=VALUE       one of {mid,miq};                                   \n";
	std::cout << "                            defaults to mid if not provided                     \n";
	std::cout << "  -h, --help     display this help and exit                                     \n";
	std::cout << "  -v, --version  output version information and exist                           \n";
}

int main( int argc, char* argv[] ) {
	std::cout << std::scientific;
	std::cerr << std::scientific;

	logger log = *logger::get();

	using storage_type = unsigned char;
	using dataset_type = dataset<storage_type>;

	std::ifstream ifs;
	std::size_t class_attribute = 0;

	dataset_type::discretization_method discretize = dataset_type::ROUND;
	mrmr_method_type method = mrmr_method_type::MID;

	bool just_write = false;

	int num_attributes = 0;

	int c;
	int option_index = 0;
	while( true ) {
		static struct option long_options[] = {
				{ "class", required_argument, 0, 'c' },
				{ "discretize", required_argument, 0, 'd' },
				{ "verbosity", required_argument, 0, 'l' },
				{ "write", no_argument, 0, 'w' },
				{ "number", required_argument, 0, 'n'},
				{ "method", required_argument, 0, 'm'},
				{ "help", no_argument, 0, 'h' },
				{ "version", no_argument, 0, 'v' }
				};
		c = getopt_long( argc, argv, "c:d:l:n:m:whv", long_options, &option_index );
		if( c == -1 ) {
			break;
		}
		switch( c ) {
			case 'c':
				class_attribute = std::strtoul( optarg, nullptr, 10 );
				if( class_attribute == 0 || errno == ERANGE ) {
					std::cerr << argv[0] << ":  -c, --class=NUM  class attribute out of range\n";
					return 1;
				}
				--class_attribute;
				break;

			case 'd':
				if( strcmp( optarg, "round" ) == 0 ) {
					discretize = dataset_type::ROUND;
				} else if( strcmp( optarg, "floor" ) == 0 ) {
					discretize = dataset_type::FLOOR;
				} else if( strcmp( optarg, "ceiling" ) == 0 ) {
					discretize = dataset_type::CEILING;
				} else {
					std::cerr << argv[0] << ": -d --discretize=VALUE  must be one of one of {round,floor,ceiling}\n";
					return 1;
				}
				break;

			case 'l':
				if( strcmp( optarg, "0" ) == 0 || strcmp( optarg, "quiet" ) == 0 ) {
					log.set_level(QUIET);
				} else if( strcmp( optarg, "1" ) == 0 || strcmp( optarg, "info" ) == 0 ) {
					log.set_level(INFO);
				} else if( strcmp( optarg, "2" ) == 0 || strcmp( optarg, "debug" ) == 0 ) {
					log.set_level(DEBUG);
				} else {
					std::cerr << argv[0] << ": " << "  -l, --verbosity=[VALUE]  one of {0,1,2,quiet,info,debug}; defaults to 0=quiet\n";
					short_usage( argv[0] );
					return 1;
				}
				break;

			case 'n':
				num_attributes = std::strtol( optarg, nullptr, 10 );
				break;

			case 'w':
				just_write = true;
				break;

			case 'h':
				usage( argv[0] );
				return 0;

			case 'm':
				if( strcmp( optarg, "mid" ) == 0) {
					method = mrmr_method_type::MID;
				} else if ( strcmp( optarg, "miq" ) == 0 ) {
					method = mrmr_method_type::MIQ;
				} else {
					std::cerr << argv[0] << ": " << "-m, --method=[VALUE]  one of {mid,miq}; defaults to MID" << std::endl;
					return 1;
				}
				break;

			case 'v':
				std::cout << "mrmr by Ryan N. Lichtenwalter, Michael Diponio v0.2 (BETA)\n";
				return 0;

			default:
				short_usage( argv[0] );
				return 1;
		}
	}

	if( optind < argc ) {
		if( optind == argc - 1 ) {
			ifs = std::ifstream( argv[optind] );
			log.message( (std::string( "FILE = " ) + std::string( argv[optind] )).c_str(), DEBUG, STANDARD );
		} else {
			std::cerr << argv[0] << ": " << "too many arguments\n";
			short_usage( argv[0] );
			return 1;
		}
	}

	// read data
	log.message( "Reading and transforming dataset and computing attribute information...", INFO, START ); 

	dataset_type data;
	if( ifs.is_open() ) {
		log.message( "Reading from file...", DEBUG, STANDARD );
		data = dataset_type( ifs, discretize );
	} else {
		log.message( "Reading from standard input...", DEBUG, STANDARD );
		data = dataset_type( std::cin, discretize );
	}
	log.message( "DONE", INFO, FINISH );

	if( just_write ) {
		log.message( "Writing dataset out standard output...", INFO, START );
		std::cout << data;
		log.message( "DONE", INFO, FINISH );
		return 0;
	}

	// perform MRMR
	std::vector<mrmr_result> results = mrmr<unsigned char>( data, class_attribute, num_attributes, method );

	// print output
	std::string cols[] = {
		"Rank", "Index", "Name", "Entropy", "Mutual Information", "mRMR score"
	};
	std::size_t col_widths[] = { 5, 6, 14, 14, 19, 14} ;

	for ( std::size_t i = 0 ; i < data.num_attributes(); i++ ) {
		if ( data.attribute_name( i ).size()  + 1 > col_widths[2] ) 
			col_widths[2] = data.attribute_name( i ).size() + 1;
	}

	for ( std::size_t i = 0; i < 6; i++) {
		std::cout << std::setw(col_widths[i]) << cols[i];
	}
	std::cout << std::endl;

	for (auto r : results) {
		std::cout << std::setw( col_widths[0] ) << r.rank
				  << std::setw( col_widths[1] ) << r.index 
				  << std::setw( col_widths[2] ) << r.name 
				  << std::setw( col_widths[3] ) << r.entropy
				  << std::setw( col_widths[4] ) << r.mutual_information
				  << std::setw( col_widths[5] ) << r.score 
				  << std::endl;
	}
}
