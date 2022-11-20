#pragma once

#include "Operator.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "Object3d.h"

//�}�b�v�̃T�C�Y(��{��1)
const int map_size = 1;
//�c���̍ő吔(�}�b�v�ɂ���ĕύX)
const int map_max_x = 25, map_max_y = 25;

class Mapchip
{
public:

	static void CsvToVector(std::vector<std::vector<int>>& mapData, std::string fName);

	static int GetChipNum(int x, int y, std::vector<int> map);

	static void ChangeChipNum(int x, int y, std::vector<int>& map);
};