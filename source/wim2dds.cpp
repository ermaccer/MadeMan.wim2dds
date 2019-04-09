// wim2dds.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "dds.h"
#include <fstream>
#include <iostream>
#include <memory>

// just important stuff, lots of guessing going on

struct WIMHeader {
	int          header;  //WIMG for Windows Image | XIMG for XBOX Image
	int          unknown[3];
	int          colorDepth;
	int          unknown2[3];
	unsigned int size;
	int          unknown3[2];
	char        dxtType[3]; //DXT5 on pc, XBOX varies
};


std::streampos getSizeToEnd(std::ifstream& is)
{
	auto currentPosition = is.tellg();
	is.seekg(0, is.end);
	auto length = is.tellg() - currentPosition;
	is.seekg(currentPosition, is.beg);
	return length;
}



int main(int argc, char* argv[])
{

	if (argc != 2) {
		printf("Usage: wim2dds <input> \n");
		return 1;
	}


	std::ifstream pFile(argv[1], std::ifstream::binary);

	if (!pFile)
		printf("ERROR: Could not open %s!", argv[1]);

	if (pFile)
	{
		WIMHeader wim;
		pFile.read((char*)&wim, sizeof(WIMHeader));

		// if it's not WIMG or XIMG, bail out!
		if (!(wim.header == 'GMIW' || wim.header == 'GMIX'))
		{
			std::cout << "ERROR: " << argv[1] << " is not a Windows or XBOX Image." << std::endl;
			return 1;
		}


		// write dds header!
		DDSHeader dds;
		dds.dwSize = (sizeof(DDSHeader) - sizeof(dds.magic));
		dds.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_MIPMAPCOUNT | DDSD_LINEARSIZE;
		dds.dwHeight = wim.size;
		dds.dwWidth = wim.size;
		dds.dwPitchOrLinearSize = 0;
		dds.dwDepth = 0;
		dds.dwMipMapCount = 0;
		dds.ddspf.dwSize = sizeof(DDS_PIXELFORMAT);
		dds.ddspf.dwFlags = DDPF_FOURCC;
		dds.ddspf.dwFourCC[0] = wim.dxtType[3];
		dds.ddspf.dwFourCC[1] = wim.dxtType[2];
		dds.ddspf.dwFourCC[2] = wim.dxtType[1];
		dds.ddspf.dwFourCC[3] = wim.dxtType[0];
		dds.ddspf.dwRGBBitCount = 256;
		dds.ddspf.dwRBitMask = 0x00000000;
		dds.ddspf.dwGBitMask = 0x00000000;
		dds.ddspf.dwBBitMask = 0x00000000;
		dds.ddspf.dwABitMask = 0x00000000;
		dds.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;
		// no need
		dds.dwCaps2 = 0;
		dds.dwCaps3 = 0;
		dds.dwCaps4 = 0;
		dds.dwReserved2 = 0;

		// set after header to get dds data
		pFile.seekg(sizeof(WIMHeader), pFile.beg);

		// create output
		std::string output = argv[1];
		int length = output.length();
		output.insert(length, ".dds");
		std::ofstream oFile(output, std::ofstream::binary);

		// write header
		oFile.write((char*)&dds, sizeof(DDSHeader));
		auto dataSize = getSizeToEnd(pFile);
		auto dataBuff = std::make_unique<char[]>((long)dataSize); 
		pFile.read(dataBuff.get(), dataSize);
		oFile.write(dataBuff.get(), dataSize);
		
           
	}

    return 0;
}

