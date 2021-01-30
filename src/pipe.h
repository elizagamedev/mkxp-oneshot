#ifndef PIPE_H
#define PIPE_H

#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

class Pipe
{
public:
	typedef enum {
		Write,
		Read,
	} Mode;

	Pipe()
	{
#ifdef _WIN32
		handle = NULL;
#else
		fd = 0;
#endif
	}

	Pipe(const char *name_, Mode mode_)
	{
		open(name_, mode_);
	}

	~Pipe()
	{
#ifndef _WIN32
		if (mode == Write)
			unlink(filename.c_str());
#endif
	}

	void open(const char *name_, Mode mode_)
	{
		name = name_;
		mode = mode_;
#ifdef _WIN32
		filename = std::string("\\\\.\\pipe\\") + name;
#else
		filename = std::string(P_tmpdir) + "/" + name;
#endif

		if (mode == Read)
		{
#ifdef _WIN32
			handle = CreateFileA(
				filename.c_str(),
				GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
				NULL);
#else
			fd = ::open(filename.c_str(), O_RDONLY | O_NONBLOCK);
#endif
		} else {
#ifdef _WIN32
			handle = CreateNamedPipeA(
				filename.c_str(),
				PIPE_ACCESS_OUTBOUND,
				PIPE_TYPE_BYTE,
				PIPE_UNLIMITED_INSTANCES,
				256, 0, 0, NULL);
#else
			mkfifo(filename.c_str(), 0666);
			fd = 0;
#endif
		}
	}

	void connect()
	{
#ifdef _WIN32
		ConnectNamedPipe(handle, NULL);
#else
		fd = ::open(filename.c_str(), O_WRONLY);
#endif
	}

	void close()
	{
#ifdef _WIN32
		CloseHandle(handle);
		handle = NULL;
#else
		::close(fd);
		fd = 0;
#endif
	}

	bool read(char *buf)
	{
#ifdef _WIN32
	OVERLAPPED overlapped;
	memset(&overlapped, 0, sizeof(overlapped));
	if (!ReadFile(handle, buf, 1, NULL, &overlapped))
	{
		if (GetLastError() == ERROR_IO_PENDING)
			CancelIo(handle);
		return false;
	}
	return true;
#else
	return ::read(fd, buf, 1) == 1;
#endif
	}

	void write(const char *buf, size_t size)
	{
#ifdef _WIN32
		WriteFile(handle, buf, size, NULL, NULL);
#else
		::write(fd, buf, size);
#endif
	}

	bool isOpen()
	{
#ifdef _WIN32
		return handle != NULL;
#else
		return fd != 0;
#endif
	}

private:
	std::string name;
	std::string filename;
	Mode mode;
#ifdef _WIN32
	HANDLE handle;
#else
	int fd;
#endif
};

#endif // PIPE_H
