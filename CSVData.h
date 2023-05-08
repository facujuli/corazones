/**
 * @file CSVData.h
 * @author Marc S. Ressl
 * @brief Reads and writes CSV files
 * @version 0.2
 * 
 * @copyright Copyright (c) 2022-2023
 * 
 */

#ifndef _CSVDATA_H
#define _CSVDATA_H

#include <string>
#include <vector>

typedef std::vector<std::vector<std::string>> CSVData;

bool readCSV(const std::string path, CSVData &data);
bool writeCSV(const std::string path, CSVData &data);

#endif
