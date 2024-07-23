#include "db_widget.h"

#include <QTableView>
#include <QHeaderView>
#include <QLayout>
#include <QPushButton>
#include "DBAbstractTableModel.h"
#include "sqlite_orm/sqlite_orm.h"
#include "prod_table.h"

DBWidget::DBWidget(QWidget *parent)
	: QWidget(parent)
	, _tbv(new QTableView) 
	, _model(new DBAbstractTableModel)
{
	_tbv->setModel(_model);
	_tbv->verticalHeader()->hide();

	resize(600, 200);

	auto lyt_v0 = new QVBoxLayout(this);
	auto lyt_v0_h0 = new QHBoxLayout; //DB功能操作区按钮布局
	addDBOptionBtn(lyt_v0_h0);
	lyt_v0->addLayout(lyt_v0_h0);
	lyt_v0->addWidget(_tbv);
}

DBWidget::~DBWidget()
{
}

void DBWidget::addDBOptionBtn(QLayout* opt_lyt)
{
	auto select_btn = new QPushButton("select");
	auto insert_btn = new QPushButton("insert");
	auto update_btn = new QPushButton("update");
	auto remove_btn = new QPushButton("remove");
	auto dcr_btn = new QPushButton("dcr view");
	auto short_btn = new QPushButton("short view");


	opt_lyt->addWidget(select_btn);
	opt_lyt->addWidget(insert_btn);
	opt_lyt->addWidget(update_btn);
	opt_lyt->addWidget(remove_btn);
	opt_lyt->addWidget(dcr_btn);
	opt_lyt->addWidget(short_btn);

	using namespace sqlite_orm;
	connect(select_btn, &QPushButton::clicked, [this] {
		
		bool filterById = false;
		bool filterByData = false;
		auto cond = where(
			(not filterById or lesser_or_equal(&ProdData::Produce::id, 10))
			&& (not filterByData or lesser_or_equal(&ProdData::Produce::date, "2024-06-30"))
			);

		//_model->updateItems(_model->getDataType(), std::move(cond));
		auto type = _model->getDataType();
		if (type == DBAbstractTableModel::DataType::PRODUCE) {
			_model->setDataFromDB(_model->getDataType(), where(lesser_or_equal(&ProdData::Produce::id, 100)));
		}
		else {
			_model->setDataFromDB(_model->getDataType(), where(lesser_or_equal(&ProdData::ProdDCR::id, 100)));
		}
		
		});
	connect(insert_btn, &QPushButton::clicked, [this] {
		_model->insertRow(1);
		});
	connect(update_btn, &QPushButton::clicked, [this] {});
	connect(remove_btn, &QPushButton::clicked, [this] {
		_model->removeRows(1, 5);
		});
	connect(dcr_btn, &QPushButton::clicked, [this] {
		_model->setDataType(DBAbstractTableModel::DataType::DCR);
		});
	connect(short_btn, &QPushButton::clicked, [this] {});
}

void DBWidget::selectProduce()
{
	/*
	* Cond c;
	* if (true) c = c && c1;
	* if (true) c = c && c2;
	* 上述方式在sqlite-orm无法使用(毕竟是静态模板), libodb里可以使用
	* sqlite 由于利用静态模板,所以无法进行运行时动态条件组合,需要特殊处理
	* https://github.com/fnc12/sqlite_orm/issues/272
	*/
	using namespace sqlite_orm;
	using namespace ProdData;
	bool filterBySn = false;
	bool filterByModel = false;
	bool filterByState = false;
	bool filterByDate = false;
	
	std::string sn_c;
	std::string model_c;
	uint16_t state_c{};
	std::string date_c;

	auto cond = where(
		(not filterBySn or is_equal(&Produce::sn, sn_c))
		&& (not filterByModel or is_equal(&Produce::model, model_c))
		&& (not filterByState or is_equal(&Produce::state, state_c))
		&& (not filterByDate or lesser_or_equal(&Produce::date, date_c))
		);

	auto type = _model->getDataType();
	if (DBAbstractTableModel::DataType::PRODUCE == type)
		_model->setDataFromDB(type, std::move(cond));

}
