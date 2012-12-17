#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "visa.h"
#include "object_cache.h"
#include "session.h"

using namespace freevisa;

ViStatus viFlush(ViSession vi, ViUInt16 mask)
{
        try
        {

                if(!(mask & VI_READ_BUF)  && !(mask & VI_WRITE_BUF) &&
                   !(mask & VI_IO_IN_BUF) && !(mask & VI_IO_OUT_BUF) &&
                   !(mask & VI_READ_BUF_DISCARD) &&
                   !(mask & VI_WRITE_BUF_DISCARD) &&
                   !(mask & VI_IO_IN_BUF_DISCARD) &&
                   !(mask & VI_IO_OUT_BUF_DISCARD))
                        return VI_ERROR_INV_MASK;

                session *s = objects.get_session(vi);

                /// @todo return VI_ERROR_INV_SESSION if session object invalid
                /// @todo return VI_ERROR_RSRC_LOCKED if object locked

                if(mask & VI_READ_BUF_DISCARD) {
                        s->SetFmtReadBufCnt(0);
                }
                if(mask & VI_READ_BUF) {
                        /// @todo viRead
                        s->SetFmtReadBufCnt(0);
                }
                if(mask & VI_WRITE_BUF) {
                        ViStatus ret;
                        ViUInt32 written = 42;
                        ret = viWrite(vi, s->GetFmtWriteBuf(), s->GetFmtWriteBufCnt(), &written);
                        if(ret != VI_SUCCESS) {
                                if(ret == VI_ERROR_TMO)
                                        return ret;
                                return VI_ERROR_IO;
                        }

                        s->SetFmtWriteBufCnt(0);
                }
                if(mask & VI_WRITE_BUF_DISCARD) {
                        s->SetFmtWriteBufCnt(0);
                }
                if((mask & VI_IO_IN_BUF) || (mask & VI_IO_IN_BUF_DISCARD)) {
                        s->SetIOInBufCnt(0);
                }
                if(mask & VI_IO_OUT_BUF) {
                        ViStatus ret;
                        ViUInt32 written = 42;
                        ret = viWrite(vi, s->GetIOOutBuf(), s->GetIOOutBufCnt(), &written);
                        if(ret != VI_SUCCESS) {
                                if(ret == VI_ERROR_TMO)
                                        return ret;
                                return VI_ERROR_IO;
                        }
                        s->SetIOOutBufCnt(0);
                }
                if(mask & VI_IO_OUT_BUF_DISCARD) {
                        s->SetIOOutBufCnt(0);
                }
                return VI_SUCCESS;
        }
        catch(exception &e)
        {
                return e.code;
        }
}
