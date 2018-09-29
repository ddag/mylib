/*************************************************************************************************
 * The extended advanced API of QDBM
 *                                                      Copyright (C) 2000-2004 Mikio Hirabayashi
 * This file is part of QDBM, Quick Database Manager.
 * QDBM is free software; you can redistribute it and/or modify it under the terms of the GNU
 * Lesser General Public License as published by the Free Software Foundation; either version
 * 2.1 of the License or any later version.  QDBM is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 * You should have received a copy of the GNU Lesser General Public License along with QDBM; if
 * not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.
 *************************************************************************************************/


#ifndef _VISTA_H                         /* duplication check */
#define _VISTA_H

#if defined(__cplusplus)                 /* export for C++ */
extern "C" {
#endif



/*************************************************************************************************
 * macros borrowing symbols from Villa
 *************************************************************************************************/


#include <depot.h>
#include <curia.h>
#include <cabin.h>
#include <stdlib.h>

#define VS_DNUM        16

#define VLREC          VSREC_
#define VLIDX          VSIDX_
#define VLLEAF         VSLEAF_
#define VLNODE         VSNODE_
#define VLCFUNC        VSCFUNC_

#define VL_CMPLEX      VS_CMPLEX_
#define VL_CMPINT      VS_CMPINT_
#define VL_CMPNUM      VS_CMPNUM_
#define VL_CMPDEC      VS_CMPDEC_

#define VILLA          VISTA_

#define VL_OREADER     VS_OREADER_
#define VL_OWRITER     VS_OWRITER_
#define VL_OCREAT      VS_OCREAT_
#define VL_ONOLCK      VS_ONOLCK_

#define VL_DOVER       VS_DOVER_
#define VL_DKEEP       VS_DKEEP_
#define VL_DDUP        VS_DDUP_

#define VL_JFORWARD    VS_JFORWARD_
#define VL_JBACKWARD   VS_JBACKWARD_

#define vlopen         vsopen_
#define vlclose        vsclose_
#define vlput          vsput_
#define vlout          vsout_
#define vlget          vsget_
#define vlvnum         vsvnum_
#define vlputlist      vsputlist_
#define vloutlist      vsoutlist_
#define vlgetlist      vsgetlist_
#define vlcurfirst     vscurfirst_
#define vlcurlast      vscurlast_
#define vlcurprev      vscurprev_
#define vlcurnext      vscurnext_
#define vlcurjump      vscurjump_
#define vlcurkey       vscurkey_
#define vlcurval       vscurval_
#define vlsettuning    vssettuning_
#define vlsync         vssync_
#define vloptimize     vsoptimize_
#define vlname         vsname_
#define vlfsiz         vsfsiz_
#define vllnum         vslnum_
#define vlnnum         vsnnum_
#define vlrnum         vsrnum_
#define vlwritable     vswritable_
#define vlfatalerror   vsfatalerror_
#define vlinode        vsinode_
#define vlmtime        vsmtime_
#define vltranbegin    vstranbegin_
#define vltrancommit   vstrancommit_
#define vltranabort    vstranabort_
#define vlremove       vsremove_
#define vlrepair       vsrepair_

#define DEPOT          CURIA

#define \
  dpopen(name, omode, bnum) \
  cropen(name, omode, ((bnum / VS_DNUM) * 4), VS_DNUM)

#define \
  dpclose(db) \
  crclose(db)

#define \
  dpput(db, kbuf, ksiz, vbuf, vsiz, dmode) \
  crput(db, kbuf, ksiz, vbuf, vsiz, dmode)

#define \
  dpout(db, kbuf, ksiz) \
  crout(db, kbuf, ksiz)

#define \
  dpget(db, kbuf, ksiz, start, max, sp) \
  crget(db, kbuf, ksiz, start, max, sp)

#define \
  dpvsiz(db, kbuf, ksiz) \
  crvsiz(db, kbuf, ksiz)

#define \
  dpiterinit(db) \
  criterinit(db)

#define \
  dpiternext(db, sp) \
  criternext(db, sp)

#define \
  dpsetalign(db, align) \
  crsetalign(db, align)

#define \
  dpsync(db) \
  crsync(db)

#define \
  dpoptimize(db, bnum) \
  croptimize(db, bnum)

#define \
  dpname(db) \
  crname(db)

#define \
  dpfsiz(db) \
  crfsiz(db)

#define \
  dpbnum(db) \
  crbnum(db)

#define \
  dpbusenum(db) \
  crbusenum(db)

#define \
  dprnum(db) \
  crrnum(db)

#define \
  dpwritable(db) \
  crwritable(db)

#define \
  dpfatalerror(db) \
  crfatalerror(db)

#define \
  dpinode(db) \
  crinode(db)

#define \
  dpmtime(db) \
  crmtime(db)

#define \
  dpfdesc(db) \
  crfdesc(db)

#define \
  dpremove(db) \
  crremove(db)

#define \
  dprepair(db) \
  crrepair(db)

#define \
  dpmemsync(db) \
  crmemsync(db)

#define \
  dpgetflags(db) \
  crgetflags(db)

#define \
  dpsetflags(db, flags) \
  crsetflags(db, flags)



/*************************************************************************************************
 * including real definition
 *************************************************************************************************/


#include "villa.h"



#if defined(__cplusplus)                 /* export for C++ */
}
#endif

#endif                                   /* duplication check */


/* END OF FILE */
