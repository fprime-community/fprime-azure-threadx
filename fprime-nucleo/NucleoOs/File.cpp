#include <FpConfig.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Os/File.hpp>
#include <Fw/Types/Assert.hpp>

#include <fx_api.h>
#include "app_azure_rtos.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <Utils/Hash/libcrc/lib_crc.h> // borrow CRC

#ifdef __cplusplus
}
#endif // __cplusplus

#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits>
#include <string.h>
#include <stdio.h>

//#define DEBUG_PRINT(x,...) printf(x,##__VA_ARGS__); fflush(stdout)
#define DEBUG_PRINT(x,...)

// Type used to piggyback on class member "m_fd" all required data to
// operate a File object without the need to add extra members in File.hpp
typedef struct fileHandleDataTag
{
    FX_FILE m_fd;      // Stored file descriptor
    FX_MEDIA* m_media; // Media reference used by the FS
} fileHandleData_t;


namespace Os {

    // Processes operations with O_WRONLY flags set
    UINT process_open_write_request(FX_MEDIA* media, FX_FILE* file, char* fileName, File::Mode mode, bool include_excl = false);

    // Verify if a file is opened
    bool is_file_open(FX_FILE* fd);

    File::File() :m_fd(0),m_mode(OPEN_NO_MODE),m_lastError(0)
    {
#ifdef __SIZEOF_POINTER__
        FW_ASSERT( !(sizeof(this->m_fd) < __SIZEOF_POINTER__) );
#else
  #error Unable to verify a pointer size!
#endif

        // ThreadX memory pool pointer
        TX_BYTE_POOL* bytePoolPtr;

        if(app_tx_get_byte_pool(&bytePoolPtr) == TX_SUCCESS)
        {
            // Allocate the memory for auxiliary watchdog data handling structure
            if (tx_byte_allocate(bytePoolPtr, (VOID **)&this->m_fd, sizeof(fileHandleData_t), TX_NO_WAIT) == TX_SUCCESS)
            {
                file_sys_media_get(&reinterpret_cast<fileHandleData_t*>(this->m_fd)->m_media);
                // TODO: Currently only one media is available. Need to be able to open any media.
            }
            else
            {
                reinterpret_cast<fileHandleData_t*>(this->m_fd)->m_media = nullptr;
                this->m_fd = 0;

                // TODO: Log memory allocation error
            }
        }
    }

    File::~File() {
        if (this->m_mode != OPEN_NO_MODE) {
            this->close();
        }
        // If file auxiliary structure exists, delete it
        if (this->m_fd)
        {
            // Release auxiliary handle allocated memory
            UINT ret = tx_byte_release((VOID *)this->m_fd);
            // TODO: if ret != TX_SUCCESS log a memory leak
        }
    }

