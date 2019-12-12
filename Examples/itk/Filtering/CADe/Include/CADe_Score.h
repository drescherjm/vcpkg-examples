#pragma once

#ifndef CADE_SCORE_H
#define CADE_SCORE_H

/////////////////////////////////////////////////////////////////////////////////////////

class CADeScore
{
public:
	CADeScore(double fScore, int nX, int nY, int nTemplateSize) : m_fScore{ fScore },
		m_nX{nX},m_nY{nY},m_nTemplateSize{nTemplateSize}
	{

	}
public:
	double m_fScore = 0.0;
	int m_nX = -1;
	int m_nY = -1;
	int m_nTemplateSize = -1;
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif // CADE_SCORE_H