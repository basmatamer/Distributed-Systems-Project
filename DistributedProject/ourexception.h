#ifndef OUREXCEPTION_H
#define OUREXCEPTION_H

#include <exception>
#include <string>


class OurException : public std::exception
{
public:
    OurException(std::string msg) : m_message(msg) { }
    const char * what () const throw ()
    {
        return m_message.c_str();
    }
private:
    std::string m_message;
};

class SendingFailedException : public OurException
{
public:
    SendingFailedException(std::string msg) : OurException(msg) { }
};

class InvalidIPException : public OurException
{
public:
    InvalidIPException(std::string msg) : OurException(msg) { }
};

class UnknownOperationException : public OurException
{
public:
    UnknownOperationException(std::string msg) : OurException(msg) { }
};

class SocketCreationFailedException : public OurException
{
public:
    SocketCreationFailedException(std::string msg) : OurException(msg) { }
};

class SocketBindingFailedException : public OurException
{
public:
    SocketBindingFailedException(std::string msg) : OurException(msg) { }
};

class DirectoryServiceUnreachableException : public OurException
{
public:
    DirectoryServiceUnreachableException(std::string msg) : OurException(msg) { }
};

class CorruptMessage : public OurException
{
public:
    CorruptMessage(std::string msg) : OurException(msg) { }
};

class PacketDataNullPointerException : public OurException
{
public:
    PacketDataNullPointerException(std::string msg) : OurException(msg) { }
};

class UnknownUsernameException : public OurException
{
public:
    UnknownUsernameException(std::string msg) : OurException(msg) { }
};




#endif // OUREXCEPTION_H
