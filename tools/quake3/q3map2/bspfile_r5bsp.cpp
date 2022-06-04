/* -------------------------------------------------------------------------------

   My feeble attempt at generating a r5 ( Apex ) .bsp file.

   A lot of this is temporary and will be altered heavily with new information.
   If you know how to c++ better than me feel free to contribute or call me words
   in discord. :)

   - Fifty#8113, also known as Fifteen, Sixteen, Seventeen, Eigtheen and FORTY

   ------------------------------------------------------------------------------- */



   /* dependencies */
#include "q3map2.h"
#include "bspfile_abstract.h"
#include <ctime>


/* constants */
#define HEADER_LUMPS 128

/* types */
struct rbspHeader_t
{
	char ident[4];		/* rBSP */
	int version;		/* 37 for r2 */
	int mapVersion;		/* 30 */
	int maxLump;		/* 127 */

	bspLump_t lumps[HEADER_LUMPS];
};


/*
   LoadR2BSPFile()
   loads a titanfall2 bsp file
 */
void LoadR5BSPFile(const char* filename)
{

}


/*
   WriteR2BSPFile()
   writes a apex bsp file
 */
void WriteR5BSPFile(const char* filename)
{
}

void CompileR5BSPFile()
{

}