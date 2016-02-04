#include "MakeOutputProjections.C"
#include "MakeAvgSepCFs.C"
#include "DrawUnityLineAvgSep.C"

void RunAllAvgSep()
{
  MakeOutputProjections();

  MakeAvgSepCFs();

  DrawUnityLineAvgSep();
}
