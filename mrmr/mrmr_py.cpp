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

#include <cstring>
#include <iostream>

#include "mrmr_py.hpp"

void * setup_mrmr( data_type type ) {
    if ( type >= uint8_type && type <= int32_type )
        return new mrmr_env( type );
    else 
        return nullptr;
}

int add_attribute_uint8( void * env, const char * name, uint8_t * data, std::size_t length ) {
    mrmr_env * m_env = static_cast< mrmr_env * >( env );

    if ( ! m_env->has_data() ) {
        m_env->init_data();
    }

    std::string name_s(name);

    int ret = 0;
    
    uint16_t * u_tmp;
    int32_t * i_tmp;

    switch ( m_env->type )
    {
        case uint8_type:
            ret = m_env->data_uint8->set_attribute( name_s, data, length );
            break;

        case uint16_type:
            u_tmp = new uint16_t[ length ];
            for ( std::size_t i = 0; i < length; i++ )
                u_tmp[i] = data[i];

            ret = m_env->data_uint16->set_attribute( name_s, u_tmp, length );
            delete [] u_tmp;
            break;

        case int32_type:
            i_tmp = new int32_t[ length ];
            for ( std::size_t i = 0; i < length; i++ )
                i_tmp[i] = data[i];

            ret = m_env->data_int32->set_attribute( name_s, i_tmp, length );
            delete [] i_tmp;
            break;

        default: 
            throw std::invalid_argument("invalid data type specified");

    }

    return ret;
}

int add_attribute_uint16( void *env, const char * name, uint16_t * data, std::size_t length ) {
    mrmr_env * m_env = static_cast< mrmr_env * >( env );

    if ( ! m_env->has_data() ) {
        m_env->init_data();
    }

    std::string name_s(name);

    int ret = 0;

    int32_t * tmp;

    switch ( m_env->type )
    {
        case uint8_type:
            m_env->error = "cannot put uint16 into uint8 type.";
            return -1;

        case uint16_type:
            ret = m_env->data_uint16->set_attribute( name_s, data, length );
            break;

        case int32_type:
            tmp = new int32_t[ length ];
            for ( std::size_t i = 0; i < length; i++ )
                tmp[i] = data[i];

            ret = m_env->data_int32->set_attribute( name_s, tmp, length );
            delete [] tmp;
            break;

        default: 
            throw std::invalid_argument("invalid data type specified");

    }

    return ret;
}

int add_attribute_int32( void *env, const char * name, int32_t * data, std::size_t length ) {
    mrmr_env * m_env = static_cast< mrmr_env * >( env );

     if ( ! m_env->has_data() ) {
        m_env->init_data();
    }

    std::string name_s(name);
    switch ( m_env->type )
    {
        case uint8_type:
            m_env->error = "cannot put type int32 in uint8 data set";
            return -1;

        case uint16_type:
            m_env->error = "cannot put type int32 in uint16 data set";
            return -1;

        case int32_type:
            return m_env->data_int32->set_attribute( name_s, data, length );

        default: 
            throw std::invalid_argument("invalid data type specified");
    }
}

int perform_mrmr( void * env, mrmr_method_type mrmr_method, unsigned int label, unsigned int num_features ) {
    mrmr_env * m_env = static_cast< mrmr_env * >( env );

    m_env->clear_results();

    if ( ! ( mrmr_method == mrmr_method_type::MID || mrmr_method == mrmr_method_type::MIQ ) ) {
        m_env->error = "invalid mRMR method";
        return -1;
    }

    if ( ! m_env->has_data() ) {
        m_env->error = "data not set";
        return -2;
    }    

    if ( label >= m_env->num_attributes() ) {
        m_env->error = "label out of range";
        return -3;
    }

    std::vector<mrmr_result> results;
    switch ( m_env->type ) {
        case uint8_type:
            results = mrmr( *m_env->data_uint8, label, num_features, mrmr_method );
            break;

        case uint16_type:
            results = mrmr( *m_env->data_uint16, label, num_features, mrmr_method );
            break;

        case int32_type:
            results = mrmr( *m_env->data_int32, label, num_features, mrmr_method );
            break;
    }

    if ( ( m_env->results_size = results.size() - 1 ) > 0 ) {
        
        m_env->ranks = new const char * [ m_env->results_size ];
        m_env->entropy = new double[ m_env->results_size ];
        m_env->mutual_information = new double[ m_env->results_size ];
        m_env->score = new double[ m_env -> results_size ];

        std::size_t i = 0;
        for ( auto it = results.begin() + 1; it != results.end(); it++ ) {
            m_env->ranks[i] = strdup( it->name.c_str() );
            m_env->entropy[i] = it->entropy;
            m_env->mutual_information[i] = it->mutual_information;
            m_env->score[i++] = it->score;
        }
    }
    
    return m_env->results_size;
}

const char ** get_feature_ranks( void * env, int * num ) {
    mrmr_env * m_env = static_cast< mrmr_env * >( env );

    *num = m_env->results_size;
    return m_env->ranks;
}

double * get_entropy( void * env, int * num ) {
    mrmr_env * m_env = static_cast< mrmr_env * >( env );

    *num = m_env->results_size;
    return m_env->entropy;
}

double * get_mutual_information( void * env, int * num ) {
    mrmr_env * m_env = static_cast< mrmr_env * >( env );

    *num = m_env->results_size;
    return m_env->mutual_information;
}

double * get_mrmr_score( void * env, int * num ) {
    mrmr_env * m_env = static_cast< mrmr_env * >( env );

    *num = m_env->results_size;
    return m_env->score;
}

const char * get_last_error( void * env ) {
    mrmr_env * m_env = static_cast< mrmr_env * >( env );
    return m_env->error.c_str();
}

void destroy_mrmr( void * env ) {
    mrmr_env * m_env = static_cast< mrmr_env * >( env );
    delete m_env;
}

