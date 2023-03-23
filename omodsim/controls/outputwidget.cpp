#include <QDateTime>
#include <QPainter>
#include <QTextStream>
#include "floatutils.h"
#include "outputwidget.h"
#include "ui_outputwidget.h"

///
/// \brief SimulationRole
///
const int SimulationRole = Qt::UserRole + 1;

///
/// \brief CaptureRole
///
const int CaptureRole = Qt::UserRole + 2;

///
/// \brief formatBinaryValue
/// \param pointType
/// \param value
/// \param outValue
/// \return
///
QString formatBinaryValue(QModbusDataUnit::RegisterType pointType, quint16 value, ByteOrder order, QVariant& outValue)
{
    QString result;
    value = toByteOrderValue(value, order);

    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            result = QString("<%1>").arg(value);
        break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
            result = QStringLiteral("<%1>").arg(value, 16, 2, QLatin1Char('0'));
        break;
        default:
        break;
    }
    outValue = value;
    return result;
}

///
/// \brief formatDecimalValue
/// \param pointType
/// \param value
/// \param outValue
/// \return
///
QString formatDecimalValue(QModbusDataUnit::RegisterType pointType, quint16 value, ByteOrder order, QVariant& outValue)
{
    QString result;
    value = toByteOrderValue(value, order);

    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            result = QStringLiteral("<%1>").arg(value, 1, 10, QLatin1Char('0'));
        break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
            result = QStringLiteral("<%1>").arg(value, 5, 10, QLatin1Char('0'));
        break;
        default:
        break;
    }
    outValue = value;
    return result;
}

///
/// \brief formatIntegerValue
/// \param pointType
/// \param value
/// \param outValue
/// \return
///
QString formatIntegerValue(QModbusDataUnit::RegisterType pointType, qint16 value ,ByteOrder order, QVariant& outValue)
{
    QString result;
    value = toByteOrderValue(value, order);

    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            result = QString("<%1>").arg(value);
        break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
            result = QStringLiteral("<%1>").arg(value, 5, 10, QLatin1Char(' '));
        break;
        default:
        break;
    }
    outValue = value;
    return result;
}

///
/// \brief formatHexValue
/// \param pointType
/// \param value
/// \param outValue
/// \return
///
QString formatHexValue(QModbusDataUnit::RegisterType pointType, quint16 value, ByteOrder order, QVariant& outValue)
{
    QString result;
    value = toByteOrderValue(value, order);

    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            result = QString("<%1>").arg(value);
        break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
            result = QStringLiteral("<%1H>").arg(value, 4, 16, QLatin1Char('0'));
        break;
        default:
        break;
    }
    outValue = value;
    return result.toUpper();
}

///
/// \brief formatFloatValue
/// \param pointType
/// \param value1
/// \param value2
/// \param flag
/// \param outValue
/// \return
///
QString formatFloatValue(QModbusDataUnit::RegisterType pointType, quint16 value1, quint16 value2, ByteOrder order, bool flag, QVariant& outValue)
{
    QString result;
    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            outValue = value1;
            result = QString("<%1>").arg(value1);
        break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
        {
            if(flag) break;

            const float value = makeFloat(value1, value2, order);
            outValue = value;
            result = QLocale().toString(value);
        }
        break;
        default:
        break;
    }
    return result;
}

///
/// \brief formatDoubleValue
/// \param pointType
/// \param value1
/// \param value2
/// \param value3
/// \param value4
/// \param flag
/// \param outValue
/// \return
///
QString formatDoubleValue(QModbusDataUnit::RegisterType pointType, quint16 value1, quint16 value2, quint16 value3, quint16 value4, ByteOrder order, bool flag, QVariant& outValue)
{
    QString result;
    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            outValue = value1;
            result = QString("<%1>").arg(value1);
        break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
        {
            if(flag) break;

            const double value = makeDouble(value1, value2, value3, value4, order);
            outValue = value;
            result = QLocale().toString(value);
        }
        break;
        default:
        break;
    }
    return result;
}

