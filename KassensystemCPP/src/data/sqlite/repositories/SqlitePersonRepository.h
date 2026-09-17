#pragma once
#include "domain/repointerface/PersonRepository.h"
#include <QSqlQuery>

class SqlitePersonRepository : public PersonRepository
{
public:
	SqlitePersonRepository() = default;
	virtual ~SqlitePersonRepository() = default;
	virtual int64_t addPersonEntry(const entry::Person& entry) override;

	virtual std::expected<entry::Person, GetEntryException> findPersonEntry(int64_t personEntryID) const override;
	virtual std::vector<entry::Person> getAllPersonEntries() const override;
private:
	entry::Person getEntryFromQuery(const QSqlQuery& query) const;
};