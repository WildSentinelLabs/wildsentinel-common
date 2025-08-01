
FileStream::FileStream(const std::string& path, FileMode mode)
    : FileStream(path, mode,
                 mode == FileMode::kAppend
                     ? FileAccess::kWrite
                     : FileAccess::kRead | FileAccess::kWrite,
                 kDefaultShare) {}

FileStream::FileStream(const std::string& path, FileMode mode,
                       FileAccess access)
    : FileStream(path, mode, access, kDefaultShare, kDefaultBufferSize) {}

FileStream::FileStream(const std::string& path, FileMode mode,
                       FileAccess access, FileShare share)
    : FileStream(path, mode, access, share, kDefaultBufferSize) {}

static constexpr FileShare kDefaultShare = FileShare::kRead;

FileStream(const std::string& path);
FileStream(const std::string& path, FileMode mode);
FileStream(const std::string& path, FileMode mode, FileAccess access);
FileStream(const std::string& path, FileMode mode, FileAccess access,
           FileShare share);
