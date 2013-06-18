/***************************************************************************

    MAME source code dependency generator

****************************************************************************

    Copyright Aaron Giles
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

        * Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
          notice, this list of conditions and the following disclaimer in
          the documentation and/or other materials provided with the
          distribution.
        * Neither the name 'MAME' nor the names of its contributors may be
          used to endorse or promote products derived from this software
          without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY AARON GILES ''AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL AARON GILES BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
    STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
    IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <zlib.h>
#include "osdcore.h"
#include "astring.h"
#include "corefile.h"
#include "tagmap.h"


/***************************************************************************
    CONSTANTS
***************************************************************************/

#define HASH_SIZE       193
#define MAX_SOURCES  65536


/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

struct include_path
{
	include_path *  next;
	astring         path;
};


struct exclude_path
{
	exclude_path *  next;
	astring         path;
	int             pathlen;
	UINT8           recursive;
};


struct list_entry
{
	list_entry *    next;
	astring         name;
};


struct file_entry;

struct dependency
{
	dependency *    next;
	file_entry *    file;
};


struct file_entry
{
	astring         name;
	dependency *    deplist;
};

typedef tagmap_t<UINT8> dependency_map;



/***************************************************************************
    GLOBAL VARIABLES
***************************************************************************/

static include_path *incpaths;
static exclude_path *excpaths;
static tagmap_t<file_entry *> file_map;
static const char *sourcelst[MAX_SOURCES];
static int sourcecount;



/***************************************************************************
    PROTOTYPES
***************************************************************************/

// core output functions
static int recurse_dir(int srcrootlen, astring &srcdir);
static file_entry &compute_dependencies(astring &srcfile);

// path helpers
static bool find_include_file(astring &srcincpath, const astring &srcfile, const astring &filename);

//-------------------------------------------------
//  isonlist - return info if item is in source
//  list or not
//-------------------------------------------------

bool isonlist(const char *drivname)
{
	if (sourcecount>0) {
		for(int i=0;i<sourcecount;i++) {
			if (strcmp(sourcelst[i],drivname)==0) return true;
		}
	}
	return false;
}

//-------------------------------------------------
//  parse_file - parse a single file, may be
//  called recursively
//-------------------------------------------------

int parse_file(const char *srcfile)
{
	// read source file
	void *buffer;
	UINT32 length;
	file_error filerr = core_fload(srcfile, &buffer, &length);
	if (filerr != FILERR_NONE)
	{
		fprintf(stderr, "Unable to read source file '%s'\n", srcfile);
		return 1;
	}

	// rip through it to find all drivers
	char *srcptr = (char *)buffer;
	char *endptr = srcptr + length;
	int linenum = 1;
	bool in_comment = false;
	while (srcptr < endptr)
	{
		char c = *srcptr++;

		// count newlines
		if (c == 13 || c == 10)
		{
			if (c == 13 && *srcptr == 10)
				srcptr++;
			linenum++;
			continue;
		}

		// skip any spaces
		if (isspace(c))
			continue;

		// look for end of C comment
		if (in_comment && c == '*' && *srcptr == '/')
		{
			srcptr++;
			in_comment = false;
			continue;
		}

		// skip anything else inside a C comment
		if (in_comment)
			continue;

		// look for start of C comment
		if (c == '/' && *srcptr == '*')
		{
			srcptr++;
			in_comment = true;
			continue;
		}

		// if we hit a C++ comment, scan to the end of line
		if (c == '/' && *srcptr == '/')
		{
			while (srcptr < endptr && *srcptr != 13 && *srcptr != 10)
				srcptr++;
			continue;
		}

		// look for an import directive
		if (c == '#')
		{
			char filename[256];
			filename[0] = 0;
			for (int pos = 0; srcptr < endptr && pos < ARRAY_LENGTH(filename) - 1 && !isspace(*srcptr); pos++)
			{
				filename[pos] = *srcptr++;
				filename[pos+1] = 0;
			}
			fprintf(stderr, "Importing drivers from '%s'\n", filename);
			parse_file(filename);
			continue;
		}
		if (c == '@')
		{
			// Used for makemak tool
			char drivname[256];
			drivname[0] = 0;
			for (int pos = 0; srcptr < endptr && pos < ARRAY_LENGTH(drivname) - 1 && !isspace(*srcptr); pos++)
			{
				drivname[pos] = *srcptr++;
				drivname[pos+1] = 0;
			}
			fprintf(stderr, "Creating make dependancy for '%s'\n", drivname);
			char *name = (char *)malloc(strlen(drivname) + 1);
			strcpy(name, drivname);
			sourcelst[sourcecount++] = name;
			continue;
		}

		srcptr--;
		for (int pos = 0; srcptr < endptr && !isspace(*srcptr); pos++)
		{
			c = *srcptr++;
		}
	}

	osd_free(buffer);

	return 0;
}