    File::Status File::open(const char* fileName, File::Mode mode) {
      return this->open(fileName, mode, true);
    }
    File::Status File::open(const char* fileName, File::Mode mode, bool include_excl) {

        FW_ASSERT(fileName);

        Status stat = OP_OK;
        UINT opStatus = FX_SUCCESS;

        CHAR* file = static_cast<char*>(const_cast<char*>(fileName));
        FX_FILE* fd     = reinterpret_cast<FX_FILE*>(this->m_fd);
        FX_MEDIA* media = reinterpret_cast<fileHandleData_t*>(this->m_fd)->m_media;

        if ((media == nullptr) || (fd == 0))
        {
        	stat = OTHER_ERROR;
        }
        else
        {
            switch (mode) {
                case OPEN_READ:
                    // flags = O_RDONLY;
                    opStatus = fx_file_open(media, fd, file, FX_OPEN_FOR_READ);
                    break;
                case OPEN_WRITE:
                    // flags = O_WRONLY | O_CREAT;
                    opStatus = process_open_write_request(media, fd, file, OPEN_WRITE);
                    break;
                case OPEN_SYNC_WRITE:
                case OPEN_SYNC_DIRECT_WRITE:
                    // Not implemented on ThreadX
                    stat = OTHER_ERROR;
                    break;
                case OPEN_CREATE:
                    // flags = O_WRONLY | O_CREAT | O_TRUNC;
                    opStatus = process_open_write_request(media, fd, file, OPEN_CREATE, include_excl);
                    break;
                case OPEN_APPEND:
                    // flags = O_WRONLY | O_CREAT | O_APPEND;
                    opStatus = process_open_write_request(media, fd, file, OPEN_APPEND);
                    break;
                default:
                    FW_ASSERT(0,(NATIVE_INT_TYPE)mode);
                    break;
            }

            if (FX_SUCCESS != opStatus)
            {
                this->m_lastError = opStatus;
                switch (opStatus)
                {
                    case FX_NO_MORE_SPACE:
                        stat = NO_SPACE;
                        break;
                    case FX_NOT_FOUND:
                        stat = DOESNT_EXIST;
                        break;
                    case FX_WRITE_PROTECT:
                    case FX_ACCESS_ERROR:
                        stat = NO_PERMISSION;
                        break;
                    case FX_ALREADY_CREATED:
                        stat = FILE_EXISTS;
                        break;
                    case FX_MEDIA_NOT_OPEN:
                    case FX_NOT_A_FILE:
                    case FX_FILE_CORRUPT:
                    case FX_FAT_READ_ERROR:
                    case FX_PTR_ERROR:
                    case FX_CALLER_ERROR:
                    case FX_SECTOR_INVALID:
                    case FX_NO_MORE_ENTRIES:
                    case FX_IO_ERROR:
                    case FX_INVALID_PATH:
                    case FX_INVALID_NAME:
                    case FX_MEDIA_INVALID:
                    case FX_NOT_OPEN:
                        stat = OTHER_ERROR;
                        break;
                    default:
                        stat = OTHER_ERROR;
                        break;
                }

                DEBUG_PRINT("Error 0x%.2x during file \"%s\" open\n", opStatus, fileName);
            }

            this->m_mode = mode;
        }

        return stat;
    }

    //TODO: check if this function works. Cannot see it being used anywhere
    bool File::isOpen(void) {
        // Check if file is already opened using a 0 bytes read operation
        return is_file_open((reinterpret_cast<FX_FILE*>(this->m_fd)));
    }

    File::Status File::prealloc(NATIVE_INT_TYPE offset, NATIVE_INT_TYPE len) {
        (VOID)offset;
        (VOID)len;

        // make sure it has been opened
        if (OPEN_NO_MODE == this->m_mode) {
            return NOT_OPENED;
        }

        // Only implemented on Linux
        File::Status fileStatus = NO_SPACE;

        return fileStatus;
    }

    File::Status File::seek(NATIVE_INT_TYPE offset, bool absolute) {

        // make sure it has been opened
        if (OPEN_NO_MODE == this->m_mode) {
            return NOT_OPENED;
        }

        Status stat = OP_OK;

        NATIVE_INT_TYPE whence = absolute?FX_SEEK_BEGIN:FX_SEEK_FORWARD;

        UINT opStatus = fx_file_relative_seek(reinterpret_cast<FX_FILE*>(this->m_fd),offset,whence);

        // No error would be a normal one for this simple
        // class, so return other error
        if (FX_SUCCESS != opStatus)
        {
            this->m_lastError = opStatus;
            switch (opStatus)
            {
                case FX_SECTOR_INVALID:
                    stat = BAD_SIZE;
                    break;
                case FX_NOT_OPEN:
                case FX_IO_ERROR:
                case FX_FILE_CORRUPT:
                case FX_NO_MORE_ENTRIES:
                case FX_PTR_ERROR:
                case FX_CALLER_ERROR:
                    stat = OTHER_ERROR;
                    break;
                default:
                    stat = OTHER_ERROR;
                    break;
            }
        }

        return stat;
    }

