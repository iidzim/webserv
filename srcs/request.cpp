/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: framdani <framdani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/26 17:16:08 by framdani          #+#    #+#             */
/*   Updated: 2022/05/26 17:16:11 by framdani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Request.hpp"
#include "../includes/webserv.hpp"
//! Method is case-sensitive

// ! recipients SHOULD ignore unrecognized header fields.
// ! rfc 7230 section 3.2.1

//! The order in which header fields with differing field names are received is not significant
//! rfc 7230 section 3.2.2

//! A server MUST reject any received request message that contains whitespace between a header field-name and colon with 400
//! rfc 7230 section 3.2.4

//! The presence of a message body in a request is signaled by a content-length or transfer-Encoding header field 
//! rfc 7230 section 3.3

 //! when a messaege does not have a transfer-Encoding header fields, a contentlength header field can provide the anticipated
 //! size as a decimal number of octets
 //! rfc 7230 section 3.2.2


 //! Any cotent-length field value greater than or eual to zero is valid[...] prevent parsing errors due to integer conversion overlows
 //! rfc 7230 section 3.3.2

//! if a message is received without transfer-Encoding and content length , the transfer-encoding overrides the content length.

//! if a valid content-length header field is present without transfer-encoding , the value defines 
	//! the expected message body length


//! if there is a message body  without content-length => response 411 (length required)

request::request():  _begin(true),_headersComplete(false), _bodyComplete(false), _isChunked(false), _isBodyExcpected(false)
{
	_port = -1;
	_host.clear();
	_data.clear();
	_rqst.method.clear();
	_rqst.URI.clear();
	_rqst.versionHTTP.clear();
	_rqst.query.clear();
	_rqst.statusCode = 200;
	_rqst.fd = -1;
	_contentLength = 0;
	_originContentLength = 0;
	_contentType.clear();
}

request::request(std::vector<serverInfo> &servers):  _begin(true),_headersComplete(false), _bodyComplete(false), _isChunked(false), _isBodyExcpected(false)
{
	_start = std::time(NULL);  
	_uploadpath.clear();
	_root.clear();
	_port = -1;
	_host.clear();
	_data.clear();
	_rqst.method.clear();
	_rqst.URI.clear();
	_rqst.versionHTTP.clear();
	_rqst.query.clear();
	_rqst.statusCode = 200;
	_rqst.fd = -1;
	_contentLength = 0;
	_originContentLength = 0;
	_contentType.clear();
	this->servers = servers;
	_size = 0;
}

request::request(const request& obj)
{
	*this = obj;
}

request& request::operator=(const request& obj)
{
	_start = obj._start;
	_begin = obj._begin;
	_port = obj._port;
	_host = obj._host;
	_data = obj._data;
	_rqst = obj._rqst;
	_headersComplete = obj._headersComplete;
	_bodyComplete = obj._bodyComplete;
	_isChunked = obj._isChunked;
	_isBodyExcpected = obj._isBodyExcpected;
	_contentLength = obj._contentLength;
	_originContentLength = obj._originContentLength;
	_contentType = obj._contentType;
	this->servers = obj.servers;
	_rqst.fd = obj._rqst.fd; //!
	_size = obj._size;
	
	return *this;
}

request::~request(){}

void request::resetTime()
{
	_start = std::time(NULL);
}

std::time_t request::getTime()
{
	return _start;
}

void request::deleteOptionalWithespaces(std::string & fieldValue)
{
	while (std::isspace(fieldValue[0]))
		fieldValue.erase(0, 1);
	while (std::isspace(fieldValue[fieldValue.size()-1]))
		fieldValue.erase(fieldValue.size()-1);
}

bool request::isFieldNameValid(const std::string &str)
{
	for (size_t i = str.size() - 1; i != 0; i--)
	{
		if (std::isspace(str[i]))
			return false;
	}
	return true;
}

std::string     request::getMimeType()
{
	mimeTypes tmp;
	std::string extension = tmp.getExtention(_contentType);
	return extension;
}

std::string     request::getContentType() { return _contentType; }

void    request::requestLine(std::istringstream &istr)
{
	//! I should convert basic_string to basic_istream to use getline

	std::string line;
	
	getline(istr, line);
   
	//! split first line by spaces
	std::vector<std::string> words;
	size_t pos = 0;

	if (line.empty() || line[line.size() - 1] != '\r')
	{
		_rqst.statusCode = 400;
		throw request::RequestNotValid();
	}
	while ((pos=line.find(" ")) != std::string::npos)
	{
		words.push_back(line.substr(0, pos));
		line.erase(0, pos+1);
	}
	words.push_back(line);
	if (words.size() != 3)//! should only be one space between them
	{
		_rqst.statusCode = 400;
		throw request::RequestNotValid();
	}
	_rqst.method = words[0];
	if (_rqst.method == "POST")
	{
		_isBodyExcpected = true;

		//create a tmp file
		tmpFile.open("tmp.txt", std::ios::out | std::ios::app);

	}
	pos = words[1].find("?");
	if (pos != std::string::npos)
	{
		_rqst.URI = words[1].substr(0, pos);
		_rqst.query = words[1].substr(pos+1, words[1].size()-1);
	}
	else
		_rqst.URI = words[1];
	//* http version
	//! erase \r at the end of the line
	words[2].erase(words[2].end() - 1);
	if (words[2] != "HTTP/1.1") //! rfc 
	{
		_rqst.statusCode = 505;
		throw request::RequestNotValid();
	}
	_rqst.versionHTTP = words[2]; 
}

bool isNumber(std::string value)
{
	for (size_t i = 0 ; i < value.size();i++)
	{
		if (!std::isdigit(value[i]))
			return false;
	}
	return true;
}

void    request::getHeaders(std::istringstream & istr)
{
	std::string line;
	//! I should erase \r at the end of each line
	std::vector<std::string> tmp;

	while (getline(istr, line))
	{
		if (line[0] == '\r') //the end of the headers
			break;
		if (line[line.size() - 1] != '\r')
		{
			_rqst.statusCode = 400;
			throw request::RequestNotValid();
		}
		line.erase(line.end()-1);

		//! put the expected headers in the right position
		//! ignore the duplication
		//! content-length && transfer-Encoding
		
		//! split by :
		size_t pos = line.find(":");
		if (pos == std::string::npos)
		{
			_rqst.statusCode = 400;
			throw request::RequestNotValid();
		}
		else if (pos != std::string::npos)
		{
			std::string fieldName = line.substr(0, pos);
			//convert to lower case
			for (size_t i = 0; i < fieldName.size(); i ++)
				fieldName[i] = tolower(fieldName[i]);
			//! check if the header is valid
			if (isFieldNameValid(fieldName))
			{
				std::string fieldValue = line.substr(pos+1, line.size()-1);
				//    std::cout<<"["<<fieldName<<"-> "<< fieldValue<<"]"<<std::endl;
				deleteOptionalWithespaces(fieldValue);
				if (fieldName == "transfer-encoding" && fieldValue == "chunked")
					_isChunked = true;
				else if (fieldName == "host")
				{

					size_t pos = fieldValue.find(":");
					if (pos != std::string::npos)
					{
						try
						{
						   _port = stoi(fieldValue.substr(pos+1, fieldValue.size()-1));
						}catch (std::exception &e)
						{
							_rqst.statusCode = 500;
							throw request::RequestNotValid();
						}
						fieldValue.erase(pos, fieldValue.size()-1);
						_host = fieldValue;
					}
				}
				else if (fieldName == "content-type")
					_contentType = fieldValue;
			   (_rqst.headers).insert(std::pair<std::string, std::string>(fieldName, fieldValue));
			}
			else
			{
				_rqst.statusCode = 400;
				throw request::RequestNotValid();
			}
		}
		else
		{
			_rqst.statusCode = 400;
			throw request::RequestNotValid();
		}   
	}
	if (_rqst.headers.find("host") == _rqst.headers.end())
	{
		_rqst.statusCode = 400;
		throw request::RequestNotValid();
	}

	if (_isBodyExcpected && !_isChunked)
	{
		//convert content-length
		if (_rqst.headers.find("content-length") == _rqst.headers.end() || !isNumber(_rqst.headers.find("content-length")->second))
		{
			_rqst.statusCode = 411; // length required
			throw request::RequestNotValid();
		}
		_originContentLength = stoul(_rqst.headers.find("content-length")->second);
	}
	if (_isBodyExcpected)
	{
		BlockMatching(servers);
		if (!_isChunked && _size !=0 && _originContentLength > _size)
		{
			_rqst.statusCode = 413; //request entity too large
			throw request::RequestNotValid();
		}
		srand(time(0));
		std::stringstream str;
		std::string st = "bodyFile";
		_rqst.bodyFile  = st + std::to_string(rand());
		if (_uploadpath.empty())
		{
			char cwd[256];
			std::string path(getcwd(cwd, sizeof(cwd)));
			_rqst.bodyFile = path + "/var/www/bodies/"+_rqst.bodyFile+getMimeType();
		}
		else
		{
			_rqst.statusCode = 201;
			_rqst.bodyFile = _uploadpath+_rqst.bodyFile+getMimeType();
		}
		_rqst.fd = open(_rqst.bodyFile.c_str(), O_CREAT | O_RDWR, 0777);
		if (_rqst.fd < 0)
		{
			_rqst.statusCode = 500;
			close(_rqst.fd);
			throw request::RequestNotValid();
		}
	}
}

void request::BlockMatching(std::vector<serverInfo> server_conf)
{
   std::string _root;

	for (size_t i = 0; i <server_conf.size(); i++)
	{
		if (_host == server_conf[i].serverName && _port == server_conf[i].port)
		{
			_size = server_conf[i].size;
			//block server choosen => I should choose location
			for (size_t j = 0; j < server_conf[i].location.size(); j++)
			{
				//choose the right location block if upload exist
				if (_rqst.URI == server_conf[i].location[j].uri+'/' || (_rqst.URI == server_conf[i].location[j].uri) ||
				((_rqst.URI.find(server_conf[i].location[j].uri + '/') == 0 && server_conf[i].location[j].uri.size() > 1 )))
				{
					//I'll need the root
					if (!server_conf[i].location[j].upload.empty())
					{
						if (server_conf[i].location[j].root.empty())
							_root = server_conf[i].root;
						else
							_root = server_conf[i].location[j].root;
						_uploadpath = _root+server_conf[i].location[j].upload;
						
					}
				}
			}
		}
		if (_port == server_conf[i].port)
		{//std::cout<<" server fiound !"<<std::endl;
			//server block choosen => I should choose location
			_size = server_conf[i].size;
			for (size_t j = 0; j < server_conf[i].location.size(); j++)
			{
			  //  std::cout<<" location fiound !"<<std::endl;
				//choose the right location block and if upload exist
				if (_rqst.URI == server_conf[i].location[j].uri+'/' || (_rqst.URI == server_conf[i].location[j].uri) ||
				((_rqst.URI.find(server_conf[i].location[j].uri + '/') == 0 && server_conf[i].location[j].uri.size() > 1 )))
				{
					if (!server_conf[i].location[j].upload.empty())
					{
						if (!server_conf[i].location[j].upload.empty())
						{
							if (server_conf[i].location[j].root.empty())
								_root = server_conf[i].root;
							else
								_root = server_conf[i].location[j].root;
							_uploadpath = _root+server_conf[i].location[j].upload;
						  //  std::cout<<"UPLOAD PATH "<<_uploadpath<<std::endl;
						}
					}
				}
				
			}

		}
	}
}

int     request::getPort()
{
	return _port;
}

std::string request::getHost()
{
	return _host;
}

const char* request::RequestNotValid::what()const throw()
{
	return "Request Not Valid !";
}

bool request::isHexadecimalFormat(std::string &number)
{
	for (size_t i = 0; i < number.size(); i++)
	{
		if (!isxdigit(number[i]))
			return false;
	}
	return true;
}

size_t request::convertHexToDecimal(std::string value)
{
	int base = 1;
	size_t result = 0;

	for (int i = value.length() - 1; i >= 0 ; i--)
	{
		if (value[i] >= '0' && value[i]<='9')
		{
			result +=(value[i]- 48) * base; 
			base*= 16;
		}
		else if (value[i] >= 'A' && value[i] <= 'F')
		{
			result +=(value[i] - 55)* base;
			base *=16;
		}
		else if (value[i] >= 'a' && value[i] <= 'f')
		{
			result += (value[i] - 87) * base;
			base*=16;
		}
	}
	return result;
}

s_requestInfo request::getRequest()
{
	return _rqst;
} 

void request::isBodyValid()
{
	tmpFile.close();
	tmpFile.open("tmp.txt", std::ios::in);
	int p;
	bool isDone = false;
	std::string line;
	size_t size;
	size_t totalBytes = 0;
	while (!isDone)
	{
		/*  FIRST LINE = SIZE */
		getline(tmpFile, line);
		if (line[line.size() - 1] != '\r')
		{
			_rqst.statusCode = 400;
			throw request::RequestNotValid();
		}
		line.erase(line.end()-1);
		if (!isHexadecimalFormat(line))
		{
			_rqst.statusCode = 400;
			throw request::RequestNotValid();
		}
		size = convertHexToDecimal(line);
		if (size == 0)
		{
			isDone = true;
			_bodyComplete = true;
			tmpFile.close();
			if (_size != 0 && totalBytes > _size)
			{
				_rqst.statusCode = 413; //request entity too large
				throw request::RequestNotValid();
			}
			return;
		}
		totalBytes+=size; //! To compare at the end with the size of the file

		//erase this line from the file

		
	   /* SECOND LINE */
		getline(tmpFile, line);
		size_t length ;
		if (line.length() < size)
		{
				line+='\n';
				p = write(_rqst.fd, line.c_str(), line.length());
				if (p < 0){
					_rqst.statusCode = 500;
					throw request::RequestNotValid();
				}
				length=line.length(); //(\n)
				std::string tmpLine;
				while(getline(tmpFile, tmpLine))
				{
					length+=tmpLine.length()+1;
					if (length>size)
						break;
					tmpLine+='\n';
					p = write(_rqst.fd, tmpLine.c_str(), tmpLine.length());
					if (p < 0){
						_rqst.statusCode = 500;
						throw request::RequestNotValid();
					}
				}
				if (tmpLine[tmpLine.size() - 1] != '\r')
				{
					_rqst.statusCode = 400;
					throw request::RequestNotValid();
				}
				tmpLine.erase(tmpLine.end()-1);
				p = write(_rqst.fd, tmpLine.c_str(), tmpLine.length());
				if (p < 0){
					_rqst.statusCode = 500;
					throw request::RequestNotValid();
				}
			}
			else
			{
				if (line[line.size() - 1] != '\r')
				{
					_rqst.statusCode = 400;
					throw request::RequestNotValid();
				}
				line.erase(line.end()-1);
				p = write(_rqst.fd, line.c_str(), line.length());
				if (p < 0){
					_rqst.statusCode = 500;
					throw request::RequestNotValid();
				}
		  }
	 }
}