/***************************************************************************
    MAIN
***************************************************************************/

/*-------------------------------------------------
    main - main entry point
-------------------------------------------------*/

void usage(const char *argv0)
{
	fprintf(stderr, "Usage:\n%s <source.lst> <srcroot> [-Iincpath [-Iincpath [...]]]\n", argv0);
	exit(1);
}

int main(int argc, char *argv[])
{
	include_path **incpathhead = &incpaths;
	exclude_path **excpathhead = &excpaths;
	astring srcdir;
	int unadorned = 0;
	sourcecount = 0;
	
	// extract arguments
	const char *srcfile = argv[1];	
	if (parse_file(srcfile))
		return 1;

	// loop over arguments
	for (int argnum = 2; argnum < argc; argnum++)
	{
		char *arg = argv[argnum];

		// include path?
		if (arg[0] == '-' && arg[1] == 'I')
		{
			*incpathhead = new include_path;
			(*incpathhead)->next = NULL;
			(*incpathhead)->path.cpy(&arg[2]).replacechr('/', PATH_SEPARATOR[0]);
			incpathhead = &(*incpathhead)->next;
		}

		// exclude path?
		else if (arg[0] == '-' && arg[1] == 'X')
		{
			*excpathhead = new exclude_path;
			(*excpathhead)->next = NULL;
			(*excpathhead)->path.cpy(&arg[2]).replacechr(PATH_SEPARATOR[0], '/');
			(*excpathhead)->recursive = ((*excpathhead)->path.replace((*excpathhead)->path.len() - 4, "/...", "") != 0);
			(*excpathhead)->pathlen = (*excpathhead)->path.len();
			excpathhead = &(*excpathhead)->next;
		}

		// ignore -include which is used by sdlmame to include sdlprefix.h before all other includes
		else if (strcmp(arg,"-include") == 0)
		{
			argnum++;
		}

		// other parameter
		else if (arg[0] != '-' && unadorned == 0)
		{
			srcdir.cpy(arg).replacechr('/', PATH_SEPARATOR[0]);
			unadorned++;
		}
		else
			usage(argv[0]);
	}

	// make sure we got 1 parameter
	if (srcdir.len() == 0)
		usage(argv[0]);


	if (sourcecount>0) 
	{	
		printf("OBJDIRS += \\\n");
		printf("\t$(OBJ)/mame/audio \\\n");
		printf("\t$(OBJ)/mame/drivers \\\n");
		printf("\t$(OBJ)/mame/layout \\\n");
		printf("\t$(OBJ)/mame/machine \\\n");
		printf("\t$(OBJ)/mame/video \\\n");
		printf("\n\n");
		printf("DRVLIBS += \\\n");
		
		for(int i=0;i<sourcecount;i++) {		
			printf("\t$(OBJ)/mame/%s.a \\\n",sourcelst[i]);
		}
		printf("\n");
	}	

	// recurse over subdirectories
	return recurse_dir(srcdir.len(), srcdir);
}



/***************************************************************************
    CORE OUTPUT FUNCTIONS
***************************************************************************/

static int compare_list_entries(const void *p1, const void *p2)
{
	const list_entry *entry1 = *(const list_entry **)p1;
	const list_entry *entry2 = *(const list_entry **)p2;
	return entry1->name.cmp(entry2->name);
}


/*-------------------------------------------------
    recurse_dependencies - recurse through the
    dependencies found, adding the mto the tagmap
    unless we already exist in the map
-------------------------------------------------*/

static void recurse_dependencies(file_entry &file, dependency_map &map)
{
	// skip if we're in an exclude path
	int filelen = file.name.len();
	for (exclude_path *exclude = excpaths; exclude != NULL; exclude = exclude->next)
		if (exclude->pathlen < filelen && strncmp(file.name, exclude->path, exclude->pathlen) == 0)
			if (exclude->recursive || file.name.chr(exclude->pathlen + 1, '/') == -1)
				return;

	// attempt to add; if we get an error, we're already present
	if (map.add(file.name, 0) != TMERR_NONE)
		return;

	// recurse the list from there
	for (dependency *dep = file.deplist; dep != NULL; dep = dep->next)
		recurse_dependencies(*dep->file, map);
}


