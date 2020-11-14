// Copyright PlatoSpace.com All Rights Reserved.

#include "Mech/Living/MRMechPaperDollWidget.h"

void UMRMechPaperDollWidget::SetOwnerMech(AMRMech* InMech)
{
    OwnerMech = InMech;

    if (OwnerMech)
    {
        NotifyOwnerMechSet();
    }
}
