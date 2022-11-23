# Memory Scanner
![C++](https://img.shields.io/badge/language-c%2B%2B-blue?style=flat-square)
[![License](https://img.shields.io/badge/license-BSD--3--Clause-brightgreen?style=flat-square)](LICENSE.txt)

Small and simple memory scanner designed to work on Windows and to be used on applications that capture the mouse/cannot be paused (for example: games). To achieve this it uses [console-menu](https://github.com/BjoernBoss/console-menu), which allows to control the user interface in the console either as usual or via `CTRL + NumPad`. This scanner works entirely from outside of the binary to scan. It only reads the binary executable file and uses the system calls `OpenProcess`, `ReadProcessMemory`, and `WriteProcessMemory` on the running process. It does not inject source code or attach a debugger.

This Memory scanner supports the following datatypes:

    int8 / uint8        /* aligned to 1 byte */
    int16 / uint16      /* aligned to 2 bytes */
    int32 / uint32      /* aligned to 4 bytes */
    int64 / uint64      /* aligned to 8 bytes */
    float               /* aligned to 4 bytes */
    double              /* aligned to 8 bytes */
    bool                /* aligned to 1 byte */
    string              /* max length 64, either null terminated or not, aligned to 1 byte */
    float2D             /* structure consisting of two floats, aligned to 4 bytes */
    float3D             /* structure consisting of three floats, aligned to 4 bytes */

While scanning, the found values can be compared relative to their last value, or relative to some user input. Found values can be stored and overwritten. Overwriting can be done once or be toggled to happen multiple times per second.

This scanner works on 32 and 64 bit binaries. It can read either only the `static` memory, which is mapped into the data sections of the binary, or only the `volatile` memory, which consists of all other allocations made, or both. While scanning, it only reads pages, which are flagged with `PAGE_READWRITE` or `PAGE_EXECUTE_READWRITE`, have been committed (i.e. `MEM_COMMIT`), and are not protected through `PAGE_GUARD` or `PAGE_NOACCESS`. 

While this program handles most errors, it will not check for allocation failures on the scan-buffers. This means that large scans, which exceed the allocatable memory amount, will case a `Segmentation Fault`. This is due to this project starting as a little helping tool and thereby not having fully integrated and proper error handling.

## Building the project
This project uses `premake5` as its build system. Example on how to create a `Visual Studio 2019 Solution`.

    $ git clone --recursive https://github.com/BjoernBoss/memory-scanner.git
    $ cd memory-scanner
    $ premake5 vs2019

The solution file is located at `build/memory-scanner.sln`.

## Implementing new datatypes
New datatypes can easily be added by implementing the `Datatype` interface:

    /* restricted: is the type orderable (i.e. less than/greater than/...) */
    Datatype::Datatype(uint8_t size, uint8_t align, const std::string& name, bool restricted);

    virtual std::string Datatype::toString(const uint8_t* value) = 0;

    virtual bool readInput(uint8_t* value, bool operation) = 0;

	virtual bool validate(const uint8_t* value) = 0;
    
	virtual bool test(const uint8_t* value, const uint8_t* compareto, Operation operation) = 0;

Afterwards the type needs to be added to the array of registered types, which is initialized in `MenuInstance::init()`.

## Example of the options after a simple scan
    +--------------------------------- scan menu ----------------------------------+
    current process: Test Window
    overwrite: disabled
    Memory: static [module: 0x0000000000340000] (type: uint32) -> scan-size: 4
    +------------------------------------------------------------------------------+
    [00] - exit
    [01] - return
    [02] - root menu
    [03] - filter out fluctuations
    [04] - scan: print
    [05] - scan: restart
    [06] - scan: refresh
    [07] - scan: unchanged
    [08] - scan: changed
    [09] - scan: decreased
    [10] - scan: increased
    [11] - scan: equal to
    [12] - scan: unequal to
    [13] - scan: less than
    [14] - scan: less-equal than
    [15] - scan: greater-equal than
    [16] - scan: greater than
    +------------------------------------------------------------------------------+

    scan successful!

    select an option (CTRL + NUMPAD; abort: '+'; backspace: '-'):
