#pragma once
#include <QWidget>
#include <QToolButton>
#include <string>

class QFrame;
class QLabel;

class TextPopupWidget : public QToolButton
{
	Q_OBJECT
public:
	enum class PopupPos { bottomLeft, bottomRight, topLeft, topRight };

	explicit TextPopupWidget(QWidget* parent = nullptr);
	TextPopupWidget(PopupPos pos, QWidget* parent = nullptr);

	void setRichText(const std::string& richText);
	void setPlainText(const std::string& plainText);
	void setRichText(const QString& richText);
	void setPlainText(const QString& plainText);
	void setPopupPos(PopupPos pos);

private:
	void showPopup();

	QFrame* popup;
	QLabel* label;
	PopupPos pos;
};