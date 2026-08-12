#pragma once
#include <QWidget>
#include <vector>

class AddTab : public QWidget
{
	Q_OBJECT
public:
	AddTab(QWidget* parent = nullptr);

private:
	std::vector<AddTabEntry*> entries;
}