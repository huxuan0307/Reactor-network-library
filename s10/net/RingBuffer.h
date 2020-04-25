#include <sys/uio.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <cstring>
#include <memory>
#include <vector>
#include <cassert>
#include <iostream>
#include "network_global.h"

class RingBuffer
{
	template<typename T>
	using vector = std::vector<T>;
public:
	static const size_t kInitialSize = 16;

	RingBuffer ();
	// default copy-ctor, dtor and assignment are fine

	size_t dataSize ()const { return dataSize_; }
	void swap (RingBuffer& rhs)
	{
		buffer_.swap (rhs.buffer_);
		std::swap (readerIndex_, rhs.readerIndex_);
		std::swap (writerIndex_, rhs.writerIndex_);
		std::swap (dataSize_, rhs.dataSize_);
	}

	size_t readableLength () const
	{
		return dataSize_;
	}

	size_t writableLength () const
	{
		return buffer_.size () - dataSize_;
	}

	// 前插和后插无所谓
	size_t prependableBytes () const
	{
		return buffer_.size () - dataSize_;
	}

	const char* peek () const
	{
		return &*begin () + readerIndex_;
	}
	std::vector<char>::iterator peek ()
	{
		return begin () + readerIndex_;
	}

	// retrieve returns void, to prevent
	// string str(retrieve(readableBytes()), readableBytes());
	// the evaluation of two functions are unspecified
	void retrieve (size_t len)
	{
		if (len <= readableLength ()) {
			readerIndex_ += len;
			dataSize_ += len;
		}
		else {
			retrieveAll ();
		}
	}

	//void retrieveUntil (const char* end)
	//{
	//	assert (peek () <= end);
	//	assert (end <= beginWrite ());
	//	retrieve (end - peek ());
	//}

	void retrieveAll ()
	{
		readerIndex_ = 0;
		writerIndex_ = 0;
		dataSize_ = 0;
	}

	std::string retrieveAllAsString(){
		return retrieveAsString(dataSize_);
	}

	std::string retrieveAsString (size_t len)
	{
		len = std::min (len, readableLength ());
		if (isReadConsistent (len)) {
			std::string str (peek (), peek () + len);
			if ((readerIndex_ += len) == buffer_.size ())
				readerIndex_ = 0;
			dataSize_ -= len;
			return str;
		}
		else {
			std::string str (peek (), end ());
			str.append (&*begin (), len - readTailLen ());
			readerIndex_ = len - readTailLen ();
			dataSize_ -= len;
			return str;
		}
	}

	void append (const std::string& str)
	{
		append (str.data (), str.length ());
	}

	void append (const char* data, size_t len)
	{
		ensureWritableBytes (len);
		if (isWriteConsistent (len)) {
			std::copy (data, data + len, beginWrite ());
			dataSize_ += len;
			if ((writerIndex_ += len) == buffer_.size ())
				writerIndex_ = 0;
		}
		else {
			std::copy (data, data + writeTailLen (), beginWrite ());
			std::copy (data + writeTailLen (), data + len, begin ());
			dataSize_ += len;
			writerIndex_ = len - writeTailLen ();
		}
	}

	void append (const void* data, size_t len)
	{
		append (static_cast<const char*>(data), len);
	}

	void ensureWritableBytes (size_t len)
	{
		if (writableLength () < len) {
			makeSpaceToFit (len);
		}
		assert (writableLength () >= len);
	}

	char* beginWrite ()
	{
		return &*begin () + writerIndex_;
	}

	const char* beginWrite () const
	{
		return &*begin () + writerIndex_;
	}

	char* beginRead(){
		return &*begin() + readerIndex_;
	}

	const char* beginRead()const{
		return &*begin() + readerIndex_;
	}

	void hasWritten (size_t len)
	{
		writerIndex_ += len;
	}

	void prepend (const void* /*restrict*/ data, size_t len)
	{
		assert (len <= prependableBytes ());
		readerIndex_ -= len;
		const char* d = static_cast<const char*>(data);
		std::copy (d, d + len, begin () + readerIndex_);
	}
	// 暂不提供shrink
	//void shrink (size_t reserve)
	//{
	//	std::vector<char> buf (readableLength () + reserve);
	//	std::copy (peek (), peek () + readableLength (), buf.begin ());
	//	buf.swap (buffer_);
	//}

	ssize_t readFromFd (int fd, int& savedErrno);
private:


	vector<char>::iterator begin ()
	{
		return buffer_.begin ();
	}

	vector<char>::const_iterator begin () const
	{
		return buffer_.cbegin ();
	}

	vector<char>::iterator end ()
	{
		return buffer_.end ();
	}

	vector<char>::const_iterator end () const
	{
		return buffer_.end ();
	}

	void makeSpaceToFit (size_t len)
	{
		if (isReadConsistent ()) {
			buffer_.resize (dataSize_ + len);
			if (!writerIndex_)
				writerIndex_ = readerIndex_ + dataSize_;
		}
		else {
			size_t  movesize = std::max (len, writerIndex_);
			vector<char> tmp (dataSize_ + movesize, 0);
			auto iter = copy (begin() + readerIndex_, end(), tmp.begin());
			copy (begin (), begin () + writerIndex_, iter);
			std::swap (tmp, buffer_);
			readerIndex_ = 0;
			writerIndex_ = dataSize_;
		}
	}

private:
	bool isReadConsistent (size_t len)
	{
		return !(writerIndex_ <= readerIndex_&& len > readTailLen ());
	}
	bool isReadConsistent ()
	{
		return (readerIndex_ < writerIndex_ || readerIndex_ == 0||writerIndex_ == 0);
	}
	bool isWriteConsistent (size_t len)
	{
		return !(readerIndex_ <= writerIndex_&& len > writeTailLen ());
	}
	bool isWriteConsistent ()
	{
		return (writerIndex_ < readerIndex_ || writerIndex_ == 0 || readerIndex_==0);
	}
	size_t readTailLen () { return buffer_.size () - readerIndex_; }
	size_t writeTailLen () { return buffer_.size () - writerIndex_; }
	std::vector<char> buffer_;
	size_t readerIndex_;
	size_t writerIndex_;
	size_t dataSize_;
};