#pragma once

#include <QDialog>
#include <QWidget>
#include <string>

class BalanceTabDialog : public QDialog
{
	Q_OBJECT
public:
	BalanceTabDialog(const std::string& dlgName, QWidget* parent);
private:

};