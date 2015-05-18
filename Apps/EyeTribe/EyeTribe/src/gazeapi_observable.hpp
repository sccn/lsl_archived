/*
 * Copyright (c) 2013-present, The Eye Tribe. 
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in
 * the LICENSE file in the root directory of this source tree. 
 *
 */

#ifndef _THEEYETRIBE_GAZEAPI_OBSERVABLE_H_
#define _THEEYETRIBE_GAZEAPI_OBSERVABLE_H_

#include <vector>

namespace gtl
{
    template <typename T>
    class Observable
    {
    public:
        typedef std::vector<T*> ObserverVector;

        virtual ~Observable()
        {}

        void add_observer(T & observer)
        {
            for (std::size_t i = 0; i < m_observers.size(); ++i)
            {
                if (&observer == m_observers[i])
                {
                    return; // Already added, just ignore
                }
            }

            m_observers.push_back(&observer);
        }

        void remove_observer(T & observer)
        {
            for (int i = m_observers.size()-1; i >= 0; --i)
            {
                if (&observer == m_observers[i])
                {
                    m_observers[i] = m_observers[m_observers.size()-1];
                    m_observers.resize(m_observers.size()-1);
                }
            }
        }

        ObserverVector const & get_observers()
        {
            return m_observers;
        }

        std::size_t size() const
        {
            return m_observers.size();
        }

        void clear()
        {
            m_observers.clear();
        }

    private:
        ObserverVector m_observers;
    };
}

#endif // _THEEYETRIBE_GAZEAPI_OBSERVABLE_H_