///
/// \brief formatAddress
/// \param pointType
/// \param address
/// \param hexFormat
/// \return
///
QString formatAddress(QModbusDataUnit::RegisterType pointType, int address, bool hexFormat)
{
    QString prefix;
    switch(pointType)
    {
        case QModbusDataUnit::Coils:
            prefix = "0";
        break;
        case QModbusDataUnit::DiscreteInputs:
            prefix = "1";
        break;
        case QModbusDataUnit::HoldingRegisters:
            prefix = "4";
        break;
        case QModbusDataUnit::InputRegisters:
            prefix = "3";
        break;
        default:
        break;
    }

    return hexFormat ? QStringLiteral("%1H").arg(address, 4, 16, QLatin1Char('0')) :
               prefix + QStringLiteral("%1").arg(address, 4, 10, QLatin1Char('0'));
}

///
/// \brief OutputListModel::OutputListModel
/// \param parent
///
OutputListModel::OutputListModel(OutputWidget* parent)
    : QAbstractListModel(parent)
    ,_parentWidget(parent)
    ,_iconPointGreen(QIcon(":/res/pointGreen.png"))
    ,_iconPointEmpty(QIcon(":/res/pointEmpty.png"))
{
}

///
/// \brief OutputListModel::rowCount
/// \return
///
int OutputListModel::rowCount(const QModelIndex&) const
{
    return _parentWidget->_displayDefinition.Length;
}

///
/// \brief OutputListModel::data
/// \param index
/// \param role
/// \return
///
QVariant OutputListModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid() ||
       !_mapItems.contains(index.row()))
    {
        return QVariant();
    }

    const auto row = index.row();
    const auto pointType = _parentWidget->_displayDefinition.PointType;
    const auto hexAddresses = _parentWidget->displayHexAddresses();

    const ItemData& itemData = _mapItems[row];
    const auto addrstr = formatAddress(pointType, itemData.Address, hexAddresses);

    switch(role)
    {
        case Qt::DisplayRole:
            return QString("%1: %2                ").arg(addrstr, itemData.ValueStr);

        case CaptureRole:
            return QString(itemData.ValueStr).remove('<').remove('>');

        case Qt::DecorationRole:
            return itemData.Simulated ? _iconPointGreen : _iconPointEmpty;

        case Qt::UserRole:
            return QVariant::fromValue(itemData);
    }

    return QVariant();
}

///
/// \brief OutputListModel::setData
/// \param index
/// \param value
/// \param role
/// \return
///
bool OutputListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
        return false;

    switch (role)
    {
        case SimulationRole:
        {
            if(_mapItems.contains(index.row()))
            {
                _mapItems[index.row()].Simulated = value.toBool();

                emit dataChanged(index, index, QList<int>() << role);
                return true;
            }
        }
        break;

        default:
        break;
    }

    return false;
}

///
/// \brief OutputListModel::isUpdated
/// \return
///
bool OutputListModel::isValid() const
{
    return _lastData.isValid();
}

///
/// \brief OutputListModel::values
/// \return
///
QVector<quint16> OutputListModel::values() const
{
    return _lastData.values();
}

///
/// \brief OutputListModel::clear
///
void OutputListModel::clear()
{
    _mapItems.clear();
    updateData(QModbusDataUnit());
}

///
/// \brief OutputListModel::update
///
void OutputListModel::update()
{
    updateData(_lastData);
}

///
/// \brief OutputListModel::updateData
/// \param data
///
void OutputListModel::updateData(const QModbusDataUnit& data)
{
    _lastData = data;

    const auto mode = _parentWidget->dataDisplayMode();
    const auto pointType = _parentWidget->_displayDefinition.PointType;
    const auto byteOrder = _parentWidget->byteOrder();

    for(int i = 0; i < rowCount(); i++)
    {
        const auto value = _lastData.value(i);

        auto& itemData = _mapItems[i];
        itemData.Address = _parentWidget->_displayDefinition.PointAddress + i;

        switch(mode)
        {
            case DataDisplayMode::Binary:
                itemData.ValueStr = formatBinaryValue(pointType, value, byteOrder, itemData.Value);
            break;

            case DataDisplayMode::Decimal:
                itemData.ValueStr = formatDecimalValue(pointType, value, byteOrder, itemData.Value);
            break;

            case DataDisplayMode::Integer:
                itemData.ValueStr = formatIntegerValue(pointType, value, byteOrder, itemData.Value);
            break;

            case DataDisplayMode::Hex:
                itemData.ValueStr = formatHexValue(pointType, value, byteOrder, itemData.Value);
            break;

            case DataDisplayMode::FloatingPt:
                itemData.ValueStr = formatFloatValue(pointType, value, _lastData.value(i+1), byteOrder,
                                          (i%2) || (i+1>=rowCount()), itemData.Value);
            break;

            case DataDisplayMode::SwappedFP:
                itemData.ValueStr = formatFloatValue(pointType, _lastData.value(i+1), value, byteOrder,
                                          (i%2) || (i+1>=rowCount()), itemData.Value);
            break;

            case DataDisplayMode::DblFloat:
                itemData.ValueStr = formatDoubleValue(pointType, value, _lastData.value(i+1), _lastData.value(i+2), _lastData.value(i+3),
                                           byteOrder, (i%4) || (i+3>=rowCount()), itemData.Value);
            break;

            case DataDisplayMode::SwappedDbl:
                itemData.ValueStr = formatDoubleValue(pointType, _lastData.value(i+3), _lastData.value(i+2), _lastData.value(i+1), value,
                                           byteOrder, (i%4) || (i+3>=rowCount()), itemData.Value);
            break;
        }
    }

    emit dataChanged(index(0), index(rowCount() - 1), QList<int>() << Qt::DisplayRole);
}

