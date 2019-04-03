#pragma once

#include <string>
#include <map>
#include <vector>
#include <glm/gtc/matrix_transform.hpp> 

struct KinectListItem
{
	std::wstring name;
	std::wstring child;
	int index;
};

extern std::map<std::wstring, KinectListItem> kinectBoneList;
extern std::vector<std::map<std::wstring, glm::vec3>> bodyInfoList;