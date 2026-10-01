#pragma once
#include <string>
#include <QString>
#include <optional>
#include <QSqlDatabase>

class SqliteDatabase
{
public:
	SqliteDatabase() = default;
	bool open(const std::string& dbPath);
	QSqlDatabase& getDatabase();

private:
	QSqlDatabase db;

	struct TableDef
	{
		QString name;
		QString body;
	};

	// structure definition
	const std::vector<TableDef>& tableDefinitions() const;

	// structure init
	QString createTableSql(const QString& name, const QString& body) const;
	void initBlankDatabase(QSqlDatabase& db);
	
	// structure check
	QMap<QString, QStringList> describeSchema(QSqlDatabase& db);
	std::optional<QMap<QString, QStringList>> expectedSchema();
	QStringList schemaDifferences(const QMap<QString, QStringList>& actual, const QMap<QString, QStringList>& expected) const;
	bool verifySchema(QSqlDatabase& db);
};