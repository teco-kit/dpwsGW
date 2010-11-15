/* life-cycle-manager - to control the life-cycle of a service
 * Copyright (C) 2007  University of Rostock
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 */

/*++++++++++++++++++++//
//----- includes -----//
//++++++++++++++++++++*/

#include <signal.h>
#include <sys/stat.h>
#include "lcm.nsmap"
#include "dpws_hosted.h"
#include "lcm_wsdl.h"
#include "lcm_metadata.h"
#include "fileio.h"
#include "lcm-defs.h"
#include "ws4d_eprllist.h"
#include "ws4d_abstract_eprlist.h"
#include "ws4d_targetcache.h"
#include "ws4d_servicecache.h"
#include "config.h"
#include <sys/types.h>
#include <utime.h>
#include "lua.h"

#ifdef WIN32
#include "windows.h"
#include <sys/stat.h>
#include <assert.h>
#include <winbase.h>
#include <process.h>
#include "direct.h"
#else
#include <dirent.h>
#endif

/*+++++++++++++++++++++++//
//----- definitions -----//
//+++++++++++++++++++++++*/

#define MAX_CHAR_LENGTH  255
#define LCM_URI_LEN      255
#define XML_SIZE         10240

/* other definitions */
#define URN_PREFIX                 'urn:'
#define UUID_PREFIX                'uuid:'
#define URN_PREFIX_                "urn:"
#define UUID_PREFIX_               "uuid:"
#define LCM_LOG_FILE               "lcm.log"
#define WORK_DIR                   "/tmp/LCM_Work_Dir"  /* full path */
#define TMP_DIR_FOR_INSTALL        "Install_TMP"
#define WORK_AUTHO                 ( S_IREAD | S_IWRITE | S_IEXEC | S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID |S_ISVTX )
#define ATTACHMENTSERVICE_URI_LEN  255
#define CREATE_MODE                777

/*+++++++++++++++++++++++++++++//
//----- enums and strucks -----//
//+++++++++++++++++++++++++++++*/

struct tags
{
  char manifest_tag[50];
} tags;


static struct tags tag_list[] = {
  {"service"},
  {"wsdl"},
  {"xml"},
  {"version"},
  {"start"},
  {"stop"},
  {"resolve"},
  {"isAlive"}
};


struct manifest_content
{
  char service_name[MAX_CHAR_LENGTH];
  char wsdl_name[MAX_CHAR_LENGTH];
  char xml_name[MAX_CHAR_LENGTH];
  char version[MAX_CHAR_LENGTH];
  char start[MAX_CHAR_LENGTH];
  char stop[MAX_CHAR_LENGTH];
  char resolve[MAX_CHAR_LENGTH];
  char isAlive[MAX_CHAR_LENGTH];
};

struct service_register_cache_s
{
  struct ws4d_epr *epr;
  char id[MAX_CHAR_LENGTH];     /* ID of the service, it used as a handle */
  char location[MAX_CHAR_LENGTH];       /* location where the service is installed */
  int status;                   /* the current state of the service */
  int valid;                    /* the status of the resolve check */
  struct manifest_content manifest;     /* all data of the manifest, stored in these struckt */
#ifdef WIN32
  HANDLE pid;                   /* the PID of the running service */
#else
  pid_t pid;                    /* the PID of the running service */
#endif
};


/*+++++++++++++++++++++//
//----- variables -----//
//+++++++++++++++++++++*/

int wst_serve_request (struct soap *soap);
int wsd_serve_request (struct soap *soap);

char *hosting_xml_path = NULL;
char *host = NULL;
struct soap service;
struct dpws_s device;
unsigned int CRCTab[256];
struct ws4d_abs_eprlist service_register;

/* Typ: pointer to function, return int, commit 2 char: */
typedef int (*int_intint_pf_t) (char *, char *);
/* Typ: pointer to function, return int, no parameter commit: */
typedef int (*int_void_pf_t) (void);


/*++++++++++++++++++++++++++++//
//----- Methos Funktions -----//
//++++++++++++++++++++++++++++*/

#include "unzip.h"

static void
change_file_date (const char *filename, uLong dosdate, tm_unz tmu_date)

{
#ifdef WIN32
  HANDLE hFile;
  FILETIME ftm, ftLocal, ftCreate, ftLastAcc, ftLastWrite;

  hFile = CreateFile (filename, GENERIC_READ | GENERIC_WRITE,
                      0, NULL, OPEN_EXISTING, 0, NULL);
  GetFileTime (hFile, &ftCreate, &ftLastAcc, &ftLastWrite);
  DosDateTimeToFileTime ((WORD) (dosdate >> 16), (WORD) dosdate, &ftLocal);
  LocalFileTimeToFileTime (&ftLocal, &ftm);
  SetFileTime (hFile, &ftm, &ftLastAcc, &ftm);
  CloseHandle (hFile);
#else
#ifdef unix
  struct utimbuf ut;
  struct tm newdate;

  WS4D_UNUSED_PARAM(dosdate);

  newdate.tm_sec = tmu_date.tm_sec;
  newdate.tm_min = tmu_date.tm_min;
  newdate.tm_hour = tmu_date.tm_hour;
  newdate.tm_mday = tmu_date.tm_mday;
  newdate.tm_mon = tmu_date.tm_mon;
  if (tmu_date.tm_year > 1900)
    newdate.tm_year = tmu_date.tm_year - 1900;
  else
    newdate.tm_year = tmu_date.tm_year;
  newdate.tm_isdst = -1;

  ut.actime = ut.modtime = mktime (&newdate);
  utime (filename, &ut);
#endif
#endif
}


static int
mymkdir (const char *dirname)
{
  int ret = 0;
#ifdef WIN32
  ret = mkdir (dirname);
#else
#ifdef unix
  ret = mkdir (dirname, 0775);
#endif
#endif
  return ret;
}


static int
makedir (char *newdir)
{
  char *buffer;
  char *p;
  int len = (int) strlen (newdir);

  if (len <= 0)
    return 0;

  buffer = (char *) malloc (len + 1);
  strcpy (buffer, newdir);

  if (buffer[len - 1] == '/')
    {
      buffer[len - 1] = '\0';
    }
  if (mymkdir (buffer) == 0)
    {
      free (buffer);
      return 1;
    }

  p = buffer + 1;
  while (1)
    {
      char hold;

      while (*p && *p != '\\' && *p != '/')
        p++;
      hold = *p;
      *p = 0;
      if ((mymkdir (buffer) == -1) && (errno == ENOENT))
        {
          printf ("couldn't create directory %s\n", buffer);
          free (buffer);
          return 0;
        }
      if (hold == 0)
        break;
      *p++ = hold;
    }
  free (buffer);
  return 1;
}

#define WRITEBUFFERSIZE   (8192)

static int unpack_archive(const char *zipfile, const char *destination)
{
  unzFile uf = NULL;
  unz_global_info gi;
  int i, err;

  WS4D_UNUSED_PARAM(destination);

#ifdef USEWIN32IOAPI
  zlib_filefunc_def ffunc;

  fill_win32_filefunc (&ffunc);
  uf = unzOpen2 (zipfile, &ffunc);
#else
  uf = unzOpen (zipfile);
#endif
  if (uf == NULL)
  {
	  return UNZ_BADZIPFILE;
  }

  err = unzGetGlobalInfo (uf, &gi);
  if (err != UNZ_OK)
  {
	  return err;
  }

  for (i = 0; i < gi.number_entry; i++)
    {
	  char filename_inzip[256];
	  char *filename_withoutpath;
	  char *p;
	  unz_file_info file_info;
	  size_t size_buf;
	  void *buf;

	  err =
	    unzGetCurrentFileInfo (uf, &file_info, filename_inzip,
	                           sizeof (filename_inzip), NULL, 0, NULL, 0);
	  if (err != UNZ_OK)
	  {
        break;
	  }

	  size_buf = WRITEBUFFERSIZE;
	  buf = (void *) malloc (size_buf);
	  if (buf == NULL)
	    {
	      printf ("Error allocating memory\n");
	      return UNZ_INTERNALERROR;
	    }

	  p = filename_withoutpath = filename_inzip;
	  while ((*p) != '\0')
	    {
	      if (((*p) == '/') || ((*p) == '\\'))
	        filename_withoutpath = p + 1;
	      p++;
	    }

	  if ((*filename_withoutpath) == '\0')
	    {
          mymkdir (filename_inzip);
	    }
	  else
	    {
		  FILE *fout = NULL;

	      fout = fopen (filename_inzip, "wb");

	      /* some zipfile don't contain directory alone before file */
	      if ((fout == NULL) && (filename_withoutpath != (char *) filename_inzip))
	      {
	        char c = *(filename_withoutpath - 1);
	        *(filename_withoutpath - 1) = '\0';
	        makedir (filename_inzip);
	        *(filename_withoutpath - 1) = c;
	        fout = fopen (filename_inzip, "wb");
	      }

	      if (fout == NULL)
	      {
	        break;
	      }
	      else
	      {
	          do
	            {
	              err = unzReadCurrentFile (uf, buf, size_buf);
	              if (err < 0)
	                {
	                  break;
	                }

	              if (err > 0)
	                if (fwrite (buf, err, 1, fout) != 1)
	                  {
	                    err = UNZ_ERRNO;
	                    break;
	                  }
	            } while (err > 0);

	          if (fout)
	            fclose (fout);

	          if (err == 0)
	            change_file_date (filename_inzip, file_info.dosDate,
	                              file_info.tmu_date);
	        }

	      if (err == UNZ_OK)
	        {
	          err = unzCloseCurrentFile (uf);
	          if (err != UNZ_OK)
	            {
	              printf ("error %d with zipfile in unzCloseCurrentFile\n", err);
	            }
	        }
	      else
	        unzCloseCurrentFile (uf);       /* don't lose the error */
	    }

	  free (buf);

      if ((i + 1) < gi.number_entry)
        {
          err = unzGoToNextFile (uf);
          if (err != UNZ_OK)
            {
              break;
            }
        }
    }

  unzCloseCurrentFile (uf);

  return 0;
}


/*+++++++++++++++++++++++++++//
// Funktion for eprlist    //
//+++++++++++++++++++++++++++*/

#define EPRLIST_LCM_ID "LCM-ServiceEp-0.1"
const char *eprlist_lcm_id = EPRLIST_LCM_ID;

static void
service_register_fdelete (struct ws4d_epr *epr, struct ws4d_epr_plugin *p);

static struct ws4d_epr_plugin_cb ws4d_service_register_callbacks =
  { service_register_fdelete, NULL, NULL };

static void
service_register_fdelete (struct ws4d_epr *epr, struct ws4d_epr_plugin *p)
{
  struct service_register_cache_s *plugin = p->data;
  WS4D_UNUSED_PARAM(epr);

  if (plugin)
    {
      /*TODO: weitere Freigaben hier!! */
      ws4d_free (plugin);
    }
}

static int
service_register_fcreate (struct ws4d_epr *epr,
                          struct ws4d_epr_plugin *plugin, void *arg)
{
  struct service_register_cache_s *data = NULL;
  WS4D_UNUSED_PARAM(arg);

  plugin->id = eprlist_lcm_id;
  plugin->cb = &ws4d_service_register_callbacks;

  data = ws4d_malloc (sizeof (struct service_register_cache_s));

  if (!data)
    return SOAP_ERR;

  data->epr = epr;
  plugin->data = data;

  return SOAP_OK;
}


static struct ws4d_epr *
add_new_service (struct ws4d_abs_eprlist *epr)
{
  struct ws4d_epr *new_list = NULL;
  ws4d_alloc_list alist;

  if (!epr)
    return NULL;

  WS4D_ALLOCLIST_INIT (&alist);

  /* add new list */
  new_list = ws4d_eprlist_alloc (&service_register);

  if (!new_list)
    return NULL;

  if (ws4d_eprlist_add (&service_register, new_list) != SOAP_OK)
    return NULL;

  /* add plugin to eprlist */
  if (ws4d_epr_register_plugin (new_list, service_register_fcreate, NULL) !=
      SOAP_OK)
    return NULL;

  return new_list;
}


static int
free_service (struct ws4d_epr *epr)
{
  soap_assert (NULL, epr, LCM_ERR);

  if (ws4d_eprlist_remove (&service_register, epr) != SOAP_OK)
    return LCM_ERR;

  if (ws4d_eprlist_free (&service_register, epr) != SOAP_OK)
    return LCM_ERR;

  return LCM_OK;
}


/*++++++++++++++++++++++++++++++++++++//
// spezial functions for the services //
//++++++++++++++++++++++++++++++++++++*/

