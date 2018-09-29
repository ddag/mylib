/*************************************************************************************************
 * Emulation of system calls
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


#include "myconf.h"



/*************************************************************************************************
 * for POSIX thread
 *************************************************************************************************/


#if defined(MYPTHREAD)


#include <pthread.h>


#define PTKEYMAX       8


static struct { void *ptr; pthread_key_t key; } _qdbm_ptkeys[PTKEYMAX];
static int _qdbm_ptknum = 0;


static void *_qdbm_gettsd(void *ptr, int size, const void *initval);


void *_qdbm_settsd(void *ptr, int size, const void *initval){
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  char *val;
  if((val = _qdbm_gettsd(ptr, size, initval)) != NULL) return val;
  if(pthread_mutex_lock(&mutex) != 0) return NULL;
  if((val = _qdbm_gettsd(ptr, size, initval)) != NULL){
    pthread_mutex_unlock(&mutex);
    return val;
  }
  if(_qdbm_ptknum >= PTKEYMAX){
    pthread_mutex_unlock(&mutex);
    return NULL;
  }
  _qdbm_ptkeys[_qdbm_ptknum].ptr = ptr;
  if(pthread_key_create(&(_qdbm_ptkeys[_qdbm_ptknum].key), free) != 0){
    pthread_mutex_unlock(&mutex);
    return NULL;
  }
  if(!(val = malloc(size))){
    pthread_key_delete(_qdbm_ptkeys[_qdbm_ptknum].key);
    pthread_mutex_unlock(&mutex);
    return NULL;
  }
  memcpy(val, initval, size);
  if(pthread_setspecific(_qdbm_ptkeys[_qdbm_ptknum].key, val) != 0){
    free(val);
    pthread_key_delete(_qdbm_ptkeys[_qdbm_ptknum].key);
    pthread_mutex_unlock(&mutex);
    return NULL;
  }
  _qdbm_ptknum++;
  pthread_mutex_unlock(&mutex);
  return val;
}


static void *_qdbm_gettsd(void *ptr, int size, const void *initval){
  char *val;
  int i;
  for(i = 0; i < _qdbm_ptknum; i++){
    if(_qdbm_ptkeys[i].ptr == ptr){
      if(!(val = pthread_getspecific(_qdbm_ptkeys[i].key))){
        if(!(val = malloc(size))) return NULL;
        memcpy(val, initval, size);
        if(pthread_setspecific(_qdbm_ptkeys[i].key, val) != 0){
          free(val);
          return NULL;
        }
      }
      return val;
    }
  }
  return NULL;
}


#endif



/*************************************************************************************************
 * for systems without mmap
 *************************************************************************************************/


#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_) || \
  defined(_SYS_FREEBSD_) || defined(_SYS_NETBSD_) || defined(_SYS_OPENBSD_) || \
  defined(_SYS_RISCOS_) || defined(MYNOMMAP)


void *_qdbm_mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset){
  char *buf, *wp;
  int rv, rlen;
  if(flags & MAP_FIXED) return MAP_FAILED;
  if(lseek(fd, SEEK_SET, offset) == -1) return MAP_FAILED;
  if(!(buf = malloc(sizeof(int) * 3 + length))) return MAP_FAILED;
  wp = buf;
  *(int *)wp = fd;
  wp += sizeof(int);
  *(int *)wp = offset;
  wp += sizeof(int);
  *(int *)wp = prot;
  wp += sizeof(int);
  rlen = 0;
  while((rv = read(fd, wp + rlen, length - rlen)) > 0){
    rlen += rv;
  }
  if(rv == -1 || rlen != length){
    free(buf);
    return MAP_FAILED;
  }
  return wp;
}


