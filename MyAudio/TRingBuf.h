#pragma once
class TRingBuf
{
public:
	TRingBuf(unsigned int BufSize=1000*1000*1000);
	void  push(char* buf, unsigned int len);
	void pop(char* buf, unsigned int len);
	void copy(char* buf, unsigned int len, unsigned int offset = 0);
protected:
	char* m_buf;
	unsigned int m_BufSize;
	unsigned int m_readPos;
	unsigned int m_writePos;
};

