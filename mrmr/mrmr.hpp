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

#ifndef MRMR_HPP
#define MRMR_HPP

#include <cmath>
#include <forward_list>
#include <vector>
#include <limits>
#include <string>
#include <vector>

#include "dataset.hpp"
#include "utils.hpp"

struct mrmr_result {
    int rank;
    int index;
    std::string name;
    double entropy;
    double mutual_information;
    double score;

    mrmr_result() {}

    mrmr_result(int rank, int index, std::string name, 
                double entropy, double mutual_information, double score):
        rank(rank), index(index), name(name), 
        entropy(entropy), mutual_information(mutual_information), score(score) { }
};

enum mrmr_method_type : char {
	MID = 0,
	MIQ = 1
};

template<typename T>
std::vector<mrmr_result> mrmr(dataset<T>& data, std::size_t class_attribute = 0, std::size_t num_features = 0, mrmr_method_type method = mrmr_method_type::MID) {

    if ( num_features == 0 )
        num_features = data.num_attributes();
    else
        num_features++;

    // compute mRMR prerequisites
    logger log = *logger::get();
	log.message( "Calculating mutual information between each attribute and class...", INFO, START );

	std::vector<double> mutual_informations( data.num_attributes() );
	std::vector<double> redundance( data.num_attributes(), 0.0 );
	std::forward_list<std::size_t> unselected;
	std::vector<std::size_t> useless;

    std::vector<mrmr_result> result;

	for( std::size_t i = 0; i < data.num_attributes(); ++i ) {
		if( i != class_attribute ) {
			if( data.attribute_entropy( i ) > 0 ) {
				mutual_informations[ i ] = data.mutual_information( class_attribute, i );
				unselected.push_front( i );
			} else {
				mutual_informations[ i ] = 0;
				useless.push_back( i );
			}
		}
	}
	unselected.reverse();
	mutual_informations[ class_attribute ] = -std::numeric_limits<double>::infinity();
    
	log.message( "DONE", INFO, FINISH );
	log.message( "Performing main mRMR computations...", INFO, START );
	
    // class variable
	double class_entropy = data.attribute_entropy( class_attribute );
	result.push_back( mrmr_result( 0, class_attribute, data.attribute_name( class_attribute ),
            class_entropy, class_entropy, std::numeric_limits<double>::quiet_NaN() ) );

	// handle special case of first attribute with highest mutual information
	std::size_t best_attribute_index = 0;
	double max = std::numeric_limits<double>::min();
	for ( auto it = mutual_informations.begin(); it != mutual_informations.end(); it++ ) {
		if ( *it >= max) {
			max = *it;
			best_attribute_index = it - mutual_informations.begin();
		}
	}

	std::size_t last_attribute_index = best_attribute_index;
	unselected.remove( best_attribute_index );

	double mrmr_score = mutual_informations.at( best_attribute_index );
    result.push_back( mrmr_result( 1, best_attribute_index, data.attribute_name( best_attribute_index ),
            data.attribute_entropy( best_attribute_index ), data.attribute_entropy( best_attribute_index ), mrmr_score ) );
	
	// main mRMR computation loop
	std::size_t rank = 2;
	while( !unselected.empty() && rank < num_features ) {
		double best_mrmr_score = -std::numeric_limits<double>::infinity();
		auto it = std::cbegin( unselected );
		auto last_it = unselected.before_begin();
		auto erase_it = last_it;
		while( it != std::cend( unselected ) ) {
			std::size_t attribute_index = *it;
			redundance.at( attribute_index ) += data.mutual_information( last_attribute_index, attribute_index );

			double redundance_value = redundance.at( attribute_index ) / (rank - 1); 
			double mutual_information = mutual_informations.at ( attribute_index );

			if( method == mrmr_method_type::MID ) {
				mrmr_score = mutual_information - redundance_value;	
			} else if( method == mrmr_method_type::MIQ ) {
				mrmr_score = mutual_information / (redundance_value + 0.0001);
			} else {
				log.message( "Invalid MRMR method speicified.", ERROR );
				return result;
			}

			if( mrmr_score >= best_mrmr_score) {
				best_mrmr_score = mrmr_score;
				best_attribute_index = attribute_index;
				erase_it = last_it;
			}

			++it;
			++last_it;
		}

        result.push_back( mrmr_result( rank++, best_attribute_index, data.attribute_name( best_attribute_index ),
            data.attribute_entropy( best_attribute_index ), data.attribute_entropy( best_attribute_index ), best_mrmr_score ) );

		unselected.erase_after( erase_it );
		last_attribute_index = best_attribute_index;
	}

	// finish by outputting useless features
	std::sort( useless.begin(), useless.end() );
	for( auto attribute_index : useless ) {
        if ( rank >= num_features )
            break;

        result.push_back( mrmr_result( rank++, attribute_index, data.attribute_name( attribute_index ), 
                0, 0, std::numeric_limits<double>::infinity() ) );
	}

	log.message( "DONE", INFO, FINISH );
	return result;
}

#endif 