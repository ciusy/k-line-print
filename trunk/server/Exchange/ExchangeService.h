#pragma once

#include "ace/OS.h"
#include "ace/Asynch_IO.h"

#define BUFFER_SIZE		2048

class ExchangeService : public ACE_Service_Handler
{
public:
	~ExchangeService()
	{
		if (this->handle () != ACE_INVALID_HANDLE)
		ACE_OS::closesocket (this->handle ());
	}

	virtual void open(ACE_HANDLE h, ACE_Message_Block&);

	// This method will be called when an asynchronous read
	// completes on a stream.
	virtual void handle_read_stream(const ACE_Asynch_Read_Stream::Result &result);

	// This method will be called when an asynchronous write
	// completes on a stream.
	virtual void handle_write_stream(const ACE_Asynch_Write_Stream::Result &result);

	/// Called when timer expires.  {tv} was the requested time value and
	/// {act} is the ACT passed when scheduling the timer.
	virtual void handle_time_out (const ACE_Time_Value &tv, const void *act = 0);

private:

	ACE_Asynch_Read_Stream reader_;
	ACE_Asynch_Write_Stream writer_;

	//	消息接收队列 - 内部循环队列
	char buffer_[BUFFER_SIZE];
	
	//	
	int buffer_start;
	int buffer_end;

};