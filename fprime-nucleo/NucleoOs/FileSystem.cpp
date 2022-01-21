#include <config/FpConfig.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Os/FileSystem.hpp>
#include <Os/File.hpp>
#include <Fw/Types/Assert.hpp>

#include "app_filex.h"

#include <stdio.h> // Needed for rename
#include <string.h>
#include <limits>

//#define DEBUG_PRINT(x,...) printf(x,##__VA_ARGS__); fflush(stdout)
#define DEBUG_PRINT(x,...)

namespace Os {

    namespace FileSystem {

    // TODO: VERY IMPORTANT TO AVOID ERRONEOUS FILE OPERATIONS
    // NOTES: (1) For all local path operations this variable needs to be thread global, otherwise a media
    //            control block corruption will occur (e.g. FAT or sectors related errors)
    //
    //        (2) Because FileSystem is not an object class but a library of functions which will
    //            be used by several (active/threaded) objects and local path operations are meant
    //            to be thread local(safe), we will have to choose one of the following options:
    //
    //            1) Make this variable access thread safe by means of some mutex
    //            2) Change the FileSystem functions signature in order to accept a FX_LOCAL_PATH to be
    //               provided by the calling FPrime objects
    //            3) Transform the FileSystem into a class with a private member FX_LOCAL_PATH. With this
    //               FPrime objects would have to create their own FileSystem object with a unique and safe
    //               FX_LOCAL_PATH.
    //
    static FX_LOCAL_PATH threadLocalPath;

    /**
     * A helper function that returns an "OP_OK" status if the newly changed data was
     * successfully flushed into the media, otherwise returns an error status.
     */
    Status flushMedia() {

        Status fileStatus = OP_OK;

        // Fetch the media
        FX_MEDIA* mediaPtr;
        file_sys_media_get(&mediaPtr);

        UINT status = fx_media_flush(mediaPtr);

        if (FX_SUCCESS != status)
        {
            switch (status)
            {
                case FX_WRITE_PROTECT:
                case FX_MEDIA_NOT_OPEN:
                    fileStatus = NO_PERMISSION;
                    break;
                case FX_IO_ERROR:
                    fileStatus = NO_SPACE;
                    break;
                case FX_FILE_CORRUPT:
                case FX_SECTOR_INVALID:
                case FX_PTR_ERROR:
                case FX_CALLER_ERROR:
                    fileStatus = OTHER_ERROR;
                    break;
                default:
                    fileStatus = OTHER_ERROR;
                    break;
            }
            DEBUG_PRINT("Error 0x%.2x while flushing the storage media\n", status);
        }

        return fileStatus;
    }

        Status createDirectory(const char* path) {

            FW_ASSERT(path);

            Status stat = OP_OK;

            // Fetch the media where the folder will be created
            FX_MEDIA* mediaPtr;
            file_sys_media_get(&mediaPtr);

            // Create a new directory on the currently set default path or
            // path set on  "path"
            UINT status = fx_directory_create(mediaPtr, static_cast<char *>(const_cast<char *>(path)));

            if (FX_SUCCESS == status)
            {
                flushMedia();
            }
            else
            {
                switch (status)
                {
                    case FX_MEDIA_NOT_OPEN:
                        stat = NO_PERMISSION;
                        break;
                    case FX_INVALID_PATH:
                    case FX_NOT_FOUND:
                    case FX_MEDIA_INVALID:
                    case FX_NO_MORE_ENTRIES:
                        stat = INVALID_PATH;
                        break;
                    case FX_NOT_DIRECTORY:
                        stat = NOT_DIR;
                        break;
                    case FX_IO_ERROR:
                        stat = NO_SPACE;
                        break;
                    case FX_FILE_CORRUPT:
                    case FX_SECTOR_INVALID:
                    case FX_FAT_READ_ERROR:
                    case FX_NO_MORE_SPACE:
                    case FX_PTR_ERROR:
                    case FX_INVALID_ATTR:
                    case FX_CALLER_ERROR:
                        stat = OTHER_ERROR;
                        break;
                    default:
                        stat = OTHER_ERROR;
                        break;
                }
                DEBUG_PRINT("Error 0x%.2x during directory \"%s\" creation\n", status, static_cast<char *>(const_cast<char *>(path)));
            }

            return stat;
        } // end createDirectory

