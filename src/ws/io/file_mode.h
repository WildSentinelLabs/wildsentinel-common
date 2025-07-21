#pragma once
namespace ws {
namespace io {
enum class FileMode {
  kCreateNew = 1,
  kCreate = 2,
  kOpen = 3,
  kOpenOrCreate = 4,
  kAppend = 5
};

}
}  // namespace ws