int _qdbm_munmap(void *start, size_t length){
  char *buf, *rp;
  int fd, offset, prot, rv, wlen;
  buf = (char *)start - sizeof(int) * 3;
  rp = buf;
  fd = *(int *)rp;
  rp += sizeof(int);
  offset = *(int *)rp;
  rp += sizeof(int);
  prot = *(int *)rp;
  rp += sizeof(int);
  if(prot & PROT_WRITE){
    if(lseek(fd, offset, SEEK_SET) == -1){
      free(buf);
      return -1;
    }
    wlen = 0;
    while(wlen < (int)length){
      rv = write(fd, rp + wlen, length - wlen);
      if(rv == -1){
        if(errno == EINTR) continue;
        free(buf);
        return -1;
      }
      wlen += rv;
    }
  }
  free(buf);
  return 0;
}


int _qdbm_msync(const void *start, size_t length, int flags){
  char *buf, *rp;
  int fd, offset, prot, rv, wlen;
  buf = (char *)start - sizeof(int) * 3;
  rp = buf;
  fd = *(int *)rp;
  rp += sizeof(int);
  offset = *(int *)rp;
  rp += sizeof(int);
  prot = *(int *)rp;
  rp += sizeof(int);
  if(prot & PROT_WRITE){
    if(lseek(fd, offset, SEEK_SET) == -1) return -1;
    wlen = 0;
    while(wlen < (int)length){
      rv = write(fd, rp + wlen, length - wlen);
      if(rv == -1){
        if(errno == EINTR) continue;
        return -1;
      }
      wlen += rv;
    }
  }
  return 0;
}


#endif



/*************************************************************************************************
 * for systems without times
 *************************************************************************************************/


#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)


clock_t _qdbm_times(struct tms *buf){
  buf->tms_utime = clock();
  buf->tms_stime = 0;
  buf->tms_cutime = 0;
  buf->tms_cstime = 0;
  return 0;
}


#endif



/*************************************************************************************************
 * for Win32
 *************************************************************************************************/


#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)


#define WINLOCKWAIT    100


int _qdbm_win32_fcntl(int fd, int cmd, struct flock *lock){
  HANDLE fh;
  OVERLAPPED ol;
  fh = (HANDLE)_get_osfhandle(fd);
  memset(&ol, 0, sizeof(OVERLAPPED));
  ol.Offset = 0;
  ol.OffsetHigh = 0;
  ol.hEvent = 0;
  if(!LockFileEx(fh, lock->l_type == F_WRLCK ? LOCKFILE_EXCLUSIVE_LOCK : 0, 0, 1, 0, &ol)){
    if(GetLastError() == ERROR_CALL_NOT_IMPLEMENTED){
      while(TRUE){
        if(LockFile(fh, 0, 0, 1, 0)) return 0;
        Sleep(WINLOCKWAIT);
      }
    }
    return -1;
  }
  return 0;
}


#endif


#if defined(_SYS_MSVC_)


DIR *_qdbm_win32_opendir(const char *name){
  char expr[1024];
  DIR *dir;
  HANDLE fh;
  WIN32_FIND_DATA data;
  sprintf(expr, "%s%c*", name, MYPATHCHR);
  if((fh = FindFirstFile(expr, &data)) == INVALID_HANDLE_VALUE) return NULL;
  if(!(dir = malloc(sizeof(DIR)))){
    FindClose(fh);
    return NULL;
  }
  dir->fh = fh;
  dir->data = data;
  dir->first = TRUE;
  return dir;
}


int _qdbm_win32_closedir(DIR *dir){
  if(!FindClose(dir->fh)){
    free(dir);
    return -1;
  }
  free(dir);
  return 0;
}


struct dirent *_qdbm_win32_readdir(DIR *dir){
  if(dir->first){
    sprintf(dir->de.d_name, "%s", dir->data.cFileName);
    dir->first = FALSE;
    return &(dir->de);
  }
  if(!FindNextFile(dir->fh, &(dir->data))) return NULL;
  sprintf(dir->de.d_name, "%s", dir->data.cFileName);
  return &(dir->de);
}