        Status removeDirectory(const char* path) {

            assert(path);

            Status stat = OP_OK;

            // Fetch the media where the folder will be created
            FX_MEDIA* mediaPtr;
            file_sys_media_get(&mediaPtr);

            UINT status = fx_directory_delete(mediaPtr, static_cast<char *>(const_cast<char *>(path)));

            if (FX_SUCCESS == status)
            {
                flushMedia();
            }
            else
            {
                switch (status)
                {
                    case FX_MEDIA_NOT_OPEN:
                    case FX_WRITE_PROTECT:
                    case FX_NO_MORE_ENTRIES:
                        stat = NO_PERMISSION;
                        break;
                    case FX_DIR_NOT_EMPTY:
                        stat = NOT_EMPTY;
                        break;
                    case FX_NOT_FOUND:
                    case FX_MEDIA_INVALID:
                        stat = INVALID_PATH;
                        break;
                    case FX_NOT_DIRECTORY:
                        stat = NOT_DIR;
                        break;
                    case FX_IO_ERROR:
                    case FX_FILE_CORRUPT:
                    case FX_SECTOR_INVALID:
                    case FX_FAT_READ_ERROR:
                    case FX_NO_MORE_SPACE:
                    case FX_PTR_ERROR:
                    case FX_CALLER_ERROR:
                        stat = OTHER_ERROR;
                        break;
                    default:
                        stat = OTHER_ERROR;
                        break;
                }
                DEBUG_PRINT("Error 0x%.2x during directory \"%s\" deletion\n", status, static_cast<char *>(const_cast<char *>(path)));
            }

            return stat;
        } // end removeDirectory

        Status readDirectory(const char* path, const U32 maxNum,
                             Fw::EightyCharString fileArray[],
                             U32& numFiles)
        {
            FW_ASSERT(fileArray != NULL);
            FW_ASSERT(path != NULL);

            Status dirStat = OP_OK;

            // Fetch the media where the directory is
            FX_MEDIA* mediaPtr = NULL;
            file_sys_media_get(&mediaPtr);

            // Get current local path
            CHAR currentLocalPathStr[FW_FIXED_LENGTH_STRING_SIZE];
            UINT fxStatus = fx_directory_local_path_get_copy(mediaPtr, currentLocalPathStr, FW_FIXED_LENGTH_STRING_SIZE);

            if (FX_SUCCESS == fxStatus)
            {
                // Perform a "cd directory" command
                fxStatus = fx_directory_local_path_set(mediaPtr, &threadLocalPath, static_cast<char *>(const_cast<char *>(path)));

                if (FX_SUCCESS != fxStatus)
                {
                    switch (fxStatus)
                    {
                        case FX_MEDIA_NOT_OPEN:
                            dirStat = NO_PERMISSION;
                            break;
                        case FX_INVALID_PATH:
                            dirStat = INVALID_PATH;
                            break;
                        case FX_PTR_ERROR:
                        case FX_NOT_IMPLEMENTED:
                            dirStat = OTHER_ERROR;
                            break;
                        default:
                            dirStat = OTHER_ERROR;
                            break;
                    }
                    DEBUG_PRINT("Error 0x%.2x while setting local directory to [\"%s\"]\n", fxStatus, static_cast<char *>(const_cast<char *>(path)));
                }
                else
                {
                    CHAR entryName[FW_FIXED_LENGTH_STRING_SIZE];

                    // Get first entry in the base directory
                    fxStatus =  fx_directory_first_entry_find(mediaPtr, entryName);

                    // Loop until FX_NO_MORE_ENTRIES or an error event occurred
                    U32 arrayIdx = 0;
                    while (FX_SUCCESS == fxStatus)
                    {
                        // Check if entry is a sub-directory
                        UINT testResult =  fx_directory_name_test(mediaPtr, entryName);

                        if (FX_NOT_DIRECTORY == testResult)
                        {
                            FW_ASSERT(arrayIdx < maxNum,
                                      static_cast<NATIVE_INT_TYPE>(arrayIdx),
                                      static_cast<NATIVE_INT_TYPE>(maxNum));

                            Fw::EightyCharString str(entryName);
                            fileArray[arrayIdx++] = str;

                            DEBUG_PRINT("Entry \"%s\" is a file\n", entryName);
                        }
                        else if (FX_SUCCESS == testResult)
                        {
                            // Entry is ".", ".." or any other sub-directory
                            DEBUG_PRINT("Entry \"%s\" is a sub-directory\n", entryName);
                        }
                        else
                        {
                            DEBUG_PRINT("Error 0x%.2x during entry [\"%s\"] test\n", fxStatus, entryName);
                            dirStat = OTHER_ERROR;
                            break;
                        }

                        // Fetch next entry
                        fxStatus =  fx_directory_next_entry_find(mediaPtr, entryName);
                    }

                    numFiles = arrayIdx;
                }

    //*********
    //TODO: If possible send a inquiry to Microsoft/ STMicroelectronics why fx_directory_local_path_restore is not working.
    //*********
                // When local a local path is set
                 if (strlen(currentLocalPathStr))
                 {
                     fxStatus =  fx_directory_local_path_set(mediaPtr, &threadLocalPath, currentLocalPathStr);

                     if (FX_SUCCESS != fxStatus)
                     {
                         DEBUG_PRINT("Error 0x%.2x when resetting the local path\n", fxStatus);
                         dirStat = OTHER_ERROR;
                     }
                 }
                 // When no local path is set the current local path to the default home directory
                 else
                 {
                     // Get current local path
                     CHAR defaultPathStr[FW_FIXED_LENGTH_STRING_SIZE];
                     UINT fxStatus = fx_directory_default_get_copy(mediaPtr, defaultPathStr, FW_FIXED_LENGTH_STRING_SIZE);

                     if (FX_SUCCESS == fxStatus)
                     {
                         fxStatus =  fx_directory_local_path_set(mediaPtr, &threadLocalPath, defaultPathStr);

                         if (FX_SUCCESS != fxStatus)
                         {
                             DEBUG_PRINT("Error 0x%.2x when resetting the local path\n", fxStatus);
                             dirStat = OTHER_ERROR;
                         }
                     }
                     else
                     {
                         DEBUG_PRINT("Error 0x%.2x when resetting the local path\n", fxStatus);
                         dirStat = OTHER_ERROR;
                     }
                 }
            }
            else
            {
                dirStat = OTHER_ERROR;
            }

            return dirStat;
        }

