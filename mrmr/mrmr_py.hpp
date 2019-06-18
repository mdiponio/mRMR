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
#include <stdexcept>
#include <string>
#include <vector>

#include "dataset.hpp"
#include "mrmr.hpp"

enum data_type {
    uint8_type,
    int32_type,
};

struct mrmr_env {
    dataset<uint8_t> * data_uint8;
    dataset<int32_t> * data_int32;

    data_type type;

    int results_size;
    const char ** ranks;
    double * entropy;
    double * mutual_information;
    double * score;

    std::string error;

    mrmr_env( data_type type ): data_uint8( nullptr ), data_int32( nullptr ), type( type ),
            results_size( 0 ), ranks( nullptr ), entropy( nullptr ), 
            mutual_information( nullptr ), score( nullptr ),  error( "" )
    { }

    void init_data() {
        switch ( type )
        {
        case uint8_type:
            data_uint8 = new dataset<uint8_t>();
            break;

        case int32_type:
            data_int32 = new dataset<int32_t>();
            break;

        default: 
            throw std::invalid_argument("invalid data type specified");
        }
    }

    std::size_t num_attributes() {
        switch ( type )
        {
        case uint8_type:
            return data_uint8 ? data_uint8->num_attributes() : 0;

        case int32_type:
            return data_int32 ? data_int32->num_attributes() : 0;

        default: 
            throw std::invalid_argument("invalid data type specified");
        }
    }

    bool has_data() {
        switch ( type )
        {
        case uint8_type:
            return !! data_uint8;

        case int32_type:
            return !! data_int32;

        default: 
            throw std::invalid_argument("invalid data type specified");
        }

    }

    void clear_results() {
        if ( ranks ) {
            for ( int i = 0; i < results_size; i++ )
                delete ranks[i];

            delete [] ranks;
        }

        if ( entropy )
            delete [] entropy;

        if ( mutual_information )
            delete [] mutual_information;

        if ( score )
            delete [] score;

        ranks = nullptr;
        entropy = nullptr;
        mutual_information = nullptr;
        score = nullptr;
        results_size = 0;
    }

    ~mrmr_env() {
        clear_results();

        if ( data_uint8 ) 
            delete data_uint8;

        if ( data_int32 )
            delete data_int32;

        data_uint8 = nullptr;
        data_int32 = nullptr;
    }
};




#ifdef __cplusplus
extern "C" {
#endif

void * setup_mrmr( data_type type );
int add_attribute_byte( void * env, const char * name, uint8_t * data, unsigned int length );
int add_attribute_int( void *env, const char * name, int32_t * data, unsigned int length );
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