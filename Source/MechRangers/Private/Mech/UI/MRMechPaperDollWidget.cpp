// Copyright PlatoSpace.com All Rights Reserved.

#include "Mech/UI/MRMechPaperDollWidget.h"

void UMRMechPaperDollWidget::SetOwnerMech(AMRMech* InMech)
{
    OwnerMech = InMech;

    if (OwnerMech)
    {
        NotifyOwnerMechSet();
    }
}
