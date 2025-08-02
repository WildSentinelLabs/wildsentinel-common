# WildSentinel Common

A comprehensive C++ library providing essential utilities and components for WildSentinel projects. This library offers a modular architecture with optional components that can be selectively built based on project requirements.

## Overview

WildSentinel Common is designed to provide robust, cross-platform foundational components for C++ applications. The library is organized into specialized modules, each addressing specific domains of functionality while maintaining high performance and reliability.

## Features

- **Modular Architecture**: Build only the components you need
- **Cross-Platform**: Windows and Unix support
- **Thread-Safe**: Concurrent data structures and thread management utilities
- **High Performance**: Optimized for real-world applications
- **Modern C++**: Leverages contemporary C++ standards and best practices

## Module Documentation

### Core (`core`)

**Location**: `src/ws/`

The core module provides fundamental types, utilities, and abstractions used throughout the library.

**Key Components**:
- `array.h`: Dynamic array container with performance optimizations
- `span.h`: Non-owning view over contiguous sequences
- `delegate.h`: Type-safe function pointer abstraction
- `machine.h`: Platform-specific machine information and utilities
- `types.h`: Common type definitions and platform abstractions

**Purpose**: Establishes the foundation layer for all other modules, providing essential data structures and cross-platform compatibility.

### Concurrency (`concurrency`)

**Location**: `src/ws/concurrency/`

