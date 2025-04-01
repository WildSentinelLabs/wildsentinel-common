#include "io/file_handle.h"

namespace ws {
namespace io {
#ifdef _WIN32
#include <windows.h>
static DWORD ParseCreationDisposition(FileMode mode, bool exists);
static DWORD ParseDesiredAccess(FileAccess access);
static DWORD ParseShareMode(FileShare share);
static OVERLAPPED OverlappedForSyncHandle(FileHandle& handle,
                                          offset_t file_offset);
static bool IsEndOfFileForNoBuffering(FileHandle& handle, offset_t file_offset);

FileHandle::FileHandle() : FileHandle(INVALID_HANDLE_VALUE) {}
FileHandle::FileHandle(void* handle)
    : handle_(handle),
      path_(),
      length_(-1),
      length_can_be_cached_(false),
      file_type_(-1) {}

FileHandle FileHandle::Open(const std::wstring& full_path, FileMode mode,
                            FileAccess access, FileShare share,
                            offset_t preallocation_size) {
  DWORD attributes = GetFileAttributesW(full_path.c_str());
  bool exists = (attributes != INVALID_FILE_ATTRIBUTES);
  DWORD creation_disposition = ParseCreationDisposition(mode, exists);
  DWORD desired_access = ParseDesiredAccess(access);
  DWORD share_mode = ParseShareMode(share);
  HANDLE h = CreateFileW(full_path.c_str(), desired_access, share_mode, nullptr,
                         creation_disposition, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (h == nullptr || h == INVALID_HANDLE_VALUE) {
    throw std::runtime_error(
        "Failed to open file: CreateFileW returned INVALID_HANDLE_VALUE.");
  }

  FileHandle handle(h);
  if (!handle.IsClosed()) {
    handle.path_ = full_path;
    handle.length_can_be_cached_ =
        (share & FileShare::kWrite) == static_cast<FileShare>(0) &&
        (access & FileAccess::kWrite) == static_cast<FileAccess>(0);

    if (preallocation_size > 0) {
      FILE_ALLOCATION_INFO allocation_info = {};
      allocation_info.AllocationSize.QuadPart = preallocation_size;
      if (!SetFileInformationByHandle(h, FileEndOfFileInfo, &allocation_info,
                                      sizeof(allocation_info))) {
        DWORD err = GetLastError();
        CloseHandle(h);
        DeleteFileW(full_path.c_str());
        throw std::runtime_error(
            "Failed to preallocate file space. Error code: " +
            std::to_string(err));
      }
    }
  }

  return handle;
}

void FileHandle::SetFileLength(FileHandle& handle, offset_t length) {
  FILE_END_OF_FILE_INFO eof_info = {};
  eof_info.EndOfFile.QuadPart = length;
  if (!SetFileInformationByHandle(handle.handle_, FileEndOfFileInfo, &eof_info,
                                  sizeof(eof_info))) {
    DWORD error_code = GetLastError();
    if (error_code == ERROR_INVALID_PARAMETER) {
      throw std::out_of_range("File length is too big.");
    } else {
      throw std::runtime_error("Error setting file length. Error code: " +
                               std::to_string(error_code));
    }
  }
}

offset_t FileHandle::ReadAtOffset(FileHandle& handle,
                                  Span<unsigned char>& buffer,
                                  offset_t file_offset) {
  OVERLAPPED overlapped = OverlappedForSyncHandle(handle, file_offset);
  DWORD num_bytes_read = 0;
  if (ReadFile(static_cast<HANDLE>(handle.Get()), buffer,
               static_cast<DWORD>(buffer.Length()), &num_bytes_read,
               &overlapped)) {
    return static_cast<offset_t>(num_bytes_read);
  } else {
    DWORD error_code = GetLastError();

    if (error_code == ERROR_HANDLE_EOF)
      return static_cast<offset_t>(num_bytes_read);

    if (IsEndOfFile(error_code, handle, file_offset)) return 0;
    throw std::runtime_error("ReadFile failed with error " +
                             std::to_string(error_code));
  }
}

offset_t FileHandle::Seek(FileHandle& handle, offset_t offset,
                          SeekOrigin origin, bool close_invalid_handle) {
  LARGE_INTEGER li_distance_to_move = {};
  li_distance_to_move.QuadPart = offset;
  LARGE_INTEGER li_new_file_pointer = {};
  if (!SetFilePointerEx(static_cast<HANDLE>(handle.Get()), li_distance_to_move,
                        &li_new_file_pointer, static_cast<DWORD>(origin))) {
    DWORD error_code = GetLastError();
    if (close_invalid_handle) {
      handle.Dispose();
    }

    throw std::runtime_error("SetFilePointerEx failed with error " +
                             std::to_string(error_code));
  }

  return li_new_file_pointer.QuadPart;
}

void FileHandle::WriteAtOffset(FileHandle& handle,
                               const ReadOnlySpan<unsigned char>& buffer,
                               offset_t file_offset) {
  if (buffer.IsEmpty()) return;

  OVERLAPPED overlapped = OverlappedForSyncHandle(handle, file_offset);
  DWORD num_bytes_written = 0;
  if (WriteFile(static_cast<HANDLE>(handle.Get()), buffer,
                static_cast<DWORD>(buffer.Length()), &num_bytes_written,
                &overlapped)) {
    if (num_bytes_written != buffer.Length())
      throw std::runtime_error("WriteFile wrote fewer bytes than expected.");
    return;
  } else {
    DWORD error_code = GetLastError();
    throw std::runtime_error("WriteFile failed with error " +
                             std::to_string(error_code));
  }
}

bool FileHandle::IsEndOfFile(offset_t error_code, FileHandle& handle,
                             offset_t file_offset) {
  switch (error_code) {
    case ERROR_HANDLE_EOF:
    case ERROR_BROKEN_PIPE:
    case ERROR_PIPE_NOT_CONNECTED:
      return true;
    case ERROR_INVALID_PARAMETER:
      if (IsEndOfFileForNoBuffering(handle, file_offset)) return true;
      break;
    default:
      break;
  }
  return false;
}

std::wstring FileHandle::GetFullPath(const std::wstring& path) {
  DWORD required_size = GetFullPathNameW(path.c_str(), 0, nullptr, nullptr);
  if (required_size == 0) {
    DWORD error_code = GetLastError();
    throw std::runtime_error("GetFullPathNameW failed with error " +
                             std::to_string(error_code));
  }

  std::wstring full_path(required_size, L'\0');
  DWORD result =
      GetFullPathNameW(path.c_str(), required_size, &full_path[0], nullptr);
  if (result == 0 || result >= required_size) {
    DWORD error_code = GetLastError();
    throw std::runtime_error("GetFullPathNameW failed with error " +
                             std::to_string(error_code));
  }

  full_path.resize(result);
  return full_path;
}

std::wstring FileHandle::Path() const { return path_; }

bool FileHandle::IsClosed() const {
  return handle_ == nullptr || handle_ == INVALID_HANDLE_VALUE;
}

bool FileHandle::CanSeek() {
  return !IsClosed() && (FileType() == static_cast<DWORD>(FILE_TYPE_DISK));
}

bool FileHandle::TryGetCachedLength(offset_t& cached_length) {
  cached_length = length_;
  return length_can_be_cached_ && cached_length >= 0;
}

void* FileHandle::Get() { return handle_; }

offset_t FileHandle::FileType() {
  offset_t file_type = file_type_;
  if (file_type == -1) file_type_ = file_type = GetFileType(handle_);
  return file_type;
}

offset_t FileHandle::FileLength() {
  if (length_can_be_cached_ && length_ >= 0) return length_;

  LARGE_INTEGER file_size = {};
  if (!GetFileSizeEx(handle_, &file_size)) {
    throw std::runtime_error("Error obtaining file size.");
  }

  length_ = file_size.QuadPart;
  return length_;
}

void FileHandle::Dispose() {
  if (!IsClosed()) {
    CloseHandle(handle_);
    handle_ = INVALID_HANDLE_VALUE;
  }

  length_can_be_cached_ = false;
}

DWORD ParseCreationDisposition(FileMode mode, bool exists) {
  switch (mode) {
    case FileMode::kCreateNew:
      if (exists) throw std::runtime_error("File already exists (CreateNew)");
      return CREATE_NEW;
      break;
    case FileMode::kCreate:
      return CREATE_ALWAYS;
      break;
    case FileMode::kOpen:
      if (!exists) throw std::runtime_error("File does not exist (Open)");
      return OPEN_EXISTING;
      break;
    case FileMode::kOpenOrCreate:
      return exists ? OPEN_EXISTING : CREATE_NEW;
      break;
    case FileMode::kAppend:
      return OPEN_ALWAYS;
      break;
    default:
      throw std::runtime_error("Unsupported file mode");
  }
}

DWORD ParseDesiredAccess(FileAccess access) {
  DWORD desired_access = 0;
  if ((access & FileAccess::kRead) == FileAccess::kRead)
    desired_access |= GENERIC_READ;
  if ((access & FileAccess::kWrite) == FileAccess::kWrite)
    desired_access |= GENERIC_WRITE;

  return desired_access;
}

DWORD ParseShareMode(FileShare share) {
  DWORD share_mode = 0;
  switch (share) {
    case FileShare::kRead:
      share_mode = FILE_SHARE_READ;
      break;
    case FileShare::kWrite:
      share_mode = FILE_SHARE_WRITE;
      break;
    case FileShare::Delete:
      share_mode = FILE_SHARE_DELETE;
      break;
    default:
      if ((share & FileShare::kRead) == FileShare::kRead)
        share_mode |= FILE_SHARE_READ;
      if ((share & FileShare::kWrite) == FileShare::kWrite)
        share_mode |= FILE_SHARE_WRITE;
      if ((share & FileShare::Delete) == FileShare::Delete)
        share_mode |= FILE_SHARE_DELETE;
      break;
  }

  return share_mode;
}

OVERLAPPED OverlappedForSyncHandle(FileHandle& handle, offset_t file_offset) {
  OVERLAPPED overlapped = {};
  if (handle.CanSeek()) {
    overlapped.Offset = static_cast<DWORD>(file_offset);
    overlapped.OffsetHigh = static_cast<DWORD>(file_offset >> 32);
  }

  return overlapped;
}

bool IsEndOfFileForNoBuffering(FileHandle& handle, offset_t file_offset) {
  if (!handle.CanSeek()) return false;
  return file_offset >= handle.FileLength();
}

#else
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#endif  // _WIN32
}  // namespace io
}  // namespace ws
