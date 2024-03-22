#include "TRingBuf.h"
#include <malloc.h>
#include <memory.h>
#include <windows.h>  

TRingBuf::TRingBuf(unsigned int BufSize) {
	m_BufSize = BufSize;
	m_buf = (char*)malloc(BufSize+10);
	memset(m_buf, 0, BufSize);
	m_readPos = 0;
	m_writePos = 0;
}
void  TRingBuf::push(char* buf, unsigned int len) {
	while (true) {
		if (m_writePos + 1 == m_readPos ||
			(0 == m_readPos && m_writePos + 1 == m_BufSize) ) {
			Sleep(30);
			continue;
		}
		unsigned int size = 0;
		if (m_writePos >= m_readPos) {
			size = m_BufSize - m_writePos;
			if (0 == m_readPos) {
				size--;
			}
		}
		else {
			size = m_readPos - m_writePos - 1;
		}
		if (size >= len) {
			size = len;
		}
		memcpy(m_buf+ m_writePos, buf, size);
		m_writePos += size;
		if (m_writePos >= m_BufSize) {
			m_writePos = 0;
		}
		if (size == len) {
			return;
		}
		buf += size;
		len -= size;
	}
}
void TRingBuf::pop(char* buf, unsigned int len) {
	if (0 == len) {
		return;
	}
	while (true) {
		if (m_readPos == m_writePos) {
			Sleep(30);
			continue;
		}
		unsigned int size = 0;
		if (m_readPos > m_writePos) {
			size = m_BufSize - m_readPos;
		}
		else {
			size = m_writePos - m_readPos;
		}
		if (size > len) {
			size = len;
		}
		memcpy(buf, m_buf + m_readPos, size);
		m_readPos += size;
		if (m_readPos >= m_BufSize) {
			m_readPos = 0;
		}
		if (size == len) {
			return;
		}
		buf += size;
		len -= size;
	}
}
void TRingBuf::copy(char* buf, unsigned int len, unsigned int offset) {
	if (0 == len) {
		return;
	}
	while (true) {
		if (m_readPos+offset == m_writePos) {
			Sleep(30);
			continue;
		}
		unsigned int size = 0;
		if (m_readPos + offset > m_writePos) {
			size = m_BufSize - offset - m_readPos;
		}
		else {
			size = m_writePos - offset- m_readPos;
		}
		if (size > len) {
			size = len;
		}
		size = m_writePos - offset - m_readPos;
		memcpy(buf, m_buf + + offset, size);

		if (size == len) {
			return;
		}
		offset += size;
		if (offset >= m_BufSize) {
			offset = 0;
		}
		buf += size;
		len -= size;
	}
}
