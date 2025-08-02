#include "ws/io/file.h"

#include <sstream>
#include <vector>

#include "ws/array.h"
#include "ws/span.h"
#include "ws/string/string_helpers.h"
#include "ws/types.h"

namespace ws {
namespace io {

StatusOr<FileStream> File::Create(const std::string& path) {
  FileStream stream;
  ASSIGN_OR_RETURN(stream,
                   FileStream::Create(path, FileMode::kCreate,
                                      FileAccess::kRead | FileAccess::kWrite,
                                      kDefaultShare));
  return std::move(stream);
}

StatusOr<FileStream> File::Open(const std::string& path) {
  FileStream stream;
  ASSIGN_OR_RETURN(stream,
                   FileStream::Create(path, FileMode::kOpenOrCreate,
                                      FileAccess::kRead | FileAccess::kWrite,
                                      kDefaultShare));
  return std::move(stream);
}

StatusOr<FileStream> File::Open(const std::string& path, FileMode mode) {
  FileAccess access = (mode == FileMode::kAppend)
                          ? FileAccess::kWrite
                          : FileAccess::kRead | FileAccess::kWrite;
  FileStream stream;
  ASSIGN_OR_RETURN(stream,
                   FileStream::Create(path, mode, access, kDefaultShare));
  return std::move(stream);
}

StatusOr<FileStream> File::Open(const std::string& path, FileMode mode,
                                FileAccess access) {
  FileStream stream;
  ASSIGN_OR_RETURN(stream,
                   FileStream::Create(path, mode, access, kDefaultShare));
  return std::move(stream);
}

StatusOr<FileStream> File::Open(const std::string& path, FileMode mode,
                                FileAccess access, FileShare share) {
  FileStream stream;
  ASSIGN_OR_RETURN(stream, FileStream::Create(path, mode, access, share));
  return std::move(stream);
}

StatusOr<std::string> File::ReadAllText(const std::string& path) {
  FileStream stream;
  ASSIGN_OR_RETURN(stream, Open(path, FileMode::kOpen, FileAccess::kRead));
  offset_t length = stream.Length();
  if (length == 0) return std::string();
  Array<unsigned char> buffer(static_cast<std::size_t>(length));
  offset_t bytes_read;
  ASSIGN_OR_RETURN(bytes_read, stream.Read(buffer));
  if (bytes_read < length)
    return Status(StatusCode::kInternalError, "Failed to read the entire file");
  return std::string(reinterpret_cast<const char*>(buffer.begin()),
                     buffer.Length());
}

StatusOr<Array<std::string>> File::ReadAllLines(const std::string& path) {
  std::string content;
  ASSIGN_OR_RETURN(content, ReadAllText(path));
  if (content.empty()) return Array<std::string>();
  NormalizeLineEndingsInPlace(content);
  Array<std::string> lines = Split(content, '\n');
  return lines;
}

StatusOr<Array<unsigned char>> File::ReadAllBytes(const std::string& path) {
  FileStream stream;
  ASSIGN_OR_RETURN(stream, Open(path));
  offset_t length = stream.Length();
  if (length == 0) return Array<unsigned char>(0);
  Array<unsigned char> buffer(length);
  ASSIGN_OR_RETURN(length, stream.Read(buffer));
  return buffer;
}

}  // namespace io
}  // namespace ws