///
/// \brief OutputListModel::find
/// \param type
/// \param addr
/// \return
///
QModelIndex OutputListModel::find(QModbusDataUnit::RegisterType type, quint16 addr) const
{
    if(_parentWidget->_displayDefinition.PointType != type)
        return QModelIndex();

    const int row = addr - _parentWidget->_displayDefinition.PointAddress;
    if(row >= 0 && row < rowCount())
        return index(row);

    return QModelIndex();
}

///
/// \brief OutputWidget::OutputWidget
/// \param parent
///
OutputWidget::OutputWidget(QWidget *parent) :
     QWidget(parent)
   , ui(new Ui::OutputWidget)
   ,_displayHexAddreses(false)
   ,_displayMode(DisplayMode::Data)
   ,_dataDisplayMode(DataDisplayMode::Hex)
   ,_byteOrder(ByteOrder::LittleEndian)
   ,_listModel(new OutputListModel(this))
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->listView->setModel(_listModel.get());
    ui->labelStatus->setAutoFillBackground(true);

    setAutoFillBackground(true);
    setForegroundColor(Qt::black);
    setBackgroundColor(Qt::lightGray);

    setStatusColor(Qt::red);
    setNotConnectedStatus();
}

///
/// \brief OutputWidget::~OutputWidget
///
OutputWidget::~OutputWidget()
{
    delete ui;
}

///
/// \brief OutputWidget::changeEvent
/// \param event
///
void OutputWidget::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        if(!_listModel->isValid())
            setNotConnectedStatus();
    }

    QWidget::changeEvent(event);
}

///
/// \brief OutputWidget::data
/// \return
///
QVector<quint16> OutputWidget::data() const
{
    return _listModel->values();
}

///
/// \brief OutputWidget::setup
/// \param dd
/// \param simulations
/// \param data
///
void OutputWidget::setup(const DisplayDefinition& dd, const ModbusSimulationMap& simulations, const QModbusDataUnit& data)
{
    _displayDefinition = dd;

    _listModel->clear();
    for(auto&& key : simulations.keys())
        _listModel->setData(_listModel->find(key.first, key.second), true, SimulationRole);

    updateData(data);
}

///
/// \brief OutputWidget::displayHexAddresses
/// \return
///
bool OutputWidget::displayHexAddresses() const
{
    return _displayHexAddreses;
}

///
/// \brief OutputWidget::setDisplayHexAddresses
/// \param on
///
void OutputWidget::setDisplayHexAddresses(bool on)
{
    _displayHexAddreses = on;
    _listModel->update();
}

///
/// \brief OutputWidget::backgroundColor
/// \return
///
QColor OutputWidget::backgroundColor() const
{
    return ui->listView->palette().color(QPalette::Base);
}

///
/// \brief OutputWidget::setBackgroundColor
/// \param clr
///
void OutputWidget::setBackgroundColor(const QColor& clr)
{
    auto pal = palette();
    pal.setColor(QPalette::Base, clr);
    pal.setColor(QPalette::Window, clr);
    setPalette(pal);
}

