#include "ws/io/directory.h"

namespace ws {
namespace io {

StatusOr<std::vector<std::string>> Directory::GetFiles(
    const std::string& path) {
  std::vector<std::string> files;

#ifdef _WIN32
  std::string searchPath = Path::NormalizePath(path) + "*";
  WIN32_FIND_DATAA findData;
  HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

  if (hFind == INVALID_HANDLE_VALUE) {
    DWORD error = GetLastError();
    if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
      return Status(StatusCode::kNotFound, "Directory not found: " + path);
    }
    return Status(StatusCode::kInternalError,
                  "Failed to open directory: " + GetLastErrorMessage());
  }

  do {
    if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
      files.push_back(std::string(findData.cFileName));
    }
  } while (FindNextFileA(hFind, &findData));

  FindClose(hFind);
#else
  DIR* dir = opendir(path.c_str());
  if (!dir) {
    if (errno == ENOENT) {
      return Status(StatusCode::kNotFound, "Directory not found: " + path);
    }
    return Status(StatusCode::kInternalError,
                  "Failed to open directory: " + GetLastErrorMessage());
  }

  struct dirent* entry;
  while ((entry = readdir(dir)) != nullptr) {
    if (entry->d_type == DT_REG ||
        (entry->d_type == DT_UNKNOWN &&
         Path::IsRegularFile(Path::NormalizePath(path) + entry->d_name)
             .ValueOr(false))) {
      files.push_back(std::string(entry->d_name));
    }
  }

  closedir(dir);
#endif

  std::sort(files.begin(), files.end());
  return files;
}

StatusOr<std::vector<std::string>> Directory::GetDirectories(
    const std::string& path) {
  std::vector<std::string> directories;

#ifdef _WIN32
  std::string searchPath = Path::NormalizePath(path) + "*";
  WIN32_FIND_DATAA findData;
  HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

  if (hFind == INVALID_HANDLE_VALUE) {
    DWORD error = GetLastError();
    if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
      return Status(StatusCode::kNotFound, "Directory not found: " + path);
    }
    return Status(StatusCode::kInternalError,
                  "Failed to open directory: " + GetLastErrorMessage());
  }

  do {
    if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
        strcmp(findData.cFileName, ".") != 0 &&
        strcmp(findData.cFileName, "..") != 0) {
      directories.push_back(std::string(findData.cFileName));
    }
  } while (FindNextFileA(hFind, &findData));

  FindClose(hFind);
#else
  DIR* dir = opendir(path.c_str());
  if (!dir) {
    if (errno == ENOENT) {
      return Status(StatusCode::kNotFound, "Directory not found: " + path);
    }
    return Status(StatusCode::kInternalError,
                  "Failed to open directory: " + GetLastErrorMessage());
  }

  struct dirent* entry;
  while ((entry = readdir(dir)) != nullptr) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    if (entry->d_type == DT_DIR ||
        (entry->d_type == DT_UNKNOWN &&
         Path::IsDirectory(Path::NormalizePath(path) + entry->d_name)
             .ValueOr(false))) {
      directories.push_back(std::string(entry->d_name));
    }
  }

  closedir(dir);
#endif

  std::sort(directories.begin(), directories.end());
  return directories;
}

StatusOr<bool> Directory::Exists(const std::string& path) {
  return Path::IsDirectory(path);
}

Status Directory::Create(const std::string& path) {
#ifdef _WIN32
  if (!CreateDirectoryA(path.c_str(), nullptr)) {
    DWORD error = GetLastError();
    if (error == ERROR_ALREADY_EXISTS) {
      bool is_directory;
      ASSIGN_OR_RETURN(is_directory, Path::IsDirectory(path));
      if (is_directory) {
        return Status();
      }

      return Status(StatusCode::kConflict,
                    "Path exists but is not a directory: " + path);
    }
    if (error == ERROR_PATH_NOT_FOUND) {
      return Status(StatusCode::kNotFound,
                    "Parent directory not found: " + path);
    }
    return Status(StatusCode::kInternalError,
                  "Failed to create directory: " + GetLastErrorMessage());
  }
#else
  if (mkdir(path.c_str(), kDefaultPermissions) != 0) {
    if (errno == EEXIST) {
      if (Path::IsDirectory(path).ValueOr(false)) {
        return Status();
      }
      return Status(StatusCode::kConflict,
                    "Path exists but is not a directory: " + path);
    }
    if (errno == ENOENT) {
      return Status(StatusCode::kNotFound,
                    "Parent directory not found: " + path);
    }
    return Status(StatusCode::kInternalError,
                  "Failed to create directory: " + GetLastErrorMessage());
  }
#endif

  return Status();
}

Status Directory::Delete(const std::string& path, bool recursive) {
  auto existsResult = Exists(path);
  if (!existsResult.Ok()) {
    return existsResult.GetStatus();
  }
  if (!existsResult.Value()) {
    return Status(StatusCode::kNotFound, "Directory not found: " + path);
  }

  if (recursive) {
    auto filesResult = GetFiles(path);
    if (!filesResult.Ok()) {
      return filesResult.GetStatus();
    }

    auto dirsResult = GetDirectories(path);
    if (!dirsResult.Ok()) {
      return dirsResult.GetStatus();
    }

    for (const auto& file : filesResult.Value()) {
      std::string filePath = Path::NormalizePath(path) + file;
#ifdef _WIN32
      if (!DeleteFileA(filePath.c_str())) {
        return Status(StatusCode::kInternalError,
                      "Failed to delete file: " + filePath + " - " +
                          GetLastErrorMessage());
      }
#else
      if (unlink(filePath.c_str()) != 0) {
        return Status(StatusCode::kInternalError,
                      "Failed to delete file: " + filePath + " - " +
                          GetLastErrorMessage());
      }
#endif
    }

    for (const auto& dir : dirsResult.Value()) {
      std::string dirPath = Path::NormalizePath(path) + dir;
      auto deleteResult = Delete(dirPath, true);
      if (!deleteResult.Ok()) {
        return deleteResult;
      }
    }
  }

#ifdef _WIN32
  if (!RemoveDirectoryA(path.c_str())) {
    DWORD error = GetLastError();
    if (error == ERROR_DIR_NOT_EMPTY) {
      return Status(StatusCode::kFailedDependency,
                    "Directory not empty (use recursive=true): " + path);
    }
    return Status(StatusCode::kInternalError,
                  "Failed to delete directory: " + GetLastErrorMessage());
  }
#else
  if (rmdir(path.c_str()) != 0) {
    if (errno == ENOTEMPTY) {
      return Status(StatusCode::kFailedDependency,
                    "Directory not empty (use recursive=true): " + path);
    }

    return Status(StatusCode::kInternalError,
                  "Failed to delete directory: " + GetLastErrorMessage());
  }
#endif

  return Status();
}

}  // namespace io
}  // namespace ws

// FIXME: TEST LINUX IMPLEMENTATION