#endif



/*************************************************************************************************
 * for ZLIB
 *************************************************************************************************/


#if defined(MYZLIB)


#include <zlib.h>

#define ZLIBBUFSIZ     8192
#define ZLIBCLEVEL     5


static char *_qdbm_deflate_impl(const char *ptr, int size, int *sp);
static char *_qdbm_inflate_impl(const char *ptr, int size, int *sp);
static unsigned int _qdbm_getcrc_impl(const char *ptr, int size);


char *(*_qdbm_deflate)(const char *, int, int *) = _qdbm_deflate_impl;
char *(*_qdbm_inflate)(const char *, int, int *) = _qdbm_inflate_impl;
unsigned int (*_qdbm_getcrc)(const char *, int) = _qdbm_getcrc_impl;


static char *_qdbm_deflate_impl(const char *ptr, int size, int *sp){
  z_stream zs;
  char *buf, *swap;
  unsigned char obuf[ZLIBBUFSIZ];
  int rv, asiz, bsiz, osiz;
  if(size < 0) size = strlen(ptr);
  zs.zalloc = Z_NULL;
  zs.zfree = Z_NULL;
  zs.opaque = Z_NULL;
  if(deflateInit(&zs, ZLIBCLEVEL) != Z_OK) return NULL;
  asiz = ZLIBBUFSIZ;
  if(!(buf = malloc(asiz))){
    deflateEnd(&zs);
    return NULL;
  }
  bsiz = 0;
  zs.next_in = (unsigned char *)ptr;
  zs.avail_in = size;
  zs.next_out = obuf;
  zs.avail_out = ZLIBBUFSIZ;
  while((rv = deflate(&zs, Z_FINISH)) == Z_OK){
    osiz = ZLIBBUFSIZ - zs.avail_out;
    if(bsiz + osiz > asiz){
      asiz = asiz * 2 + osiz;
      if(!(swap = realloc(buf, asiz))){
        free(buf);
        deflateEnd(&zs);
        return NULL;
      }
      buf = swap;
    }
    memcpy(buf + bsiz, obuf, osiz);
    bsiz += osiz;
    zs.next_out = obuf;
    zs.avail_out = ZLIBBUFSIZ;
  }
  if(rv != Z_STREAM_END){
    free(buf);
    deflateEnd(&zs);
    return NULL;
  }
  osiz = ZLIBBUFSIZ - zs.avail_out;
  if(bsiz + osiz > asiz){
    asiz = asiz * 2 + osiz;
    if(!(swap = realloc(buf, asiz))){
      free(buf);
      deflateEnd(&zs);
      return NULL;
    }
    buf = swap;
  }
  memcpy(buf + bsiz, obuf, osiz);
  bsiz += osiz;
  *sp = bsiz;
  deflateEnd(&zs);
  return buf;
}


static char *_qdbm_inflate_impl(const char *ptr, int size, int *sp){
  z_stream zs;
  char *buf, *swap;
  unsigned char obuf[ZLIBBUFSIZ];
  int rv, asiz, bsiz, osiz;
  zs.zalloc = Z_NULL;
  zs.zfree = Z_NULL;
  zs.opaque = Z_NULL;
  if(inflateInit(&zs) != Z_OK) return NULL;
  asiz = ZLIBBUFSIZ;
  if(!(buf = malloc(asiz))){
    deflateEnd(&zs);
    return NULL;
  }
  bsiz = 0;
  zs.next_in = (unsigned char *)ptr;
  zs.avail_in = size;
  zs.next_out = obuf;
  zs.avail_out = ZLIBBUFSIZ;
  while((rv = inflate(&zs, Z_NO_FLUSH)) == Z_OK){
    osiz = ZLIBBUFSIZ - zs.avail_out;
    if(bsiz + osiz >= asiz){
      asiz = asiz * 2 + osiz;
      if(!(swap = realloc(buf, asiz))){
        free(buf);
        deflateEnd(&zs);
        return NULL;
      }
      buf = swap;
    }
    memcpy(buf + bsiz, obuf, osiz);
    bsiz += osiz;
    zs.next_out = obuf;
    zs.avail_out = ZLIBBUFSIZ;
  }
  if(rv != Z_STREAM_END){
    free(buf);
    inflateEnd(&zs);
    return NULL;
  }
  osiz = ZLIBBUFSIZ - zs.avail_out;
  if(bsiz + osiz >= asiz){
    asiz = asiz * 2 + osiz;
    if(!(swap = realloc(buf, asiz))){
      free(buf);
      deflateEnd(&zs);
      return NULL;
    }
    buf = swap;
  }
  memcpy(buf + bsiz, obuf, osiz);
  bsiz += osiz;
  buf[bsiz] = '\0';
  if(sp) *sp = bsiz;
  inflateEnd(&zs);
  return buf;
}