        Status removeFile(const char* path) {

            FW_ASSERT(path);

            Status stat = OP_OK;

            // Fetch the media where the folder will be created
            FX_MEDIA* mediaPtr;
            file_sys_media_get(&mediaPtr);

            UINT status = fx_file_delete(mediaPtr, static_cast<char *>(const_cast<char *>(path)));

            if (FX_SUCCESS == status)
            {
                flushMedia();
            }
            else
            {
                switch (status)
                {
                    case FX_NOT_FOUND:
                    case FX_MEDIA_INVALID:
                        stat = INVALID_PATH;
                        break;
                    case FX_MEDIA_NOT_OPEN:
                    case FX_WRITE_PROTECT:
                        stat = NO_PERMISSION;
                        break;
                    case FX_NOT_A_FILE:
                        stat = IS_DIR;
                        break;
                    case FX_ACCESS_ERROR:
                    case FX_FILE_CORRUPT:
                    case FX_SECTOR_INVALID:
                    case FX_FAT_READ_ERROR:
                    case FX_NO_MORE_ENTRIES:
                    case FX_NO_MORE_SPACE:
                    case FX_IO_ERROR:
                    case FX_PTR_ERROR:
                    case FX_CALLER_ERROR:
                        stat = OTHER_ERROR;
                        break;
                    default:
                        stat = OTHER_ERROR;
                        break;
                }
                DEBUG_PRINT("Error 0x%.2x on file\"%s\" removal\n", status, static_cast<char *>(const_cast<char *>(path)));
            }

            return stat;
        } // end removeFile