/*----------------------------------
// err_print
// -----------------
//
// print the given error to output
//----------------------------------*/
static void
err_print (const char *message)
{
#ifdef LOG_INTO_FILE
  FILE *log_file = fopen (LCM_LOG_FILE, "a+");
  char string_tmp[MAX_CHAR_LENGTH + 20];
  time_t time_tmp;
  struct tm *time_loc;

  /* get current time */
  time (&time_tmp);

  /* convert time */
  time_loc = localtime (&time_tmp);

  /* set string */
  sprintf (&string_tmp[0], "%d.%d.%d %d:%d:%d:\t%s\n", time_loc->tm_mday,
           time_loc->tm_mon,
           (time_loc->tm_year + 1900),
           time_loc->tm_hour, time_loc->tm_min, time_loc->tm_sec, message);

  /* write data */
  fputs (&string_tmp[0], log_file);

  /* put to stream */
  fflush (log_file);

  /* close handle */
  fclose (log_file);
#endif

#ifdef LOG_CONSOLE
  printf ("%s\n", message);

#endif
}


/*----------------------------------
// service_exit
// -----------------
//
// exit the service and cleanup memory
//----------------------------------*/
static void
service_exit ()
{
  char string_tmp[MAX_CHAR_LENGTH];

  err_print ("LCM: shutting down...");

  /* deleate metadata xml file */
  sprintf (&string_tmp[0], "%s/metadata.xml", hosting_xml_path);
  remove (string_tmp);

  /* deleate all data in the work folder and the work folder */
  sprintf (&string_tmp[0], "rm -r %s/", WORK_DIR);
  system (string_tmp);

  /* shutdown stack */
  ws4d_eprlist_done (&service_register);
  dpws_done (&device);
  soap_end (&service);
  soap_done (&service);

  exit (0);
}


/*----------------------------------
// find_cache
// -----------------
//
// search the cache and return the
// index
/----------------------------------*/
static struct ws4d_epr *
find_cache (char *search_id)
{
  struct ws4d_epr *elem = NULL, *iter = NULL;
  struct service_register_cache_s *p_plugin = NULL;

  /* go for each entry */
  ws4d_eprlist_foreach (elem, iter, &service_register)
  {
    if (elem)
      {
        p_plugin =
          (struct service_register_cache_s *) ws4d_epr_get_plugindata (elem,
                                                                       eprlist_lcm_id);
        if (!strcmp (p_plugin->id, search_id))
          {
            return elem;
          }
      }
  }

  return NULL;
}


/*----------------------------------
// set cache state
// -----------------
//
// set the given state to the
// cache element
/----------------------------------*/
static int
set_cache_state (struct ws4d_epr *elem, int state)
{
  struct service_register_cache_s *p_plugin = NULL;

  soap_assert (NULL, elem, LCM_ERR);

  p_plugin =
    (struct service_register_cache_s *) ws4d_epr_get_plugindata (elem,
                                                                 eprlist_lcm_id);

  if (!p_plugin)
    return LCM_ERR;

  p_plugin->status = state;

  return LCM_OK;
}


/*----------------------------------
// return the state
// -----------------
//
// return the state to the cache
// element
/----------------------------------*/
static int
get_cache_state (struct ws4d_epr *elem)
{
  struct service_register_cache_s *p_plugin = NULL;

  soap_assert (NULL, elem, LCM_ERR);

  p_plugin =
    (struct service_register_cache_s *) ws4d_epr_get_plugindata (elem,
                                                                 eprlist_lcm_id);

  if (!p_plugin)
    return LCM_ERR;

  return p_plugin->status;
}


/*----------------------------------
// set cache valid-state
// -----------------
//
// set the given valid-state to
// the cache element
/----------------------------------*/
static int
set_cache_valid (struct ws4d_epr *elem, int state)
{
  struct service_register_cache_s *p_plugin = NULL;

  soap_assert (NULL, elem, LCM_OK);

  p_plugin =
    (struct service_register_cache_s *) ws4d_epr_get_plugindata (elem,
                                                                 eprlist_lcm_id);

  if (!p_plugin)
    return LCM_ERR;

  p_plugin->valid = state;

  return LCM_OK;
}


/*----------------------------------
// return the valid state
// -----------------
//
// return the valid state to the
// cache element
/----------------------------------*/
static int
get_cache_valid (struct ws4d_epr *elem)
{
  struct service_register_cache_s *p_plugin = NULL;

  soap_assert (NULL, elem, LCM_ERR);

  p_plugin =
    (struct service_register_cache_s *) ws4d_epr_get_plugindata (elem,
                                                                 eprlist_lcm_id);

  if (!p_plugin)
    return LCM_ERR;

  return p_plugin->valid;
}


/*----------------------------------
// set the location
// -----------------
//
// set the given location to the
// cache element
/----------------------------------*/
static int
set_cache_location (struct ws4d_epr *elem, char *location)
{
  struct service_register_cache_s *p_plugin = NULL;
  int size = 0;

  soap_assert (NULL, elem && location, LCM_ERR);

  p_plugin =
    (struct service_register_cache_s *) ws4d_epr_get_plugindata (elem,
                                                                 eprlist_lcm_id);

  if (!p_plugin)
    return LCM_ERR;

  size = (strlen (location) + 1);

  if (size > MAX_CHAR_LENGTH)
    return LCM_ERR;

  memcpy (p_plugin->location, location, sizeof (char) * size);

  return LCM_OK;
}


/*----------------------------------
// return the location
// -----------------
//
// return the location to the
// cache element
/----------------------------------*/
static char *
get_cache_location (struct ws4d_epr *elem)
{
  struct service_register_cache_s *p_plugin = NULL;

  soap_assert (NULL, elem, NULL);

  p_plugin =
    (struct service_register_cache_s *) ws4d_epr_get_plugindata (elem,
                                                                 eprlist_lcm_id);

  if (!p_plugin)
    return NULL;

  return p_plugin->location;
}


/*----------------------------------
// set the id
// -----------------
//
// set the given id to the cache
// element
/----------------------------------*/
static int
set_cache_id (struct ws4d_epr *elem, char *id)
{
  struct service_register_cache_s *p_plugin = NULL;
  int size = 0;

  soap_assert (NULL, elem && id, LCM_ERR);

  p_plugin =
    (struct service_register_cache_s *) ws4d_epr_get_plugindata (elem,
                                                                 eprlist_lcm_id);

  if (!p_plugin)
    return LCM_ERR;

  size = (strlen (id) + 1);

  if (size > MAX_CHAR_LENGTH)
    return LCM_ERR;

  memcpy (p_plugin->id, id, sizeof (char) * size);

  return LCM_OK;
}


/*----------------------------------
// return the id
// -----------------
//
// return the id to the cache element
/----------------------------------*/
static char *
get_cache_id (struct ws4d_epr *elem)
{
  struct service_register_cache_s *p_plugin = NULL;

  soap_assert (NULL, elem, NULL);

  p_plugin =
    (struct service_register_cache_s *) ws4d_epr_get_plugindata (elem,
                                                                 eprlist_lcm_id);

  if (!p_plugin)
    return NULL;

  return p_plugin->id;
}


/*----------------------------------
// return the name
// -----------------
//
// return the name to the cache
// element
/----------------------------------*/
static char *
get_cache_name (struct ws4d_epr *elem)
{
  struct service_register_cache_s *p_plugin = NULL;

  soap_assert (NULL, elem, NULL);

  p_plugin =
    (struct service_register_cache_s *) ws4d_epr_get_plugindata (elem,
                                                                 eprlist_lcm_id);

  if (!p_plugin)
    return NULL;

  return p_plugin->manifest.service_name;
}


/*----------------------------------
 // return the version
 // -----------------
 //
 // return the version to the cache
 // element
 /----------------------------------*/
static char *
get_cache_version (struct ws4d_epr *elem)
{
  struct service_register_cache_s *p_plugin = NULL;

  soap_assert (NULL, elem, NULL);

  p_plugin =
    (struct service_register_cache_s *) ws4d_epr_get_plugindata (elem,
                                                                 eprlist_lcm_id);

  if (!p_plugin)
    return NULL;

  return p_plugin->manifest.version;
}


/*----------------------------------
 // return the wsdl location
 // ------------------------
 //
 // return the wsdl loacation to the
 // cache element
 /----------------------------------*/
static char *
get_cache_wsdl (struct ws4d_epr *elem)
{
  struct service_register_cache_s *p_plugin = NULL;

  soap_assert (NULL, elem, NULL);

  p_plugin =
    (struct service_register_cache_s *) ws4d_epr_get_plugindata (elem,
                                                                 eprlist_lcm_id);

  if (!p_plugin)
    return NULL;

  return p_plugin->manifest.wsdl_name;
}


/*----------------------------------
 // return the xml location
 // ------------------------
 //
 // return the xml loacation to the
 // cache element
 /----------------------------------*/
static char *
get_cache_xml (struct ws4d_epr *elem)
{
  struct service_register_cache_s *p_plugin = NULL;

  soap_assert (NULL, elem, NULL);

  p_plugin =
    (struct service_register_cache_s *) ws4d_epr_get_plugindata (elem,
                                                                 eprlist_lcm_id);

  if (!p_plugin)
    return NULL;

  return p_plugin->manifest.xml_name;
}


/*----------------------------------
 // return the manifest-struct
 // ----------------------------
 //
 // return the manifest struct to
 // the cache element
 /----------------------------------*/
static struct manifest_content *
get_cache_manifest_struct (struct ws4d_epr *elem)
{
  struct service_register_cache_s *p_plugin = NULL;

  soap_assert (NULL, elem, NULL);

  p_plugin =
    (struct service_register_cache_s *) ws4d_epr_get_plugindata (elem,
                                                                 eprlist_lcm_id);

  if (!p_plugin)
    return NULL;

  return &p_plugin->manifest;
}


/*----------------------------------
 // set the pid
 // -----------------
 //
 // set the given pid to the cache
 // element
 /----------------------------------*/
#ifdef WIN32
static int
set_cache_pid (struct ws4d_epr *elem, HANDLE pid)
#else
static int
set_cache_pid (struct ws4d_epr *elem, pid_t pid)
#endif
{
  struct service_register_cache_s *p_plugin = NULL;
  int size = 0;

  soap_assert (NULL, elem && pid, LCM_ERR);

  p_plugin =
    (struct service_register_cache_s *) ws4d_epr_get_plugindata (elem,
                                                                 eprlist_lcm_id);

  if (!p_plugin)
    return LCM_ERR;

#ifdef WIN32
  size = sizeof (HANDLE);
#else
  size = sizeof (pid_t);
#endif

  memcpy (&p_plugin->pid, &pid, size);

  return LCM_OK;
}


/*----------------------------------
 // set the pid to zero
 // -----------------
 //
 // set the pid to zero
 /----------------------------------*/
static int
reset_cache_pid (struct ws4d_epr *elem)
{
  struct service_register_cache_s *p_plugin = NULL;

  soap_assert (NULL, elem, LCM_ERR);

  p_plugin =
    (struct service_register_cache_s *) ws4d_epr_get_plugindata (elem,
                                                                 eprlist_lcm_id);

  if (!p_plugin)
    return LCM_ERR;

  p_plugin->pid = 0;

  return LCM_OK;
}


/*----------------------------------
 // return the pid
 // -----------------
 //
 // return the pid to the cache element
 /----------------------------------*/
#ifdef WIN32
static HANDLE
get_cache_pid (struct ws4d_epr * elem)
#else
static pid_t
get_cache_pid (struct ws4d_epr * elem)
#endif
{
  struct service_register_cache_s *p_plugin = NULL;

  soap_assert (NULL, elem, LCM_ERR);

  p_plugin =
    (struct service_register_cache_s *) ws4d_epr_get_plugindata (elem,
                                                                 eprlist_lcm_id);

  if (!p_plugin)
    return LCM_ERR;

  return p_plugin->pid;
}


/*----------------------------------
// compare
// -----------------
//
// compare the given string with
// given index of the tag-list.
// return 0 if they are equal and
// -1 wenn they are different
/----------------------------------*/
static int
taglist_compare (char *string, int arr_index)
{
  unsigned int i = 0;
  unsigned int size = strlen (tag_list[arr_index].manifest_tag);

  if (size > strlen (string))
    {
      /* the given string is shorter then the tag */
      return -1;
    }

  for (i = 0; i < size; i++)
    {
      if (string[i] != tag_list[arr_index].manifest_tag[i])
        {
          /* not conform */
          return LCM_ERR;
        }
    }

  return LCM_OK;
}


/*----------------------------------
 // convert
 // -----------------
 //
 // convert the given line
 /----------------------------------*/
