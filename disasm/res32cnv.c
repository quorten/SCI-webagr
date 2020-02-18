#include <stdio.h>

typedef unsigned int DWORD;
typedef unsigned short WORD;

 /* Predefined resource types */ 
    #define    RT_NEWRESOURCE      0x2000 
    #define    RT_ERROR            0x7fff 
    #define    RT_CURSOR           1 
    #define    RT_BITMAP           2 
    #define    RT_ICON             3 
    #define    RT_MENU             4 
    #define    RT_DIALOG           5 
    #define    RT_STRING           6 
    #define    RT_FONTDIR          7 
    #define    RT_FONT             8 
    #define    RT_ACCELERATORS     9 
    #define    RT_RCDATA           10 
    #define    RT_MESSAGETABLE     11 
    #define    RT_GROUP_CURSOR     12 
    #define    RT_GROUP_ICON       14 
    #define    RT_VERSION          16 
    #define    RT_NEWBITMAP        (RT_BITMAP|RT_NEWRESOURCE) 
    #define    RT_NEWMENU          (RT_MENU|RT_NEWRESOURCE) 
    #define    RT_NEWDIALOG        (RT_DIALOG|RT_NEWRESOURCE)
    #define    RT_MANIFEST         24

/* Memory Flags */
    #define    MOVEABLE            0x0010 
    #define    FIXED               ~MOVEABLE 
    #define    PURE                0x0020 
    #define    IMPURE              ~PURE 
    #define    PRELOAD             0x0040 
    #define    LOADONCALL          ~PRELOAD 
    #define    DISCARDABLE         0x1000

struct ResHead_tag
{
	DWORD dataSize;
	DWORD headerSize;
	WORD type_magic;
	WORD type;
	WORD name_magic;
	WORD name;
	DWORD dataVersion;
	WORD memoryFlags;
	WORD languageId;
	DWORD version;
	DWORD characteristics;
} __attribute__((packed));
typedef struct ResHead_tag ResHead;

/* Each header is padded to a DWORD aligned boundary.  */

struct Res32TblHead_tag
{
	DWORD characteristics;
	DWORD time;
	DWORD version;
	WORD num_names;
	WORD num_ids;
} __attribute__((packed));
typedef struct Res32TblHead_tag Res32TblHead;

struct Res32Entry_tag
{
	DWORD id;
	/* The file offset of the indicated ID, with MSB set.  */
	DWORD value;
} __attribute__((packed));
typedef struct Res32Entry_tag Res32Entry;

struct Res32Leaf_tag
{
	DWORD addr;
	DWORD size;
	DWORD codepage;
	DWORD reserved;
} __attribute__((packed));
typedef struct Res32Leaf_tag Res32Leaf;

/*

First comes the header.
Then the list of all entry structures together.

Then the objects referenced are packed following that.

*/

/* How to compute base: Take the VMA base address of the `.rsrc'
   section and subtract the overall PE image base address from
   that.  */
unsigned long base;

/* TODO: Add error messages.  */
/* TODO: Add handling for named resources, currently we only handle
   resources specified by ID.  */
/* TODO: Use expandable arrays for the resources, or either dynamic
   memory allocation.  */
int main(int argc, char **argv)
{
	FILE *fp;
	long oldpos;

	Res32TblHead head32;
	Res32Entry type_entries_d[32];
	unsigned type_entries_len;
	Res32Entry name_entries_d[32];
	unsigned name_entries_len;
	Res32Entry lang_entries_d[32];
	unsigned lang_entries_len;
	WORD type;
	WORD name;
	WORD languageId;

	unsigned i;

	if (argc < 3)
		return 1;

	if (sscanf(argv[1], "%li", &base) != 1)
		return 1;

	fp = fopen(argv[2], "rb");
	if (fp == NULL)
		return 1;

	{ /* First write a "null" resource item as the header.  */
		ResHead magic_head;
		magic_head.dataSize = 0;
		magic_head.headerSize = sizeof(ResHead);
		magic_head.type_magic = 0xffff;
		magic_head.type = 0;
		magic_head.name_magic = 0xffff;
		magic_head.name = 0;
		magic_head.dataVersion = 0;
		magic_head.memoryFlags = 0;
		magic_head.languageId = 0;
		magic_head.version = 0;
		magic_head.characteristics = 0;
		fwrite(&magic_head, sizeof(ResHead), 1, stdout);
	}

	/* Walk through the 32-bit resource table.  */
	/* Read the type table header.  */
	if (fread(&head32, sizeof(Res32TblHead), 1, fp) != 1)
		return 1;
	type_entries_len = head32.num_ids;
	if (type_entries_len > 32) type_entries_len = 32;
	if (fread(type_entries_d, sizeof(Res32Entry), type_entries_len, fp)
		!= type_entries_len)
		return 1;
	for (i = 0; i < type_entries_len; i++)
	{
		/* Read each name table header.  */
		unsigned j;
		type = type_entries_d[i].id;
		if (fseek(fp, 0x7fffffff & type_entries_d[i].value, SEEK_SET) == -1)
			return 1;
		if (fread(&head32, sizeof(Res32TblHead), 1, fp) != 1)
			return 1;
		name_entries_len = head32.num_ids;
		if (name_entries_len > 32) name_entries_len = 32;
		if (fread(name_entries_d, sizeof(Res32Entry), name_entries_len, fp)
			!= name_entries_len)
			return 1;
		for (j = 0; j < name_entries_len; j++)
		{
			/* Read each language table header.  */
			unsigned k;
			name = name_entries_d[j].id;
			if (fseek(fp, 0x7fffffff & name_entries_d[j].value, SEEK_SET) == -1)
				return 1;
			if (fread(&head32, sizeof(Res32TblHead), 1, fp) != 1)
				return 1;
			lang_entries_len = head32.num_ids;
			if (lang_entries_len > 32) lang_entries_len = 32;
			if (fread(lang_entries_d, sizeof(Res32Entry), lang_entries_len, fp)
				!= lang_entries_len)
				return 1;
			for (k = 0; k < lang_entries_len; k++)
			{
				/* Process the leaf.  */
				Res32Leaf leaf;
				ResHead res_head;
				unsigned m;
				languageId = lang_entries_d[k].id;
				if (fseek(fp, 0x7fffffff & lang_entries_d[k].value, SEEK_SET) == -1)
					return 1;
				if (fread(&leaf, sizeof(Res32Leaf), 1, fp) != 1)
					return 1;
				if (fseek(fp, leaf.addr - base, SEEK_SET) == -1)
					return 1;
				/* Write out a resource chunk.  */
				res_head.dataSize = leaf.size;
				res_head.headerSize = sizeof(ResHead);
				res_head.type_magic = 0xffff;
				res_head.type = type;
				res_head.name_magic = 0xffff;
				res_head.name = name;
				res_head.dataVersion = 0;
				res_head.memoryFlags = MOVEABLE | PURE | DISCARDABLE;
				res_head.languageId = languageId;
				res_head.version = 0;
				res_head.characteristics = 0;
				fwrite(&res_head, sizeof(ResHead), 1, stdout);
				for (m = 0; m < leaf.size; m++)
				{
					int ch = getc(fp);
					if (ch == EOF) break;
					putc((char)ch, stdout);
				}
				/* Align to DWORD boundary.  */
				if ((leaf.size & (4 - 1)) != 0)
				{
					unsigned padding = 4 - (leaf.size & (4 - 1));
					for (m = 0; m < padding; m++) putc('\0', stdout);
				}
			}
		}
	}
	return 0;
}
