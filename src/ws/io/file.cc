#include "ws/io/file.h"

namespace ws {
namespace io {

StatusOr<FileStream> File::Create(const std::string& path) {
  return File::Open(path, FileMode::kCreate);
}

StatusOr<FileStream> File::Open(const std::string& path) {
  return File::Open(path, FileMode::kOpenOrCreate);
}

StatusOr<FileStream> File::Open(const std::string& path, FileMode mode) {
  FileAccess access = (mode == FileMode::kAppend)
                          ? FileAccess::kWrite
                          : FileAccess::kRead | FileAccess::kWrite;

  return File::Open(path, mode, access);
}

StatusOr<FileStream> File::Open(const std::string& path, FileMode mode,
                                FileAccess access) {
  return File::Open(path, mode, access, kDefaultShare);
}

StatusOr<FileStream> File::Open(const std::string& path, FileMode mode,
                                FileAccess access, FileShare share) {
  return FileStream::Create(path, mode, access, share);
}

StatusOr<bool> File::Exists(const std::string& path) {
  if (path.empty())
    return Status(StatusCode::kBadRequest, "Path cannot be empty");

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

Status File::Move(const std::string& sourcePath,
                  const std::string& destinationPath) {
  if (sourcePath.empty())
    return Status(StatusCode::kBadRequest, "Source path cannot be empty");

  if (destinationPath.empty())
    return Status(StatusCode::kBadRequest, "Destination path cannot be empty");

  if (sourcePath == destinationPath) return Status();
#ifdef _WIN32
  if (!MoveFileA(sourcePath.c_str(), destinationPath.c_str())) {
    DWORD error = GetLastError();
    if (error == ERROR_FILE_NOT_FOUND)
      return Status(StatusCode::kNotFound,
                    "Source file not found: " + sourcePath);

    if (error == ERROR_PATH_NOT_FOUND)
      return Status(StatusCode::kNotFound,
                    "Source or destination path not found");

    if (error == ERROR_ALREADY_EXISTS)
      return Status(StatusCode::kConflict,
                    "Destination file already exists: " + destinationPath);

    if (error == ERROR_ACCESS_DENIED)
      return Status(StatusCode::kForbidden, "Access denied when moving file");

    return Status(StatusCode::kInternalError,
                  "Failed to move file: " + GetLastErrorMessage());
  }
#else
  if (rename(sourcePath.c_str(), destinationPath.c_str()) != 0) {
    if (errno == ENOENT)
      return Status(StatusCode::kNotFound,
                    "Source file not found: " + sourcePath);

    if (errno == EEXIST)
      return Status(StatusCode::kConflict,
                    "Destination file already exists: " + destinationPath);

    if (errno == EACCES || errno == EPERM)
      return Status(StatusCode::kForbidden,
                    "Permission denied when moving file");

    if (errno == EXDEV)
      return Status(StatusCode::kNotImplemented,
                    "Cross-device move not supported. Source and destination "
                    "must be on the same filesystem");

    return Status(StatusCode::kInternalError,
                  "Failed to move file: " + GetLastErrorMessage());
  }
#endif

  return Status();
}

Status File::Delete(const std::string& path) {
  if (path.empty())
    return Status(StatusCode::kBadRequest, "Path cannot be empty");

#ifdef _WIN32
  if (!DeleteFileA(path.c_str())) {
    DWORD error = GetLastError();
    if (error == ERROR_FILE_NOT_FOUND)
      return Status(StatusCode::kNotFound, "File not found: " + path);

    if (error == ERROR_PATH_NOT_FOUND)
      return Status(StatusCode::kNotFound, "Path not found: " + path);

    if (error == ERROR_ACCESS_DENIED)
      return Status(StatusCode::kForbidden,
                    "Access denied when deleting file: " + path);

    if (error == ERROR_SHARING_VIOLATION)
      return Status(StatusCode::kConflict,
                    "File is in use and cannot be deleted: " + path);

    return Status(StatusCode::kInternalError,
                  "Failed to delete file: " + GetLastErrorMessage());
  }
#else
  if (unlink(path.c_str()) != 0) {
    if (errno == ENOENT)
      return Status(StatusCode::kNotFound, "File not found: " + path);

    if (errno == EACCES || errno == EPERM)
      return Status(StatusCode::kForbidden,
                    "Permission denied when deleting file: " + path);

    if (errno == EISDIR)
      return Status(StatusCode::kBadRequest,
                    "Path is a directory, not a file: " + path);

    if (errno == EBUSY)
      return Status(StatusCode::kConflict,
                    "File is in use and cannot be deleted: " + path);

    return Status(StatusCode::kInternalError,
                  "Failed to delete file: " + GetLastErrorMessage());
  }
#endif

  return Status();
}

StatusOr<std::string> File::ReadAllText(const std::string& path) {
  FileStream stream;
  ASSIGN_OR_RETURN(stream, Open(path, FileMode::kOpen, FileAccess::kRead));
  offset_t length = stream.Length();
  if (length == 0) return std::string();
  container_type buffer(static_cast<std::size_t>(length));
  offset_t bytes_read;
  ASSIGN_OR_RETURN(bytes_read, stream.Read(buffer));
  if (bytes_read < length)
    return Status(StatusCode::kInternalError, "Failed to read the entire file");

  return std::string(reinterpret_cast<const char*>(buffer.begin()),
                     buffer.size());
}

StatusOr<std::vector<std::string>> File::ReadAllLines(const std::string& path) {
  std::string content;
  ASSIGN_OR_RETURN(content, ReadAllText(path));
  if (content.empty()) return std::vector<std::string>();
  NormalizeLineEndingsInPlace(content);
  return Split(content, '\n');
}

StatusOr<File::container_type> File::ReadAllBytes(const std::string& path) {
  FileStream stream;
  ASSIGN_OR_RETURN(stream, Open(path));
  offset_t length = stream.Length();
  if (length == 0) return container_type(0);
  container_type buffer(length);
  offset_t bytes_read;
  ASSIGN_OR_RETURN(bytes_read, stream.Read(buffer));
  if (bytes_read < length)
    return Status(StatusCode::kInternalError, "Failed to read the entire file");

  return buffer;
}
}  // namespace io
}  // namespace ws