        Status moveDirectory(const char* originPath, const char* destPath) {

            FW_ASSERT(originPath);
            FW_ASSERT(destPath);

            Status stat = OP_OK;

            // Fetch the media where the folder will be created
            FX_MEDIA* mediaPtr;
            file_sys_media_get(&mediaPtr);

            UINT fxStatus = fx_directory_rename(mediaPtr, static_cast<char *>(const_cast<char *>(originPath)), static_cast<char *>(const_cast<char *>(destPath)));

            if (FX_SUCCESS == fxStatus)
            {
                flushMedia();
            }
            else
            {
                switch (fxStatus)
                {
                    case FX_MEDIA_NOT_OPEN:
                    case FX_WRITE_PROTECT:
                        stat = NO_PERMISSION;
                        break;
                    case FX_NO_MORE_SPACE:
                        stat = NO_SPACE;
                        break;
                    case FX_INVALID_PATH:
                        stat = INVALID_PATH;
                        break;
                    case FX_NOT_DIRECTORY:
                        stat = NOT_DIR;
                        break;
                    case FX_NO_MORE_ENTRIES:
                        stat = FILE_LIMIT;
                        break;
                    case FX_NOT_FOUND:
                    case FX_INVALID_NAME:
                    case FX_IO_ERROR:
                    case FX_FILE_CORRUPT:
                    case FX_SECTOR_INVALID:
                    case FX_FAT_READ_ERROR:
                    case FX_MEDIA_INVALID:
                    case FX_ALREADY_CREATED:
                    case FX_PTR_ERROR:
                    case FX_CALLER_ERROR:
                        stat = OTHER_ERROR;
                        break;
                    default:
                        stat = OTHER_ERROR;
                        break;
                }
                DEBUG_PRINT("Error 0x%.2x moving directory \"%s\" to directory \"%s\"\n", fxStatus, static_cast<char *>(const_cast<char *>(originPath)),static_cast<char *>(const_cast<char *>(destPath)));
            }

            return stat;
        }

        Status moveFile(const char* originPath, const char* destPath) {

            FW_ASSERT(originPath);
            FW_ASSERT(destPath);

            Status stat = OP_OK;

            // Fetch the media where the folder will be created
            FX_MEDIA* mediaPtr;
            file_sys_media_get(&mediaPtr);

            UINT fxStatus = fx_file_rename(mediaPtr, static_cast<char *>(const_cast<char *>(originPath)), static_cast<char *>(const_cast<char *>(destPath)));

            if (FX_SUCCESS == fxStatus)
            {
                flushMedia();
            }
            else
            {
                switch (fxStatus)
                {
                    case FX_MEDIA_NOT_OPEN:
                    case FX_ACCESS_ERROR:
                    case FX_WRITE_PROTECT:
                        stat = NO_PERMISSION;
                        break;
                    case FX_NO_MORE_SPACE:
                        stat = NO_SPACE;
                        break;
                    case FX_INVALID_PATH:
                        stat = INVALID_PATH;
                        break;
                    case FX_INVALID_NAME:
                    case FX_NOT_A_FILE:
                        stat = NOT_DIR;
                        break;
                    case FX_NO_MORE_ENTRIES:
                        stat = FILE_LIMIT;
                        break;
                    case FX_NOT_FOUND:
                    case FX_IO_ERROR:
                    case FX_ALREADY_CREATED:
                    case FX_MEDIA_INVALID:
                    case FX_FILE_CORRUPT:
                    case FX_SECTOR_INVALID:
                    case FX_FAT_READ_ERROR:
                    case FX_PTR_ERROR:
                    case FX_CALLER_ERROR:
                        stat = OTHER_ERROR;
                        break;
                    default:
                        stat = OTHER_ERROR;
                        break;
                }
                DEBUG_PRINT("Error 0x%.2x moving file \"%s\" to file \"%s\"\n", fxStatus, static_cast<char *>(const_cast<char *>(originPath)),static_cast<char *>(const_cast<char *>(destPath)));
            }

            return stat;

        } // end moveFile

        Status handleFileError(File::Status fileStatus) {
            Status fileSystemStatus = OTHER_ERROR;

            switch(fileStatus) {
                case File::NO_SPACE:
                    fileSystemStatus = NO_SPACE;
                    break;
                case File::NO_PERMISSION:
                    fileSystemStatus = NO_PERMISSION;
                    break;
                case File::DOESNT_EXIST:
                    fileSystemStatus = INVALID_PATH;
                    break;
                default:
                    fileSystemStatus = OTHER_ERROR;
            }
            return fileSystemStatus;
        } // end handleFileError

