#pragma once

#ifndef EXPORTCSV_H
#define EXPORTCSV_H

#include <memory>
#include <string>
#include <vector>

/////////////////////////////////////////////////////////////////////////////////////////

class exportCSV
{
public:
	exportCSV(const std::vector<std::string> & vecFiles);
	~exportCSV();

public:
	int	process();

private:
	struct exPrivate;
	std::unique_ptr<exPrivate> m_pPrivate;
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif // EXPORTCSV_H
