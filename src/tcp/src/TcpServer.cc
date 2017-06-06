#include "TcpServer.h"
#include "Log.h"

TcpServer::TcpServer()
{
}

TcpServer::TcpServer(EventLoop* loop_ptr)
{
	loop_ptr_ = loop_ptr;
}

int TcpServer::init(const std::string& server_addr, const int server_port)
{
	if(loop_ptr_)
	{
		loop_ptr_ = new EventLoop;
	}
	
	if(loop_ptr_)
	{
		tcp_acceptor_ptr_ = std::make_shared<TcpAcceptor>(loop_ptr_);
		if(tcp_acceptor_ptr_)
		{
			if(tcp_acceptor_ptr_->init(server_addr, server_port) < 0)
			{
				LOG4CPLUS_INFO(_logger, "tcp server init failed");
				return -1;
			}
			tcp_acceptor_ptr_->setReadCallback(std::bind(&TcpServer::newConnectionCallback, this, std::placeholders::_1));
			tcp_acceptor_ptr_->enableRead();
			tcp_acceptor_ptr_->registerIntoLoop();
		}
		else
		{
			LOG4CPLUS_INFO(_logger, "tcp server init failed");
			return -1;
		}
	}
	else
	{
		LOG4CPLUS_INFO(_logger, "tcp server init failed");
		return -1;
	}
	LOG4CPLUS_INFO(_logger, "tcp server init successfully");
	return 0;
}

void TcpServer::start()
{
	if(loop_ptr_)
	{
		LOG4CPLUS_INFO(_logger, "start tcp server") ;
		loop_ptr_->loop();
	}
	else
	{
		LOG4CPLUS_WARN(_logger, "event loop ptr null! please create a event loop first!") ;
	}
}

void TcpServer::newConnectionCallback(std::shared_ptr<TcpConnection>& tcp_connection_ptr)
{
	if(tcp_connection_ptr)
	{
		tcp_connection_ptr->setReadCallback(std::bind(&TcpServer::newMessageCallback, this, std::placeholders::_1, std::placeholders::_2));
		tcp_connection_ptr->enableRead();
		tcp_connection_ptr->registerIntoLoop();
		tcp_connection_map_[tcp_connection_ptr->getConnId()] = tcp_connection_ptr;
	}
}

void TcpServer::newMessageCallback(std::shared_ptr<TcpConnection> tcp_connection_ptr, const TrantorTimestamp timestamp)
{
	LOG4CPLUS_DEBUG(_logger, "new message received: "<<tcp_connection_ptr->getReadBufferPtr()->retrieveAllAsString()) ;
	tcp_connection_ptr->write("Hello!", 6);
	if(tcp_connection_ptr && newMessageCallback_)
	{
		newMessageCallback_(tcp_connection_ptr, timestamp);
	}
}

void TcpServer::closeCallback(std::shared_ptr<TcpConnection> tcp_connection_ptr)
{
	tcp_connection_map_.erase(tcp_connection_ptr->getConnId());
	if(closeCallback_)
	{
		closeCallback_(tcp_connection_ptr);
	}
}