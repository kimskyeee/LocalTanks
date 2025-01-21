// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OccupyUI.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UOccupyUI : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	//플레이어-Green, AI-Red
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UProgressBar* OProgressBar_Green;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UProgressBar* OProgressBar_Red;

	//점령당하고 있을때 프로그레스바 위에 나오는 텍스트 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* OingText_Green;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* OingText_Red;

	//점령당하고 있을때 프로그레스바 위에 나오는 숫자텍스트 (퍼센테이지)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* OingNumText_Green;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* OingNumText_Red;

	//점령이 완료되었을때 프로그레스바 위에 나오는 텍스트 (100퍼센트일때)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* OSuccessText_Green;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* OSuccessText_Red;

	void OProgressBarGreen(float CurrentP, float TotalP);
	void OProgressBarRed(float CurrentP, float TotalP);
	void OProgressReset();
};

