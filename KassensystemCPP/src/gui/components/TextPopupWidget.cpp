#include "gui/components/TextPopupWidget.h"
#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QScreen>
#include <QtGlobal>

TextPopupWidget::TextPopupWidget(QWidget* parent)
	: TextPopupWidget(PopupPos::bottomRight, parent) {}

TextPopupWidget::TextPopupWidget(PopupPos pos, QWidget* parent)
	: QToolButton(parent), pos(pos)
{
	setMaximumWidth(30);
	setArrowType(pos==PopupPos::bottomLeft || pos == PopupPos::bottomRight ? Qt::DownArrow : Qt::UpArrow);

	popup = new QFrame(this, Qt::Popup);
	popup->setFrameShape(QFrame::StyledPanel);

	auto* layout = new QVBoxLayout(popup);
	layout->setContentsMargins(0, 0, 0, 0);

	label = new QLabel(popup);
	label->setMargin(8);
	layout->addWidget(label);

	connect(this, &QToolButton::clicked, this, &TextPopupWidget::showPopup);

	setPlainText(QString());
}

void TextPopupWidget::setRichText(const QString& richText)
{
	label->setTextFormat(Qt::RichText);
	label->setText(richText);
}

void TextPopupWidget::setPlainText(const QString& plainText)
{
	label->setTextFormat(Qt::PlainText);
	label->setText(plainText);
}

void TextPopupWidget::setRichText(const std::string& richText)
{
	setRichText(QString::fromStdString(richText));
}

void TextPopupWidget::setPlainText(const std::string& plainText)
{
	setPlainText(QString::fromStdString(plainText));
}

void TextPopupWidget::setPopupPos(PopupPos pos)
{
	this->pos = pos;
	setArrowType(pos == PopupPos::bottomLeft || pos == PopupPos::bottomRight ? Qt::DownArrow : Qt::UpArrow);
}

void TextPopupWidget::showPopup()
{
	popup->adjustSize();
	const QSize s = popup->size();

	int x = 0;
	int y = 0;

	switch (pos)
	{
	case PopupPos::bottomLeft:
		x = width() - s.width();
		y = height();
		break;
	case PopupPos::bottomRight:
		x = 0;
		y = height();
		break;
	case PopupPos::topLeft:
		x = width() - s.width();
		y = -s.height();
		break;
	case PopupPos::topRight:
		x = 0;
		y = -s.height();
		break;
	}

	QPoint globalPos = mapToGlobal(QPoint(x, y));

	if (const QScreen* scr = screen())
	{
		const QRect avail = scr->availableGeometry();
		globalPos.setX(qBound(avail.left(), globalPos.x(), qMax(avail.left(), avail.right() - s.width() + 1)));
		globalPos.setY(qBound(avail.top(), globalPos.y(), qMax(avail.top(), avail.bottom() - s.height() + 1)));
	}

	popup->move(globalPos);
	popup->show();
}