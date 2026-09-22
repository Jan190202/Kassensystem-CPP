#pragma once
#include <QDialog>
#include <string>

class QLineEdit;

class PayTabAddPersonDialog : public QDialog
{
	Q_OBJECT
public:
	struct inputs
	{
		std::string firstName, lastName, nickName, info;
	};

	PayTabAddPersonDialog(QWidget* parent);

	PayTabAddPersonDialog::inputs getInputs() const;

private:
	QLineEdit*		edtFirstName;
	QLineEdit*		edtLastName;
	QLineEdit*		edtNickName;
	QLineEdit*		edtInfo;
};