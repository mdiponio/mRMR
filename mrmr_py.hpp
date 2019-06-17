/*
Copyright (C) 2019 Michael Diponio
Email: mdiponio@gmail.com

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

#ifndef MRMR_PY
#define MRMR_PY

#include <cstdint>
#include <string>
#include <vector>

#include "dataset.hpp"
#include "mrmr.hpp"

struct mrmr_env {
    dataset<uint8_t> * data;

    int results_size;
    const char ** ranks;
    double * entropy;
    double * mutual_information;
    double * score;

    std::string error;

    mrmr_env(): data( nullptr ), results_size( 0 ), ranks( nullptr ), entropy( nullptr ), 
            mutual_information( nullptr ), score( nullptr ),  error( "" )
    {}

    void clear_results() {
        if ( ranks ) {
            for ( int i = 0; i < results_size; i++ )
                delete ranks[i];

            delete ranks;
        }

        if ( entropy )
            delete entropy;

        if ( mutual_information )
            delete mutual_information;

        if ( score )
            delete score;

        ranks = nullptr;
        entropy = nullptr;
        mutual_information = nullptr;
        score = nullptr;
        results_size = 0;
    }

    ~mrmr_env() {
        clear_results();

        if ( data )
            delete data;
    }
};


#ifdef __cplusplus
extern "C" {
#endif

void * setup_mrmr();
int add_attribute( void * env, const char * name, uint8_t * data, unsigned int length );
int perform_mrmr( void * env, unsigned int label, unsigned int num_features );
const char ** get_feature_ranks( void * env, int * num );
double * get_entropy( void * env, int * num );
double * get_mutual_information( void * env, int * num );
double * get_mrmr_score( void * env, int * num );
const char * get_last_error( void * env );
void destroy_mrmr( void * env );

#ifdef __cplusplus
}
#endif

#endif 