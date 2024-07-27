#ifndef SYSTEMUS_FILTERWIDGET_H
#define SYSTEMUS_FILTERWIDGET_H

#include <SystemusWidgets/global.h>

#include <QtWidgets/qwidget.h>

class QCheckBox;
class QFormLayout;

namespace Systemus {

namespace Ui {
class FilterWidget;
}

class FilterWidgetItem;

class FilterWidgetPrivate;
class SYSTEMUS_WIDGETS_EXPORT FilterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FilterWidget(QWidget *parent = nullptr);
    ~FilterWidget();

    QString searchQuery() const;
    Q_SLOT void setSearchQuery(const QString &query);
    Q_SIGNAL void searchQueryChanged(const QString &query);

    FilterWidgetItem *item(int index) const;
    int itemCount() const;

    FilterWidgetItem *addBool(const QString &name, const QString &property);
    FilterWidgetItem *addBool(const QString &name, const QString &property, const QString &checked, const QString &unchecked);
    FilterWidgetItem *addBool(const QString &name, const QString &property, const QString &checked, const QString &unchecked, const QString &partially);

    FilterWidgetItem *addInt(const QString &name, const QString &property);
    FilterWidgetItem *addInt(const QString &name, const QString &property, int min, int max);
    FilterWidgetItem *addInt(const QString &name, const QString &property, int min, int max, const QString &prefix, const QString &suffix);

    FilterWidgetItem *addDouble(const QString &name, const QString &property);
    FilterWidgetItem *addDouble(const QString &name, const QString &property, double min, double max);
    FilterWidgetItem *addDouble(const QString &name, const QString &property, double min, double max, const QString &prefix, const QString &suffix);

    FilterWidgetItem *addString(const QString &name, const QString &property);
    FilterWidgetItem *addString(const QString &name, const QString &property, int maxLength);
    FilterWidgetItem *addString(const QString &name, const QString &property, const QStringList &values);

    FilterWidgetItem *addDate(const QString &name, const QString &property);
    FilterWidgetItem *addDate(const QString &name, const QString &property, const QDate &min, const QDate &max);

    FilterWidgetItem *addTime(const QString &name, const QString &property);
    FilterWidgetItem *addTime(const QString &name, const QString &property, const QTime &min, const QTime &max);

    FilterWidgetItem *addDateTime(const QString &name, const QString &property);
    FilterWidgetItem *addDateTime(const QString &name, const QString &property, const QDateTime &min, const QDateTime &max);

private:
    Ui::FilterWidget *ui;
    QScopedPointer<FilterWidgetPrivate> d;

    friend class FilterWidgetItem;
};

class SYSTEMUS_WIDGETS_EXPORT FilterWidgetItem
{
public:
    enum ItemType {
        BoolItem,
        IntItem,
        DoubleItem,
        StringItem,
        DateItem,
        TimeItem
    };

    FilterWidgetItem(QWidget *widget, FilterWidget *filter);
    FilterWidgetItem(const QList<QWidget *> &widgets, FilterWidget *filter);
    virtual ~FilterWidgetItem();

    QString title() const;
    void setTitle(const QString &title);

    bool isActive() const;
    void setActive(bool active = true);

    QString propertyName() const;
    void setPropertyName(const QString &name);

    virtual bool hasValue() const;
    virtual QVariant value() const;
    virtual void setValue(const QVariant &value);

    QVariant minimum() const;
    void setMinimum(const QVariant &min);

    QVariant maximum() const;
    void setMaximum(const QVariant &max);

    virtual bool hasRange() const;
    virtual QVariantPair range() const;
    virtual void setRange(const QVariantPair &range);

    QWidget *widget(int index = 0) const;
    int widgetCount() const;
    QList<QWidget *> widgets() const;

    virtual int itemType() const = 0;

protected:
    virtual void translateItem();

    QCheckBox *_title;
    QFormLayout *_layout;

    QString _propertyName;

    QList<QWidget *> _widgets;
};

}

#endif // SYSTEMUS_FILTERWIDGET_H