static unsigned int _qdbm_getcrc_impl(const char *ptr, int size){
  int crc;
  if(size < 0) size = strlen(ptr);
  crc = crc32(0, Z_NULL, 0);
  return crc32(crc, (unsigned char *)ptr, size);
}


#else


char *(*_qdbm_deflate)(const char *, int, int *) = NULL;
char *(*_qdbm_inflate)(const char *, int, int *) = NULL;
unsigned int (*_qdbm_getcrc)(const char *, int) = NULL;


#endif



/*************************************************************************************************
 * for ICONV
 *************************************************************************************************/


#if defined(MYICONV)


#include <iconv.h>

#define ICONVCHECKSIZ  32768
#define ICONVMISSMAX   256
#define ICONVALLWRAT   0.001


static char *_qdbm_iconv_impl(const char *ptr, int size,
                              const char *icode, const char *ocode, int *sp, int *mp);
static const char *_qdbm_encname_impl(const char *ptr, int size);
static int _qdbm_encmiss(const char *ptr, int size, const char *icode, const char *ocode);


char *(*_qdbm_iconv)(const char *, int, const char *, const char *,
                     int *, int *) = _qdbm_iconv_impl;
const char *(*_qdbm_encname)(const char *, int) = _qdbm_encname_impl;


static char *_qdbm_iconv_impl(const char *ptr, int size,
                              const char *icode, const char *ocode, int *sp, int *mp){
  iconv_t ic;
  char *obuf, *wp, *rp;
  size_t isiz, osiz;
  int miss;
  if(size < 0) size = strlen(ptr);
  isiz = size;
  if((ic = iconv_open(ocode, icode)) == (iconv_t)-1) return NULL;
  osiz = isiz * 5;
  if(!(obuf = malloc(osiz + 1))){
    iconv_close(ic);
    return NULL;
  }
  wp = obuf;
  rp = (char *)ptr;
  miss = 0;
  while(isiz > 0){
    if(iconv(ic, (void *)&rp, &isiz, &wp, &osiz) == -1){
      if(errno == EILSEQ && (*rp == 0x5c || *rp == 0x7e)){
        *wp = *rp;
        wp++;
        rp++;
        isiz--;
      } else if(errno == EILSEQ || errno == EINVAL){
        rp++;
        isiz--;
        miss++;
      } else {
        break;
      }
    }
  }
  *wp = '\0';
  if(iconv_close(ic) == -1){
    free(obuf);
    return NULL;
  }
  if(sp) *sp = wp - obuf;
  if(mp) *mp = miss;
  return obuf;
}


