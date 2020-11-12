// Copyright PlatoSpace.com All Rights Reserved.

#include "MRDisplayWidgetComponent.h"

#include "MechRangers/Gameplay/Mech/Widgets/MRMechPaperDollWidget.h"

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
