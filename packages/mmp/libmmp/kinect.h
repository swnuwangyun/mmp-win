#pragma once

#include <string>
#include <map>
#include <vector>
#include <glm/gtc/matrix_transform.hpp> 

struct KinectListItem
{
	std::wstring name;	// mmd中骨骼点的日文名称，unicode编码
	std::wstring child; // Kinect子节点名称
	int index;			// mmd中骨骼点的索引
};

// Kinect中19个骨骼点和mmd中的映射关系
extern std::map<std::wstring, KinectListItem> kinectBoneList;

// Kinect设备模拟数据
extern std::vector<std::map<std::wstring, glm::vec3>> bodyInfoList;