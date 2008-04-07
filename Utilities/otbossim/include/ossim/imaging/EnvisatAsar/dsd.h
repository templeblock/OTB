#ifndef dsd_h
#define dsd_h

#include <iostream>
#include <ossim/imaging/EnvisatAsar/EnvisatAsarRecord.h>
/**
 * @ingroup dsdRecord
 * @brief This class is able to read the ASAR dsd record 
 * @author Magellium, Pacome Dentraygues
 * @version 1.0
 * @date 23-11-07
 */
class dsd : public EnvisatAsarRecord
{
public:
	/**
	 * @brief Constructor
	 */
	dsd();

	/**
	 * @brief Destructor
	 */
	~dsd();

	/**
	 * @brief This function write the dsd in a stream
	 */
	friend std::ostream& operator<<(std::ostream& os, const dsd& data);

	/**
	 * @brief This function read a dsd from a stream
	 */
	friend std::istream& operator>>(std::istream& is, dsd& data);

	/**
	 * @brief Copy constructor
	 */
	dsd(const dsd& rhs);

	/**
	 * @brief Copy operator
	 */
	dsd& operator=(const dsd& rhs);

	/**
	 * @brief This function is able to create a new instance of the class
	 */
	EnvisatAsarRecord* Instanciate()
	{
		return new dsd();
	};

	/**
	 * @brief This function is able to create a new instance of the class initialised with the data of the calling instance
	 */
	EnvisatAsarRecord* Clone()
	{
		return new dsd(*this);
	};

	/**
	 * @brief Read the class data from a stream
	 */
	void Read(std::istream& is)
	{
		is>>*this;
	};

	/**
	 * @brief Write the class to a stream
	 */
	void Write(std::ostream& os)
	{
		os<<*this;
	};


	/**				
	* @brief Data set Name	
	*/				
	std::string   get_ds_name()	
	{
		return _ds_name;
	}
	/**				
	* @brief Data set Type	
	*/				
	std::string   get_ds_type()	
	{
		return _ds_type;
	}
	/**				
	* @brief Data set Filename
	*/				
	std::string   get_filename()	
	{
		return _filename;
	}
	/**				
	* @brief Data set Offset	
	*/				
	double   get_ds_offset()	
	{
		return _ds_offset;
	}
	/**				
	* @brief Data set Size	
	*/				
	double   get_ds_size()	
	{
		return _ds_size;
	}
	/**				
	* @brief Number of data set record	
	*/				
	long   get_num_dsr()	
	{
		return _num_dsr;
	}
	/**				
	* @brief Data set record size
	*/				
	long   get_dsr_size()	
	{
		return _dsr_size;
	}
	
	/**				
	* @brief Data set Name	
	*/				
	std::string   _ds_name;	
	/**				
	* @brief Data set Type	
	*/				
	std::string   _ds_type;	
	/**				
	* @brief Data set Filename
	*/				
	std::string   _filename;	
	/**				
	* @brief Data set Offset	
	*/				
	double	_ds_offset;	
	/**				
	* @brief Data set Size	
	*/				
	double	_ds_size;	
	/**				
	* @brief Number of data set record	
	*/				
	long	_num_dsr;	
	/**				
	* @brief Data set record size
	*/				
	long	_dsr_size;	

private:

};

#endif


