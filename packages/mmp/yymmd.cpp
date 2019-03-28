#include "yymmd.h"
#include "yymmdimpl.h"

YY_MMD_API IYYMMD* GetInstance()
{
	return YYMMDImpl::GetInstance();
}