/*-------------------------------------------------
    recurse_dir - recurse through a directory
-------------------------------------------------*/

static int recurse_dir(int srcrootlen, astring &srcdir)
{
	static const osd_dir_entry_type typelist[] = { ENTTYPE_DIR, ENTTYPE_FILE };
	int result = 0;

	// iterate first over directories, then over files
	for (int entindex = 0; entindex < ARRAY_LENGTH(typelist) && result == 0; entindex++)
	{
		osd_dir_entry_type entry_type = typelist[entindex];

		// open the directory and iterate through it
		osd_directory *dir = osd_opendir(srcdir);
		if (dir == NULL)
		{
			result = 1;
			goto error;
		}

		// build up the list of files
		const osd_directory_entry *entry;
		list_entry *list = NULL;
		int found = 0;
		while ((entry = osd_readdir(dir)) != NULL)
			if (entry->type == entry_type && entry->name[0] != '.')
			{
				list_entry *lentry = new list_entry;
				lentry->name.cpy(entry->name);
				lentry->next = list;
				list = lentry;
				found++;
			}

		// close the directory
		osd_closedir(dir);

		// skip if nothing found
		if (found == 0)
			continue;

		// allocate memory for sorting
		list_entry **listarray = new list_entry *[found];
		found = 0;
		for (list_entry *curlist = list; curlist != NULL; curlist = curlist->next)
			listarray[found++] = curlist;

		// sort the list
		qsort(listarray, found, sizeof(listarray[0]), compare_list_entries);

		// rebuild the list
		list = NULL;
		while (--found >= 0)
		{
			listarray[found]->next = list;
			list = listarray[found];
		}
		delete[] listarray;

		// iterate through each file
		for (list_entry *curlist = list; curlist != NULL && result == 0; curlist = curlist->next)
		{
			astring srcfile;

			// build the source filename
			srcfile.printf("%s%c%s", srcdir.cstr(), PATH_SEPARATOR[0], curlist->name.cstr());

			// if we have a file, output it
			if (entry_type == ENTTYPE_FILE)
			{
				// make sure we care, first
				if (core_filename_ends_with(curlist->name, ".c"))
				{
					dependency_map depend_map;

					// find dependencies
					file_entry &file = compute_dependencies(srcfile);
					recurse_dependencies(file, depend_map);
					astring fn = astring(curlist->name);
					fn.replace(".c","");
					if (isonlist(fn)) 
					{
						// convert the target from source to object (makes assumptions about rules)
						astring target(file.name);
						target.replace(0, "src/", "$(OBJ)/");
						target.replace(0, "drivers/", "");
						target.replace(0, ".c", ".a");
						printf("\n%s : \\\n", target.cstr());

						// iterate over the hashed dependencies and output them as well
						for (dependency_map::entry_t *entry = depend_map.first(); entry != NULL; entry = depend_map.next(entry)) 
						{
							astring t(entry->tag());
							t.replace(0, "src/", "$(OBJ)/");
							t.replace(0, ".c", ".o");
							if (core_filename_ends_with(t, ".o"))
							{
								printf("\t%s \\\n", t.cstr());
							}
						}
						
						printf("\n");
						printf("\n");
						for (dependency_map::entry_t *entry = depend_map.first(); entry != NULL; entry = depend_map.next(entry)) 
						{
							astring t(entry->tag());
							if (core_filename_ends_with(t, ".lay"))
							{
								astring target2(file.name);
								target2.replace(0, "src/", "$(OBJ)/");
								target2.replace(0, ".c", ".o");

								t.replace(0, "src/", "$(OBJ)/");
								t.replace(0, ".lay", ".lh");
								
								printf("%s:	%s\n", target2.cstr(), t.cstr());
							}
						}
					}
				}
			}
		}

		// free all the allocated entries
		while (list != NULL)
		{
			list_entry *next = list->next;
			delete list;
			list = next;
		}
	}

error:
	return result;
}

static bool check_file(astring &srcincpath)
{
	// see if we can open it
	core_file *testfile;
	if (core_fopen(srcincpath, OPEN_FLAG_READ, &testfile) == FILERR_NONE)
	{
		// close the file
		core_fclose(testfile);
		return true;
	}
	return false;
}

/*-------------------------------------------------
    output_file - output a file, converting to
    HTML
-------------------------------------------------*/

