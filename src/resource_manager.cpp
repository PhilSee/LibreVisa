#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "resource_manager.h"
#include "resource_creator.h"

#include "resource.h"
#include "object_cache.h"

namespace freevisa {

ViStatus resource_manager::Open(ViRsrc rsrcName, ViAccessMode /*accessMode*/, ViUInt32 /*timeout*/, ViSession *vi)
{
        try
        {
                resource *res = create(rsrcName);
                *vi = objects.add(res);
                return VI_SUCCESS;
        }
        catch(std::bad_alloc &e)
        {
                return VI_ERROR_ALLOC;
        }
        catch(exception &e)
        {
                return e.code;
        }
}

ViStatus resource_manager::Close()
{
        // Closing the last instance of the resource manager has no effect.
        return VI_SUCCESS;
}

ViStatus resource_manager::GetAttribute(ViAttr attr, void *attrState)
{
        switch(attr)
        {
        case VI_ATTR_RSRC_NAME:
                *reinterpret_cast<ViRsrc>(attrState) = '\0';
                return VI_SUCCESS;

        case VI_ATTR_RM_SESSION:
                *reinterpret_cast<ViSession *>(attrState) = VI_NULL;
                return VI_SUCCESS;

        default:
                return resource::GetAttribute(attr, attrState);
        }
}

ViStatus resource_manager::SetAttribute(ViAttr attr, ViAttrState attrState)
{
        return resource::SetAttribute(attr, attrState);
}

ViStatus resource_manager::ParseRsrc(
        ViRsrc,//rsrc,
        ViUInt16 *,//intfType,
        ViUInt16 *,//intfNum,
        ViString *,//rsrcClass,
        ViString *,//unaliasedExpandedRsrcName,
        ViString *)//aliasIfExists)
{
        return VI_ERROR_NSUP_OPER;
}

void resource_manager::register_creator(resource_creator const &cre)
{
        creators.push_back(&cre);
}

resource *resource_manager::create(ViRsrc rsrcName)
{
        std::string const name = rsrcName;

        std::vector<std::string> components;
        std::string::const_iterator component_begin = name.begin();
        for(std::string::const_iterator i = name.begin(); i < name.end(); ++i)
        {
                if(*i != ':')
                        continue;
                std::string::const_iterator next = i + 1;
                if(next == name.end())
                        continue;
                if(*next != ':')
                        continue;
                components.push_back(std::string(component_begin, i));
                component_begin = next + 1;
        }
        components.push_back(std::string(component_begin, name.end()));

        for(creator_iterator i = creators.begin(); i != creators.end(); ++i)
        {
                resource *rsrc = (*i)->create(components);
                if(rsrc)
                        return rsrc;
        }
        throw exception(VI_ERROR_RSRC_NFOUND);
}

resource_manager default_resource_manager;

}
