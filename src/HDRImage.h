#pragma once

#include <stdlib.h>

class HDRImage
{
public:
	HDRImage(const char * filename);
	~HDRImage();

	float * getData();
	size_t	getSize();
	int		getWidth();
	int		getHeight();
private:
	void	findMinMax();
	float * m_data;
	size_t	m_size;
	int		m_width, m_height;
};