static const char *_qdbm_encname_impl(const char *ptr, int size){
  const char *hypo;
  int i, miss, cr;
  if(size < 0) size = strlen(ptr);
  if(size > ICONVCHECKSIZ) size = ICONVCHECKSIZ;
  if(size >= 2 && (!memcmp(ptr, "\xfe\xff", 2) || !memcmp(ptr, "\xff\xfe", 2))) return "UTF-16";
  for(i = 0; i < size - 1; i += 2){
    if(ptr[i] == 0 && ptr[i+1] != 0) return "UTF-16BE";
    if(ptr[i+1] == 0 && ptr[i] != 0) return "UTF-16LE";
  }
  for(i = 0; i < size - 3; i++){
    if(ptr[i] == 0x1b){
      i++;
      if(ptr[i] == '(' && strchr("BJHI", ptr[i+1])) return "ISO-2022-JP";
      if(ptr[i] == '$' && strchr("@B(", ptr[i+1])) return "ISO-2022-JP";
    }
  }
  if(_qdbm_encmiss(ptr, size, "US-ASCII", "UTF-16BE") < 1) return "US-ASCII";
  if(_qdbm_encmiss(ptr, size, "UTF-8", "UTF-16BE") < 1) return "UTF-8";
  hypo = NULL;
  cr = FALSE;
  for(i = 0; i < size; i++){
    if(ptr[i] == 0xd){
      cr = TRUE;
      break;
    }
  }
  if(cr){
    if((miss = _qdbm_encmiss(ptr, size, "Shift_JIS", "EUC-JP")) < 1) return "Shift_JIS";
    if(!hypo && miss / (double)size <= ICONVALLWRAT) hypo = "Shift_JIS";
    if((miss = _qdbm_encmiss(ptr, size, "EUC-JP", "UTF-16BE")) < 1) return "EUC-JP";
    if(!hypo && miss / (double)size <= ICONVALLWRAT) hypo = "EUC-JP";
  } else {
    if((miss = _qdbm_encmiss(ptr, size, "EUC-JP", "UTF-16BE")) < 1) return "EUC-JP";
    if(!hypo && miss / (double)size <= ICONVALLWRAT) hypo = "EUC-JP";
    if((miss = _qdbm_encmiss(ptr, size, "Shift_JIS", "EUC-JP")) < 1) return "Shift_JIS";
    if(!hypo && miss / (double)size <= ICONVALLWRAT) hypo = "Shift_JIS";
  }
  if((miss = _qdbm_encmiss(ptr, size, "UTF-8", "UTF-16BE")) < 1) return "UTF-8";
  if(!hypo && miss / (double)size <= ICONVALLWRAT) hypo = "UTF-8";
  if((miss = _qdbm_encmiss(ptr, size, "CP932", "UTF-16BE")) < 1) return "CP932";
  if(!hypo && miss / (double)size <= ICONVALLWRAT) hypo = "CP932";
  return hypo ? hypo : "ISO-8859-1";
}


static int _qdbm_encmiss(const char *ptr, int size, const char *icode, const char *ocode){
  iconv_t ic;
  char obuf[ICONVCHECKSIZ], *wp, *rp;
  size_t isiz, osiz;
  int miss;
  isiz = size;
  if((ic = iconv_open(ocode, icode)) == (iconv_t)-1) return ICONVMISSMAX;
  miss = 0;
  rp = (char *)ptr;
  while(isiz > 0){
    osiz = ICONVCHECKSIZ;
    wp = obuf;
    if(iconv(ic, (void *)&rp, &isiz, &wp, &osiz) == -1){
      if(errno == EILSEQ || errno == EINVAL){
        rp++;
        isiz--;
        miss++;
        if(miss >= ICONVMISSMAX) break;
      } else {
        break;
      }
    }
  }
  if(iconv_close(ic) == -1) return ICONVMISSMAX;
  return miss;
}


#else


char *(*_qdbm_iconv)(const char *, int, const char *, const char *, int *, int *) = NULL;
const char *(*_qdbm_encname)(const char *, int) = NULL;


#endif



/*************************************************************************************************
 * common settings
 *************************************************************************************************/


int _qdbm_dummyfunc(void){
  return 0;
}



/* END OF FILE */
