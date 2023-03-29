#ifndef SESSION_NODE_H
#define SESSION_NODE_H
#pragma once

#include <unordered_map>
#include <WinSock2.h>

#ifndef MAX_CONNECTION
#define MAX_CONNECTION FD_SETSIZE
#endif

#define RECV_BUFSIZE 24576

namespace CatNet
{
	//Is linked list node
	class SessionNode
	{
	private:
		int m_Index;
		SOCKET m_Socket;
		SOCKADDR_IN m_SessionAddr;

		char m_RecvBuf[RECV_BUFSIZE];
		int m_RecvBufWritePos;

		SessionNode* m_Prev;
		SessionNode* m_Next;

	public:
		SessionNode() : m_Index{}, m_Socket{ INVALID_SOCKET }, m_SessionAddr{}, m_RecvBuf{ '\0', }, m_RecvBufWritePos{}, m_Prev{ nullptr }, m_Next{ nullptr } {}
		~SessionNode() = default;

		void SetIndex(int Index) { m_Index = Index; }
		int& GetIndex() { return m_Index; }
		int GetIndex() const { return  m_Index; }

		SOCKET& GetSocket() { return m_Socket; }
		SOCKET GetSocket() const { return m_Socket; }

		SOCKADDR_IN& GetSessionAddr() { return m_SessionAddr; }
		SOCKADDR_IN GetSessionAddr() const { return m_SessionAddr; }

		void ClearRecvBuffer() { memset(m_RecvBuf, '\0', RECV_BUFSIZE); }
		bool AddToRecvBuffer(char NewBuffer[], size_t NewBufferLength)
		{
			if (m_RecvBufWritePos + strlen(m_RecvBuf) > NewBufferLength)
				return false;

			strcat_s(m_RecvBuf + m_RecvBufWritePos, NewBufferLength, NewBuffer);
			m_RecvBufWritePos += static_cast<int>(NewBufferLength);
			return true;
		}

		char* GetRecvBuffer() { return m_RecvBuf; }
		char* GetRecvBuffer(int Pos) { return m_RecvBuf + Pos; }
		void SetRecvBufferWritePos(int Pos) { m_RecvBufWritePos = Pos; }
		//int ShiftRecvBuffer(int ShiftSize);

		int& GetRecvBufferWritePos() { return m_RecvBufWritePos; }
		int GetRecvBufferWritePos() const { return m_RecvBufWritePos; }

		int CheckCompletedPacket(char CompletedPacket[], int* CompletedPacketLength)
		{
			*CompletedPacketLength = static_cast<int>(strlen(CompletedPacket));

			return 1;
		}

		void Clear()
		{
			ClearRecvBuffer();
			m_RecvBufWritePos = 0;
		}
		void CloseSession() const
		{
			closesocket(m_Socket);
		}

		SessionNode*& GetPrev() { return m_Prev; }
		SessionNode* GetPrev() const { return m_Prev; }
		SessionNode*& GetNext() { return m_Next; }
		SessionNode* GetNext() const { return m_Next; }

		void SetNewConnectedSession(const SOCKET& Socket, const SOCKADDR_IN& Address)
		{
			m_Socket = Socket;
			m_SessionAddr = Address;
		}
	};

	class SessionList
	{
	private:
		bool indexList[MAX_CONNECTION + 1];
		std::unordered_map<int, SessionNode*> indexSessionMap;
		std::unordered_map<SOCKET, int> socketIndexMap;
		int m_SessionCount{};

	public:

		SessionList() : indexList{ false }
		{
			m_SessionCount = 0;
		}
		~SessionList()
		{
			indexSessionMap.clear();
			socketIndexMap.clear();
		}

		int GetSessionIndexBySocket(SOCKET Socket)
		{
			return socketIndexMap[Socket];
		}

		SessionNode* GetSessionNodeBySocket(SOCKET Socket)
		{
			return indexSessionMap[GetSessionIndexBySocket(Socket)];
		}

		void AddSession(SessionNode* newSession)
		{
			indexList[newSession->GetIndex()] = true;
			indexSessionMap[newSession->GetIndex()] = newSession;
			socketIndexMap[newSession->GetSocket()] = newSession->GetIndex();
			m_SessionCount++;
		}

		void RemoveSession(int index)
		{
			indexList[index] = false;
			SOCKET temp = indexSessionMap[index]->GetSocket();
			socketIndexMap.erase(temp);
			indexSessionMap.erase(index);
			m_SessionCount--;
		}

		SessionList(const SessionList&) = delete;
		SessionList(SessionList&&) = delete;

		SessionList& operator=(const SessionList&) = delete;
		SessionList& operator=(SessionList&&) = delete;
		SessionNode*& GetSessionNodeByIndex(int Index) { return indexSessionMap[Index]; }
		bool CheckIndex(int index) { return indexList[index]; }
		int GetSessionCount() { return m_SessionCount; }
	};
}

#endif