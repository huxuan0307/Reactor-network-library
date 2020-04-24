
#include "RingBuffer.h"

constexpr int extraBufferSize = 65536;

ssize_t RingBuffer::readFromFd (int fd, int& savedErrno)
{
	char extraBuffer[extraBufferSize];
	iovec vec[3];
	ssize_t cnt;
	size_t writablelen = writableLength ();
	size_t writetaillen = writeTailLen ();
	if (isWriteConsistent ()) {
		vec[0].iov_base = beginWrite ();
		vec[0].iov_len = writableLength ();
		vec[1].iov_base = extraBuffer;
		vec[1].iov_len = extraBufferSize;
		cnt = readv (fd, vec, 2);
	}
	else {
		vec[0].iov_base = beginWrite ();
		vec[0].iov_len = writeTailLen ();
		// vec[1].iov_base = &*begin();
		// vec[1].iov_len = writableLength() - writeTailLen();
		vec[1].iov_base = extraBuffer;
		vec[1].iov_len = extraBufferSize;
		cnt = readv (fd, vec, 2);
	}
	if (cnt < 0) {
		savedErrno = errno;
	}
	if (isWriteConsistent ()) {
		if (static_cast<size_t>(cnt) <= writablelen) {
			if ((writerIndex_ += cnt) == buffer_.size ())
				writerIndex_ = 0;
			dataSize_ += cnt;
			// do nothing else 
		}
		else {
			// buffer is full
			// add cnt-writableLength space to fill extraBuffer in 
			writerIndex_ = readerIndex_;
			dataSize_ += writablelen;
			int extraLen = cnt - writablelen;
			append (extraBuffer, extraLen);
		}
	}
	else {
		if (static_cast<size_t>(cnt) <= writetaillen) {
			if ((writerIndex_ += cnt) == buffer_.size ())
				writerIndex_ = 0;
			dataSize_ += cnt;
			// do nothing else
		}
		else {
			dataSize_ += writetaillen;
			writerIndex_ = 0;
			int extraLen = cnt - writetaillen;
			append (extraBuffer, extraLen);
		}
	}
	return cnt;

}