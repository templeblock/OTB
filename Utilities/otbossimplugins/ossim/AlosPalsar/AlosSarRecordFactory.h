#ifndef AlosSarRecordFactory_h
#define AlosSarRecordFactory_h


#include <AlosSarRecordHeader.h>
#include <AlosSarRecord.h>
#include <map>
/**
 * @ingroup AlosSar
 * @brief This class is a facory able to construct Record base classes given the id of the record wanted
 * @author Magellium, Pacome Dentraygues
 * @version 1.0
 * @date 23-11-07
 */
class AlosSarRecordFactory
{
public:
	/**
	 * @brief Constructor
	 */
	AlosSarRecordFactory();
	/**
	 * @brief Destructor
	 */
	~AlosSarRecordFactory();

	/**
	 * @brief Add a new Record type available in this factory
	 * @param record Record to add in the factory
	 * @param id Id of the new avalaible Record
	 */
	void RegisterRecord(int id, AlosSarRecord * record);

	/**
	 * @brief Instanciate a new Record
	 * @param id Id of the Record we want to instanciate
	 */
	AlosSarRecord* Instanciate(int id) ;
protected:

	/**
	 * @brief Contain all the available Records for the factory
	 */
	std::map<int, AlosSarRecord*> _availableRecords;

private:
};

#endif