static int
convert (char *string, struct manifest_content *manifest)
{
  int i = 0;                    /* control variable  for tmp-string */
  int k = 0;
  int length = strlen (string);
  int numcmds = sizeof (tag_list) / sizeof (struct tags);

  /* convert strings */
  for (i = 0; i < length; i++)
    {
      /* search for commands */
      for (k = 0; k < numcmds; k++)
        {
          /* check if tag was found */
          if (!taglist_compare (&string[i], k))
            {
              i += strlen (tag_list[k].manifest_tag) + 1;

              /* deleate space and additional character */
              while (!isalnum (string[i]))
                {
                  i++;
                }

              /* save data */
              memcpy ((manifest->service_name +
                       k * MAX_CHAR_LENGTH * sizeof (char)), &string[i],
                      strlen (&string[i]));

              return LCM_OK;
            }
        }
    }

  return LCM_OK;
}


/*----------------------------------
 // readout_manifest
 // -----------------
 //
 // readout the content of the manifest
 // file and store all data into cache
 /----------------------------------*/
static int
readout_manifest (char *manifest_location, struct manifest_content *manifest)
{
  char string[200];
  FILE *sf_in;

  sf_in = fopen (manifest_location, "r");

  /* read file, line by line */
  while ((fscanf (sf_in, "%[^\n]", string)) != EOF)
    {
      convert (string, manifest);
      fgetc (sf_in);            /* read in the '\n' character and moves file */
    }
  fclose (sf_in);

  return LCM_OK;
}


/*----------------------------------
 // search_urn
 // -----------------
 //
 // search the given URN, if the given
 // URN already exist
 /----------------------------------*/
static int
search_urn (char *urn)
{
  WS4D_UNUSED_PARAM(urn);

  /* da der Stack das Probe bei einem service noch nicht unterstützt muß dass hier noch warten
   *
   struct wsa_eprlist_elem            *service = NULL;
   ws4d_alloc_list           alist;
   ws4d_qnamelist                type_list;
   
   // allocate and prepare service to resolve
   service = wsa_eprlist_alloc_elem (1, &alist);
   wsa_eprlist_elem_set_Addrs (service, wsa_eprlist_elem_get_Addrs (service));
   
   ws4d_qnamelist_init (&type_list);
   ws4d_qnamelist_addstring
   ("\"http://www.ws4d.org/services/life-cycle-manager\":life-cycle-manager",
   &type_list, &alist);
   
   service = wsa_eprlist_alloc_elem (1, &alist);
   if (dpws_probe( &service, &type_list, NULL, 600, 100, NULL, 1, service) == SOAP_OK)
   {
   // ERROR: URN wird bereits verwendet
   return LCM_ERR;
   }
   else
   {
   // alles OK, kein service mit der URN gefunden
   return LCM_OK;
   }
   */
  return LCM_OK;
}


/*----------------------------------
 // CRC
 // -----------------
 //
 // generate CRC-checksum
 /----------------------------------*/
static void
InitCRC ()
{
  int i = 0;
  int j = 0;

  for (i = 0; i < 256; i++)
    {
      unsigned int C = i;

      for (j = 0; j < 8; j++)
        C = (C & 1) ? (C >> 1) ^ 0xEDB88320L : (C >> 1);

      CRCTab[i] = C;
    }
}

/*----------------------------------
 // version_check
 // -----------------
 //
 // check if the new_version is higher
 // then the old_version one
 /----------------------------------*/
static int
check_version (char *new_version, char *old_version)
{
  int i = 0;
  int compare = 0;
  int lenght_new = strlen (new_version);
  int lenght_old = strlen (old_version);

  for (i = 0; (i < lenght_new) && (i < lenght_old); i++)
    {
      compare = strncmp (&new_version[i], &old_version[i], 1);

      if (compare > 0)
        {
          /* version number is higher */
          return LCM_OK;
        }
      else if (compare < 0)
        {
          /* version number is lower */
          return LCM_ERR;
        }
    }

  /* if the function come here, then the strings are equal */
  if (lenght_new > lenght_old)
    {
      /* bothe strings are equal till lenght_old.
       * lenght_new is longer, so is must be subversion,
       * and it is newer */
      return LCM_OK;

    }

  /* lenght_old is longer, but untill hier everything is correct -> error */
  return LCM_ERR;
}


/*----------------------------------
 // copy_file
 // -----------------
 //
 // copy a file
 /----------------------------------*/
static void
copy_file (char *to, char *file)
{
  FILE *in;
  FILE *out;
  int ch;

  open (to, CREATE_MODE);
  in = fopen (file, "rb");
  out = fopen (to, "wb");

  while ((ch = fgetc (in)) != EOF)
    fputc (ch, out);

  fclose (in);
  fclose (out);
}


/*----------------------------------
 // update_fail
 // -----------------
 //
 // the update failed, so restore the
 // old version
 /----------------------------------*/
static void
update_fail (struct ws4d_epr *new_elem, struct ws4d_epr *old_elem)
{
  char string_tmp[MAX_CHAR_LENGTH];
  char string_remove[MAX_CHAR_LENGTH];
  char string_to[MAX_CHAR_LENGTH];
  struct dirent *ep;

#ifdef WIN32
  /* TODO Win32 !!! */
#else
  DIR *dp;

  /*++++
   * list all files and subdirectories and deleate it
   * ++++ */
  sprintf (&string_tmp[0], "%s/%s/", WORK_DIR, get_cache_location (new_elem));
  dp = opendir (&string_tmp[0]);

  if (dp != NULL)
    {
      while ((ep = readdir (dp)))
        {
          /* deleate all with no '.'- and '..'-sign or not the old version number include */
          if ((strcmp (".", ep->d_name) != 0)
              && (strcmp ("..", ep->d_name) != 0)
              && (strcmp (get_cache_version (old_elem), ep->d_name) != 0))
            {
              sprintf (&string_remove[0], "%s/%s", &string_tmp[0],
                       ep->d_name);

              /* deleate file */
              remove (&string_remove[0]);
            }
        }
      (void) closedir (dp);
    }
  else
    {
      sprintf (&string_tmp[0], "Could not open the directory: %s/%s/",
               WORK_DIR, get_cache_location (old_elem));
      err_print (&string_tmp[0]);
    }

  /*++++
   * move old files
   *++++ */
  sprintf (&string_tmp[0], "%s/%s/%s/", WORK_DIR,
           get_cache_location (old_elem), get_cache_version (old_elem));
  dp = opendir (&string_tmp[0]);

  if (dp != NULL)
    {
      while ((ep = readdir (dp)))
        {
          /* move all files with no '.' or '..' sign */
          if ((strcmp (".", ep->d_name) != 0)
              && (strcmp ("..", ep->d_name) != 0))
            {
              sprintf (&string_remove[0], "%s/%s", &string_tmp[0],
                       ep->d_name);
              sprintf (&string_to[0], "%s/%s/%s", WORK_DIR,
                       get_cache_location (old_elem), ep->d_name);

              /* copy file */
              copy_file (&string_to[0], &string_remove[0]);

              /* deleate file */
              remove (&string_remove[0]);
            }
        }
      (void) closedir (dp);
    }
  else
    {
      sprintf (&string_tmp[0], "Could not open the directory: %s/%s/%s/",
               WORK_DIR, get_cache_location (old_elem),
               get_cache_version (old_elem));
      err_print (&string_tmp[0]);
    }

  /*++++
   * // remove old folder
   * //++++ */
  sprintf (&string_tmp[0], "%s/%s/%s/", WORK_DIR,
           get_cache_location (old_elem), get_cache_version (old_elem));
  remove (&string_remove[0]);

#endif
}


/*----------------------------------
 // move_dir
 // -----------------
 //
 // copy all files from one directory
 // to the destination directory
 /----------------------------------*/
static int
move_dir (char *to, char *from)
{
  char string_remove[MAX_CHAR_LENGTH];
  char string_to[MAX_CHAR_LENGTH];
  struct dirent *ep;

#ifdef WIN32
  /* TODO Win32 !!! */
#else
  DIR *dp;

  dp = opendir (from);

  if (dp != NULL)
    {
      while ((ep = readdir (dp)))
        {
          /* move every file with no '.' or '..' sign */
          if ((strcmp (".", ep->d_name) != 0)
              && (strcmp ("..", ep->d_name) != 0))
            {
              sprintf (&string_remove[0], "%s/%s", from, ep->d_name);
              sprintf (&string_to[0], "%s/%s", to, ep->d_name);

              /* copy file */
              copy_file (&string_to[0], &string_remove[0]);

              /* deleate file */
              remove (&string_remove[0]);
            }
        }
      (void) closedir (dp);
    }
  else
    {
      sprintf (&string_to[0], "Could not open the directory: %s", from);
      err_print (&string_to[0]);

      return LCM_ERR;
    }
#endif

  return LCM_OK;
}


/*----------------------------------
 // cleanup_dir
 // -----------------
 //
 // remove all files from the given
 // directory
 /----------------------------------*/
static int
cleanup_dir (const char *dir)
{
  char string_remove[MAX_CHAR_LENGTH];
  struct dirent *ep;

#ifdef WIN32
  /* TODO Win32 !!! */
#else
  DIR *dp;

  dp = opendir (dir);

  if (dp != NULL)
    {
      while ((ep = readdir (dp)))
        {
          /* move every file with no '.' or '..' sign */
          if ((strcmp (".", ep->d_name) != 0)
              && (strcmp ("..", ep->d_name) != 0))
            {
              sprintf (&string_remove[0], "%s/%s", dir, ep->d_name);
              err_print (&string_remove[0]);

              /* deleate file */
              if (remove (&string_remove[0]) != 0)
                {
                  /* maybe it is a directory, deleate all file inside */
                  cleanup_dir (&string_remove[0]);
                  rmdir (&string_remove[0]);
                }
            }
        }
      (void) closedir (dp);
    }
  else
    {
      sprintf (&string_remove[0], "Could not open the directory: %s", dir);
      err_print (&string_remove[0]);

      return LCM_ERR;
    }
#endif

  return LCM_OK;
}

/*++++++++++++++++++++++++++++++++++++++++++//
 // void parse_xml(char * file, char * xml)  //
 //----------------------------------------  //
 //++++++++++++++++++++++++++++++++++++++++++*/
static void
read_file (char *file, char *xml)
{
  FILE *fd;
  int i = 0;
  char string_tmp[MAX_CHAR_LENGTH];

  fd = fopen (file, "r");

  if (fd == NULL)
    {
      sprintf (&string_tmp[0], "File: %s not found", file);
      err_print (&string_tmp[0]);

      exit (0);
    }

  fseek (fd, 0L, SEEK_SET);

  while ((xml[i++] = fgetc (fd)) != EOF);

  xml[i] = '\0';
  fclose (fd);
}


/*++++++++++++++++++++++//
 //--> main function   //
 //++++++++++++++++++++++*/
