/* 
 * Copyright (C) 2012 Simon Richter
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "object_cache.h"

#include "session.h"

#include <cassert>

namespace librevisa {

object *object_cache::get_object(ViObject vi) throw(exception)
{
        if(vi == VI_NULL)
                throw exception(VI_WARN_NULL_OBJECT);

        {
                smap::iterator i = sessions.find(vi);
                if(i != sessions.end())
                        return &i->second;
        }

        throw exception(VI_ERROR_INV_OBJECT);
}

session *object_cache::get_session(ViSession vi) throw(exception)
{
        if(vi == VI_NULL)
                throw exception(VI_WARN_NULL_OBJECT);

        {
                smap::iterator i = sessions.find(vi);
                if(i != sessions.end())
                        return &i->second;
        }

        throw exception(VI_ERROR_INV_OBJECT);
}

void object_cache::remove(ViObject vi) throw(exception)
{
        assert(vi != VI_NULL);

        smap::iterator si = sessions.find(vi);
        if(si != sessions.end())
        {
                sessions.erase(si);
                return;
        }

        throw(VI_ERROR_INV_OBJECT);
}

ViSession object_cache::add(resource *res) throw(exception)
{
        return sessions.insert(std::make_pair(find_id(), session(res))).first->first;
}

ViObject object_cache::find_id() throw(exception)
{
        unsigned int id = 0;
        if(!sessions.empty())
        {
                unsigned int maxses = sessions.rbegin()->first;
                if(maxses > id)
                        id = maxses;
        }
        ++id;
        if(!id)
                throw exception(VI_ERROR_ALLOC);

        return id;
}

object_cache objects;

}