        /**
         * A helper function that returns an "OP_OK" status if the given file
         * exists and can be read from, otherwise returns an error status.
         */
        Status initAndCheckFileStats(const char* filePath,
                                     ULONG* size = NULL) {

            FW_ASSERT(filePath);
            Status fileStatus = OP_OK;

            // Fetch the media where the folder will be created
            FX_MEDIA* mediaPtr;
            file_sys_media_get(&mediaPtr);

            // Retrieve information about a directory entry
            // The maximum entry size is limited to a ULONG or 4GB
            UINT entryAttributes;
            ULONG entrySize;

            UINT status = fx_directory_information_get(mediaPtr, static_cast<char *>(const_cast<char *>(filePath)),
                                                       &entryAttributes,
                                                       &entrySize,
                                                       NULL, NULL, NULL, NULL, NULL, NULL);

            if (FX_SUCCESS != status)
            {
                switch (status)
                {
                    case FX_NOT_FOUND:
                        fileStatus = INVALID_PATH;
                        break;
                    case FX_NO_MORE_SPACE:
                        fileStatus = NO_SPACE;
                        break;
                    case FX_MEDIA_NOT_OPEN:
                    case FX_IO_ERROR:
                    case FX_MEDIA_INVALID:
                    case FX_FILE_CORRUPT:
                    case FX_FAT_READ_ERROR:
                    case FX_SECTOR_INVALID:
                    case FX_PTR_ERROR:
                    case FX_CALLER_ERROR:
                        fileStatus = OTHER_ERROR;
                        break;
                    default:
                        fileStatus = OTHER_ERROR;
                        break;
                }
                DEBUG_PRINT("Error 0x%.2x while getting entry \"%s\" info\n", status, static_cast<char *>(const_cast<char *>(filePath)));
            }
            else
            {
                if (entryAttributes & FX_READ_ONLY)
                {
                    fileStatus = NO_PERMISSION;
                }

                if (entryAttributes & FX_DIRECTORY)
                {
                    fileStatus = IS_DIR;
                }

                if (size)
                {
                    *size = entrySize;
                }
            }

            return fileStatus;
        }

        /**
         * A helper function that writes all the file information in the source
         * file to the destination file (replaces/appends to end/etc. depending
         * on destination file mode).
         *
         * Files must already be open and will remain open after this function
         * completes.
         *
         * @param source File to copy data from
         * @param destination File to copy data to
         * @param size The number of bytes to copy
         */
        Status copyFileData(File& source, File& destination, U64 size) {
            U8 fileBuffer[FILE_SYSTEM_CHUNK_SIZE];
            File::Status file_status;

            // Set loop limit
            const U64 copyLoopLimit = (((U64)size/FILE_SYSTEM_CHUNK_SIZE)) + 2;

            U64 loopCounter = 0;
            NATIVE_INT_TYPE chunkSize;
            while(loopCounter < copyLoopLimit) {
                chunkSize = FILE_SYSTEM_CHUNK_SIZE;
                file_status = source.read(&fileBuffer, chunkSize, false);
                if(file_status != File::OP_OK) {
                    return handleFileError(file_status);
                }

                if(chunkSize == 0) {
                    //file has been successfully copied
                    break;
                }

                file_status = destination.write(fileBuffer, chunkSize, true);
                if(file_status != File::OP_OK) {
                    return handleFileError(file_status);
                }
                loopCounter++;
            }
            FW_ASSERT(loopCounter < copyLoopLimit);

            return FileSystem::OP_OK;
        } // end copyFileData

        Status copyFile(const char* originPath, const char* destPath) {
            FileSystem::Status fs_status;
            File::Status file_status;

            U64 fileSize = 0;

            File source;
            File destination;

            fs_status = initAndCheckFileStats(originPath);
            if(FileSystem::OP_OK != fs_status) {
                return fs_status;
            }

            // Get the file size:
            fs_status = FileSystem::getFileSize(originPath, fileSize); //!< gets the size of the file (in bytes) at location path
            if(FileSystem::OP_OK != fs_status) {
                return fs_status;
            }

            file_status = source.open(originPath, File::OPEN_READ);
            if(file_status != File::OP_OK) {
                return handleFileError(file_status);
            }

            file_status = destination.open(destPath, File::OPEN_WRITE);
            if(file_status != File::OP_OK) {
                return handleFileError(file_status);
            }

            fs_status = copyFileData(source, destination, fileSize);

            (void) source.close();
            (void) destination.close();

            fs_status = flushMedia();

            return fs_status;
        } // end copyFile

