#include "TreeObject.h"
#include <Objects/Enemies/EnemyBase.h>
#include <Objects/PlayerObject.h>
#include <Engine/Log.h>
#include <UI/GameUI.h>

void TreeObject::Begin()
{
	TreeMesh = new MeshComponent();
	Attach(TreeMesh);
	TreeMesh->Load("Tree");
	GetTransform().Scale = 1.5;
	Collider = new CollisionComponent();
	Attach(Collider);
	Collider->Load(TreeMesh->GetModelData().GetMergedVertices(), TreeMesh->GetModelData().GetMergedIndices());
}

void TreeObject::Update()
{
#if !EDITOR
	PlayerObject::GetPlayer()->EnemyNearTree = false;
	for (auto o : Objects::AllObjects)
	{
		if (dynamic_cast<EnemyBase*>(o))
		{
			if(Vector3::Distance(o->GetTransform().Location, GetTransform().Location) < 50)
			PlayerObject::GetPlayer()->EnemyNearTree = true;
		}
	}
	if (Health <= 0.f && !IsDead)
	{
		IsDead = true;
		PlayerObject::GetPlayer()->DisplayedUI->ShowMessage("The tree has been destroyed!", Vector3(1, 0, 0));
		PlayerObject::GetPlayer()->QuitGameToMenu();
	}
#endif
}

void TreeObject::Destroy()
{
}
