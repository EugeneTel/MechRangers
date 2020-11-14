// Copyright PlatoSpace.com All Rights Reserved.

#include "UI/MRDisplayWidgetComponent.h"

#include "Mech/Living/MRMechPaperDollWidget.h"

void UMRDisplayWidgetComponent::SetOwnerMech(AMRMech* InMech)
{
    OwnerMech = InMech;

    // set owner to widget
    UMRMechPaperDollWidget* PaperDollWidget = Cast<UMRMechPaperDollWidget>(GetUserWidgetObject());
    if (PaperDollWidget && OwnerMech)
    {
        PaperDollWidget->SetOwnerMech(OwnerMech);
    }
}
