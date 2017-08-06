#include "nuvotonmod.h"

NuvotonMod::NuvotonMod(QObject *parent )
{
  this->setParent(parent);
}

NuvotonMod::~NuvotonMod()
{
  qDebug() << "Desrtuctor NuvotonMod";
}

//double NuvotonMod::test()
//{
//  return 3;
//}