    File::Status File::read(void * buffer, NATIVE_INT_TYPE &size, bool waitForFull) {

        FW_ASSERT(buffer);

        // make sure it has been opened
        if (OPEN_NO_MODE == this->m_mode) {
            size = 0;
            return NOT_OPENED;
        }
        // Validate read size before entering reading loop. Linux's read call expects size_t, which
        // is defined as an unsigned value. Thus 0 and negative values rejected.
        if (size <= 0) {
            size = 0;
            return BAD_SIZE;
        }

        ULONG accSize = 0; // accumulated size

        Status stat = OP_OK;

        NATIVE_INT_TYPE maxIters = size*2; // loop limit; couldn't block more times than number of bytes

        while (maxIters > 0) {

            ULONG readSize;

            UINT status = fx_file_read(reinterpret_cast<FX_FILE*>(this->m_fd), static_cast<UCHAR*>(buffer), (ULONG)size-accSize, &readSize);

            if (readSize != (ULONG)(size)-accSize) {
                // could be an error || end of file
                if (FX_SUCCESS != status) {

                    // end of file
                    if (FX_END_OF_FILE == status) {
                        break;
                    }

                    switch (status) {
                        case FX_NOT_OPEN:
                        case FX_FILE_CORRUPT:
                        case FX_NO_MORE_SPACE:
                        case FX_PTR_ERROR:
                        case FX_CALLER_ERROR:
                            DEBUG_PRINT("Error 0x%.2x during read of 0x%.8x, addrMod %PRId64, size %d, sizeMod %d\n",
                                        status, (U64) buffer, ((U64) buffer) % 512, size, size % 512);
                            stat = OTHER_ERROR;
                            break;
                        default:
                            stat = OTHER_ERROR;
                            break;
                    }
                    this->m_lastError = status;
                    accSize = 0;
                    break; // break out of while loop
                } else { // partial read so adjust read point and size
                    accSize += readSize;
                    if (not waitForFull) {
                        break; // break out of while loop
                    } else {
                        // in order to move the pointer ahead, we need to cast it
                        U8* charPtr = (U8*)buffer;
                        charPtr = &charPtr[readSize];
                        buffer = (void*)charPtr;
                    }
                    maxIters--; // decrement loop count
                }

            } else { // got number we wanted
                accSize += readSize;
                break; // break out of while loop
            }

            maxIters--; // decrement loop count

        } // end read while loop

        // make sure we didn't exceed loop count
        FW_ASSERT(maxIters > 0);

        size = accSize;

        return stat;
    }

    File::Status File::write(const void * buffer, NATIVE_INT_TYPE &size, bool waitForDone) {

        // Parameter used only on Linux
        (VOID)waitForDone;

        // make sure it has been opened
        if (OPEN_NO_MODE == this->m_mode) {
            size = 0;
            return NOT_OPENED;
        }
        // Validate write size before entering writing loop. Linux's read call expects size_t, which
        // is defined as an unsigned value. Thus 0 and negative values rejected.
        if (size <= 0) {
            size = 0;
            return BAD_SIZE;
        }

        Status stat = OP_OK;

        UINT status = fx_file_write(reinterpret_cast<FX_FILE*>(this->m_fd), static_cast<VOID*>(const_cast<VOID*>(buffer)), (ULONG)size);

        if (FX_SUCCESS != status)
        {
            switch (status)
            {
                case FX_NO_MORE_SPACE:
                    stat = NO_SPACE;
                    break;
                case FX_NOT_OPEN:
                case FX_ACCESS_ERROR:
                case FX_IO_ERROR:
                case FX_WRITE_PROTECT:
                case FX_FILE_CORRUPT:
                case FX_SECTOR_INVALID:
                case FX_FAT_READ_ERROR:
                case FX_NO_MORE_ENTRIES:
                case FX_PTR_ERROR:
                case FX_CALLER_ERROR:
                    DEBUG_PRINT("Error 0x%.2x during write of 0x0%llx, addrMod %d, size %d, sizeMod %d\n",
                                status, (U64) buffer, ((U64) buffer) % 512, size, size % 512);
                    stat = OTHER_ERROR;
                    break;
                default:
                    stat = OTHER_ERROR;
                    break;
            }
            this->m_lastError = status;
        }

        return stat;
    }

