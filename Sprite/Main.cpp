#include "Application.h"
#include <memory>

int main(int argc, char** argv)
{
#ifdef _DEBUG
	// Detect memory leaks
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// _CrtSetBreakAlloc(972);
#endif

	std::unique_ptr<Application> application = std::make_unique<Application>();
	return application->Execute();
}