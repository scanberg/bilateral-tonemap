#include "HDRImage.h"

extern "C" {
	#include "rgbe.h"
}

#include <cstdio>
#include <cstdlib>

HDRImage::HDRImage(const char * filename)
{
	m_data = NULL;
	m_size = 0;
	m_width = m_height = 0;

	int width, height;
	float * data;
	size_t size;
	FILE * f;

	f = fopen(filename,"rb");

	if(!f)
	{
		printf("Error reading from file %s \n", filename);
		return;
	}

	if(RGBE_ReadHeader(f,&width,&height,NULL) == RGBE_RETURN_FAILURE)
	{
		printf("Error reading header from file \n");
		fclose(f);
		return;
	}

	size = sizeof(float)*3*width*height;
	data = (float *)malloc(size);

	if(RGBE_ReadPixels_RLE(f,data,width,height) == RGBE_RETURN_FAILURE)
	{
		printf("Error reading data \n");
		free(data);
		fclose(f);
		return;
	}

	m_data = data;
	m_size = size;
	m_width = width;
	m_height = height;

	fclose(f);
}

HDRImage::~HDRImage()
{
	if(m_data)
		delete[] m_data;
}

float * HDRImage::getData()
{
	return m_data;
}

size_t HDRImage::getSize()
{
	return m_size;
}

int	HDRImage::getWidth()
{
	return m_width;
}

int	HDRImage::getHeight()
{
	return m_height;
}