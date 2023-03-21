#ifndef PROCESSSESSION_H
#define PROCESSSESSION_H
#pragma once

#include <WinSock2.h>
#include <iostream>

#include "SessionNode.h"

#define SEND_BUFSIZE    512
#define MAX_STRING_LENGTH_FOR_PACKET 200

namespace CatNet
{
    class SessionNode;
    enum
    {
        SESSION_STATE_EMPTY = 0,            // 0
        SESSION_STATE_CLOSEREDY,            // 1
        SESSION_STATE_NEWCONNECTION,        // 2
        SESSION_STATE_READPACKET,           // 3
    };

    struct GetString {
        char str[MAX_STRING_LENGTH_FOR_PACKET + 1];
        WORD str_len{};

        GetString(WORD len) : str{'\0',}
        {
            str_len = len;
        }
    };
    struct PacketMessage
    {
        int   BufferWritePos;         // Same as the length of the data inside the buffer.
        int   BufferReadPos;
        char  Buffer[SEND_BUFSIZE];

        PacketMessage() : BufferWritePos(0), BufferReadPos(0), Buffer{ '\0', }
        {
        }

        PacketMessage(const PacketMessage&) = default;

        ~PacketMessage()
        {
            Reset();
        }

        void Reset()
        {
            BufferWritePos = 0;
            BufferReadPos = 0;
            memset(Buffer, '\0', SEND_BUFSIZE);
        }

        char* GetBuffer() { return Buffer; }
        int GetBufferLength() const { return BufferWritePos; }

        int DecodePacket(char Packet[], int PacketLength)
        {
            if (PacketLength < 0)
                return 0;
            memset(Buffer, '\0', SEND_BUFSIZE);
            memcpy(Buffer, Packet, PacketLength);

            return BufferWritePos = PacketLength;
        }
        int CopyBufferTo(char* Dest) const
        {
            strcpy_s(Dest, BufferWritePos, Buffer);

            return BufferWritePos;
        }
        int CopyBufferFrom(char* Origin, int OriginLength)
        {
            strcpy_s(Buffer, OriginLength, Origin);

        	return BufferWritePos = OriginLength;
        }

        WORD StrManager(char* copy, const char* source, WORD len) const
        {
            if (_msize(copy) <= len)
            {
                return 0;
            }

            const WORD itr{ static_cast<WORD>(strlen(source)) };

            strncpy_s(copy, _msize(copy), source, len);

            if (itr > len)
                copy[len] = '\0';
            else
                copy[itr] = '\0';


            return itr + 1;
        }

        template <typename T>
        PacketMessage& Add(T& data)
        {
            const int DataSize{ static_cast<int>(sizeof(T)) };
            CopyMemory(&Buffer[BufferWritePos], (CONST PVOID) & data, DataSize);
            BufferWritePos += DataSize;

            return *this;
        }

        PacketMessage& AddString(const char*& data)
        {
            const int StringLength{ static_cast<int>(strlen(data) + 1) };
            //strcpy_s(Buffer + BufferWritePos, StringLength, data);
            memcpy(Buffer + BufferWritePos, data, StringLength);
            BufferWritePos += StringLength - 1;
            return *this;
        }

        template <typename T>
        PacketMessage& Fetch(T& data)
        {
            const int DataSize{ static_cast<int>(sizeof(T)) };
            CopyMemory(reinterpret_cast<PVOID>(&data), reinterpret_cast<CONST PVOID>(&Buffer[BufferReadPos]), DataSize);
            BufferReadPos += DataSize;
            return *this;
        }

        PacketMessage& FetchByte(unsigned char* data)
        {
            CopyMemory(data, reinterpret_cast<CONST PVOID>( & Buffer[BufferReadPos]), 1);
            BufferReadPos += 1;
            return *this;
        }

        PacketMessage& FetchString(GetString& str)
        {
            const int DataSize{ str.str_len + 1 };
            StrManager(str.str, (char*)&Buffer[BufferReadPos], str.str_len);
            BufferReadPos += DataSize;
            return *this;
        }

        PacketMessage& FetchString(const char*& data)
        {
            const int DataSize = static_cast<int>(strlen(&Buffer[BufferReadPos]) + 1);
            lstrcpy(reinterpret_cast<LPTSTR>(const_cast<char*>(data)), reinterpret_cast<LPCTSTR>(&Buffer[BufferReadPos]));
            BufferReadPos += DataSize;
            return *this;
        }

        template <typename T>
        PacketMessage& operator << (const T& data) { return Add(data); }
        PacketMessage& operator << (const char* data) { return AddString(data); }
        PacketMessage& operator << (char* data) { return AddString(const_cast<const char*&>(data)); }

        template <typename T>
        PacketMessage& Write(const T& data) { return Add(data); }
        PacketMessage& Write(const char* data) { return AddString(data); }
        PacketMessage& Write(char* data) { return AddString(const_cast<const char*&>(data)); }

        template <typename T>
        PacketMessage& operator >> (T& data) { return Fetch(data); }
        PacketMessage& operator >> (const char* data) { return FetchString(data); }
        PacketMessage& operator >> (char* data) { return FetchString(const_cast<const char*&>(data)); }
        PacketMessage& operator >> (GetString& data) { return FetchString(data); }

        template <typename T>
        PacketMessage Read(const T& data) { return Fetch(data); }
        PacketMessage Read(const char* data) { return FetchString(data); }
        PacketMessage Read(char* data) { return FetchString(const_cast<const char*&>(data)); }
        PacketMessage Read(GetString& data) { return FetchString(data); }
    };

    struct ProcessSession {
        int          m_SessionState;
        int          m_SessionIndex;
        SOCKADDR_IN  m_SessionAddress;
        PacketMessage m_PacketMessage;

        ProcessSession* m_Prev;
        ProcessSession* m_Next;

        ProcessSession() : m_SessionAddress{}
        {
            m_SessionState = 0;
            m_SessionIndex = 0;
            m_Prev = nullptr;
            m_Next = nullptr;
        }
    };
    struct ProcessSessionList {
        ProcessSession* m_Head;
        ProcessSession* m_Tail;

        ProcessSessionList()
        {
            m_Head = nullptr;
            m_Tail = nullptr;
        }

        void Attach(SessionNode* SessionNode, int State, int PacketLength, char Packet[])
        {
            ProcessSession* session{ new ProcessSession{} };

            if (!m_Head)
                m_Head = m_Tail = session;
            else
            {
                m_Tail->m_Next = session;
                session->m_Prev = m_Tail;
                m_Tail = session;
            }

        	session->m_SessionIndex = SessionNode->GetIndex();
            session->m_SessionAddress = SessionNode->GetSessionAddr();
            session->m_SessionState = State;
            session->m_PacketMessage.DecodePacket(Packet, PacketLength);
        }
        ProcessSession* GetFirstSession() { return m_Head; };
        void DeleteFirstSession()
        {
            if (!m_Head)
                return;

            if (m_Head == m_Tail)
            {
                delete m_Head;
                m_Head = nullptr;
                m_Tail = nullptr;
                return;
            }

            const ProcessSession* session{ m_Head };

            m_Head = session->m_Next;

            delete session;

            if (m_Head)
                m_Head->m_Prev = nullptr;
        };
    };
}

#endif