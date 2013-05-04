#include <Foundation/PCH.h>
#include <Foundation/Configuration/Plugin.h>
#include <Foundation/Strings/StringBuilder.h>
#include <Foundation/IO/OSFile.h>
#include <Foundation/Configuration/Implementation/Win/Plugin_Win.h>
#include <Foundation/Logging/Log.h>

#if EZ_ENABLED(EZ_PLATFORM_WINDOWS)

void ezPlugin::GetPluginPaths(const char* szPluginName, ezStringBuilder& sOldPath, ezStringBuilder& sNewPath)
{
  sOldPath = ezOSFile::GetApplicationDirectory();
  sOldPath.AppendPath(szPluginName);
  sOldPath.Append(".dll");

  sNewPath = ezOSFile::GetApplicationDirectory();
  sNewPath.AppendPath(szPluginName);
  sNewPath.Append(".loaded");
}

ezResult UnloadPluginModule(ezPluginModule& Module, const char* szPluginFile)
{
  // reset last error code
  if (GetLastError()) { }

  if (FreeLibrary(Module) == FALSE)
  {
    DWORD err = GetLastError();

    LPVOID lpMsgBuf = NULL;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
        err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL);

    ezLog::Error("Could not unload plugin '%s'. Error-Code %u (\"%s\")", szPluginFile, err, lpMsgBuf);

    return EZ_FAILURE;
  }

  Module = NULL;
  return EZ_SUCCESS;
}

ezResult LoadPluginModule(const char* szFileToLoad, ezPluginModule& Module, const char* szPluginFile)
{
  // reset last error code
  if (GetLastError()) { }

  Module = LoadLibrary(szFileToLoad);

  if (Module == NULL)
  {
    DWORD err = GetLastError();

    LPVOID lpMsgBuf = NULL;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
        err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL);

    ezLog::Error("Could not load plugin '%s'. Error-Code %u (\"%s\")", szPluginFile, err, lpMsgBuf);

    LocalFree(lpMsgBuf);

    return EZ_FAILURE;
  }

  return EZ_SUCCESS;
}

#endif

