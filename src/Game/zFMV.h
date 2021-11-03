#ifndef ZFMV_H
#define ZFMV_H

#include <types.h>

enum eFMVFile
{
	eFMVFile_PromoFOP,
	eFMVFile_PromoTak,
	eFMVFile_PromoJN,
	eFMVFile_PromoSB,
	eFMVFile_LogoHeavyIron,
	eFMVFile_LogoNick,
	eFMVFile_LogoRW,
	eFMVFile_LogoTHQ,
	eFMVFile_Demo1,
	eFMVFile_Demo2,
	eFMVFile_Demo3,
	eFMVFile_LogoPixar,
	eFMVFileCount
};

uint32 zFMVPlay(char* filename, uint32 buttons, float32 time, bool skippable, bool lockController);
char* zFMVFileGetName(eFMVFile fileEnum);

#endif