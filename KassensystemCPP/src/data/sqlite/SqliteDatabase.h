#pragma once
#include <QSqlDatabase>
#include <string>

class SqliteDatabase
{
public:
	SqliteDatabase() = default;;
	bool openDatabase(const std::string& dbPath);
	void commitChanges();
	void rollbackChanges();

	QSqlDatabase& getConnection();
private:
	void initBlankDatabase(QSqlDatabase&);
};