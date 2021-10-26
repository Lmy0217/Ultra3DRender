#ifndef V3DEXCEPTION_H
#define V3DEXCEPTION_H

#include <exception>
#include <QtCore/QString>
class V3dException: public std::exception
{
protected:
	QString exception_str;
public:
	virtual const QString& getEception_str() const 
	{ return  exception_str;};
};

class MemoryException: public V3dException
{
public:
	MemoryException()
	{
		exception_str = "Memory Problem! not enough memory!";
	}
	const QString& getException_str() const
	{
		return this ->exception_str;
	}
};

class dataFormException: public V3dException
{
public:
	dataFormException()
	{
		exception_str = "Data Form Problem! wrong data!";
	}
	const QString& getException_str() const
	{
		return this ->exception_str;
	}
};
#endif