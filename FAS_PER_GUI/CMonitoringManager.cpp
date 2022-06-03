#include "pch.h"
#include "CMonitoringManager.h"

CMonitoringManager::CMonitoringManager()
{

}


CMonitoringManager::~CMonitoringManager()
{

}

CMonitoringManager* CMonitoringManager::GetInstance()
{
	static CMonitoringManager _instance;
	return &_instance;
}