static file_entry &compute_dependencies(astring &srcfile)
{
	// see if we already have an entry
	astring normalfile(srcfile);
	normalfile.replacechr(PATH_SEPARATOR[0], '/');
	file_entry *foundfile = file_map.find(normalfile);
	if (foundfile != NULL)
		return *foundfile;

	// create a new header entry
	file_entry &file = *new file_entry;
	file.deplist = NULL;
	file.name = normalfile;
	file_map.add(file.name, &file);

	// read the source file
	UINT32 filelength;
	char *filedata;
	if (core_fload(srcfile, (void **)&filedata, &filelength) != FILERR_NONE)
	{
		fprintf(stderr, "Unable to read file '%s'\n", srcfile.cstr());
		return file;
	}

	astring audiofile = astring(srcfile);
	audiofile.replace("drivers","audio");
	if (check_file(audiofile))
	{
		dependency *dep = new dependency;
		dep->next = file.deplist;
		file.deplist = dep;
		dep->file = &compute_dependencies(audiofile);
	}

	astring machinefile = astring(srcfile);
	machinefile.replace("drivers","machine");
	if (check_file(machinefile))
	{
		dependency *dep = new dependency;
		dep->next = file.deplist;
		file.deplist = dep;
		dep->file = &compute_dependencies(machinefile);
	}
	
	astring videofile = astring(srcfile);
	videofile.replace("drivers","video");
	if (check_file(videofile))
	{
		dependency *dep = new dependency;
		dep->next = file.deplist;
		file.deplist = dep;
		dep->file = &compute_dependencies(videofile);
	}
	

	// find the #include directives in this file
	for (int index = 0; index < filelength; index++)
		if (filedata[index] == '#' && strncmp(&filedata[index + 1], "include", 7) == 0)
		{
			// first make sure we're not commented or quoted
			bool just_continue = false;
			for (int scan = index; scan > 2 && filedata[scan] != 13 && filedata[scan] != 10; scan--)
				if ((filedata[scan] == '/' && filedata[scan - 1] == '/') || filedata[scan] == '"')
				{
					just_continue = true;
					break;
				}
			if (just_continue)
				continue;

			// scan forward to find the quotes or bracket
			index += 7;
			int scan;
			for (scan = index; scan < filelength && filedata[scan] != '<' && filedata[scan] != '"' && filedata[scan] != 13 && filedata[scan] != 10; scan++) ;

			// ignore if not found or if it's bracketed
			if (scan >= filelength || filedata[scan] != '"')
				continue;
			int start = ++scan;

			// find the closing quote
			while (scan < filelength && filedata[scan] != '"')
				scan++;
			if (scan >= filelength)
				continue;

			// find the include file
			astring filename(&filedata[start], scan - start);
			astring target;

			filename.replace(".lh",".lay");
			
			// create a new dependency
			if (find_include_file(target, srcfile, filename))
			{
				dependency *dep = new dependency;
				dep->next = file.deplist;
				file.deplist = dep;
				dep->file = &compute_dependencies(target);
			}
			// create a new dependency
		}

	osd_free(filedata);
	return file;
}



/***************************************************************************
    HELPERS
***************************************************************************/

/*-------------------------------------------------
    find_include_file - find an include file
-------------------------------------------------*/

static bool find_include_file(astring &srcincpath, const astring &srcfile, const astring &filename)
{
	// iterate over include paths and find the file
	for (include_path *curpath = incpaths; curpath != NULL; curpath = curpath->next)
	{
		// a '.' include path is specially treated
		if (curpath->path == ".")
			srcincpath.cpysubstr(srcfile, 0, srcfile.rchr(0, PATH_SEPARATOR[0]));
		else
			srcincpath.cpy(curpath->path);

		// append the filename piecemeal to account for directories
		int lastsepindex = 0;
		int sepindex;
		while ((sepindex = filename.chr(lastsepindex, '/')) != -1)
		{
			astring pathpart(filename, lastsepindex, sepindex - lastsepindex);

			// handle .. by removing a chunk from the incpath
			if (pathpart == "..")
			{
				int sepindex_part = srcincpath.rchr(0, PATH_SEPARATOR[0]);
				if (sepindex_part != -1)
					srcincpath.substr(0, sepindex_part);
			}

			// otherwise, append a path separator and the pathpart
			else
				srcincpath.cat(PATH_SEPARATOR).cat(pathpart);

			// advance past the previous index
			lastsepindex = sepindex + 1;
		}

		// now append the filename
		srcincpath.cat(PATH_SEPARATOR).catsubstr(filename, lastsepindex, -1);

		// see if we can open it
		core_file *testfile;
		if (core_fopen(srcincpath, OPEN_FLAG_READ, &testfile) == FILERR_NONE)
		{
			// close the file
			core_fclose(testfile);
			return true;
		}
	}
	return false;
}
