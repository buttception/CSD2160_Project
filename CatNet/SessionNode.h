#ifndef SESSION_NODE_H
#define SESSION_NODE_H
#pragma once

#include <WinSock2.h>

#ifndef MAX_CONNECTION
#define MAX_CONNECTION FD_SETSIZE
#endif

#define RECV_BUFSIZE 4096

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

		int& GetIndex() { return m_Index; }
		int GetIndex() const { return  m_Index; }

		SOCKET& GetSocket() { return m_Socket; }
		SOCKET GetSocket() const { return m_Socket; }

		SOCKADDR_IN& GetSessionAddr() { return m_SessionAddr; }
		SOCKADDR_IN GetSessionAddr() const { return m_SessionAddr; }

		void ClearRecvBuffer() { _strset_s(m_RecvBuf, '\0'); }
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
		SessionNode* IndexList[MAX_CONNECTION + 1];

	public:
		struct ListData
		{
		private:
			int m_NodeCount {};
			SessionNode* m_Head {nullptr};
			SessionNode* m_Tail {nullptr};

		public:
			int& GetNodeCount() { return m_NodeCount; }
			int GetNodeCount() const { return m_NodeCount; }

			SessionNode*& GetHead() { return m_Head; }
			SessionNode* GetHead() const { return m_Head; }

			SessionNode*& GetTail() { return m_Tail; }
			SessionNode* GetTail() const { return m_Tail; }

		public:
			int AttachNode(SessionNode* node)
			{
				if (!m_Head)
					m_Head = m_Tail = node;
				else
				{
					node->GetPrev() = m_Tail;
					m_Tail->GetNext() = node;
					m_Tail = node;
				}

				return ++m_NodeCount;
			}
			SessionNode* DetachFirstNode()
			{
				if (!m_Head)
					return m_Head;

				m_Head = m_Head->GetNext();
				m_Head->GetPrev() = nullptr;

				return m_Head;
			}
			SessionNode* DetachLastNode()
			{
				if (!m_Tail)
					return m_Tail;

				m_Tail = m_Tail->GetPrev();
				m_Tail->GetNext() = nullptr;

				return m_Tail;
			}
			int DetachNode(SessionNode* Node)
			{
				SessionNode* node{ m_Head };
				while (node)
				{
					if (node == Node)
					{
						SessionNode* next{ node->GetNext() }, *prev{ node->GetPrev() };

						prev->GetNext() = next;
						next->GetPrev() = prev;

						node->GetNext() = node->GetPrev() = nullptr;
						break;
					}

					node = node->GetNext();
				}

				return --m_NodeCount;
			}

			int GetSessionIndexBySocket(SOCKET Socket) const
			{
				SessionNode* node{ m_Head };
				while (node)
				{
					if (node->GetSocket() == Socket)
						break;

					node = node->GetNext();
				}

				return node->GetIndex();
			}

			SessionNode* GetSessionNodeBySocket(SOCKET Socket) const
			{
				SessionNode* node{ m_Head };
				while (node)
				{
					if (node->GetSocket() == Socket)
						break;

					node = node->GetNext();
				}

				return node;
			}
		} ActiveList, EmptyList;

	public:
		SessionList() : IndexList{ nullptr, }
		{
			int i{};
			for (auto& session_node : IndexList)
			{
				session_node = new SessionNode{};
				session_node->GetIndex() = i++;
			}
		}
		~SessionList()
		{
			for (auto& session_node : IndexList)
			{
				delete session_node;
				session_node = nullptr;
			}
		}

		SessionList(const SessionList&) = delete;
		SessionList(SessionList&&) = delete;

		SessionList& operator=(const SessionList&) = delete;
		SessionList& operator=(SessionList&&) = delete;
	public:
		SessionNode*& GetSessionNodeByIndex(int Index) { return IndexList[Index]; }
		SessionNode* GetSessionNodeByIndex(int Index) const { return IndexList[Index]; }
	};
}

#endif