///
/// \brief OutputWidget::foregroundColor
/// \return
///
QColor OutputWidget::foregroundColor() const
{
    return ui->listView->palette().color(QPalette::Text);
}

///
/// \brief OutputWidget::setForegroundColor
/// \param clr
///
void OutputWidget::setForegroundColor(const QColor& clr)
{
    auto pal = ui->listView->palette();
    pal.setColor(QPalette::Text, clr);
    ui->listView->setPalette(pal);
}

///
/// \brief OutputWidget::statusColor
/// \return
///
QColor OutputWidget::statusColor() const
{
    return ui->labelStatus->palette().color(QPalette::WindowText);
}

///
/// \brief OutputWidget::setStatusColor
/// \param clr
///
void OutputWidget::setStatusColor(const QColor& clr)
{
    auto pal = ui->labelStatus->palette();
    pal.setColor(QPalette::WindowText, clr);
    ui->labelStatus->setPalette(pal);
}

///
/// \brief OutputWidget::font
/// \return
///
QFont OutputWidget::font() const
{
    return ui->listView->font();
}

///
/// \brief OutputWidget::setFont
/// \param font
///
void OutputWidget::setFont(const QFont& font)
{
    ui->listView->setFont(font);
    ui->labelStatus->setFont(font);
}

///
/// \brief OutputWidget::setStatus
/// \param status
///
void OutputWidget::setStatus(const QString& status)
{
    if(status.isEmpty())
    {
        ui->labelStatus->setText(status);
    }
    else
    {
        const auto info = QString("*** %1 ***").arg(status);
        if(info != ui->labelStatus->text())
        {
            ui->labelStatus->setText(info);
        }
    }
}

///
/// \brief OutputWidget::paint
/// \param rc
/// \param painter
///
void OutputWidget::paint(const QRect& rc, QPainter& painter)
{
    const auto textStatus = ui->labelStatus->text();
    auto rcStatus = painter.boundingRect(rc.left(), rc.top(), rc.width(), rc.height(), Qt::TextWordWrap, textStatus);
    painter.drawText(rcStatus, Qt::TextWordWrap, textStatus);

    rcStatus.setBottom(rcStatus.bottom() + 4);
    painter.drawLine(rc.left(), rcStatus.bottom(), rc.right(), rcStatus.bottom());
    rcStatus.setBottom(rcStatus.bottom() + 4);

    int cx = rc.left();
    int cy = rcStatus.bottom();
    for(int i = 0; i < _listModel->rowCount(); ++i)
    {
        const auto text = _listModel->data(_listModel->index(i), Qt::DisplayRole).toString().trimmed();
        auto rcItem = painter.boundingRect(cx, cy, rc.width() - cx, rc.height() - cy, Qt::TextSingleLine, text);

        if(rcItem.right() > rc.right()) break;
        else if(rcItem.bottom() < rc.bottom())
        {
            painter.drawText(rcItem, Qt::TextSingleLine, text);
        }
        else
        {
            cy = rcStatus.bottom();
            cx = rcItem.right() + 10;

            rcItem = painter.boundingRect(cx, cy, rc.width() - cx, rc.height() - cy, Qt::TextSingleLine, text);
            if(rcItem.right() > rc.right()) break;

            painter.drawText(rcItem, Qt::TextSingleLine, text);
        }

        cy += rcItem.height();
    }
}

///
/// \brief OutputWidget::updateTraffic
/// \param request
/// \param server
///
void OutputWidget::updateTraffic(const QModbusRequest& request, int server)
{
    updateTrafficWidget(true, server, request);
}

///
/// \brief OutputWidget::updateTraffic
/// \param response
/// \param server
///
void OutputWidget::updateTraffic(const QModbusResponse& response, int server)
{
    updateTrafficWidget(false, server, response);
}

///
/// \brief OutputWidget::updateData
///
void OutputWidget::updateData(const QModbusDataUnit& data)
{
    _listModel->updateData(data);
}

///
/// \brief OutputWidget::setSimulated
/// \param type
/// \param addr
/// \param on
///
void OutputWidget::setSimulated(QModbusDataUnit::RegisterType type, quint16 addr, bool on)
{
    _listModel->setData(_listModel->find(type, addr), on, SimulationRole);
}

///
/// \brief OutputWidget::displayMode
/// \return
///
DisplayMode OutputWidget::displayMode() const
{
    return _displayMode;
}

