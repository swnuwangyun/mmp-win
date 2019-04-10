#include "yymmd.h"
#include "yymmdimpl.h"

YY_MMD_API YYMMD::IYYMMD* GetInstance()
{
	return YYMMDImpl::GetInstance();
}