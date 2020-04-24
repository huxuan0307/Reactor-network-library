#include "Logging.h"
#include "LogDebug.h"
#include "Timestamp.h"
std::string MessageLogger_file;

using namespace std;

Debug_inner MessageLogger::debug () const
{
	Debug_inner d (MessageLogger_file);
	d.stream->fs << Timestamp::now().toFormatString() << " [" << setw(10) <<  MessageLogger_type[static_cast<int>(d.stream->type)] + ":" + to_string(getpid()) + "]";
	return d;
}

Debug_inner MessageLogger::info () const
{

	Debug_inner d (MessageLogger_file);
	d.setMsgType (MsgType::InfoMsg);
	d.stream->fs << Timestamp::now().toFormatString() << " [" << setw(10) <<  MessageLogger_type[static_cast<int>(d.stream->type)] + ":" + to_string(getpid()) + "]";
	return d;
}

Debug_inner MessageLogger::warning () const
{
	Debug_inner d (MessageLogger_file);
	d.setMsgType (MsgType::WarningMsg);
    d.stream->fs << Timestamp::now().toFormatString()<< " [" << setw(10) <<  MessageLogger_type[static_cast<int>(d.stream->type)] + ":" + to_string(getpid()) + "]";
	return d;
}

Debug_inner MessageLogger::critical () const
{
	Debug_inner d (MessageLogger_file);
	d.setMsgType (MsgType::CriticalMsg);

	d.stream->fs << Timestamp::now().toFormatString() << " [" << setw(10) <<  MessageLogger_type[static_cast<int>(d.stream->type)] + ":" + to_string(getpid()) + "]";
	return d;
}

Debug_inner MessageLogger::fatal () const
{
	Debug_inner d (MessageLogger_file);
	d.setMsgType (MsgType::FatalMsg);
	d.stream->fs << Timestamp::now().toFormatString() << " [" << setw(10) <<  MessageLogger_type[static_cast<int>(d.stream->type)] + ":" + to_string(getpid()) + "]";
	return d;
}

string bitstreamToString_help(int num, const char *str, int n);

string bitstreamToString(const char* s, const char* p_end)
{
	string res;

	int num = 0;
	const char* str = s;
	for (; str + 0x10 < p_end; str += 0x10)	{
		res+=bitstreamToString_help(num, str, 16);
		num += 0x10;
	}
	res+=bitstreamToString_help(num, str, p_end - str);
	return res;
}

string bitstreamToString(const string&s)
{
	return bitstreamToString(s.data(), s.data()+s.size());
}

string bitstreamToString_help(int num, const char *str, int n)
{
	stringstream ss;
	ss << setfill('0') << setw(8) << hex << num << " ";
	ss << setfill(' ');
	ss.unsetf(ios::hex);
	for (int i = 0; i < n; i++)
	{
		if (i % 8 == 0 && i)
			ss << " -";

		ss << " " << setw(2) << setfill('0') << hex << (((unsigned int) str[i]) & 0xff);

	}
	for (int i = n; i < 16; i++)
		ss << "   ";
	ss << "\t";
	for (int i = 0; i < n; i++)
	{
		if (str[i] > 32 && str[i] <= 126)
			ss << str[i];
		else
			ss << '.';
	}
	ss << endl;
	return ss.str();
}