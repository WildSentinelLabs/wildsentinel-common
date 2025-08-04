#include "ws/io/path.h"

namespace ws {
namespace io {

std::filesystem::path Path::GetFullPath(const std::string& input) {
  std::filesystem::path user_path(input);
  return std::filesystem::absolute(user_path);
}

std::filesystem::path Path::GetFullPath(const std::wstring& input) {
  std::filesystem::path user_path(input);
  return std::filesystem::absolute(user_path);
}

StatusOr<bool> Path::IsFile(const std::string& path) {
#ifdef _WIN32
  DWORD attrs = GetFileAttributesA(path.c_str());
  if (attrs == INVALID_FILE_ATTRIBUTES) {
    DWORD error = GetLastError();
    if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND)
      return false;

    return Status(StatusCode::kInternalError,
                  "Failed to check file existence: " + GetLastErrorMessage());
  }

  return (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0;
#else
  struct stat statbuf;
  if (stat(path.c_str(), &statbuf) != 0) {
    if (errno == ENOENT) return false;
    return Status(StatusCode::kInternalError,
                  "Failed to check file existence: " + GetLastErrorMessage());
  }

  return S_ISREG(statbuf.st_mode);
#endif
}

StatusOr<bool> Path::IsDirectory(const std::string& path) {
#ifdef _WIN32
  DWORD attrs = GetFileAttributesA(path.c_str());
  if (attrs == INVALID_FILE_ATTRIBUTES) {
    DWORD error = GetLastError();
    if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND)
      return false;

    return Status(
        StatusCode::kInternalError,
        "Failed to check directory existence: " + GetLastErrorMessage());
  }

  return (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0;
#else
  struct stat statbuf;
  if (stat(path.c_str(), &statbuf) != 0) {
    if (errno == ENOENT) return false;
    return Status(
        StatusCode::kInternalError,
        "Failed to check directory existence: " + GetLastErrorMessage());
  }

  return S_ISDIR(statbuf.st_mode);
#endif
}

StatusOr<bool> Path::IsSpecialFile(const std::string& path) {
#ifdef _WIN32
  DWORD attrs = GetFileAttributesA(path.c_str());
  if (attrs == INVALID_FILE_ATTRIBUTES) {
    DWORD error = GetLastError();
    if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND)
      return false;

    return Status(StatusCode::kInternalError,
                  "Failed to check file type: " + GetLastErrorMessage());
  }

  return (attrs & (FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_DEVICE)) != 0;
#else
  struct stat statbuf;
  if (stat(path.c_str(), &statbuf) != 0) {
    if (errno == ENOENT) return false;
    return Status(StatusCode::kInternalError,
                  "Failed to check file type: " + GetLastErrorMessage());
  }

  return S_ISCHR(statbuf.st_mode) ||   // Character device
         S_ISBLK(statbuf.st_mode) ||   // Block device
         S_ISFIFO(statbuf.st_mode) ||  // Named pipe/FIFO
         S_ISSOCK(statbuf.st_mode);    // Socket
#endif
}

StatusOr<bool> Path::IsSymbolicLink(const std::string& path) {
#ifdef _WIN32
  DWORD attrs = GetFileAttributesA(path.c_str());
  if (attrs == INVALID_FILE_ATTRIBUTES) {
    DWORD error = GetLastError();
    if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND)
      return false;

    return Status(StatusCode::kInternalError,
                  "Failed to check link type: " + GetLastErrorMessage());
  }

  return (attrs & FILE_ATTRIBUTE_REPARSE_POINT) != 0;
#else
  struct stat statbuf;
  if (lstat(path.c_str(), &statbuf) != 0) {
    if (errno == ENOENT) return false;
    return Status(StatusCode::kInternalError,
                  "Failed to check link type: " + GetLastErrorMessage());
  }

  return S_ISLNK(statbuf.st_mode);
#endif
}

std::string Path::NormalizePath(const std::string& path) {
  std::string normalized = path;
  if (!normalized.empty() && normalized.back() != '/' &&
      normalized.back() != '\\') {
#ifdef _WIN32
    normalized += "\\";
#else
    normalized += "/";
#endif
  }
  return normalized;
}

StatusOr<std::string> Path::GetParent(const std::string& path) {
  if (path.empty())
    return Status(StatusCode::kBadRequest, "Path cannot be empty");

  std::string normalizedPath = path;
  while (!normalizedPath.empty() &&
         (normalizedPath.back() == '/' || normalizedPath.back() == '\\')) {
    normalizedPath.pop_back();
  }

  if (normalizedPath.empty())
    return Status(StatusCode::kBadRequest,
                  "Invalid path: root path has no parent");

#ifdef _WIN32
  if (normalizedPath.length() == 2 && normalizedPath[1] == ':')
    return Status(StatusCode::kBadRequest,
                  "Drive root has no parent: " + normalizedPath);

  size_t lastSeparator = normalizedPath.find_last_of("\\/");
  if (lastSeparator == std::string::npos) return std::string(".");
  if (lastSeparator == 0) return std::string("\\");
  if (lastSeparator == 2 && normalizedPath[1] == ':')
    return normalizedPath.substr(0, lastSeparator + 1);

  return normalizedPath.substr(0, lastSeparator);
#else
  size_t lastSeparator = normalizedPath.find_last_of('/');
  if (lastSeparator == std::string::npos) return std::string(".");
  if (lastSeparator == 0) return std::string("/");
  return normalizedPath.substr(0, lastSeparator);
#endif
}
}  // namespace io
}  // namespace ws