    // NOTE(mereweth) - see http://lkml.iu.edu/hypermail/linux/kernel/1005.2/01845.html
    // recommendation from Linus Torvalds, but doesn't seem to be that fast
    File::Status File::bulkWrite(const void * buffer, NATIVE_UINT_TYPE &totalSize,
                                 NATIVE_INT_TYPE chunkSize) {

        // make sure it has been opened
        if (OPEN_NO_MODE == this->m_mode) {
            totalSize = 0;
            return NOT_OPENED;
        }
        // Validate read size before entering reading loop. Linux's read call expects size_t, which
        // is defined as an unsigned value. Thus 0 and negative values rejected.
        if (totalSize == 0) {
            totalSize = 0;
            return BAD_SIZE;
        }
        else if (chunkSize <= 0) {
            totalSize = 0;
            return BAD_SIZE;
        }

        NATIVE_UINT_TYPE newBytesWritten = 0;

        for (NATIVE_UINT_TYPE idx = 0; idx < totalSize; idx += chunkSize) {
            NATIVE_INT_TYPE size = chunkSize;
            // if we're on the last iteration and length isn't a multiple of chunkSize
            if (idx + chunkSize > totalSize) {
                size = totalSize - idx;
            }
            const NATIVE_INT_TYPE toWrite = size;
            FW_ASSERT(idx + size <= totalSize, idx + size);
            const Os::File::Status fileStatus = this->write((U8*) buffer + idx, size, false);
            if (!(fileStatus == Os::File::OP_OK
                  && size == static_cast<NATIVE_INT_TYPE>(toWrite))) {
                totalSize = newBytesWritten;
                return fileStatus;
            }

            newBytesWritten += toWrite;
        }

        totalSize = newBytesWritten;
        return OP_OK;
    }

    File::Status File::flush() {

        // make sure it has been opened
        if (OPEN_NO_MODE == this->m_mode) {
            return NOT_OPENED;
        }

        File::Status stat = OP_OK;

        // This service flushes all cached sectors and directory entries of any
        // modified files to the physical media (ThreadX  does not support the flush
        // operation on a single file)
        UINT status = fx_media_flush(reinterpret_cast<fileHandleData_t*>(this->m_fd)->m_media);

        if (FX_SUCCESS != status) {
            switch (status) {
                case FX_MEDIA_NOT_OPEN:
                case FX_FILE_CORRUPT:
                case FX_SECTOR_INVALID:
                case FX_IO_ERROR:
                case FX_WRITE_PROTECT:
                case FX_PTR_ERROR:
                case FX_CALLER_ERROR:
                    DEBUG_PRINT("Error 0x%.2x during flush operation\n");
                    stat = OTHER_ERROR;
                    break;
                default:
                    stat = OTHER_ERROR;
                    break;
            }
        }

        return stat;
    }

    void File::close(void) {
        if (this->m_mode != OPEN_NO_MODE)
        {
            (void)::fx_file_close(reinterpret_cast<FX_FILE*>(this->m_fd));
        }
        this->m_mode = OPEN_NO_MODE;
        memset (reinterpret_cast<FX_FILE*>(this->m_fd),0,sizeof(FX_FILE));
    }

    NATIVE_INT_TYPE File::getLastError(void) {
        return this->m_lastError;
    }

    //TODO: Verify if "errno" has any meaning on ThreadX
    const char* File::getLastErrorString(void) {
        return "TODO: Map ThreadX error codes to strings";
    }