Thread-safe data structures and synchronization primitives for concurrent programming. Non-blocking collections are based on [oneTBB](https://github.com/uxlfoundation/oneTBB) algorithms.

**Key Components**:
- `spin_mutex.h`: High-performance spinlock implementation
- `collections/blocking_queue.h`: Thread-safe queue with blocking operations
- `collections/concurrent_queue.h`: Lock-free concurrent queue (oneTBB-based)
- `collections/concurrent_unordered_map.h`: Thread-safe hash map (oneTBB-based)
- `collections/concurrent_unordered_set.h`: Thread-safe hash set (oneTBB-based)
- `collections/synchronized_queue.h`: Mutex-protected queue operations

**Purpose**: Enables safe and efficient multi-threaded programming with battle-tested concurrent data structures and synchronization mechanisms.

### Imaging (`imaging`)

**Location**: `src/ws/imaging/`

Complete image processing and manipulation framework supporting various formats and operations.

**Key Components**:
- `image.h`: Main image class with comprehensive manipulation capabilities
- `image_decoder.h` / `image_encoder.h`: Format-specific encoding/decoding
- `image_converter.h`: Image format and color space conversion
- `color_space.h`: Color space definitions and transformations
- `chroma_subsampling.h`: Chroma subsampling support for compression
- `image_compression_options.h`: Configurable compression parameters

**Purpose**: Provides a unified interface for image processing tasks, supporting multiple formats, color spaces, and compression algorithms for multimedia applications.

### Input/Output (`io`)

**Location**: `src/ws/io/`

File system operations, stream handling, and data serialization utilities.

**Key Components**:
- `buffer_stream.h`: Memory-based stream operations
- `file_access.h`: File system access abstractions
- `file_format_detector.h`: Automatic file format detection

**Purpose**: Abstracts file system operations and provides efficient stream-based I/O capabilities with automatic format detection.

### Logging (`logging`)

**Location**: `src/ws/logging/`

Structured logging framework with configurable levels and output destinations.

**Key Components**:
- `log_level.h`: Hierarchical logging level system
- Configurable output formatters and destinations
- Performance-optimized logging macros

**Purpose**: Provides comprehensive logging capabilities for debugging, monitoring, and audit trails in production applications.

### Pooling (`pooling`)

**Location**: `src/ws/pooling/`

Object pooling implementations for memory management optimization.

**Key Components**:
- `blocking_object_pool.h`: Thread-safe object pool with blocking acquisition

**Purpose**: Reduces memory allocation overhead and improves performance through efficient object reuse patterns.

### Status (`status`)

**Location**: `src/ws/status/`

Error handling and status reporting framework inspired by modern error handling patterns. Built on [Abseil](https://github.com/abseil/abseil-cpp) status utilities.

**Key Components**:
- `status_code.h`: Comprehensive status code enumeration
- `status_or.h`: Result type combining values with error status (Abseil-based)

**Purpose**: Provides robust error handling mechanisms that encourage explicit error checking and propagation.

### String (`string`)

**Location**: `src/ws/string/`

String manipulation and formatting utilities.

**Key Components**:
- `format.h`: Type-safe string formatting utilities

**Purpose**: Offers safe and efficient string operations with modern formatting capabilities.

### Threading (`threading`)

**Location**: `src/ws/threading/`

High-level threading abstractions and task management.

**Key Components**:
- `cancellation_token_source.h`: Cooperative cancellation mechanisms
- Thread pool implementations
- Task scheduling utilities

**Purpose**: Simplifies multi-threaded programming with higher-level abstractions for task management and coordination.

## Building

### Prerequisites

- CMake 3.15 or higher
- C++17 compatible compiler
- Platform-specific build tools (Visual Studio on Windows, GCC/Clang on Unix)

### Build Options

The library supports selective module building through CMake options:

```bash
# Build all modules (default)
cmake -B build

# Build specific modules
cmake -B build -DWSCOMMON_BUILD_IMAGING=ON -DWSCOMMON_BUILD_IO=ON

# Build as shared libraries
cmake -B build -DWSCOMMON_BUILD_DLL=ON
```

### Quick Start

```bash
git clone https://github.com/WildSentinelLabs/wildsentinel-common.git
cd wildsentinel-common
cmake -B build
cmake --build build
```

## Integration

### CMake Integration

#### Option 1: Using find_package (Installed Package)

```cmake
find_package(WsCommon REQUIRED)
target_link_libraries(your_target PRIVATE WsCommon::core WsCommon::imaging)
```

#### Option 2: Using FetchContent (Recommended)

```cmake
include(FetchContent)

FetchContent_Declare(
  wildsentinel-common
  GIT_REPOSITORY https://github.com/WildSentinelLabs/wildsentinel-common.git
  GIT_TAG        main  # or specific version tag
)

FetchContent_MakeAvailable(wildsentinel-common)

target_link_libraries(your_target PRIVATE WsCommon::core WsCommon::imaging)
```

### Usage Example

```cpp
#include "ws/imaging/image.h"
#include "ws/status/status_or.h"

using namespace ws;

auto result = imaging::Image::Create(std::move(components), width, height,
                                   ColorSpace::kRGB, ChromaSubsampling::k444);
if (result.ok()) {
    auto image = std::move(result).value();
    // Process image...
}
```

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.

### Third-Party Dependencies

This project incorporates several third-party libraries under their respective licenses:

- **Abseil LTS 20250512.1** - Apache 2.0 License
  - Used for: Core utilities, data structures, and status handling (see [`status`](#status-status) module)
  - Source: https://github.com/abseil/abseil-cpp

- **oneTBB 2022.2.0** - Apache 2.0 License
  - Used for: Threading, parallel algorithms, and non-blocking concurrent collections (see [`concurrency`](#concurrency-concurrency) module)
  - Source: https://github.com/uxlfoundation/oneTBB

For complete third-party license information, see [THIRD-PARTY-NOTICES](THIRD-PARTY-NOTICES).

### Common Clauses

This software is provided under the Apache 2.0 license with the Commons Clause License Condition v1.0:

**Copyright (c) 2024 WildSentinelLabs**

The Software is provided to you under the Apache 2.0 license as defined above, subject to the following condition:

Without limiting other conditions in the License, the grant of rights under the License will not include, and the License does not grant to you, the right to "Sell" the Software.

**FOR PURPOSES OF THE FOREGOING, "SELL" MEANS PRACTICING ANY OR ALL OF THE RIGHTS GRANTED TO YOU UNDER THE LICENSE TO PROVIDE TO THIRD PARTIES, FOR A FEE OR OTHER CONSIDERATION (INCLUDING WITHOUT LIMITATION FEES FOR HOSTING OR CONSULTING/SUPPORT SERVICES RELATED TO THE SOFTWARE).**

When using this library in your projects:
- Include proper attribution in your documentation
- Respect the Apache 2.0 license terms and Commons Clause restrictions
- Review third-party dependency licenses for compliance requirements
- Any license notice or attribution required by the license must also include the Commons Clause License Condition notice
- Consider contributing improvements back to the community

## Support

For questions, issues, or contributions, please visit our [GitHub repository](https://github.com/WildSentinelLabs/wildsentinel-common) or contact the WildSentinel Labs team.
