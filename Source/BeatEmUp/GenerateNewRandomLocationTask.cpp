// Fill out your copyright notice in the Description page of Project Settings.


#include "GenerateNewRandomLocationTask.h"

#include "EnemyBTController.h"

EBTNodeResult::Type UGenerateNewRandomLocationTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	UBehaviorTreeComponent* BTComp = &OwnerComp;
	if(!BTComp) return EBTNodeResult::Failed;

	AEnemyBTController* BTController = Cast<AEnemyBTController>(BTComp->GetOwner());
	if(!BTController) return EBTNodeResult::Failed;

	BTController->GenerateNewRandomLocation();
	return EBTNodeResult::Succeeded;
}
