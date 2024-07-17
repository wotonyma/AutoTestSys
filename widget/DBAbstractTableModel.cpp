#include "DBAbstractTableModel.h"

#include "sqlite_orm/sqlite_orm.h"

#include "prod_table.h"

#include "qvariant_cvt.hpp"

DBAbstractTableModel::DBAbstractTableModel(QObject *parent)
	: QAbstractTableModel(parent)
    , _db(nullptr)
{
    _db = std::make_unique<ProdDB::ProdDBStorage>("prod.db");

    _horizontalHeader = {
        { "id", "序列号", "类型", "状态", "日期" },
        { "id", "线路", "状态", "电阻" },
        { "id", "线路1", "线路2", "状态", "电阻" }
    };
    //_data_type = DataType::INVALID;
    _data_type = DataType::PRODUCE;
}

DBAbstractTableModel::~DBAbstractTableModel()
{
}

int DBAbstractTableModel::rowCount(const QModelIndex& parent) const
{
    switch (_data_type)
    {
    case DBAbstractTableModel::DataType::PRODUCE:
        return _prods.size();
        break;
    case DBAbstractTableModel::DataType::DCR:
        return _dcrs.size();
        break;
    case DBAbstractTableModel::DataType::SHORT:
        return _shorts.size();
        break;
    case DBAbstractTableModel::DataType::INVALID:
        break;
    default:
        break;
    }
	return -1;
}

int DBAbstractTableModel::columnCount(const QModelIndex& parent) const
{
    if (_data_type == DataType::INVALID) 
    { 
        return -1;
    }
    size_t idx = static_cast<size_t>(_data_type);
	return _horizontalHeader[idx].size();
}

QVariant DBAbstractTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
	    return QVariant();

    switch (role)
    {
        /*显示数据*/
    case Qt::DisplayRole:
        return displayData(_data_type, index);
        break;
        /*对齐处理*/
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
        break;
        /*字体处理*/
    case Qt::FontRole:
    default:
        return QVariant();
        break;
    }
    return QVariant();
}

QVariant DBAbstractTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    size_t idx = static_cast<size_t>(_data_type);
    if (orientation == Qt::Horizontal) {    //水平表头
        if (Qt::DisplayRole == role && section < _horizontalHeader[idx].count()) {
            return _horizontalHeader[idx][section];
        }
    }
    else if (orientation == Qt::Vertical) {
        //操作差不多，返回垂直表头，如果需要的话
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

/// <summary>
/// 这个是可选函数，如果表格使用了渲染即delegate，那么这个函数可以设置哪一些行列可以编辑
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
Qt::ItemFlags DBAbstractTableModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::ItemFlags();

    //col 3 = state
    if (index.column() != 0)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

    return QAbstractTableModel::flags(index);
}

bool DBAbstractTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    switch (role)
    {
    case Qt::EditRole:
        return updateData(_data_type, index, value);
        break;
    default:
        break;
    }
    return false;
}

bool DBAbstractTableModel::insertRows(int row, int count, const QModelIndex& parent)
{
    if (row < 0 || row > rowCount() || count < 1)
        return false;
    /*从什么位置开始插入 first为插入后row的index, last为插入后最后row的index*/
    beginInsertRows(parent, row, row + count - 1);
    if (_data_type == DataType::PRODUCE) {
        for (int i = 0; i < count; ++i)
            _prods.emplace_back(ProdData::Produce{ -1, "", "", 0, std::nullopt }); //只支持尾部插入
    }
    else if (_data_type == DataType::DCR) {
        for (int i = 0; i < count; ++i)
            _dcrs.emplace_back(ProdData::ProdDCR{0, 0, 0, 0}); //只支持尾部插入
    }
    else if (_data_type == DataType::SHORT) {
        for (int i = 0; i < count; ++i)
            _shorts.emplace_back(ProdData::ProdShort{ 0, 0, 0, 0, 0 });
    }
    else {}

    endInsertRows();
    return true;
}

bool DBAbstractTableModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if (row < 0 || row > rowCount() - 1 || count < 1)
        return false;

    beginRemoveRows(parent, row, row + count - 1);

    if (_data_type == DataType::PRODUCE)
        _prods.erase(_prods.begin() + row, _prods.begin() + row + count - 1);
    else if (_data_type == DataType::DCR)
        _dcrs.erase(_dcrs.begin() + row, _dcrs.begin() + row + count - 1);
    else if (_data_type == DataType::SHORT)
        _shorts.erase(_shorts.begin() + row, _shorts.begin() + row + count - 1);
    else {}


    endRemoveRows();
    return false;
}

void DBAbstractTableModel::setDBStorage(const std::string& filename)
{
    _db = std::make_unique<ProdDB::ProdDBStorage>(filename);
}

QVariant DBAbstractTableModel::displayData(DataType type, const QModelIndex& index) const
{
    int row = index.row();
    int col = index.column();
    switch (type)
    {
    case DataType::PRODUCE:
        return qutils::cvt_from_element(_prods[row], col);
        break;
    case DataType::DCR:
        return qutils::cvt_from_element(_dcrs[row], col);
        break;
    case DataType::SHORT:
        return qutils::cvt_from_element(_shorts[row], col);
        break;
    case DataType::INVALID:
    default:
        break;
    }
    return QVariant();
}

bool DBAbstractTableModel::updateData(DataType type, const QModelIndex& index, const QVariant& var)
{
    if (!var.isValid() || var.isNull())
        return false;

    int row = index.row();
    int col = index.column();
    switch (type)
    {
    case DataType::PRODUCE:
        qutils::cvt_to_element(var, _prods[row], col);
        break;
    case DBAbstractTableModel::DataType::DCR:
        qutils::cvt_to_element(var, _dcrs[row], col);
        break;
    case DBAbstractTableModel::DataType::SHORT:
        qutils::cvt_to_element(var, _shorts[row], col);
        break;
    case DBAbstractTableModel::DataType::INVALID:
    default:
        return false;
    }

    emit dataChanged(index, index);
    return true;
}

