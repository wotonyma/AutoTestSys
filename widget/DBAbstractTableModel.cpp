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
    using func = std::function<int()>;
    static const func get_row[] = {
        [this]() { return _prods.rows(); },
        [this]() { return _dcrs.rows(); },
        [this]() { return _shorts.rows(); },
        []() { return -1; } /*invalid type*/
    };
    size_t id = static_cast<size_t>(_data_type);
    return get_row[id]();
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

    using namespace ProdData;
    using func = std::function<void()>;
    static const func insert[] = {
        [this]() { _prods.insert(Produce{-1, "", "", 0, std::nullopt}); },
        [this]() { _dcrs.insert(ProdDCR{0, 0, 0, 0}); },
        [this]() { _shorts.insert(ProdShort{ 0, 0, 0, 0, 0 }); },
        [](){ }
    };
    size_t id = static_cast<size_t>(_data_type);
    insert[id]();

    endInsertRows();
    return true;
}

bool DBAbstractTableModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if (row < 0 || row > rowCount() - 1 || count < 1)
        return false;

    beginRemoveRows(parent, row, row + count - 1);
    using func = std::function<void(int)>;
    static const func remove[] = {
        [this](int ri) { _prods.remove(ri); },
        [this](int ri) { _dcrs.remove(ri); },
        [this](int ri) { _shorts.remove(ri); },
        [](int){  }
    };

    size_t id = static_cast<size_t>(_data_type);
    remove[id](row);

    endRemoveRows();
    return true;
}

void DBAbstractTableModel::setDBStorage(const std::string& filename)
{
    _db = std::make_unique<ProdDB::ProdDBStorage>(filename);
}

QVariant DBAbstractTableModel::displayData(DataType type, const QModelIndex& index) const
{
    int row = index.row();
    int col = index.column();

    using func = std::function<QVariant(int, int)>;
    static const func disp[] = {
        [this](int row, int col){ return _prods.elem_to_qvar(row, col); },
        [this](int row, int col){ return _dcrs.elem_to_qvar(row, col); },
        [this](int row, int col){ return _shorts.elem_to_qvar(row, col); },
        [](int, int) { return QVariant(); }
    };
    size_t id = static_cast<size_t>(type);
    return disp[id](row, col);
}

bool DBAbstractTableModel::updateData(DataType type, const QModelIndex& index, const QVariant& var)
{
    int row = index.row();
    int col = index.column();

    using func = std::function<void(int, int, const QVariant&)>;
    static const func update[] = {
        [this](int row, int col, const QVariant& var) { _prods.elem_from_qvar(row, col, var); },
        [this](int row, int col, const QVariant& var) { _prods.elem_from_qvar(row, col, var); },
        [this](int row, int col, const QVariant& var) { _prods.elem_from_qvar(row, col, var); },
        [](int, int, const QVariant&) {  },
    };
    
    auto id = static_cast<size_t>(type);
    update[id](row, col, var);

    emit dataChanged(index, index);
    return true;
}

