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

#include "mrmr_py.hpp"

void * setup_mrmr() {
    return new mrmr_env();
}

int add_attribute( void * env, const char * name, uint8_t * data, unsigned int length ) {
    mrmr_env * m_env = static_cast< mrmr_env * >( env );

    if ( ! m_env->data ) {
        m_env->data = new dataset<uint8_t>();
    }

    std::string name_s(name);
    return m_env->data->set_attribute( name_s, data, length );
}

int perform_mrmr( void * env, unsigned int label, unsigned int num_features ) {
    mrmr_env * m_env = static_cast< mrmr_env * >( env );

    m_env->clear_results();

    if ( ! m_env->data ) {
        m_env->error = "data not set";
        return -1;
    }

    if ( label >= m_env->data->num_attributes() ) {
        m_env->error = "label out of range";
        return -2;
    }

    auto results = mrmr( *m_env->data, label, num_features );

    if ( ( m_env->results_size = results.size() - 1 ) > 0 ) {
        
        m_env->ranks = new const char * [ m_env->results_size ];
        m_env->entropy = new double[ m_env->results_size ];
        m_env->mutual_information = new double[ m_env->results_size ];
        m_env->score = new double[ m_env -> results_size ];

        std::size_t i = 0;
        for ( auto it = results.begin() + 1; it != results.end(); it++ ) {
            m_env->ranks[i] = it->name.c_str();
            m_env->entropy[i] = it->entropy;
            m_env->mutual_information[i] = it->mutual_information;
            m_env->score[i] = it->score;
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

