#include "io/file_handle.h"

namespace ws {
namespace io {

std::filesystem::path FileHandle::GetFullPath(const std::string& input) {
  std::filesystem::path user_path(input);
  return std::filesystem::absolute(user_path);
}

std::filesystem::path FileHandle::GetFullPath(const std::wstring& input) {
  std::filesystem::path user_path(input);
  return std::filesystem::absolute(user_path);
}

std::filesystem::path FileHandle::Path() const { return path_; }

#ifdef _WIN32
static DWORD ParseCreationDisposition(FileMode mode, bool exists);
static DWORD ParseDesiredAccess(FileAccess access);
static DWORD ParseShareMode(FileShare share);
static OVERLAPPED OverlappedForSyncHandle(FileHandle& handle,
                                          offset_t file_offset);
static bool IsEndOfFileForNoBuffering(FileHandle& handle, offset_t file_offset);

FileHandle::FileHandle() : FileHandle(INVALID_HANDLE_VALUE) {}
FileHandle::FileHandle(void* fd)
    : fd_(fd),
      path_(),
      length_(-1),
      length_can_be_cached_(false),
      file_type_(-1) {}

FileHandle FileHandle::Open(const std::filesystem::path& full_path,
                            FileMode mode, FileAccess access, FileShare share,
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
  if (!SetFileInformationByHandle(handle.fd_, FileEndOfFileInfo, &eof_info,
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
                                  Span<unsigned char> buffer,
                                  offset_t file_offset) {
  OVERLAPPED overlapped = OverlappedForSyncHandle(handle, file_offset);
  DWORD num_bytes_read = 0;
  if (ReadFile(
          static_cast<HANDLE>(handle.fd_), static_cast<unsigned char*>(buffer),
          static_cast<DWORD>(buffer.Length()), &num_bytes_read, &overlapped)) {
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
  if (!SetFilePointerEx(static_cast<HANDLE>(handle.fd_), li_distance_to_move,
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
                               ReadOnlySpan<unsigned char> buffer,
                               offset_t file_offset) {
  if (buffer.Empty()) return;

  OVERLAPPED overlapped = OverlappedForSyncHandle(handle, file_offset);
  DWORD num_bytes_written = 0;
  if (WriteFile(static_cast<HANDLE>(handle.fd_),
                static_cast<const unsigned char*>(buffer),
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

bool FileHandle::IsClosed() const {
  return fd_ == nullptr || fd_ == INVALID_HANDLE_VALUE;
}

bool FileHandle::CanSeek() {
  return !IsClosed() && (FileType() == static_cast<DWORD>(FILE_TYPE_DISK));
}

bool FileHandle::TryGetCachedLength(offset_t& cached_length) {
  cached_length = length_;
  return length_can_be_cached_ && cached_length >= 0;
}

offset_t FileHandle::FileType() {
  offset_t file_type = file_type_;
  if (file_type == -1) file_type_ = file_type = GetFileType(fd_);
  return file_type;
}

offset_t FileHandle::FileLength() {
  if (length_can_be_cached_ && length_ >= 0) return length_;

  LARGE_INTEGER file_size = {};
  if (!GetFileSizeEx(fd_, &file_size)) {
    throw std::runtime_error("Error obtaining file size.");
  }

  length_ = file_size.QuadPart;
  return length_;
}

void FileHandle::Dispose() {
  if (!IsClosed()) {
    CloseHandle(fd_);
    fd_ = INVALID_HANDLE_VALUE;
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
    case FileShare::kDelete:
      share_mode = FILE_SHARE_DELETE;
      break;
    default:
      if ((share & FileShare::kRead) == FileShare::kRead)
        share_mode |= FILE_SHARE_READ;
      if ((share & FileShare::kWrite) == FileShare::kWrite)
        share_mode |= FILE_SHARE_WRITE;
      if ((share & FileShare::kDelete) == FileShare::kDelete)
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
static int ParseCreationDisposition(FileMode mode, bool exists);
static int ParseDesiredAccess(FileAccess access);
static int ParseShareMode(FileShare share);

FileHandle::FileHandle() : FileHandle(-1) {}

FileHandle::FileHandle(int fd)
    : fd_(fd),
      path_(),
      length_(-1),
      length_can_be_cached_(false),
      file_type_(-1) {}

FileHandle FileHandle::Open(const std::filesystem::path& full_path,
                            FileMode mode, FileAccess access, FileShare share,
                            offset_t preallocation_size) {
  bool exists = std::filesystem::exists(full_path);
  int flags = ParseCreationDisposition(mode, exists) |
              ParseDesiredAccess(access) | ParseShareMode(share);
  int fd = ::open(full_path.c_str(), flags, 0666);
  if (fd == -1)
    throw std::runtime_error("Error opening file: " +
                             std::string(std::strerror(errno)));

  if (preallocation_size > 0) {
#ifdef __APPLE__
    fstore_t store = {0};
    store.fst_flags = F_ALLOCATECONTIG;
    store.fst_posmode = F_PEOFPOSMODE;
    store.fst_offset = 0;
    store.fst_length = preallocation_size;
    store.fst_bytesalloc = 0;
    int ret = fcntl(fd, F_PREALLOCATE, &store);
    if (ret == -1) {
      store.fst_flags = F_ALLOCATEALL;
      ret = fcntl(fd, F_PREALLOCATE, &store);
    }

    if (ret == -1) {
      ::close(fd);
      throw std::runtime_error("Error preallocating space (F_PREALLOCATE): " +
                               std::string(std::strerror(errno)));
    }
#else
    int ret = posix_fallocate(fd, 0, preallocation_size);
    if (ret != 0) {
      ::close(fd);
      throw std::runtime_error("Error preallocating space: " +
                               std::string(std::strerror(ret)));
    }
#endif
  }

  FileHandle handle(fd);
  handle.path_ = full_path;
  handle.length_can_be_cached_ = (access == FileAccess::kRead);
  return handle;
}

void FileHandle::SetFileLength(FileHandle& handle, offset_t length) {
  if (::ftruncate(handle.fd_, length) == -1) {
    if (errno == EFBIG || errno == EINVAL)
      throw std::out_of_range("File size is too large.");
    else
      throw std::runtime_error("Error setting file length: " +
                               std::string(std::strerror(errno)));
  }

  handle.length_ = length;
  handle.length_can_be_cached_ = true;
}

offset_t FileHandle::ReadAtOffset(FileHandle& handle,
                                  Span<unsigned char> buffer,
                                  offset_t file_offset) {
  ssize_t bytes_read = pread(handle.fd_, static_cast<unsigned char*>(buffer),
                             buffer.Length(), file_offset);
  if (bytes_read == -1)
    throw std::runtime_error("Error reading file: " +
                             std::string(std::strerror(errno)));

  return static_cast<offset_t>(bytes_read);
}

offset_t FileHandle::Seek(FileHandle& handle, offset_t offset,
                          SeekOrigin origin, bool close_invalid_handle) {
  off_t new_offset = lseek(handle.fd_, offset, static_cast<int>(origin));
  if (new_offset == (off_t)-1) {
    if (close_invalid_handle) handle.Dispose();
    throw std::runtime_error("Error seeking file: " +
                             std::string(std::strerror(errno)));
  }

  return new_offset;
}

void FileHandle::WriteAtOffset(FileHandle& handle,
                               ReadOnlySpan<unsigned char> buffer,
                               offset_t file_offset) {
  if (buffer.Empty()) return;
  ssize_t bytes_written =
      pwrite(handle.fd_, static_cast<const unsigned char*>(buffer),
             buffer.Length(), file_offset);
  if (bytes_written == -1)
    throw std::runtime_error("Error writing file: " +
                             std::string(std::strerror(errno)));

  if (static_cast<size_t>(bytes_written) != buffer.Length())
    throw std::runtime_error("Wrote fewer bytes than expected.");
}

bool FileHandle::IsEndOfFile(offset_t error_code, FileHandle& handle,
                             offset_t file_offset) {
  offset_t length = handle.FileLength();
  return file_offset >= length;
}

bool FileHandle::IsClosed() const { return fd_ == -1; }

bool FileHandle::CanSeek() {
  if (IsClosed()) return false;
  struct stat st;
  if (fstat(fd_, &st) == -1) return false;
  return S_ISREG(st.st_mode);
}

bool FileHandle::TryGetCachedLength(offset_t& cached_length) {
  cached_length = length_;
  return length_can_be_cached_ && length_ >= 0;
}

offset_t FileHandle::FileType() {
  if (file_type_ == -1) {
    struct stat st;
    if (fstat(fd_, &st) == -1)
      throw std::runtime_error("Error obtaining file type: " +
                               std::string(std::strerror(errno)));

    file_type_ = st.st_mode;
  }
  return file_type_;
}

offset_t FileHandle::FileLength() {
  if (length_can_be_cached_ && length_ >= 0) return length_;
  struct stat st;
  if (fstat(fd_, &st) == -1)
    throw std::runtime_error("Error obtaining file size: " +
                             std::string(std::strerror(errno)));

  length_ = st.st_size;
  length_can_be_cached_ = true;
  return length_;
}

void FileHandle::Dispose() {
  if (!IsClosed()) {
    ::close(fd_);
    fd_ = -1;
  }

  length_can_be_cached_ = false;
}

int ParseCreationDisposition(FileMode mode, bool exists) {
  switch (mode) {
    case FileMode::kCreateNew:
      if (exists) throw std::runtime_error("File already exists (CreateNew)");
      return O_CREAT | O_EXCL;
    case FileMode::kCreate:
      return O_CREAT | O_TRUNC;
    case FileMode::kOpen:
      if (!exists) throw std::runtime_error("File does not exist (Open)");
      return 0;
    case FileMode::kOpenOrCreate:
      return exists ? 0 : O_CREAT;
    case FileMode::kAppend:
      return O_CREAT | O_APPEND;
    default:
      throw std::runtime_error("Unsupported file mode");
  }
}

int ParseDesiredAccess(FileAccess access) {
  int accessInt = static_cast<int>(access);
  bool canRead = (accessInt & static_cast<int>(FileAccess::kRead)) != 0;
  bool canWrite = (accessInt & static_cast<int>(FileAccess::kWrite)) != 0;

  if (canRead && canWrite)
    return O_RDWR;
  else if (canRead)
    return O_RDONLY;
  else if (canWrite)
    return O_WRONLY;
  else
    throw std::runtime_error("Unsupported file access mode");
}

int ParseShareMode(FileShare /*share*/) { return 0; }
#endif
}  // namespace io
}  // namespace ws
