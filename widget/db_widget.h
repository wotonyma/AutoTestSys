#pragma once

#include <QWidget>

class QTableView;
class DBAbstractTableModel;
class QLayout;

class DBWidget : public QWidget
{
	Q_OBJECT

public:
	DBWidget(QWidget* parent = nullptr);
	~DBWidget();

private:
	void addDBOptionBtn(QLayout* opt_lyt);

	void selectProduce();
	void selectDcr();
	void selectShort();


	QTableView* _tbv;
	DBAbstractTableModel* _model;
};
