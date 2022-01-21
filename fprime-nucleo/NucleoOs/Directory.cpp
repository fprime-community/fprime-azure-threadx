#include <FpConfig.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Os/Directory.hpp>
#include <Fw/Types/Assert.hpp>

#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <string.h>

namespace Os {

    Directory::Directory() :m_dir(0),m_lastError(0) {
    }

    Directory::~Directory() {
        this->close();
    }

    Directory::Status Directory::open(const char* dirName) {

  	  (void)dirName;

    	// TODO: implement if required
        Status stat = OTHER_ERROR;;
        return stat;
    }

    Directory::Status Directory::rewind() {

    	// TODO: implement if required
        Status stat = OTHER_ERROR;
        return stat;
    }

    Directory::Status Directory::read(char * fileNameBuffer, U32 bufSize) {

	  FW_ASSERT(fileNameBuffer);
	  (void)bufSize;

      // TODO: implement if required
      return OTHER_ERROR;

    }

    Directory::Status Directory::read(char * fileNameBuffer, U32 bufSize, I64& inode) {

        FW_ASSERT(fileNameBuffer);
        (void)bufSize;
        (void)inode;

    	// TODO: implement if required
        Status stat = OTHER_ERROR;
        return stat;
    }

    bool Directory::isOpen(void) {
        return this->m_dir > 0;
    }

    void Directory::close(void) {
    	// TODO: implement if required
        this->m_dir = 0;
    }

    NATIVE_INT_TYPE Directory::getLastError(void) {
        return this->m_lastError;
    }

    const char* Directory::getLastErrorString(void) {
        return strerror(this->m_lastError);
    }

}