void request::parse(char *buffer, size_t r)
{
	size_t i;
	int p;
	resetTime();
	if (!_headersComplete)
	{
		i = 0;
		while (i < r)
		{
			//Copies the first n characters from the array of characters pointed b
			_data+=buffer[i];i++;
		}
	}
	else if (_headersComplete && _isBodyExcpected)
	{
		if (!_isChunked)
		{
			size_t i = 0;
			while (i < r && _contentLength < _originContentLength)
			{
				_contentLength++;i++;
			}
			p = write(_rqst.fd, buffer, i);
			if (p < 0){
				_rqst.statusCode = 500;
				throw request::RequestNotValid();
			}
		}
		else if (_isChunked)
		{
			size_t i = 0;
			while(i < r)
			{
				_data+=buffer[i]; i++;
			}
			tmpFile<<_data;
			if (_data.find("0\r\n\r\n") != std::string::npos)
			{
				isBodyValid();
			}
			_data.clear();
		}
	}
	size_t pos = 0;
	if ((pos = _data.find("\r\n\r\n")) != std::string::npos && !_headersComplete) 
	{
			std::istringstream istr(_data);
			_headersComplete = true;
			requestLine(istr);
			getHeaders(istr);
			if (_isBodyExcpected)
			{
				std::string leftdata = _data.substr(pos+4, _data.size()-1);
				if (!_isChunked)
				{
					
					size_t i = 0;
					while (i < leftdata.size() && _contentLength < _originContentLength) //&& _contentLength < stoul(_rqst.headers["content-length"]))
					{
						_contentLength++;
						i++;
					}
					p = write(_rqst.fd, leftdata.c_str(), i);
					if (p < 0){
						_rqst.statusCode = 500;
						throw request::RequestNotValid();
					}
				}
				else
				{
					tmpFile<<leftdata;
					if (leftdata.find("0\r\n\r\n") != std::string::npos)
						isBodyValid();
				}
			}
			_data.clear();
	}
	if (_isBodyExcpected && !_isChunked)
	{
		if (_contentLength == _originContentLength)
			_bodyComplete = true;
	}
}