        Status appendFile(const char* originPath, const char* destPath, bool createMissingDest) {
            FileSystem::Status fs_status;
            File::Status file_status;
            U64 fileSize = 0;

            File source;
            File destination;

            fs_status = initAndCheckFileStats(originPath);
            if(FileSystem::OP_OK != fs_status) {
                return fs_status;
            }

            // Get the file size:
            fs_status = FileSystem::getFileSize(originPath, fileSize); //!< gets the size of the file (in bytes) at location path
            if(FileSystem::OP_OK != fs_status) {
                return fs_status;
            }

            file_status = source.open(originPath, File::OPEN_READ);
            if(file_status != File::OP_OK) {
                return handleFileError(file_status);
            }

            // If needed, check if destination file exists (and exit if not)
            if(!createMissingDest) {
                fs_status = initAndCheckFileStats(destPath);
                if(FileSystem::OP_OK != fs_status) {
                    return fs_status;
                }
            }

            file_status = destination.open(destPath, File::OPEN_APPEND);
            if(file_status != File::OP_OK) {
                return handleFileError(file_status);
            }

            fs_status = copyFileData(source, destination, fileSize);

            (void) source.close();
            (void) destination.close();

            fs_status = flushMedia();

            return fs_status;
        } // end appendFile

        Status getFileSize(const char* path, U64& size) {

            FW_ASSERT(path);
            Status fileStat = OP_OK;
            ULONG entrySize;

            fileStat = initAndCheckFileStats(path, &entrySize);

            if(FileSystem::OP_OK == fileStat) {
                // Only check size on success
                size = entrySize;
            }

            return fileStat;
        } // end getFileSize

        Status changeWorkingDirectory(const char* path) {

            Status dirStat = OP_OK;

            // Fetch the media where the directory is
            FX_MEDIA* mediaPtr = NULL;
            file_sys_media_get(&mediaPtr);

            // Perform a "cd directory" command
            UINT fxStatus = fx_directory_local_path_set(mediaPtr, &threadLocalPath, static_cast<char *>(const_cast<char *>(path)));

            if (FX_SUCCESS != fxStatus)
            {
                switch (fxStatus)
                {
                    case FX_MEDIA_NOT_OPEN:
                        dirStat = NO_PERMISSION;
                        break;
                    case FX_INVALID_PATH:
                        dirStat = INVALID_PATH;
                        break;
                    case FX_PTR_ERROR:
                    case FX_NOT_IMPLEMENTED:
                        dirStat = OTHER_ERROR;
                        break;
                    default:
                        dirStat = OTHER_ERROR;
                        break;
                }
                DEBUG_PRINT("Error 0x%.2x while setting local directory to [\"%s\"]\n", fxStatus, static_cast<char *>(const_cast<char *>(path)));
            }

            return dirStat;
        } // end changeWorkingDirectory

        Status getFreeSpace(const char* path, U64& totalBytes, U64& freeBytes) {

            Status dirStat = OP_OK;
            ULONG localFreeBytes = 0;

            // Fetch the media where the directory is
            FX_MEDIA* mediaPtr = NULL;
            file_sys_media_get(&mediaPtr);


            UINT fxStatus = fx_media_space_available(mediaPtr, &localFreeBytes);

            if (FX_SUCCESS != fxStatus)
            {
                switch (fxStatus)
                {
                    case FX_MEDIA_NOT_OPEN:
                        dirStat = NO_PERMISSION;
                        break;
                    case FX_INVALID_PATH:
                        dirStat = INVALID_PATH;
                        break;
                    case FX_PTR_ERROR:
                    case FX_CALLER_ERROR:
                        dirStat = OTHER_ERROR;
                        break;
                    default:
                        dirStat = OTHER_ERROR;
                        break;
                }
                DEBUG_PRINT("Error 0x%.2x when getting media space containing [\"%s\"] path\n", fxStatus, static_cast<char *>(const_cast<char *>(path)));
            }
            else
            {
                // Total size must be calculated from the initial disk size
                totalBytes = 0;
                freeBytes  = localFreeBytes;
            }

            return dirStat;
        }