int
main (int argc, char **argv)
{
  char string_tmp[MAX_CHAR_LENGTH];

#ifdef WIN32

#else
  DIR *dir;
  struct sigaction sa;
#endif

  /*++++++++++++++++++
   * Init LCM
   * +++++++++++++++++*/

  /* init cache */
  ws4d_eprlist_init (&service_register, ws4d_eprllist_init, NULL);

  /* check if dir exist */
#ifdef WIN32
  /* TODO Win32: erkennen ob Fehlgeschlagen oder Dir bereits existiert!! */
  if (GetCurrentDirectory (MAX_CHAR_LENGTH, &string_tmp[0]) > 0)
    {
      sprintf (&string_tmp[0], "%s/%s", string_tmp, WORK_DIR);

      if (CreateDirectory (&string_tmp[0], NULL))
        {
          /* new directory created */
        }
      else
        {
          /* new directory maby already there!? -> cleanup */
          err_print ("directory exist -> cleanup");
          cleanup_dir (WORK_DIR);
        }
    }
  else
    {
      err_print (error_list[LCM_ERR_CREAT_WORK_DIR].string);

      return 0;
    }
#else
  dir = opendir (WORK_DIR);

  if (!dir)
    {
      /* dir not exist */
      if (mkdir (WORK_DIR, WORK_AUTHO) != 0)
        {
          err_print (error_list[LCM_ERR_CREAT_WORK_DIR].string);

          return 0;
        }
    }
  else
    {
      /* close dir-handle */
      closedir (dir);
      /* new directory maby already there!? -> cleanup */
      err_print ("-> cleanup Work-Folder...");
      cleanup_dir (WORK_DIR);
      err_print ("-> cleanup completed");
    }
#endif /* check if dir exist */

  /*+++++++++++++++++++++++++++++++
   *  parsing command line options
   *++++++++++++++++++++++++++++++*/
  while ((argc > 1) && (argv[1][0] == '-'))
    {
      char *option = &argv[1][1];

      switch (option[0])
        {
        case 'i':
          if (strlen (option) > 2)
            {
              ++option;
              host = option;
            }
          else
            {
              --argc;
              ++argv;
              host = argv[1];
            }
          sprintf (&string_tmp[0], "LCM: Set host to \"%s\"", host);
          err_print (&string_tmp[0]);
          break;
        case 'l':
          if (strlen (option) > 2)
            {
              ++option;
              hosting_xml_path = option;
            }
          else
            {
              --argc;
              ++argv;
              hosting_xml_path = argv[1];
            }
          sprintf (&string_tmp[0], "LCM: Set hosting_xml_path to \"%s\"",
                   hosting_xml_path);
          err_print (&string_tmp[0]);
          break;
        default:
          sprintf (&string_tmp[0], "LCM: Bad option %s", argv[1]);
          err_print (&string_tmp[0]);
        }

      --argc;
      ++argv;
    }

  if (host == NULL || hosting_xml_path == NULL)
    {
      sprintf (&string_tmp[0], "LCM: No host ore XML-path was specified!");
      err_print (&string_tmp[0]);
      sprintf (&string_tmp[0], "\t -h: host IP-Adress");
      err_print (&string_tmp[0]);
      sprintf (&string_tmp[0], "\t -l: XML-Path");
      err_print (&string_tmp[0]);

      exit (0);
    }

  /*+++++++++++++++++++++++++++++++
   *  init DPWS-Service
   *++++++++++++++++++++++++++++++*/
  soap_init1 (&service, SOAP_ENC_MTOM);
  soap_omode (&service, SOAP_XML_INDENT);
  soap_set_namespaces (&service, lcm_namespaces);
  wsa_register_handle (&service);

  /* initialize services */
  if (dpws_init (&device, host)
      || lcm_setup_LCM (&device, &service, LCMHOSTEDSERVICE_WSDL, 100))
    {
      err_print ("LCM: Can't init services");
      dpws_done (&device);

      exit (0);
    }

  /* Set Metadata */
  lcm_set_wsdl (&device);

  /* Update Metadata */
  if (dpws_update_Metadata (&device))
    {
      err_print ("LCM: Can't init metadata");
      dpws_done (&device);

      exit (0);
    }

  /* install signal handler for SIGINT or Ctrl-C */
#ifdef WIN32
  signal (SIGINT, service_exit);
#else
  memset (&sa, 0, sizeof (sa));
  sa.sa_handler = service_exit;
  sigaction (SIGINT, &sa, NULL);
#endif
  err_print ("LCM: ready to serve... (Ctrl-C for shut down)");

  for (;;)
    {
      struct soap *handle = NULL, *soap_set[] = SOAP_HANDLE_SET (&service);
      int (*serve_requests[]) (struct soap * soap) =
        SOAP_SERVE_SET (lcm_serve_request);

      err_print ("LCM: waiting for request");
      handle = dpws_maccept (&device, 100000, 1, soap_set);

      if (handle)
        {
          sprintf (&string_tmp[0], "LCM: processing request from %s:%d",
                   inet_ntoa (handle->peer.sin_addr),
                   ntohs (handle->peer.sin_port));
          err_print (&string_tmp[0]);

          if (dpws_mserve (handle, 1, serve_requests))
            {
              soap_print_fault (handle, stderr);
            }

          /* clean up soaps internaly allocated memory */
          soap_end (handle);
        }
    }

  return -1;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++//
 //------------ Service functions ----------------//
 //+++++++++++++++++++++++++++++++++++++++++++++++*/

/*----------------------------------
 // __ns1__installService
 // -----------------
 //
 // install a new service
 /----------------------------------*/
int
__ns1__install (struct soap *soap,
                struct _ns1__install *ns1__install,
                struct _ns1__installResponse *ns1__installResponse)
{
  char string_tmp[MAX_CHAR_LENGTH];
  char dir_string[MAX_CHAR_LENGTH];
  char *xml;
  FILE *datei;
  struct ws4d_epr *cache_elem = NULL;
  struct ws4d_epr *cache_elem_tmp = NULL;
  struct soap parser;
  struct _wsm__Metadata Metadata;

#ifdef WIN32

#else
  DIR *dir;
#endif

  WS4D_UNUSED_PARAM(soap);

  sprintf (&string_tmp[0], "---> incoming query for installService");
  err_print (&string_tmp[0]);

  /*------------------------------------------
   // create a new cache element
   //-----------------------------------------*/
  cache_elem = add_new_service (&service_register);

  if (!cache_elem)
    {
      /* new cache entry could not created */
      err_print (error_list[LCM_ERR_CACHE_ERROR].string);
      ns1__installResponse->status = LCM_ERR_CACHE_ERROR;
      ns1__installResponse->serviceID = 0;
      return SOAP_OK;
    }

  /*----------------------------------------------------
   // check the given file and create folder
   // 1. create folder for service in the WORK_DIR
   // 2. copy the zip-file
   // 3. unpack the zip-file
   // 4. check if manifest exist
   // 5. readout the data out of the manifest
   // 6. check if the service file exist
   //---------------------------------------------------*/

  /*++++
   * // create TMP-folder for the service
   * //++++ */
  sprintf (string_tmp, "%s/%s", WORK_DIR, TMP_DIR_FOR_INSTALL);
  if (set_cache_location (cache_elem, string_tmp) != LCM_OK)
    {
      /* error in the cache */
      err_print (error_list[LCM_ERR_CACHE_ERROR].string);
      ns1__installResponse->status = LCM_ERR_CACHE_ERROR;
      ns1__installResponse->serviceID = 0;
      set_cache_state (cache_elem, LCM_ERR);
      set_cache_valid (cache_elem, LCM_INVALID);
      free_service (cache_elem);
      return SOAP_OK;
    }

#ifdef WIN32
  if (!CreateDirectory (string_tmp, NULL))
    {
      err_print (error_list[LCM_ERR_WORK_DIR_EXIST].string);
      ns1__installResponse->status = LCM_ERR_WORK_DIR_EXIST;
      ns1__installResponse->serviceID = 0;
      set_cache_state (cache_elem, LCM_ERR);
      set_cache_valid (cache_elem, LCM_INVALID);
      free_service (cache_elem);
      return SOAP_OK;
    }
#else
  dir = opendir (string_tmp);

  if (!dir)
    {
      /* dir not exist */
      if (mkdir (string_tmp, WORK_AUTHO) != 0)
        {
          err_print (error_list[LCM_ERR_CREAT_WORK_DIR].string);
          ns1__installResponse->status = LCM_ERR_CREAT_WORK_DIR;
          ns1__installResponse->serviceID = 0;
          set_cache_state (cache_elem, LCM_ERR);
          set_cache_valid (cache_elem, LCM_INVALID);
          free_service (cache_elem);
          return SOAP_OK;
        }
    }
  else
    {
      /* ERROR folder exist, close dir-handle */
      closedir (dir);
      err_print (error_list[LCM_ERR_WORK_DIR_EXIST].string);
      ns1__installResponse->status = LCM_ERR_WORK_DIR_EXIST;
      ns1__installResponse->serviceID = 0;
      set_cache_state (cache_elem, LCM_ERR);
      set_cache_valid (cache_elem, LCM_INVALID);
      free_service (cache_elem);
      return SOAP_OK;
    }
#endif
  /*++++
   * // save attachment in the WORK-folder
   * //++++ */

  sprintf (&string_tmp[0], "Attachment: Attachment size(%d)",
           ns1__install->Param.__size);
  err_print (&string_tmp[0]);
  sprintf (&string_tmp[0], "%s/service_0.zip",
           get_cache_location (cache_elem));

  if (writefile
      (&string_tmp[0], ns1__install->Param.__ptr,
       ns1__install->Param.__size) < 0)
    {
      /* can not save attachment --> error */
      err_print (error_list[LCM_ERR_SAVE_ATTACHMENT].string);
      ns1__installResponse->status = LCM_ERR_SAVE_ATTACHMENT;
      ns1__installResponse->serviceID = 0;
      set_cache_state (cache_elem, LCM_ERR);
      set_cache_valid (cache_elem, LCM_INVALID);
      /* deleate folder */
      sprintf (&dir_string[0], "rm -r %s/", get_cache_location (cache_elem));
      system (dir_string);
      free_service (cache_elem);
      /* return */
      return SOAP_OK;
    }

  /*++++
   * // unpack the archives
   * //++++ */
  sprintf (&string_tmp[0], "%s/service_0.zip",
           get_cache_location (cache_elem));

  unpack_archive(string_tmp, get_cache_location (cache_elem));

  chdir ("../..");

  /*++++
   * // check if there is a manifest
   * //++++ */
  sprintf (&string_tmp[0], "%s/manifest", get_cache_location (cache_elem));
  err_print ("manifest-> ");
  err_print (string_tmp);
  datei = fopen (&string_tmp[0], "rb");

  if (datei == NULL)
    {
      /* no manifest exist --> error */
      err_print (error_list[LCM_ERR_NO_MANIFEST].string);
      ns1__installResponse->status = LCM_ERR_NO_MANIFEST;
      ns1__installResponse->serviceID = 0;
      set_cache_state (cache_elem, LCM_ERR);
      set_cache_valid (cache_elem, LCM_INVALID);
      /* deleate folder */
      sprintf (&dir_string[0], "rm -r %s/", get_cache_location (cache_elem));
      system (dir_string);
      free_service (cache_elem);
      /* return */
      return SOAP_OK;
    }
  else
    {
      err_print ("manifest exist -> OK");
    }
  fclose (datei);

  /*------------------------------------------------------------------------------
   // readout the data out of the manifest
   //-----------------------------------------------------------------------------*/
  if (readout_manifest
      (&string_tmp[0], get_cache_manifest_struct (cache_elem)) != LCM_OK)
    {
      /* no service file exist --> fehler */
      err_print (error_list[LCM_ERR_READ_MANIFEST].string);
      ns1__installResponse->status = LCM_ERR_READ_MANIFEST;
      ns1__installResponse->serviceID = 0;
      set_cache_state (cache_elem, LCM_ERR);
      set_cache_valid (cache_elem, LCM_INVALID);
      /* deleate folder */
      sprintf (&dir_string[0], "rm -r %s/", get_cache_location (cache_elem));
      system (dir_string);
      free_service (cache_elem);
      /* return */
      return SOAP_OK;
    }
  else
    {
      /* everthing is fine, print all assumed data */
      err_print ("readout from manifest -->");
      sprintf (&string_tmp[0], "\tservice_name: %s",
               get_cache_name (cache_elem));
      err_print (&string_tmp[0]);
      sprintf (&string_tmp[0], "\twsdl_name   : %s",
               get_cache_wsdl (cache_elem));
      err_print (&string_tmp[0]);
      sprintf (&string_tmp[0], "\txml_name    : %s",
               get_cache_xml (cache_elem));
      err_print (&string_tmp[0]);
      sprintf (&string_tmp[0], "\tversion     : %s",
               get_cache_version (cache_elem));
      err_print (&string_tmp[0]);
    }

  /*++++
   * check if the service exist
   *++++ */
  sprintf (&string_tmp[0], "%s/%s", get_cache_location (cache_elem),
           get_cache_name (cache_elem));
  datei = fopen (&string_tmp[0], "rb");

  if (datei == NULL)
    {
      /* file not found --> error */
      err_print (error_list[LCM_ERR_NO_SERVICE].string);
      ns1__installResponse->status = LCM_ERR_NO_SERVICE;
      ns1__installResponse->serviceID = 0;
      set_cache_state (cache_elem, LCM_ERR);
      set_cache_valid (cache_elem, LCM_INVALID);
      /* deleate folder */
      sprintf (&dir_string[0], "rm -r %s/", get_cache_location (cache_elem));
      system (dir_string);
      free_service (cache_elem);
      /* return */
      return SOAP_OK;
    }
  else
    {
      /* the assumed name is correct */
      fclose (datei);
    }

  sprintf (&string_tmp[0], "using service-file: %s",
           get_cache_name (cache_elem));
  err_print (&string_tmp[0]);

  /*++++
   * readout the ID
   *++++ */
  xml = (char *) malloc (XML_SIZE * sizeof (char));
  if (xml == NULL)
    {
      err_print (error_list[LCM_ERR_MALLOC].string);
      ns1__installResponse->status = LCM_ERR_MALLOC;
      ns1__installResponse->serviceID = 0;
      set_cache_state (cache_elem, LCM_ERR);
      set_cache_valid (cache_elem, LCM_INVALID);
      /* deleate folder */
      sprintf (&dir_string[0], "rm -r %s/", get_cache_location (cache_elem));
      system (dir_string);
      free_service (cache_elem);
      /* return */
      return SOAP_OK;
    }

  sprintf (&string_tmp[0], "%s/%s", get_cache_location (cache_elem),
           get_cache_xml (cache_elem));
  read_file (&string_tmp[0], xml);
  soap_init (&parser);
  soap_set_namespaces (&parser, lcm_namespaces);
  soap_begin_deser (&parser, xml, strlen (xml), lcm_namespaces);
  soap_default__wsm__Metadata (&parser, &Metadata);

  if (soap_get__wsm__Metadata (&parser, &Metadata, "wsm:Metadata", "") ==
      NULL)
    {
      err_print (error_list[LCM_ERR_METADATA].string);
      ns1__installResponse->status = LCM_ERR_METADATA;
      ns1__installResponse->serviceID = 0;
      set_cache_state (cache_elem, LCM_ERR);
      set_cache_valid (cache_elem, LCM_INVALID);
      /* deleate folder */
      sprintf (&dir_string[0], "rm -r %s/", get_cache_location (cache_elem));
      system (dir_string);
      free_service (cache_elem);
      free (xml);
      /* return */
      return SOAP_OK;
    }

  /*++++
   * check if the ID did not exist
   *++++ */
  cache_elem_tmp =
    find_cache (Metadata.wsm__MetadataSection->union_MetadataSection.
                wsdp__Relationship->wsdp__Hosted->wsdp__ServiceId);
  if (cache_elem_tmp)
    {
      /* the given ID was found in cache, no doubl ID is allowed */
      err_print (error_list[LCM_ERR_URN_IN_USE].string);
      ns1__installResponse->status = LCM_ERR_URN_IN_USE;
      ns1__installResponse->serviceID = 0;
      set_cache_state (cache_elem, LCM_ERR);
      set_cache_valid (cache_elem, LCM_INVALID);
      /* deleate folder */
      sprintf (&dir_string[0], "rm -r %s/", get_cache_location (cache_elem));
      system (dir_string);
      free_service (cache_elem);
      free (xml);
      /* return */
      return SOAP_OK;
    }

  /* save the ID */
  set_cache_id (cache_elem,
                Metadata.wsm__MetadataSection->union_MetadataSection.
                wsdp__Relationship->wsdp__Hosted->wsdp__ServiceId);
  free (xml);
  sprintf (&string_tmp[0], "using ID: %s", get_cache_id (cache_elem));
  err_print (&string_tmp[0]);

  /*++++
   * create final folder for the service, name of the folder is the serviceID
   *++++ */
  sprintf (string_tmp, "%s/%s", WORK_DIR, get_cache_id (cache_elem));
#ifdef WIN32
  if (!CreateDirectory (string_tmp, NULL))
    {
      err_print (error_list[LCM_ERR_WORK_DIR_EXIST].string);
      ns1__installResponse->status = LCM_ERR_WORK_DIR_EXIST;
      ns1__installResponse->serviceID = 0;
      set_cache_state (cache_elem, LCM_ERR);
      set_cache_valid (cache_elem, LCM_INVALID);
      /* deleate folder */
      sprintf (&dir_string[0], "rm -r %s/", get_cache_location (cache_elem));
      system (dir_string);
      free_service (cache_elem);
      /* return */
      return SOAP_OK;
    }
#else
  dir = opendir (string_tmp);
  if (!dir)
    {
      /* dir not exist */
      if (mkdir (string_tmp, WORK_AUTHO) != 0)
        {
          err_print (error_list[LCM_ERR_CREAT_WORK_DIR].string);
          ns1__installResponse->status = LCM_ERR_CREAT_WORK_DIR;
          ns1__installResponse->serviceID = 0;
          set_cache_state (cache_elem, LCM_ERR);
          set_cache_valid (cache_elem, LCM_INVALID);
          /* deleate folder */
          sprintf (&dir_string[0], "rm -r %s/",
                   get_cache_location (cache_elem));
          system (dir_string);
          free_service (cache_elem);
          /* return */
          return SOAP_OK;
        }
    }
  else
    {
      /* ERROR folder exist, close dir-handle */
      closedir (dir);
      err_print (error_list[LCM_ERR_WORK_DIR_EXIST].string);
      ns1__installResponse->status = LCM_ERR_WORK_DIR_EXIST;
      ns1__installResponse->serviceID = 0;
      set_cache_state (cache_elem, LCM_ERR);
      set_cache_valid (cache_elem, LCM_INVALID);
      /* deleate folder */
      sprintf (&dir_string[0], "rm -r %s/", get_cache_location (cache_elem));
      system (dir_string);
      free_service (cache_elem);
      return SOAP_OK;
    }
#endif
  /* move all data */
  if (move_dir (string_tmp, get_cache_location (cache_elem)) != LCM_OK)
    {
      /* ERROR folder exist, close dir-handle */
#ifdef WIN32
      /* TODO Win32 !!! */
#else
      closedir (dir);
#endif
      err_print (error_list[LCM_ERR_DIR_NOT_MOVED].string);
      ns1__installResponse->status = LCM_ERR_DIR_NOT_MOVED;
      ns1__installResponse->serviceID = 0;
      set_cache_state (cache_elem, LCM_ERR);
      set_cache_valid (cache_elem, LCM_INVALID);
      /* deleate folder */
      sprintf (&dir_string[0], "rm -r %s/", get_cache_location (cache_elem));
      system (dir_string);
      sprintf (&dir_string[0], "rm -r %s/", string_tmp);
      system (dir_string);
      free_service (cache_elem);
      /* return */
      return SOAP_OK;
    }

  /* deleate TMP-folder */
  sprintf (&dir_string[0], "rm -r %s/", get_cache_location (cache_elem));
  system (dir_string);

  /* set new location of the services */
  if (set_cache_location (cache_elem, string_tmp) != LCM_OK)
    {
      /*error in the cache */
      err_print (error_list[LCM_ERR_CACHE_ERROR].string);
      ns1__installResponse->status = LCM_ERR_CACHE_ERROR;
      ns1__installResponse->serviceID = 0;
      set_cache_state (cache_elem, LCM_ERR);
      set_cache_valid (cache_elem, LCM_INVALID);
      /* deleate folder */
      sprintf (&dir_string[0], "rm -r %s/", get_cache_location (cache_elem));
      system (dir_string);
      sprintf (&dir_string[0], "rm -r %s/", string_tmp);
      system (dir_string);
      free_service (cache_elem);
      /* return */
      return SOAP_OK;
    }

  /*-----------------------------------------
   // TODO: test if everything is insert correct
   //-----------------------------------------*/
  if (ns1__installResponse->status != SOAP_ERR)
    {
      /* everything is fine, set the status */
      set_cache_state (cache_elem, LCM_INSTALL);
      set_cache_valid (cache_elem, LCM_VALID);
    }
  else
    {
      /* error occurred */
      set_cache_state (cache_elem, LCM_ERR);
      set_cache_valid (cache_elem, LCM_INVALID);
      err_print (error_list[LCM_ERR_INTERNAL_CACHE].string);
      ns1__installResponse->status = LCM_ERR_INTERNAL_CACHE;
      ns1__installResponse->serviceID = 0;
      set_cache_state (cache_elem, LCM_ERR);
      set_cache_valid (cache_elem, LCM_INVALID);
      /* deleate folder */
      sprintf (&dir_string[0], "rm -r %s/", get_cache_location (cache_elem));
      system (dir_string);
      free_service (cache_elem);
      /* return */
      return SOAP_OK;
    }

  /*-------------------------
   // set return parameters
   //-------------------------*/
  ns1__installResponse->status = SOAP_OK;
  ns1__installResponse->serviceID = get_cache_id (cache_elem);
  set_cache_state (cache_elem, LCM_INSTALL);
  set_cache_valid (cache_elem, LCM_VALID);

  return SOAP_OK;
}


/*----------------------------------
 // __ns1__uninstallService
 // -----------------
 //
 // uninstall the given Service
 /----------------------------------*/
int
__ns1__uninstall (struct soap *soap,
                  struct _ns1__uninstall *ns1__uninstall,
                  struct _ns1__uninstallResponse *ns1__uninstallResponse)
{
  struct ws4d_epr *cache_elem = NULL;
  char dir[MAX_CHAR_LENGTH];
  char string_tmp[MAX_CHAR_LENGTH];

  WS4D_UNUSED_PARAM(soap);

  err_print ("---> incoming query for uninstallService");
  sprintf (&string_tmp[0], "\t ServiceID: %s", ns1__uninstall->serviceID);
  err_print (&string_tmp[0]);

  ns1__uninstallResponse->status = SOAP_OK;

  /*------------------------------------------------
   // test if handle is OK
   // search for cache entry
   //-----------------------------------------------*/
  cache_elem = find_cache (ns1__uninstall->serviceID);
  if (!cache_elem)
    {
      /* service can not found in the cache */
      err_print (error_list[LCM_ERR_NO_VALID_HANDLE].string);
      ns1__uninstallResponse->status = LCM_ERR_NO_VALID_HANDLE;
      return SOAP_OK;
    }

  /*------------------------------------------------------------------------
   // ensure that the service is not running, when running stop it
   //-----------------------------------------------------------------------*/
  if (get_cache_pid (cache_elem) != 0)
    {
  /*---------------------------------------------
  // deleate metadata xml
  //--------------------------------------------*/
      sprintf (&string_tmp[0], "%s/%s.xml", hosting_xml_path,
               get_cache_id (cache_elem));
      remove (string_tmp);

      /* service is running, so stop it */
#ifdef WIN32
      /* TODO: Win32 */
#else
      kill (get_cache_pid (cache_elem), SIGTERM);
      err_print ("stopping Service");
      sleep (1);
      kill (get_cache_pid (cache_elem), SIGKILL);
      set_cache_pid (cache_elem, 0);
#endif
    }

  /*-----------------------------
   // deleate folder
   //----------------------------*/
  sprintf (&dir[0], "rm -r %s/%s/", WORK_DIR, get_cache_id (cache_elem));
  if (system (dir) != 0)
    {
      err_print (error_list[LCM_ERR_DELETE_DIR].string);
      ns1__uninstallResponse->status = LCM_ERR_DELETE_DIR;
      set_cache_state (cache_elem, LCM_LEER);
      set_cache_valid (cache_elem, LCM_INVALID);
      return SOAP_OK;
    }
  err_print ("all data removed");

  /*--------------------------------------------
   // deleate the corressponding cache element
   //-------------------------------------------*/
  set_cache_state (cache_elem, LCM_LEER);
  set_cache_valid (cache_elem, LCM_INVALID);
  free_service (cache_elem);

  return SOAP_OK;
}


/*----------------------------------
 // __ns1__startService
 // -----------------
 //
 // start a spezific Service
 /----------------------------------*/
int
__ns1__start (struct soap *soap,
              struct _ns1__start *ns1__start,
              struct _ns1__startResponse *ns1__startResponse)
{
  char string_tmp[MAX_CHAR_LENGTH];
  char string_tmp_2[MAX_CHAR_LENGTH];
  struct ws4d_epr *cache_elem = NULL;

#ifdef WIN32
#else
  pid_t kind1;
  char *argumente[4];
#endif

  WS4D_UNUSED_PARAM(soap);

  err_print ("---> incoming query for startService");
  sprintf (&string_tmp[0], "\t ServiceID: %s", ns1__start->serviceID);
  err_print (&string_tmp[0]);
  ns1__startResponse->status = SOAP_OK;

  /*---------------------------------------------
  // find the cache element
  //-------------------------------------------*/
  cache_elem = find_cache (ns1__start->serviceID);
  if (!cache_elem)
    {
      /* service did not found in the cache */
      err_print (error_list[LCM_ERR_NO_VALID_HANDLE].string);
      ns1__startResponse->status = LCM_ERR_NO_VALID_HANDLE;
      return SOAP_OK;
    }

  if (get_cache_pid (cache_elem) != 0)
    {
      /* service already started */
      err_print (error_list[LCM_ERR_ALREADY_RUNNING].string);
      ns1__startResponse->status = LCM_ERR_ALREADY_RUNNING;
      return SOAP_OK;
    }

  /*-------------------
   // start service
   //------------------*/
  err_print ("--->  starte Service  <---");
#ifdef WIN32
  /* TODO Win32 !!! */
#else
  /* start new process */
  switch (kind1 = fork ())
    {
    case -1:
      err_print (error_list[LCM_ERR_FORK].string);
      ns1__startResponse->status = LCM_ERR_FORK;
      set_cache_state (cache_elem, LCM_ERR);
      return SOAP_OK;

    case 0:
      sprintf (&string_tmp[0],
               "starting Service under new process with pid: %d", getpid ());
      err_print (&string_tmp[0]);
      sprintf (&string_tmp[0], "%s/%s", get_cache_location (cache_elem),
               get_cache_name (cache_elem));
      /* set the authorization */
      sprintf (string_tmp_2, "chmod 777 %s", string_tmp);
      system (string_tmp_2);
      /* start service */
      argumente[0] = get_cache_id (cache_elem);
      argumente[1] = (char *) "-h";
      argumente[2] = host;
      argumente[3] = NULL;
      execvp (string_tmp, argumente);
      err_print ("--->> Service wurde beendet <<---");
      exit (0);

    default:
      sprintf (&string_tmp[0], "new Service starting with pid: %d", kind1);
      err_print (&string_tmp[0]);
      /* save pid */
      set_cache_pid (cache_elem, kind1);
      set_cache_state (cache_elem, LCM_RUNNING);
      set_cache_valid (cache_elem, LCM_VALID);
    }
#endif
  /*-------------------
   // copy the metadata.xml to the hosting_service
   //------------------*/
  err_print ("transfer metadata to hosting-service:");
  /* xml-file-from */
  sprintf (&string_tmp[0], "%s/%s", get_cache_location (cache_elem),
           get_cache_xml (cache_elem));
  err_print (&string_tmp[0]);
  /* xml-file-to */
  sprintf (&string_tmp_2[0], "%s/%s.xml", hosting_xml_path,
           get_cache_id (cache_elem));
  err_print (&string_tmp_2[0]);
  /* copy file */
  copy_file (string_tmp_2, string_tmp);

  return SOAP_OK;
}


/*----------------------------------
 // __ns1__stopService
 // -----------------
 //
 // stop a spezific Service
 /----------------------------------*/
int
__ns1__stop (struct soap *soap,
             struct _ns1__stop *ns1__stop,
             struct _ns1__stopResponse *ns1__stopResponse)
{
  char string_tmp[MAX_CHAR_LENGTH];
  struct ws4d_epr *cache_elem = NULL;

#ifdef WIN32
  DWORD ThreadExitCode = 0;
#endif

  WS4D_UNUSED_PARAM(soap);

  err_print ("---> incoming query for stopService");
  sprintf (&string_tmp[0], "\t ServiceID: %s", ns1__stop->serviceID);
  err_print (&string_tmp[0]);

  ns1__stopResponse->status = SOAP_OK;

  /*---------------------------------------------
   // search for cache element
   //--------------------------------------------*/
  cache_elem = find_cache (ns1__stop->serviceID);
  if (!cache_elem)
    {
      /* service not found in the cache */
      err_print (error_list[LCM_ERR_NO_VALID_HANDLE].string);
      ns1__stopResponse->status = LCM_ERR_NO_VALID_HANDLE;

      return SOAP_OK;
    }

  /*---------------------------------------------
   // deleate metadata xml
   //--------------------------------------------*/
  sprintf (&string_tmp[0], "%s/%s.xml", hosting_xml_path,
           get_cache_id (cache_elem));
  remove (string_tmp);

  /*---------------------------------
   // stopp service process
   //--------------------------------*/
  if (get_cache_pid (cache_elem) == 0
      || get_cache_pid (cache_elem) == LCM_ERR)
    {
      if (get_cache_state (cache_elem) != LCM_RUNNING)
        {
          /* the PID is not set, but the service is running */
          err_print (error_list[LCM_ERR_VALID_PID].string);
          ns1__stopResponse->status = LCM_ERR_VALID_PID;
          set_cache_state (cache_elem, LCM_NOT_SPECIFIED);
          return SOAP_OK;
        }
      else
        {
          /* the PID is not set and the service is not running */
          err_print (error_list[LCM_ERR_DEVICE_IS_NOT_RUNNING].string);
          ns1__stopResponse->status = LCM_ERR_DEVICE_IS_NOT_RUNNING;
          return SOAP_OK;
        }
    }

  /* send signal */
#ifdef WIN32
  /* TODO Win32 !!! */
  /*
   * if( !TerminateThread( cache[ index ].pid, ThreadExitCode ) )
   * {
   * // thread konnte nicht gestoppt werden
   * err_print( error_list[ LCM_ERR_THREAD_NOT_TERMINATE ].string );
   * ns1__stopResponse->status = LCM_ERR_THREAD_NOT_TERMINATE;
   * set_cache_state( cache_elem, LCM_NOT_SPECIFIED );
   * set_cache_valid( cache_elem, LCM_INVALID );
   * return SOAP_OK;
   * }
   * else
   * {
   * err_print("Thread terminate");
   * }
   */
#else
  kill (get_cache_pid (cache_elem), SIGTERM);
  sleep (3);
  kill (get_cache_pid (cache_elem), SIGKILL);
#endif

  /* reset PID */
  reset_cache_pid (cache_elem);
  err_print ("Service ist gestoppt");
  set_cache_state (cache_elem, LCM_RESOLVE);
  set_cache_valid (cache_elem, LCM_VALID);

  return SOAP_OK;
}


/*----------------------------------
 // __ns1__ListAll
 // -----------------
 //
 // return the UUID of all cache-entries
 /----------------------------------*/
int
__ns1__ListAll (struct soap *soap,
                struct _ns1__ListAll *ns1__ListAll,
                struct _ns1__ListAllResponse *ns1__ListAllResponse)
{
  struct ws4d_epr *elem = NULL, *iter = NULL;
  char string_tmp[MAX_CHAR_LENGTH];
  int list_index = 0;

  WS4D_UNUSED_PARAM(soap);
  WS4D_UNUSED_PARAM(ns1__ListAll);

  err_print ("---> incoming query for getAllServices");

  memset (ns1__ListAllResponse, 0, sizeof (struct _ns1__ListAllResponse));

  /* get count of installed services */
  ws4d_eprlist_foreach (elem, iter, &service_register)
  {
    if (elem)
      {
        list_index++;
      }
  }

  /* malloc memory for response */
  ns1__ListAllResponse->services = malloc (list_index * sizeof (char *));

  /* go through all services */
  ws4d_eprlist_foreach (elem, iter, &service_register)
  {
    if (elem)
      {
        err_print
          ("-------------------------------------------------------------------------");
        sprintf (&string_tmp[0], "\t id      : %s",
                 (get_cache_id (elem)) ? (get_cache_id (elem)) : ("NULL"));
        err_print (&string_tmp[0]);
        sprintf (&string_tmp[0], "\t location: %s",
                 (get_cache_location (elem)) ? (get_cache_location (elem))
                 : ("NULL"));
        err_print (&string_tmp[0]);
        sprintf (&string_tmp[0], "\t status  : %d", get_cache_state (elem));
        err_print (&string_tmp[0]);
        sprintf (&string_tmp[0], "\t valid   : %d", get_cache_valid (elem));
        err_print (&string_tmp[0]);
        sprintf (&string_tmp[0], "\t name    : %s",
                 (get_cache_name (elem)) ? (get_cache_name (elem))
                 : ("NULL"));
        err_print (&string_tmp[0]);
        sprintf (&string_tmp[0], "\t wsdl    : %s",
                 (get_cache_wsdl (elem)) ? (get_cache_wsdl (elem))
                 : ("NULL"));
        err_print (&string_tmp[0]);
        sprintf (&string_tmp[0], "\t xml     : %s",
                 (get_cache_xml (elem)) ? (get_cache_xml (elem)) : ("NULL"));
        err_print (&string_tmp[0]);
        sprintf (&string_tmp[0], "\t version : %s",
                 (get_cache_version (elem)) ? (get_cache_version (elem))
                 : ("NULL"));
        err_print (&string_tmp[0]);
        sprintf (&string_tmp[0], "\t pid     : %d", get_cache_pid (elem));
        err_print (&string_tmp[0]);
        err_print
          ("-------------------------------------------------------------------------");

        /* response */
        ns1__ListAllResponse->services[ns1__ListAllResponse->
                                       __sizeservices++] =
          get_cache_id (elem);
      }

    sprintf (&string_tmp[0], "array-size: %d",
             ns1__ListAllResponse->__sizeservices);
    err_print (&string_tmp[0]);

    for (list_index = 0; list_index < ns1__ListAllResponse->__sizeservices; list_index++)
      {
        sprintf (&string_tmp[0], "\t service %d: %s", list_index,
                 ns1__ListAllResponse->services[list_index]);
        err_print (&string_tmp[0]);
      }
  }

  return SOAP_OK;
}


/*----------------------------------
 // __ns1__getServiceState
 // -----------------
 //
 // return the state of a spezific Service, for
 // example if it is running, stoppt ore resolved
 /----------------------------------*/
int
__ns1__getState (struct soap *soap,
                 struct _ns1__getState *ns1__getState,
                 struct _ns1__getStateResponse *ns1__getStateResponse)
{
  char string_tmp[MAX_CHAR_LENGTH];
  struct ws4d_epr *cache_elem = NULL;

  WS4D_UNUSED_PARAM(soap);

  err_print ("---> incoming query for getServiceState");
  sprintf (&string_tmp[0], "\t ServiceID: %s", ns1__getState->serviceID);
  err_print (&string_tmp[0]);

  /*---------------------------------------------
   // search for cache element
   //--------------------------------------------*/
  cache_elem = find_cache (ns1__getState->serviceID);
  if (!cache_elem)
    {
      /* service not found in the cache */
      err_print (error_list[LCM_ERR_NO_VALID_HANDLE].string);
      ns1__getStateResponse->serviceState = LCM_INVALID;
      ns1__getStateResponse->status = LCM_ERR_NO_VALID_HANDLE;
      return SOAP_OK;
    }

  /* write data */
  ns1__getStateResponse->serviceState = get_cache_state (cache_elem);   /* the current state of the service */
  ns1__getStateResponse->valid = get_cache_valid (cache_elem);  /* show the result of the resolve check */
  ns1__getStateResponse->uuid = get_cache_id (cache_elem);      /* ID of the service, it is used as the handle of the service */
  ns1__getStateResponse->soFile = get_cache_name (cache_elem);  /* the location, where the service is installed */
  ns1__getStateResponse->host = host;   /* the IP, which the service hase to use */
  ns1__getStateResponse->version = get_cache_version (cache_elem);      /* version number */
  ns1__getStateResponse->status = SOAP_OK;

  /*---------------------------------------------
   // print to console
   //--------------------------------------------*/
  sprintf (&string_tmp[0], "\t id      : %s",
           (get_cache_id (cache_elem)) ? (get_cache_id (cache_elem))
           : ("NULL"));
  err_print (&string_tmp[0]);
  sprintf (&string_tmp[0], "\t location: %s",
           (get_cache_location (cache_elem))
           ? (get_cache_location (cache_elem)) : ("NULL"));
  err_print (&string_tmp[0]);
  sprintf (&string_tmp[0], "\t status  : %d", get_cache_state (cache_elem));
  err_print (&string_tmp[0]);
  sprintf (&string_tmp[0], "\t valid   : %d", get_cache_valid (cache_elem));
  err_print (&string_tmp[0]);
  sprintf (&string_tmp[0], "\t name    : %s",
           (get_cache_name (cache_elem)) ? (get_cache_name (cache_elem))
           : ("NULL"));
  err_print (&string_tmp[0]);
  sprintf (&string_tmp[0], "\t wsdl    : %s",
           (get_cache_wsdl (cache_elem)) ? (get_cache_wsdl (cache_elem))
           : ("NULL"));
  err_print (&string_tmp[0]);
  sprintf (&string_tmp[0], "\t xml     : %s",
           (get_cache_xml (cache_elem)) ? (get_cache_xml (cache_elem))
           : ("NULL"));
  err_print (&string_tmp[0]);
  sprintf (&string_tmp[0], "\t version : %s",
           (get_cache_version (cache_elem)) ? (get_cache_version (cache_elem))
           : ("NULL"));
  err_print (&string_tmp[0]);
  sprintf (&string_tmp[0], "\t pid     : %d", get_cache_pid (cache_elem));
  err_print (&string_tmp[0]);

  return SOAP_OK;
}


/*----------------------------------
 // __ns1__resolveService
 // -----------------
 //
 // when a Service is installed, it is essential
 // to check for dependencies, this function
 // initiate the check for a spezific Service
 /----------------------------------*/
int
__ns1__resolve (struct soap *soap,
                struct _ns1__resolve *ns1__resolve,
                struct _ns1__resolveResponse *ns1__resolveResponse)
{
  char string_tmp[MAX_CHAR_LENGTH];
  struct ws4d_epr *cache_elem = NULL;

  WS4D_UNUSED_PARAM(soap);

  err_print ("---> incoming query for resolveService");
  sprintf (&string_tmp[0], "\t ServiceID: %s", ns1__resolve->serviceID);
  err_print (&string_tmp[0]);

  /*---------------------------------------------
   // search for cache element
   //--------------------------------------------*/
  cache_elem = find_cache (ns1__resolve->serviceID);
  if (!cache_elem)
    {
      /* serviceID not found in cache */
      err_print (error_list[LCM_ERR_NO_VALID_HANDLE].string);
      ns1__resolveResponse->status = LCM_ERR_NO_VALID_HANDLE;
      return SOAP_OK;
    }

  /* TODO: Service-Abhängigkeiten testen */

  /* TODO: Resolve Funktion anspringen */

  set_cache_state (cache_elem, LCM_RESOLVE);
  set_cache_valid (cache_elem, LCM_VALID);
  ns1__resolveResponse->status = LCM_OK;

  return SOAP_OK;
}


/*----------------------------------
 // __ns1__updateService
 // -----------------
 //
 // when a newer version of an installed
 // Service are available and like to
 // be installed, this function will
 // replace the old with the new
 // version, all old files will deleted
 /----------------------------------*/
int
__ns1__update (struct soap *soap,
               struct _ns1__update *ns1__update,
               struct _ns1__updateResponse *ns1__updateResponse)
{
  int res;
  char *xml;
  char old_version_folder[MAX_CHAR_LENGTH];
  char string_tmp[MAX_CHAR_LENGTH];
  char string_from[MAX_CHAR_LENGTH];
  char string_to[MAX_CHAR_LENGTH];
  char *argumente[4];
  FILE *datei;
  struct ws4d_epr *save_cache = NULL;
  struct ws4d_epr *cache_elem = NULL;
  struct soap parser;
  struct _wsm__Metadata Metadata;
  ws4d_alloc_list alist;

#ifdef WIN32

#else
  DIR *dir;
  struct dirent *p_dir;
  pid_t kind1;
#endif

  WS4D_UNUSED_PARAM(soap);

  err_print ("---> incoming query for updateService");
  sprintf (&string_tmp[0], "\t ServiceID: %s", ns1__update->serviceID);
  err_print (&string_tmp[0]);

  /*++++
   * // search for cache element
   * //++++ */
  cache_elem = find_cache (ns1__update->serviceID);
  if (!cache_elem)
    {
      /* service not found */
      err_print (error_list[LCM_ERR_NO_VALID_HANDLE].string);
      ns1__updateResponse->status = LCM_ERR_NO_VALID_HANDLE;
      return SOAP_OK;
    }

  /*++++
   * // do a backup of the cache element
   * //++++ */
  WS4D_ALLOCLIST_INIT (&alist);
  save_cache = ws4d_eprlist_alloc (&service_register);
  ws4d_epr_register_plugin (save_cache, service_register_fcreate, NULL);
  memcpy ((struct service_register_cache_s *)
          ws4d_epr_get_plugindata (save_cache, eprlist_lcm_id),
          (struct service_register_cache_s *)
          ws4d_epr_get_plugindata (cache_elem, eprlist_lcm_id),
          sizeof (struct service_register_cache_s));

  /*----------------------------------------------------
   // check the given file and create folder
   // 1. create folder for service in the WORK_DIR
   // 2. copy the zip-file
   // 3. unpack the zip-file
   // 4. check if manifest exist
   // 5. readout the data out of the manifest
   // 6. check if the service file exist
   //---------------------------------------------------*/

  /*++++
   * // create sub-folder for the old service version
   * //++++ */
  sprintf (&old_version_folder[0], "%s/%s/%s", WORK_DIR,
           get_cache_id (cache_elem), get_cache_version (cache_elem));

#ifdef WIN32
  /* TODO Win32 */
#else
  dir = opendir (&old_version_folder[0]);
  if (!dir)
    {
      /* dir not exist */
      if (mkdir (&old_version_folder[0], WORK_AUTHO) != 0)
        {
          err_print (error_list[LCM_ERR_CREAT_WORK_DIR].string);

          /* move back old cache element */
          memcpy ((struct service_register_cache_s *)
                  ws4d_epr_get_plugindata (cache_elem, eprlist_lcm_id),
                  (struct service_register_cache_s *)
                  ws4d_epr_get_plugindata (save_cache, eprlist_lcm_id),
                  sizeof (struct service_register_cache_s));

          ws4d_eprlist_remove (&service_register, save_cache);
          ws4d_eprlist_free (&service_register, save_cache);

          /* return */
          ns1__updateResponse->status = LCM_ERR_CREAT_WORK_DIR;

          return SOAP_OK;
        }
    }
  else
    {
      /* ERROR folder exist, close dir-handle */
      closedir (dir);
      err_print (error_list[LCM_ERR_CREAT_WORK_DIR].string);

      /* move back old cache element */
      memcpy ((struct service_register_cache_s *)
              ws4d_epr_get_plugindata (cache_elem, eprlist_lcm_id),
              (struct service_register_cache_s *)
              ws4d_epr_get_plugindata (save_cache, eprlist_lcm_id),
              sizeof (struct service_register_cache_s));

      ws4d_eprlist_remove (&service_register, save_cache);
      ws4d_eprlist_free (&service_register, save_cache);

      /* return */
      ns1__updateResponse->status = LCM_ERR_CREAT_WORK_DIR;

      return SOAP_OK;
    }
#endif

  /*++++
   * // move all data into the sub-folder
   * //++++ */
  sprintf (&string_tmp[0], "%s/%s/", WORK_DIR, get_cache_id (cache_elem));

#ifdef WIN32
  /* TODO: Win32 */
#else
  dir = opendir (&string_tmp[0]);
  if (dir != NULL)
    {
      while (NULL != (p_dir = readdir (dir)))
        {
          /* move everything that not include the '.' or '..' character ore the old versionnumber */
          if ((strcmp (".", p_dir->d_name) != 0)
              && (strcmp ("..", p_dir->d_name) != 0)
              && (strcmp (get_cache_version (cache_elem), p_dir->d_name) !=
                  0))
            {
              sprintf (&string_from[0], "%s/%s", &string_tmp[0],
                       p_dir->d_name);
              sprintf (&string_to[0], "%s/%s/%s/%s", WORK_DIR,
                       get_cache_id (cache_elem),
                       get_cache_version (cache_elem), p_dir->d_name);

              /* move file */
              copy_file (&string_to[0], &string_from[0]);

              /* deleate file */
              remove (&string_from[0]);
            }
        }
      (void) closedir (dir);
    }
  else
    {
      sprintf (&string_tmp[0], "%s: %s/%s/",
               error_list[LCM_ERR_OPEN_WORK_DIR].string, WORK_DIR,
               get_cache_id (cache_elem));
      err_print (&string_tmp[0]);

      /* move back old cache element */
      memcpy ((struct service_register_cache_s *)
              ws4d_epr_get_plugindata (cache_elem, eprlist_lcm_id),
              (struct service_register_cache_s *)
              ws4d_epr_get_plugindata (save_cache, eprlist_lcm_id),
              sizeof (struct service_register_cache_s));
      ws4d_eprlist_remove (&service_register, save_cache);
      ws4d_eprlist_free (&service_register, save_cache);

      /* return */
      ns1__updateResponse->status = LCM_ERR_OPEN_WORK_DIR;
      return SOAP_OK;
    }
#endif

  /*++++
   * // save attachment in the WORK-folder
   * //++++ */
  res = 0;
  sprintf (&string_tmp[0], "Attachment: Attachment size(%d)",
           ns1__update->Param.__size);
  err_print (&string_tmp[0]);
  sprintf (&string_tmp[0], "%s/service_%s.zip",
           get_cache_location (cache_elem), get_cache_version (cache_elem));

  if (writefile
      (&string_tmp[0], ns1__update->Param.__ptr,
       ns1__update->Param.__size) < 0)
    {
      /* can not save attachment --> error */
      err_print (error_list[LCM_ERR_SAVE_ATTACHMENT].string);

      /* update_fail -> restore old state */
      update_fail (cache_elem, save_cache);

      /* move back old cache element */
      memcpy ((struct service_register_cache_s *)
              ws4d_epr_get_plugindata (cache_elem, eprlist_lcm_id),
              (struct service_register_cache_s *)
              ws4d_epr_get_plugindata (save_cache, eprlist_lcm_id),
              sizeof (struct service_register_cache_s));
      ws4d_eprlist_remove (&service_register, save_cache);
      ws4d_eprlist_free (&service_register, save_cache);
      /* return */
      ns1__updateResponse->status = LCM_ERR_SAVE_ATTACHMENT;
      return SOAP_OK;
    }

  /*++++
   * // do CRC-check
   * //++++ */
  /*
   * if( ns1__updateService->CRC != 0 )
   * {
   * // CRC-Code wurde mit angegeben -> testen
   * sprintf( &string_tmp[0], "CRC-Checksumm: %X\n", ns1__updateService->CRC);
   * err_print( &string_tmp[0] );
   * if( ns1__updateService->CRC != CRC( 0, ns1__updateService->Param.__ptr, ns1__updateService->Param.__size ) )
   * {
   * // CRC-Test fehlgeschlagen
   * err_print( error_list[ LCM_ERR_ATTACHMENT_CRC ].string );
   * //update_fail -> restore old state
   * update_fail( cache_elem, save_cache );
   * // alten Cache-Eintrag zurück kopieren
   * memcpy( (struct service_register_cache_s *) wsa_eprlist_get_plugindata (cache_elem, eprlist_lcm_id),
   * (struct service_register_cache_s *) wsa_eprlist_get_plugindata (save_cache, eprlist_lcm_id),
   * sizeof( struct service_register_cache_s ) );
   * //return;
   * ns1__updateServiceResponse->status    = LCM_ERR_ATTACHMENT_CRC;
   * return SOAP_OK;
   * }
   * err_print( " --> OK\n");
   * }
   */
  /*++++
   * // unpack the zip-file
   * //++++ */

  sprintf (&string_tmp[0], "%s/service_%s.zip",
           get_cache_location (cache_elem), get_cache_version (cache_elem));
  unpack_archive(string_tmp, get_cache_location (cache_elem));

  chdir ("../..");


  /*++++
   * // check if manifest exist
   * //++++ */
  sprintf (&string_tmp[0], "%s/manifest", get_cache_location (cache_elem));
  datei = fopen (&string_tmp[0], "rb");
  if (datei == NULL)
    {
      /* no manifest exist --> error */
      err_print (error_list[LCM_ERR_NO_MANIFEST].string);

      /* update_fail -> restore old state */
      update_fail (cache_elem, save_cache);

      /* move back old cache element */
      memcpy ((struct service_register_cache_s *)
              ws4d_epr_get_plugindata (cache_elem, eprlist_lcm_id),
              (struct service_register_cache_s *)
              ws4d_epr_get_plugindata (save_cache, eprlist_lcm_id),
              sizeof (struct service_register_cache_s));
      ws4d_eprlist_remove (&service_register, save_cache);
      ws4d_eprlist_free (&service_register, save_cache);
      /* return */
      ns1__updateResponse->status = LCM_ERR_NO_MANIFEST;
      return SOAP_OK;
    }
  else
    {
      err_print ("manifest exist -> OK");
    }
  fclose (datei);

  /* readout all data out of the manifest file and save to cache */
  if (readout_manifest
      (&string_tmp[0], get_cache_manifest_struct (cache_elem)) != LCM_OK)
    {
      /* error while reading the manifest */
      err_print (error_list[LCM_ERR_READ_MANIFEST].string);

      /* update_fail -> restore old state */
      update_fail (cache_elem, save_cache);

      /* move back old cache element */
      memcpy ((struct service_register_cache_s *)
              ws4d_epr_get_plugindata (cache_elem, eprlist_lcm_id),
              (struct service_register_cache_s *)
              ws4d_epr_get_plugindata (save_cache, eprlist_lcm_id),
              sizeof (struct service_register_cache_s));
      ws4d_eprlist_remove (&service_register, save_cache);
      ws4d_eprlist_free (&service_register, save_cache);
      /* return */
      ns1__updateResponse->status = LCM_ERR_READ_MANIFEST;
      return SOAP_OK;
    }
  else
    {
      /* everything is fine, so print the assumend data */
      err_print ("readout from manifest -->");
      sprintf (&string_tmp[0], "\tservice_name: %s",
               get_cache_name (cache_elem));
      err_print (&string_tmp[0]);
      sprintf (&string_tmp[0], "\twsdl_name   : %s",
               get_cache_wsdl (cache_elem));
      err_print (&string_tmp[0]);
      sprintf (&string_tmp[0], "\txml_name    : %s",
               get_cache_xml (cache_elem));
      err_print (&string_tmp[0]);
      sprintf (&string_tmp[0], "\tversion     : %s",
               get_cache_version (cache_elem));
      err_print (&string_tmp[0]);
    }

  /*++++
   * // check if the version is newer
   * //++++ */
  if (check_version
      (get_cache_version (cache_elem),
       get_cache_version (save_cache)) != LCM_OK)
    {
      /* the given version is not newer --> error */
      err_print (error_list[LCM_ERR_NO_NEWER_VERSION].string);

      /* update_fail -> restore old state */
      update_fail (cache_elem, save_cache);

      /* move back old cache element */
      memcpy ((struct service_register_cache_s *)
              ws4d_epr_get_plugindata (cache_elem, eprlist_lcm_id),
              (struct service_register_cache_s *)
              ws4d_epr_get_plugindata (save_cache, eprlist_lcm_id),
              sizeof (struct service_register_cache_s));
      ws4d_eprlist_remove (&service_register, save_cache);
      ws4d_eprlist_free (&service_register, save_cache);
      /* return */
      ns1__updateResponse->status = LCM_ERR_NO_NEWER_VERSION;
      return SOAP_OK;
    }
  err_print ("version-check: OK");

  /*++++
   * check if a service is included
   *++++ */

  sprintf (&string_tmp[0], "%s/%s", get_cache_location (cache_elem),
           get_cache_name (cache_elem));
  datei = fopen (&string_tmp[0], "rb");
  if (datei == NULL)
    {
      /* no service was found --> error */
      err_print (error_list[LCM_ERR_NO_SERVICE].string);

      /* update_fail -> restore old state */
      update_fail (cache_elem, save_cache);

      /* move back old cache element */
      memcpy ((struct service_register_cache_s *)
              ws4d_epr_get_plugindata (cache_elem, eprlist_lcm_id),
              (struct service_register_cache_s *)
              ws4d_epr_get_plugindata (save_cache, eprlist_lcm_id),
              sizeof (struct service_register_cache_s));
      ws4d_eprlist_remove (&service_register, save_cache);
      ws4d_eprlist_free (&service_register, save_cache);
      /* return */
      ns1__updateResponse->status = LCM_ERR_NO_SERVICE;
      return SOAP_OK;
    }
  else
    {
      /* everything is fine */
      fclose (datei);
    }

  sprintf (&string_tmp[0], "using service-file: %s",
           get_cache_name (cache_elem));
  err_print (&string_tmp[0]);

  /*++++
   * readout the serviceID
   *++++ */
  xml = (char *) malloc (XML_SIZE * sizeof (char));
  if (xml == NULL)
    {
      err_print (error_list[LCM_ERR_MALLOC].string);

      /* update_fail -> restore old state */
      update_fail (cache_elem, save_cache);

      /* move back old cache element */
      memcpy ((struct service_register_cache_s *)
              ws4d_epr_get_plugindata (cache_elem, eprlist_lcm_id),
              (struct service_register_cache_s *)
              ws4d_epr_get_plugindata (save_cache, eprlist_lcm_id),
              sizeof (struct service_register_cache_s));
      ws4d_eprlist_remove (&service_register, save_cache);
      ws4d_eprlist_free (&service_register, save_cache);
      /* return */
      ns1__updateResponse->status = LCM_ERR_MALLOC;
      return SOAP_OK;
    }

  sprintf (&string_tmp[0], "%s/%s", get_cache_location (cache_elem),
           get_cache_xml (cache_elem));
  read_file (&string_tmp[0], xml);
  soap_init (&parser);
  soap_set_namespaces (&parser, lcm_namespaces);
  soap_begin_deser (&parser, xml, strlen (xml), lcm_namespaces);
  soap_default__wsm__Metadata (&parser, &Metadata);
  if (soap_get__wsm__Metadata (&parser, &Metadata, "wsm:Metadata", "") ==
      NULL)
    {
      err_print (error_list[LCM_ERR_METADATA].string);

      /* update_fail -> restore old state */
      update_fail (cache_elem, save_cache);

      /* move back old cache element */
      memcpy ((struct service_register_cache_s *)
              ws4d_epr_get_plugindata (cache_elem, eprlist_lcm_id),
              (struct service_register_cache_s *)
              ws4d_epr_get_plugindata (save_cache, eprlist_lcm_id),
              sizeof (struct service_register_cache_s));
      ws4d_eprlist_remove (&service_register, save_cache);
      ws4d_eprlist_free (&service_register, save_cache);
      /* return */
      ns1__updateResponse->status = LCM_ERR_METADATA;
      free (xml);
      return SOAP_OK;
    }

  /*++++
   * check if the serviceID is equal
   *++++ */
  if (strcmp (get_cache_id (cache_elem), get_cache_id (save_cache)))
    {
      /* the serviceID's are not equal */
      err_print (error_list[LCM_ERR_ID].string);
      ns1__updateResponse->status = LCM_ERR_ID;
      set_cache_state (cache_elem, LCM_ERR);
      set_cache_valid (cache_elem, LCM_INVALID);
      update_fail (cache_elem, save_cache);
      /* deleate folder */
      sprintf (&string_tmp[0], "rm -r %s/", get_cache_location (cache_elem));
      system (string_tmp);
      /* move back old cache element */
      memcpy ((struct service_register_cache_s *)
              ws4d_epr_get_plugindata (cache_elem, eprlist_lcm_id),
              (struct service_register_cache_s *)
              ws4d_epr_get_plugindata (save_cache, eprlist_lcm_id),
              sizeof (struct service_register_cache_s));
      ws4d_eprlist_remove (&service_register, save_cache);
      ws4d_eprlist_free (&service_register, save_cache);
      free (xml);
      /* return */
      return SOAP_OK;
    }

  /* save the ID */
  set_cache_id (cache_elem,
                Metadata.wsm__MetadataSection->union_MetadataSection.
                wsdp__Relationship->wsdp__Hosted->wsdp__ServiceId);
  free (xml);
  sprintf (&string_tmp[0], "using ID: %s", get_cache_id (cache_elem));
  err_print (&string_tmp[0]);

  /*++++
   * TODO: check if everything is saved correct
   *++++ */
  if (ns1__updateResponse->status != SOAP_ERR)
    {
      /* all fine -> set status */
      set_cache_state (cache_elem, LCM_INSTALL);
      set_cache_valid (cache_elem, LCM_VALID);
    }
  else
    {
      /* error occured */
      set_cache_state (cache_elem, LCM_ERR);
      set_cache_valid (cache_elem, LCM_INVALID);
      err_print (error_list[LCM_ERR_INTERNAL_CACHE].string);
      /* update_fail -> restore old state */
      update_fail (cache_elem, save_cache);
      /* move back old cache element */
      memcpy ((struct service_register_cache_s *)
              ws4d_epr_get_plugindata (cache_elem, eprlist_lcm_id),
              (struct service_register_cache_s *)
              ws4d_epr_get_plugindata (save_cache, eprlist_lcm_id),
              sizeof (struct service_register_cache_s));
      ws4d_eprlist_remove (&service_register, save_cache);
      ws4d_eprlist_free (&service_register, save_cache);
      /* return */
      ns1__updateResponse->status = LCM_ERR_INTERNAL_CACHE;
      return SOAP_OK;
    }

  /*---------------------------------------------
   // deleate the metadata xml
   //--------------------------------------------*/
  sprintf (&string_tmp[0], "%s/%s.xml", hosting_xml_path,
           get_cache_id (save_cache));
  remove (string_tmp);

  /*---------------------------------
   // stopp the process of the service
   //--------------------------------*/
  if (get_cache_pid (save_cache) == 0
      || get_cache_pid (save_cache) == LCM_ERR)
    {
      if (get_cache_state (save_cache) != LCM_RUNNING)
        {
          /* PID is not set, but service is running */
          err_print (error_list[LCM_ERR_VALID_PID].string);
          ns1__updateResponse->status = LCM_ERR_VALID_PID;
          set_cache_state (cache_elem, LCM_NOT_SPECIFIED);
          /* update_fail -> restore old state */
          update_fail (cache_elem, save_cache);
          /* move back old cache element */
          memcpy ((struct service_register_cache_s *)
                  ws4d_epr_get_plugindata (cache_elem, eprlist_lcm_id),
                  (struct service_register_cache_s *)
                  ws4d_epr_get_plugindata (save_cache, eprlist_lcm_id),
                  sizeof (struct service_register_cache_s));
          ws4d_eprlist_remove (&service_register, save_cache);
          ws4d_eprlist_free (&service_register, save_cache);
          return SOAP_OK;
        }
      else
        {
          /* PID is not set and service is stopped */
          err_print (error_list[LCM_ERR_DEVICE_IS_NOT_RUNNING].string);
          ns1__updateResponse->status = LCM_ERR_DEVICE_IS_NOT_RUNNING;
          set_cache_state (cache_elem, LCM_NOT_SPECIFIED);
          /* update_fail -> restore old state */
          update_fail (cache_elem, save_cache);
          /* move back old cache element */
          memcpy ((struct service_register_cache_s *)
                  ws4d_epr_get_plugindata (cache_elem, eprlist_lcm_id),
                  (struct service_register_cache_s *)
                  ws4d_epr_get_plugindata (save_cache, eprlist_lcm_id),
                  sizeof (struct service_register_cache_s));
          ws4d_eprlist_remove (&service_register, save_cache);
          ws4d_eprlist_free (&service_register, save_cache);
          return SOAP_OK;
        }
    }

  /* send signal */
#ifdef WIN32
  /* TODO Win32 !!! */
  /*
   * if( !TerminateThread( cache[ index ].pid, ThreadExitCode ) )
   * {
   * // thread konnte nicht gestoppt werden
   * err_print( error_list[ LCM_ERR_THREAD_NOT_TERMINATE ].string );
   * ns1__updateServiceResponse->status = LCM_ERR_THREAD_NOT_TERMINATE;
   * set_cache_state( cache_elem, LCM_NOT_SPECIFIED );
   * set_cache_valid( cache_elem, LCM_INVALID );
   * //update_fail -> restore old state
   * update_fail( cache_elem, save_cache );
   * // alten Cache-Eintrag zurück kopieren
   * memcpy( (struct service_register_cache_s *) ws4d_epr_get_plugindata (cache_elem, eprlist_lcm_id),
   * (struct service_register_cache_s *) ws4d_epr_get_plugindata (save_cache, eprlist_lcm_id),
   * sizeof( struct service_register_cache_s ) );
   * ws4d_eprlist_remove ( &service_register, save_cache );
   * ws4d_eprlist_free ( &service_register, save_cache );
   * return SOAP_OK;
   * }
   * else
   * {
   * err_print("Thread terminate");
   * }
   */
#else
  kill (get_cache_pid (save_cache), SIGTERM);
  sleep (3);
  kill (get_cache_pid (save_cache), SIGKILL);
#endif
  /* reset PID */
  reset_cache_pid (save_cache);
  err_print ("Service ist gestoppt");
  set_cache_state (save_cache, LCM_RESOLVE);
  set_cache_valid (save_cache, LCM_VALID);

  /*-------------------
   // start service
   //------------------*/
  err_print ("--->  starte Service  <---");
#ifdef WIN32
  /* TODO Win32 !!! */
#else

  /* start new process */
  switch (kind1 = fork ())
    {
    case -1:
      err_print (error_list[LCM_ERR_FORK].string);
      ns1__updateResponse->status = LCM_ERR_FORK;
      set_cache_state (cache_elem, LCM_ERR);
      return SOAP_OK;

    case 0:
      sprintf (&string_tmp[0],
               "starting Service under new process with pid: %d", getpid ());
      err_print (&string_tmp[0]);
      sprintf (&string_tmp[0], "%s/%s", get_cache_location (cache_elem),
               get_cache_name (cache_elem));
      /* set the authorization */
      sprintf (string_to, "chmod 777 %s", string_tmp);
      system (string_to);
      /* start service */
      argumente[0] = get_cache_id (cache_elem);
      argumente[1] = (char *) "-h";
      argumente[2] = host;
      argumente[3] = NULL;
      execvp (string_tmp, argumente);
      err_print ("--->> Service wurde beendet <<---");
      exit (0);

    default:
      sprintf (&string_tmp[0], "new Service starting with pid: %d", kind1);
      err_print (&string_tmp[0]);
      /* save pid */
      set_cache_pid (cache_elem, kind1);
      set_cache_state (cache_elem, LCM_RUNNING);
      set_cache_valid (cache_elem, LCM_VALID);
    }
#endif

  /*-------------------
   // copy metadata.xml to hosting_service
   //------------------*/
  /* xml-file-from */
  sprintf (&string_tmp[0], "%s/%s", get_cache_location (cache_elem),
           get_cache_xml (cache_elem));
  /* xml-file-to */
  sprintf (&string_to[0], "%s/%s.xml", hosting_xml_path,
           get_cache_id (cache_elem));
  /* copy file */
  copy_file (string_to, string_tmp);

  /*++++
   * set return parameters
   *++++ */
  ns1__updateResponse->status = LCM_OK;

  /* free memory */
  ws4d_eprlist_remove (&service_register, save_cache);
  ws4d_eprlist_free (&service_register, save_cache);

  return SOAP_OK;
}