void request::print_request()
{
	std::cout<<"Method : "<<_rqst.method<<std::endl;
	std::cout<<"URI : "<<_rqst.URI<<std::endl;
	std::cout<<"Query "<<_rqst.query<<std::endl;
	std::cout<<"http version : "<<_rqst.versionHTTP<<std::endl;

	 //! print the map
	std::map<std::string, std::string>::iterator it = _rqst.headers.begin();
	std::map<std::string, std::string>::iterator ite = _rqst.headers.end();
	while (it != ite)
	{
		std::cout<<it->first<<" "<<it->second<<std::endl;
		it++;
	}
}

void request::closefds()
{
	if (_isBodyExcpected)
	{
		close(_rqst.fd);
		tmpFile.close();
		std::remove("tmp.txt");
	}
}

bool request::getComplete()
{
	if (_headersComplete && (_bodyComplete || !_isBodyExcpected))
		return true;
	return false;
}

bool request::isComplete()
{
	if (_headersComplete && (_bodyComplete || !_isBodyExcpected))
	{
		if (_bodyComplete)
		{
			close(_rqst.fd);
			tmpFile.close();
			std::remove("tmp.txt");
		}
		return true;
	}
	return false;
}

void request::forceStopParsing()
{
	_headersComplete = true;
	if (_isBodyExcpected)
		_bodyComplete = true;
}
