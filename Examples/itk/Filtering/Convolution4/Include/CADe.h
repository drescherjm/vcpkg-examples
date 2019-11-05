#pragma once

#ifndef CADE_H
#define CADE_H

/////////////////////////////////////////////////////////////////////////////////////////

#include <memory>
#include <string>

/////////////////////////////////////////////////////////////////////////////////////////

class CADe
{
public:
	CADe();
	virtual ~CADe();

public:
	void addTemplate(std::string strPath, uint16_t nTemplateSize);
	void addImageFile(std::string strPath);

	void setMaximumScoresPerTemplate(uint8_t nScores);
	void setMaximumScoresPerImage(uint8_t nScores);
	void enableDebugVisualizationMode(bool bDebug);

	bool excute();

	std::string getErrorMessages() const;

private:
	class cePrivate;
	std::unique_ptr<cePrivate> m_pPrivate;
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif // CADE_H