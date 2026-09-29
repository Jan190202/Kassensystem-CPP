#include <QToolTip>
#include <QCursor>
#include <QEvent>

class InstantToolTipFilter : public QObject
{
public:
	using QObject::QObject;

protected:
	bool eventFilter(QObject* obj, QEvent* event) override
	{
		if (event->type() == QEvent::Enter)
		{
			if (auto* w = qobject_cast<QWidget*>(obj); w && !w->toolTip().isEmpty())
				QToolTip::showText(QCursor::pos(), w->toolTip(), w);
		}
		return QObject::eventFilter(obj, event);
	}
};