/*
 * Copyright (c) 2017, Solarflare Communications Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _SYS_EFSYS_H
#define _SYS_EFSYS_H
#include <vmkapi.h>

typedef vmk_int8                int8_t;
typedef vmk_uint8               uint8_t;
typedef vmk_int16               int16_t;
typedef vmk_uint16              uint16_t;
typedef vmk_int32               int32_t;
typedef vmk_uint32              uint32_t;
typedef vmk_int64               int64_t;
typedef vmk_uint64              uint64_t;

#ifdef TARGET_CIM_64
typedef long unsigned int       size_t;
#else
typedef unsigned int            size_t;
#endif
typedef vmk_Bool                boolean_t;
typedef char*                   caddr_t;
typedef vmk_uintptr_t           uintptr_t;


#define EFSYS_ASSERT(_exp)
#define EFSYS_ASSERT3(_x, _op, _y, _t)

#define EFSYS_ASSERT3U(_x, _op, _y)     EFSYS_ASSERT3(_x, _op, _y, uint64_t)
#define EFSYS_ASSERT3S(_x, _op, _y)     EFSYS_ASSERT3(_x, _op, _y, int64_t)
#define EFSYS_ASSERT3P(_x, _op, _y)     EFSYS_ASSERT3(_x, _op, _y, uintptr_t)

/* SAL annotations used for Windows builds */
#define __in
#define __in_opt
#define __in_ecount(_n)
#define __in_ecount_opt(_n)
#define __in_bcount(_n)
#define __in_bcount_opt(_n)

#define __out
#define __out_opt
#define __out_ecount(_n)
#define __out_ecount_opt(_n)
#define __out_bcount(_n)
#define __out_bcount_opt(_n)
#define __out_bcount_part(_n, _l)
#define __out_bcount_part_opt(_n, _l)

#define __deref_out

#define __inout
#define __inout_opt
#define __inout_ecount(_n)
#define __inout_ecount_opt(_n)
#define __inout_bcount(_n)
#define __inout_bcount_opt(_n)
#define __inout_bcount_full_opt(_n)

#define __deref_out_bcount_opt(n)

#define __checkReturn
#define __success(_x)

#define __drv_when(_p, _c)


/* Compilation options */
#define EFSYS_OPT_IMAGE_LAYOUT 1
#define EFSYS_OPT_MEDFORD 1
#define EFSYS_OPT_MEDFORD2 1



#define EFSYS_PROBE(_name)

#define EFSYS_PROBE1(_name, _type1, _arg1)

#define EFSYS_PROBE2(_name, _type1, _arg1, _type2, _arg2)

#define EFSYS_PROBE3(_name, _type1, _arg1, _type2, _arg2,               \
            _type3, _arg3)

#define EFSYS_PROBE4(_name, _type1, _arg1, _type2, _arg2,               \
            _type3, _arg3, _type4, _arg4)

#define EFSYS_PROBE5(_name, _type1, _arg1, _type2, _arg2,               \
            _type3, _arg3, _type4, _arg4, _type5, _arg5)

#define EFSYS_PROBE6(_name, _type1, _arg1, _type2, _arg2,               \
            _type3, _arg3, _type4, _arg4, _type5, _arg5,                \
            _type6, _arg6)

#define EFSYS_PROBE7(_name, _type1, _arg1, _type2, _arg2,               \
            _type3, _arg3, _type4, _arg4, _type5, _arg5,                \
            _type6, _arg6, _type7, _arg7)


#endif  /* _SYS_EFSYS_H */

