#pragma once

#include <QAbstractTableModel>
#include <vector>
#include <memory>
#include "prod_data.h"
#include "prod_storage.h"
#include "model_data.h"

class DBAbstractTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	DBAbstractTableModel(QObject *parent = nullptr);
	~DBAbstractTableModel();

	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
	virtual bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
	virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

	enum class DataType {
		PRODUCE = 0,
		DCR,
		SHORT,
		INVALID
	};
	template<typename Cond>
	void setDataFromDB(DataType type, Cond&& cond);

	inline void setDataType(DataType t) { _data_type = t; }
	inline decltype(auto) getDataType() const { return _data_type; }
	inline void setDBStorage(const std::string& filename);

private:
	QVariant displayData(DataType type,const QModelIndex& index) const;
	bool updateData(DataType type, const QModelIndex& index, const QVariant& var);

	DataType _data_type;	//当前使用model
	QVector<QStringList> _horizontalHeader; //各model表头

	std::unique_ptr<ProdDB::ProdDBStorage> _db;

	ModelData<ProdData::Produce> _prods;
	ModelData<ProdData::ProdDCR> _dcrs;
	ModelData<ProdData::ProdShort> _shorts;
};

template<typename Cond>
inline void DBAbstractTableModel::setDataFromDB(DataType type, Cond&& cond)
{
	beginResetModel();

	using func = std::function<void(Cond&& cond)>;

	static const func querys[] = {
		[this](Cond&& cond) { _prods.set_data(_db->query<ProdData::Produce>(std::forward<Cond>(cond))); },
		[this](Cond&& cond) { _dcrs.set_data(_db->query<ProdData::ProdDCR>(std::forward<Cond>(cond))); },
		[this](Cond&& cond) { _shorts.set_data(_db->query<ProdData::ProdShort>(std::forward<Cond>(cond))); },
		[](Cond&&){ }
	};
	auto id = static_cast<size_t>(type); //data type id
	querys[id](std::forward<Cond>(cond));

	endResetModel();
}
