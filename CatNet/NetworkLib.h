#ifndef _NETWORK_LIB
#define _NETWORK_LIB
#pragma once


#include <WinSock2.h>
#include <thread>
#include <list>

#include "SessionNode.h"
#include "ProcessSession.h"

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS

namespace CatNet
{
#define PACKET_HEADER           0xFFFB
#define PACKET_HEADER_SIZE      2
#define PACKET_HEADER_DATATYPE  unsigned short int
#define PACKET_TRAILER          0xFFFA
#define PACKET_TRAILER_SIZE     2
#define PACKET_TRAILER_DATATYPE unsigned short int
#define PACKET_LENGTH_SIZE      2
#define PACKET_LENGTH_DATATYPE  unsigned short int
}

namespace  CatNet
{

    enum class ERROR_TYPE : unsigned int
    {
        CATNET_ERROR_NOERROR = 0,
        CATNET_ERROR_WSASTARTUP,		    // 1
        CATNET_ERROR_WRONGAPPTYPE,		    // 2
        CATNET_ERROR_SOCKETFAILED,		    // 3
        CATNET_ERROR_BINDFAILED,		    // 4
        CATNET_ERROR_LISTENFAILED,          // 5
        CATNET_ERROR_CREATERECVTHREADFAILD, // 6
        CATNET_ERROR_SELECTRETURNEDERROR,   // 7
        CATNET_ERROR_ACCEPTFAILED,          // 8
    };

    // App Type.
    enum class APP_TYPE : unsigned int
    {
        APPTYPE_SERVER = 1,
        APPTYPE_CLIENT
    };

    // Protocol Type.
    enum class  PROTOCOL_TYPE : unsigned int
    {
        PROTOCOL_TCP = 1,
        PROTOCOL_UDP
    };
}

namespace CatNet
{
    //-------------------------
    // Class definitions for Network Objects.
    class NetLib
    {
    private:
        APP_TYPE m_AppType{};
        PROTOCOL_TYPE m_ProtocolType{};
        int     m_PortNumber{};

        WSADATA m_WsaData{};
        SOCKET  m_MainSocket{};

        HANDLE  m_RecvThreadHandle{};
        DWORD   m_RecvThreadID{};

        ERROR_TYPE m_ErrorCode;

        // Constructor and Destructor.
    public:
        NetLib() = default;
        ~NetLib() = default;

        // Getters & Setters.
    protected:
        SOCKET GetMainSocket() const { return m_MainSocket; }
        APP_TYPE    GetAppType() const { return m_AppType; };
        PROTOCOL_TYPE GetProtocolType() const { return  m_ProtocolType; }
        int    GetPortNumber() const { return  m_PortNumber; }

        HANDLE& GetRecvThreadHandle() { return m_RecvThreadHandle; }
        HANDLE GetRecvThreadHandle() const { return  m_RecvThreadHandle; }

        DWORD& GetRecvThreadID() { return m_RecvThreadID; }
        DWORD GetRecvThreadID() const { return m_RecvThreadID; }

    private:
        int EncodePacket(PacketMessage& PacketMessage, char Packet[])
        {
            PacketMessage.Reset();

            PacketMessage <<  Packet;

            return PacketMessage.BufferWritePos;
        }
        int DecodePacket(char Packet[], PacketMessage PacketMessage)
        {
            return PacketMessage.DecodePacket(Packet, static_cast<int>(strlen(Packet)));
        }

    public:

        ERROR_TYPE& GetErrorCode() { return m_ErrorCode; }

        ERROR_TYPE GetErrorCode() const { return m_ErrorCode; };

        char* GetErrorMessage() const
        {
            static char errorMessage[256]{ '\0', };
            switch (m_ErrorCode)
            {
            case ERROR_TYPE::CATNET_ERROR_WSASTARTUP:
                strcpy_s(errorMessage, "WSA Startup failed");
                break;
            case ERROR_TYPE::CATNET_ERROR_WRONGAPPTYPE:
                strcpy_s(errorMessage, "Application Type does not match");
                break;
            case ERROR_TYPE::CATNET_ERROR_SOCKETFAILED:
                strcpy_s(errorMessage, "Socket failed to open");
                break;
            case ERROR_TYPE::CATNET_ERROR_BINDFAILED:
                strcpy_s(errorMessage, "Bind to socket failed");
                break;
            case ERROR_TYPE::CATNET_ERROR_LISTENFAILED:
                strcpy_s(errorMessage, "Failed to listen to socket");
                break;
            case ERROR_TYPE::CATNET_ERROR_SELECTRETURNEDERROR:
                strcpy_s(errorMessage, "Selecting socket returned error");
                break;
            case ERROR_TYPE::CATNET_ERROR_ACCEPTFAILED:
                strcpy_s(errorMessage, "Accepting packet returned failed");
                break;
            case ERROR_TYPE::CATNET_ERROR_NOERROR:
            default:
                strcpy_s(errorMessage, "No error");
            }
            return errorMessage;
        }

    protected:
        int InitCatNet(int AppType, int ProtocolType, int PortNumber)
        {
            const WORD wVersion = MAKEWORD(2, 0);

            if (WSAStartup(wVersion, &m_WsaData))
            {
                m_ErrorCode = ERROR_TYPE::CATNET_ERROR_WSASTARTUP;
                return 0;
            }

            m_AppType = static_cast<APP_TYPE>(AppType);
            m_ProtocolType = static_cast<PROTOCOL_TYPE>(ProtocolType);
            m_PortNumber = PortNumber;

            m_MainSocket = socket(
                AF_INET,        // The address family. AF_INET specifies TCP/IP
                static_cast<int>(m_ProtocolType),    // Protocol type. SOCK_STREM specified TCP
                0               // Protoco Name. Should be 0 for AF_INET address family
            );

            if (m_MainSocket == INVALID_SOCKET)
            {
                m_ErrorCode = ERROR_TYPE::CATNET_ERROR_SOCKETFAILED;
                //WSACleanup();
                return 0;
            }
            return 1;
        }

        // General functions for Winsock.
    public:
        int SendPacket(SOCKET SendSocket, PacketMessage packet_message)
        {
            int nCntSend;
            char buf[SEND_BUFSIZE]{ '\0', };
            //strcpy_s(buf, packet_message.Buffer);
            memcpy(buf, packet_message.Buffer, packet_message.GetBufferLength());
            
            //EncodePacket(packet_message, buf);
            char* pBuffer{buf};
            memcpy(pBuffer, buf, packet_message.GetBufferLength());
            int BufferLen{ packet_message.BufferWritePos };

            while ((nCntSend = send(SendSocket, pBuffer, BufferLen, 0) != BufferLen))
            {
                if (nCntSend == -1)
                {
                    m_ErrorCode = ERROR_TYPE::CATNET_ERROR_ACCEPTFAILED;
                    break;
                }
                if (nCntSend == packet_message.BufferWritePos)
                    break;

                pBuffer += nCntSend;
                BufferLen -= nCntSend;
            }
            return packet_message.BufferWritePos - BufferLen;
        }
    };
}

#endif;