// Fill out your copyright notice in the Description page of Project Settings.


#include "InvestigateLastSeenLocationTask.h"

#include "EnemyBTController.h"

EBTNodeResult::Type UInvestigateLastSeenLocationTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if(!BlackboardComp) return EBTNodeResult::Failed;

	AEnemyBTController* BTController = Cast<AEnemyBTController>(OwnerComp.GetAIOwner());
	if(!BTController) return EBTNodeResult::Failed;

	FVector LastSeenLocation = BlackboardComp->GetValueAsVector("LastSeenLocation");
	
	BTController->MoveToLocation(LastSeenLocation, 5.0f, true, true, true, true);
	if(FVector::Dist(BTController->GetPawn()->GetActorLocation(), LastSeenLocation) < 50.f)	{
		BTController->HideInvestigatingIcon();
		BlackboardComp->ClearValue("LastSeenLocation");
		BlackboardComp->SetValueAsBool("ChasePlayer", false);
	}

	return EBTNodeResult::Succeeded;
}