    File::Status File::calculateCRC32(U32 &crc)
    {

        // make sure it has been opened
        if (OPEN_NO_MODE == this->m_mode) {
            crc = 0;
            return NOT_OPENED;
        }

        const U32 maxChunkSize = 32;
        const U32 initialSeed = 0xFFFFFFFF;

        // Seek to beginning of file
        Status status = seek(0, true);
        if (status != OP_OK) {
            crc = 0;
            return status;
        }

        U8 file_buffer[maxChunkSize];

        bool endOfFile = false;

        U32 seed = initialSeed;
        const U32 maxIters = std::numeric_limits<U32>::max(); // loop limit
        U32 numIters = 0;

        while (!endOfFile && numIters < maxIters) {

            ++numIters;
            NATIVE_INT_TYPE chunkSize = maxChunkSize;

            status = read(file_buffer, chunkSize, false);
            if (status == OP_OK) {
                // chunkSize modified by file.read

                if (chunkSize == 0) {
                    endOfFile = true;
                    continue;
                }

                int chunkIdx = 0;

                while (chunkIdx < chunkSize) {
                  seed = update_crc_32(seed, file_buffer[chunkIdx]);
                  chunkIdx++;
                }

            } else {
                crc = 0;
                return status;
            }
        }

        if (!endOfFile) {
            crc = 0;
            return OTHER_ERROR;
        }
        else {
            crc = seed;
            return OP_OK;
        }
    }

    UINT process_open_write_request(FX_MEDIA* media, FX_FILE* file, char* fileName, File::Mode mode, bool include_excl)
    {
        UINT opStatus = FX_SUCCESS;
        UINT fileAttributes;

        // Get file attributes(when available)
        opStatus = fx_file_attributes_read(media, fileName, &fileAttributes);

        // File exists
        if (FX_SUCCESS == opStatus)
        {
            // flags |= O_EXCL
            // Check required on OPEN_CREATE mode
            if (include_excl)
            {
                opStatus = FX_ALREADY_CREATED;
            }
            // Check required on OPEN_WRITE and OPEN_CREATE modes
            else if (fileAttributes & FX_READ_ONLY)
            {
                opStatus = FX_READ_ONLY;
            }

            // TODO: CHECK if file is already open (if possible) and when open with data, if the file pointer is moved
            if (is_file_open(file))
            {
                opStatus = FX_ACCESS_ERROR;
            }
        }
        // File does not exist
        else if(FX_NOT_FOUND == opStatus)
        {
            // Create a new file "fileName" in the specified file in the default
            //  directory or in the directory path supplied with the file name
            opStatus = fx_file_create(media, fileName);
        }

        // On success open file on write mode
        if (FX_SUCCESS == opStatus)
        {
            opStatus = fx_file_open(media, file, fileName, FX_OPEN_FOR_WRITE);

            // Perform mode specific operations
            if (FX_SUCCESS == opStatus)
            {
                switch (mode)
                {
                    case File::Mode::OPEN_WRITE:
                        // Nothing else is required to be done
                        break;
                    case File::Mode::OPEN_CREATE:
                        // flags |= O_TRUNC
                        // Truncates the file to size 0 and releases its cluster(s)
                        opStatus = fx_file_truncate_release(file, 0);
                        break;
                    case File::Mode::OPEN_APPEND:
                        // flags |= O_APPEND
                        // Positions the internal file write pointer to a 0 byte relative offset from
                        // the end of the file
                        opStatus = fx_file_relative_seek(file, 0, FX_SEEK_END);
                        break;
                    default:
                        FW_ASSERT(0,(NATIVE_INT_TYPE)mode);
                        break;
                }
            }
        }

        return opStatus;
    }

    bool is_file_open(FX_FILE* fd)
    {
        UCHAR buffer[sizeof(ULONG)];
        ULONG actualBytes;

        // Check if file is already opened using a 0 bytes read operation
        return ((FX_NOT_OPEN == fx_file_read(fd, buffer, 0, &actualBytes)) ? false : true);
    }
}