///
/// \brief OutputWidget::setDisplayMode
/// \param mode
///
void OutputWidget::setDisplayMode(DisplayMode mode)
{
    _displayMode = mode;
    switch(mode)
    {
        case DisplayMode::Data:
            ui->stackedWidget->setCurrentIndex(0);
        break;

        case DisplayMode::Traffic:
            ui->stackedWidget->setCurrentIndex(1);
        break;

        default:
        break;
    }
}

///
/// \brief OutputWidget::dataDisplayMode
/// \return
///
DataDisplayMode OutputWidget::dataDisplayMode() const
{
    return _dataDisplayMode;
}

///
/// \brief OutputWidget::setDataDisplayMode
/// \param mode
///
void OutputWidget::setDataDisplayMode(DataDisplayMode mode)
{
    _dataDisplayMode = mode;
    _listModel->update();
}

///
/// \brief OutputWidget::byteOrder
/// \return
///
const ByteOrder& OutputWidget::byteOrder() const
{
    return _byteOrder;
}

///
/// \brief OutputWidget::setByteOrder
/// \param order
///
void OutputWidget::setByteOrder(ByteOrder order)
{
    _byteOrder = order;
    _listModel->update();
}


///
/// \brief OutputWidget::on_listView_doubleClicked
/// \param item
///
void OutputWidget::on_listView_doubleClicked(const QModelIndex& index)
{
    if(!index.isValid()) return;
    auto itemData = _listModel->data(index, Qt::UserRole).value<OutputListModel::ItemData>();

    switch(_displayDefinition.PointType)
    {
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
        {
            switch(_dataDisplayMode)
            {
                case DataDisplayMode::FloatingPt:
                case DataDisplayMode::SwappedFP:
                    if(index.row() % 2)
                    {
                        const auto idx = _listModel->index(index.row() - 1);
                        if(idx.isValid()) itemData = _listModel->data(idx, Qt::UserRole).value<OutputListModel::ItemData>();
                    }
                break;

                case DataDisplayMode::DblFloat:
                case DataDisplayMode::SwappedDbl:
                    if(index.row() % 4)
                    {
                        const auto idx = _listModel->index(index.row() - index.row() % 4);
                        if(idx.isValid()) itemData = _listModel->data(idx, Qt::UserRole).value<OutputListModel::ItemData>();
                    }
                break;

                default:
                break;
            }
        }
        break;

        default:
        break;
    }

    emit itemDoubleClicked(itemData.Address, itemData.Value);
}

///
/// \brief OutputWidget::setNotConnectedStatus
///
void OutputWidget::setNotConnectedStatus()
{
    setStatus(tr("NOT CONNECTED!"));
}

///
/// \brief OutputWidget::setInvalidLengthStatus
///
void OutputWidget::setInvalidLengthStatus()
{
    setStatus(tr("Invalid Data Length Specified"));
}

///
/// \brief OutputWidget::updateTrafficWidget
/// \param request
/// \param pdu
///
void OutputWidget::updateTrafficWidget(bool request, int server, const QModbusPdu& pdu)
{
    QByteArray rawData;
    rawData.push_back(server);
    rawData.push_back(pdu.functionCode() | ( pdu.isException() ? QModbusPdu::ExceptionByte : 0));
    rawData.push_back(pdu.data());

    QString text;
    for(auto&& c : rawData)
    {
        switch(_dataDisplayMode)
        {
            case DataDisplayMode::Decimal:
            case DataDisplayMode::Integer:
                text+= QString("[%1]").arg(QString::number((uchar)c), 3, '0');
            break;

            default:
                text+= QString("[%1]").arg(QString::number((uchar)c, 16), 2, '0');
            break;
        }
    }
    if(text.isEmpty()) return;

    ui->plainTextEdit->moveCursor(QTextCursor::End);

    QTextCharFormat fmt;
    fmt.setForeground(request? foregroundColor() : Qt::white);
    fmt.setBackground(request? Qt::transparent : Qt::black);
    ui->plainTextEdit->mergeCurrentCharFormat(fmt);

    if(request && ui->plainTextEdit->toPlainText().length() > 22000)
        ui->plainTextEdit->clear();

    ui->plainTextEdit->insertPlainText(text);
    ui->plainTextEdit->moveCursor(QTextCursor::End);
}
