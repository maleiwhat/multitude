
#include <Radiant/PlatformUtils.hpp>
#include <Radiant/Trace.hpp>

int main()
{
  Radiant::trace("Application path = %s",
		 Radiant::PlatformUtils::getExecutablePath().c_str());
  Radiant::trace("Application system-wide resource dir = %s",
		 Radiant::PlatformUtils::getModuleGlobalDataPath
		 ("MultiTude/PlatformTest", true).c_str());
  Radiant::trace("Application user-specific resource dir = %s",
		 Radiant::PlatformUtils::getModuleUserDataPath
		 ("MultiTude/PlatformTest", true).c_str());
}
