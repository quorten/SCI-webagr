/* Print out list of indirection addresses.  */

#include <stdio.h>

#define BASE 0x0002c000

int main()
{
	unsigned i;
	FILE *fp = fopen("idata.bin", "rb");
	if (fp == NULL)
		return 1;
	for (i = 0; i < 512; i++)
	{
		unsigned word;
		int ch;
		long oldpos;
		/* Little endian read */
		word = 0;
		ch = getc(fp); if (ch == EOF) break; word |= ch << 0;
		ch = getc(fp); if (ch == EOF) break; word |= ch << 8;
		ch = getc(fp); if (ch == EOF) break; word |= ch << 16;
		ch = getc(fp); if (ch == EOF) break; word |= ch << 24;
		if ((word & 0xfffff000) != BASE)
			continue;
		oldpos = ftell(fp);
		printf("0x%08lx -> 0x%08x -> ", oldpos + BASE - 4, word);
		fseek(fp, word & 0x00000fff, SEEK_SET);
		/* Little endian read */
		word = 0;
		ch = getc(fp); if (ch == EOF) break; word |= ch << 0;
		ch = getc(fp); if (ch == EOF) break; word |= ch << 8;
		ch = getc(fp); if (ch == EOF) break; word |= ch << 16;
		ch = getc(fp); if (ch == EOF) break; word |= ch << 24;
		if ((word & 0xfffff000) == BASE)
			printf("0x%08x\n", word);
		else
		{
			char buffer[40];
			unsigned buflen = 0;
			fseek(fp, -4 + 2, SEEK_CUR);
			do
			{
				ch = getc(fp); if (ch == EOF) break;
				buffer[buflen++] = ch;
			} while (buflen < 40 && buffer[buflen-1] != '\0');
			buffer[40-1] = '\0';
			puts(buffer);
		}
		fseek(fp, oldpos, SEEK_SET);
	}
	return 0;
}
