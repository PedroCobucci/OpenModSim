#include "modbuslimits.h"
#include "displaydefinition.h"
#include "dialogdisplaydefinition.h"
#include "ui_dialogdisplaydefinition.h"

///
/// \brief DialogDisplayDefinition::DialogDisplayDefinition
/// \param parent
///
DialogDisplayDefinition::DialogDisplayDefinition(FormModSim* parent) :
    QFixedSizeDialog(parent),
    ui(new Ui::DialogDisplayDefinition)
{
    ui->setupUi(this);
    ui->lineEditScanRate->setInputRange(20, 10000);
    ui->lineEditPointAddress->setInputRange(ModbusLimits::addressRange());
    ui->lineEditLength->setInputRange(ModbusLimits::lengthRange());
    ui->lineEditSlaveAddress->setInputRange(ModbusLimits::slaveRange());

    const auto dd = parent->displayDefinition();
    ui->lineEditScanRate->setValue(dd.ScanRate);
    ui->lineEditPointAddress->setValue(dd.PointAddress);
    ui->lineEditSlaveAddress->setValue(dd.DeviceId);
    ui->lineEditLength->setValue(dd.Length);
    ui->comboBoxPointType->setCurrentPointType(dd.PointType);

    ui->buttonBox->setFocus();
}

///
/// \brief DialogDisplayDefinition::~DialogDisplayDefinition
///
DialogDisplayDefinition::~DialogDisplayDefinition()
{
    delete ui;
}

///
/// \brief DialogDisplayDefinition::accept
///
void DialogDisplayDefinition::accept()
{
    DisplayDefinition dd;
    dd.DeviceId = ui->lineEditSlaveAddress->value<int>();
    dd.PointAddress = ui->lineEditPointAddress->value<int>();
    dd.PointType = ui->comboBoxPointType->currentPointType();
    dd.Length = ui->lineEditLength->value<int>();
    dd.ScanRate = ui->lineEditScanRate->value<int>();
    ((FormModSim*)parentWidget())->setDisplayDefinition(dd);

    QFixedSizeDialog::accept();
}
