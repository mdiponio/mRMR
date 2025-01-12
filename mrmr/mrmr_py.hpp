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

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

enum data_type: char {
    uint8_type = 0,
    uint16_type = 1,
    int32_type = 2,
};

struct mrmr_env {
    dataset< uint8_t > * data_uint8;
    dataset< uint16_t > * data_uint16;
    dataset< int32_t > * data_int32;

    data_type type;

    int results_size;
    const char ** ranks;
    double * entropy;
    double * mutual_information;
    double * score;

    std::string error;

    mrmr_env( data_type type ): data_uint8( nullptr ), data_uint16( nullptr ), data_int32( nullptr ), type( type ),
            results_size( 0 ), ranks( nullptr ), entropy( nullptr ), 
            mutual_information( nullptr ), score( nullptr ),  error( "" )
    { }

    void init_data() {
        switch ( type )
        {
        case uint8_type:
            data_uint8 = new dataset< uint8_t >();
            break;

        case uint16_type:
            data_uint16 = new dataset< uint16_t >();
            break;

        case int32_type:
            data_int32 = new dataset< int32_t >();
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

        case uint16_type:
            return data_uint16 ? data_uint16->num_attributes() : 0;

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

        case uint16_type:
            return !! data_uint16;

        case int32_type:
            return !! data_int32;

        default: 
            throw std::invalid_argument("invalid data type specified");
        }

    }

    void clear_results() {
        if( ranks ) {
            for ( int i = 0; i < results_size; i++ )
                delete ranks[i];

            delete [] ranks;
        }

        if( entropy )
            delete [] entropy;

        if( mutual_information )
            delete [] mutual_information;

        if( score )
            delete [] score;

        ranks = nullptr;
        entropy = nullptr;
        mutual_information = nullptr;
        score = nullptr;
        results_size = 0;
    }

    ~mrmr_env() {
        clear_results();

        if( data_uint8 ) 
            delete data_uint8;

        if( data_uint16 )
            delete data_uint16;

        if( data_int32 )
            delete data_int32;

        data_uint8 = nullptr;
        data_uint16 = nullptr;
        data_int32 = nullptr;
    }
};

extern "C" {
	DLL_EXPORT void * setup_mrmr(data_type type);
	DLL_EXPORT int add_attribute_uint8(void * env, const char * name, uint8_t * data, std::size_t length);
	DLL_EXPORT int add_attribute_uint16(void * env, const char * name, uint16_t * data, std::size_t length);
	DLL_EXPORT int add_attribute_int32(void *env, const char * name, int32_t * data, std::size_t length);
	DLL_EXPORT int perform_mrmr(void * env, mrmr_method_type method, unsigned int label, unsigned int num_features);
	DLL_EXPORT const char ** get_feature_ranks(void * env, int * num);
	DLL_EXPORT double * get_entropy(void * env, int * num);
	DLL_EXPORT double * get_mutual_information(void * env, int * num);
	DLL_EXPORT double * get_mrmr_score(void * env, int * num);
	DLL_EXPORT const char * get_last_error(void * env);
	DLL_EXPORT void destroy_mrmr(void * env);
}

#endif 