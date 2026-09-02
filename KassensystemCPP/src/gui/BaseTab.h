#pragma once
#include <QWidget>

class BaseTab : public QWidget
{
	Q_OBJECT
public:
	explicit BaseTab(QWidget* parent = nullptr) : QWidget(parent) {};
	virtual ~BaseTab() = default;
	virtual void initialize() = 0;
	virtual void refresh() = 0;
};