        // Public function to get the file count for a given directory.
        Status getFileCount (const char* directory, U32& fileCount) {

            FW_ASSERT(directory);
            fileCount = 0;

            Status dirStat = OP_OK;

            // Fetch the media where the directory is
            FX_MEDIA* mediaPtr = NULL;
            file_sys_media_get(&mediaPtr);

            // Get current local path
            CHAR currentLocalPathStr[FW_FIXED_LENGTH_STRING_SIZE];
            UINT fxStatus = fx_directory_local_path_get_copy(mediaPtr, currentLocalPathStr, FW_FIXED_LENGTH_STRING_SIZE);

            if (FX_SUCCESS == fxStatus)
            {
                // Perform a "cd directory" command
                UINT fxStatus = fx_directory_local_path_set(mediaPtr, &threadLocalPath, static_cast<char *>(const_cast<char *>(directory)));

                if (FX_SUCCESS != fxStatus)
                {
                    switch (fxStatus)
                    {
                        case FX_MEDIA_NOT_OPEN:
                            dirStat = NO_PERMISSION;
                            break;
                        case FX_INVALID_PATH:
                            dirStat = INVALID_PATH;
                            break;
                        case FX_PTR_ERROR:
                        case FX_NOT_IMPLEMENTED:
                            dirStat = OTHER_ERROR;
                            break;
                        default:
                            dirStat = OTHER_ERROR;
                            break;
                    }
                    DEBUG_PRINT("Error 0x%.2x while setting local directory to [\"%s\"]\n", fxStatus, static_cast<char *>(const_cast<char *>(directory)));
                }
                else
                {
                    CHAR* entryName = NULL;

                    // Get first entry in the base directory
                    fxStatus =  fx_directory_first_entry_find(mediaPtr, entryName);

                    // Loop until FX_NO_MORE_ENTRIES or an error event occurred
                    while (FX_SUCCESS == fxStatus)
                    {
                        // Check if entry is a sub-directory
                        UINT testResult =  fx_directory_name_test(mediaPtr, entryName);

                        if (FX_NOT_DIRECTORY == testResult)
                        {
                            // Entry is a file
                            fileCount++;
                            DEBUG_PRINT("Entry \"%s\" is a file\n", entryName);
                        }
                        else if (FX_SUCCESS == testResult)
                        {
                            // Entry is ".", ".." or any other sub-directory
                            DEBUG_PRINT("Entry \"%s\" is a sub-directory\n", entryName);
                        }
                        else
                        {
                            DEBUG_PRINT("Error 0x%.2x during entry [\"%s\"] test\n", fxStatus, entryName);
                            dirStat = OTHER_ERROR;
                            break;
                        }

                        // Fetch next entry
                        fxStatus =  fx_directory_next_entry_find(mediaPtr, entryName);
                    }
                }
    //*********
    //TODO: If possible send a inquiry to Microsoft why fx_directory_local_path_restore is not working.
    //*********
                // When local a local path is set
                if (strlen(currentLocalPathStr))
                {
                    fxStatus =  fx_directory_local_path_set(mediaPtr, &threadLocalPath, currentLocalPathStr);

                    if (FX_SUCCESS != fxStatus)
                    {
                        DEBUG_PRINT("Error 0x%.2x when resetting the local path\n", fxStatus);
                        dirStat = OTHER_ERROR;
                    }
                }
                // When no local path is set the current local path to the default home directory
                else
                {
                    // Get current local path
                    CHAR defaultPathStr[FW_FIXED_LENGTH_STRING_SIZE];
                    UINT fxStatus = fx_directory_default_get_copy(mediaPtr, defaultPathStr, FW_FIXED_LENGTH_STRING_SIZE);

                    if (FX_SUCCESS == fxStatus)
                    {
                        fxStatus =  fx_directory_local_path_set(mediaPtr, &threadLocalPath, defaultPathStr);

                        if (FX_SUCCESS != fxStatus)
                        {
                            DEBUG_PRINT("Error 0x%.2x when resetting the local path\n", fxStatus);
                            dirStat = OTHER_ERROR;
                        }
                    }
                    else
                    {
                        DEBUG_PRINT("Error 0x%.2x when resetting the local path\n", fxStatus);
                        dirStat = OTHER_ERROR;
                    }
                }
            }
            else
            {
                dirStat = OTHER_ERROR;
            }

            return dirStat;
        } //end getFileCount

    } // end FileSystem namespace

} // end Os namespace
