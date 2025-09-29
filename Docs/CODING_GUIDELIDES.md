# Coding Guidelines
This project follows these coding guidelines. To be specific, every C/C++ header/source file must be aligned with these rules.

> [!NOTE]
> If you're looking for an overview about contributing with this project, consider reading [our contributing guidelines](CONTRIBUTING.md) first. Your ideas are very welcome :\).

## C/C++ Headers
1. **Type Guards**: every headers must have type guard, however we don't allow `#pragma once` directive. Instead, write header files using the template below, where `<header_name>` is the header file name (without .h extension), and `<header_content>` contains all declarations.
```c
#ifndef _<header_name>_H_
#define _<header_name>_H_
<header_content>
#endif // _<header_name>_H_
```

2. **Define, don't implementate:** 