# LocalTanks
Local Tanks는 TPS 게임 World of Tanks의 **적 탱크를 AI로 재구현, 게임 내 전투 관련 UI**를 언리얼 엔진으로 구현한 프로젝트입니다.

🎥 아래의 사진을 클릭시 전체 게임 플레이 영상을 보실 수 있습니다.

[![Video Label](http://img.youtube.com/vi/QvqlY-0BTxU/0.jpg)](https://youtu.be/QvqlY-0BTxU)


<br>

## 사용 기술
- Unreal Engine 5.5
- C++ / Blueprint 혼합 구조
- Rider

<br>

## 프로젝트 개요
| 항목 | 내용 |
| --- | --- |
| **장르** | 밀리터리 TPS 게임 |
| **플랫폼** | PC |
| **맵 컨셉** | 랜덤하게 스폰되는 AI탱크를 모두 처치하고 점령지를 탈환하는 맵입니다. |
| **개발 내용** | Off-Screen Indicator UI, 미니맵UI 를 포함해 전투 경험을 강화하는 UI 개발, AI탱크 개발 |

<br>

## 구현 상세 설명
각 기능의 더 자세한 설명은 [여기](https://kimskye.notion.site/Local-Tanks-1ceb7f13b7a680dcb4eeccc7445a982a?pvs=74)서 확인하실 수 있습니다.

---

## 1. 점령지 Off-Screen Indicator UI 
특수한 맵 이벤트(블랙홀 등장 → 중력 영향 → 파괴)를 중심으로 구성된 고유 게임 맵을 설계했습니다.

<img width="857" height="452" alt="image" src="https://github.com/user-attachments/assets/da54e64a-fa0f-4a8e-af2e-cff07daf6ef6" />

- `GameMode`에서 게임이 시작되면 플레이어들의 시작 위치를 원형으로 배치
- `GameState`에서 사망 이벤트 발생 시 남은 생존자 수와 팀 분포를 확인하여 **승리 조건**을 체크
    - 승리조건
        1. 남은 팀이 하나뿐인가? → 해당 팀이 Win
        2. 생존자가 한 명 뿐인가? → 해당 플레이어가 포함된 팀이 Win
    - 조건 충족시, `Multicast`로 승리 팀을 공지하고 게임을 종료

<br>

---

## 2. 미니맵 UI
- `PhysicsLinearVelocity` 를 통해 중력(≈구심력)과 궤도운동(≈원심력) 시스템을 구현,
- 플레이어와 물체가 특정 궤도와 속도로 블랙홀 주변을 공전할 수 있게 설계했습니다.

https://github.com/user-attachments/assets/431aef92-07c1-45b3-a0c9-0fac9f9d3528

- 컴포넌트가 부착된 물체만 블랙홀과 상호작용합니다.

<br>

### 캐릭터 정렬하기

<table>
  <tr>
    <td><img src="https://github.com/user-attachments/assets/b95c2759-ff98-486f-91de-a0a7177406ef" width="350"/></td>
    <td><img src="https://github.com/user-attachments/assets/0de8dfef-7347-4c7e-8806-9175c92ad4a8" width="350"/></td>
  </tr>
</table>

- 블랙홀의 영향을 받기 위해서는 캐릭터의 **캡슐 컴포넌트의 물리가 활성화**되어 있어야합니다.
- 블랙홀이 종료된 후, 캐릭터가 다시 움직이기 위해서는 **캡슐 컴포넌트의 물리가 비활성화** 되어 있어야 합니다.

<br>

https://github.com/user-attachments/assets/9e344bad-19d8-4643-8a12-50a33ed517e7

- 이를 위해 `FTimerManager`를 활용해 시간 기반 정렬(AlignCharacter)을 구현했습니다.
  - 블랙홀 종료 후, **물리 시뮬레이션과 Tick을 비활성화**
  - 일정 시간 동안 타이머를 돌려서 **Roll/Pitch를 0으로 보정**
  - 정렬이 완료되면 Tick을 다시 활성화하여 정상 상태 복귀

<br>

---

## 3. Beahavior Tree를 활용한 AI 탱크 개발

- 게임 중 플레이어가 사망하면 바로 게임을 이탈하지 않고 **관전자 시점에서 맵을 향해 물체를 투척하는** Spectator 모드를 구현했숩니다.

### 관전자 모드 전환
- 플레이어가 죽음 상태가 되었을 때 `SetViewTarget`을 활용해 시점을 전환한 후, `SpectatorPawn`으로 변경합니다.

https://github.com/user-attachments/assets/d1165e47-d2c8-4cf1-97de-3896f5d2573b


<br>

### 물체 투척하기
- 사용자의 마우스 클릭을 월드상의 레이(ray)로 변환 → ClickLocation, ClickDirection 획득
- **서버에서** 투사체를 생성하고 `TargetLocation` 방향으로 `UProjectileMovementComponent`에 속도를 부여합니다.
  
https://github.com/user-attachments/assets/83515985-1842-4bf9-987a-12d08b01b80b


---

<br>

## 트러블 슈팅
- **점령지 Off-Screen Indicator 화면 해상도 문제들** 을 해결했습니다.
- 프로젝트 관련 트러블 슈팅은 [여기](https://www.notion.so/kimskye/Local-Tanks-1ceb7f13b7a680dcb4eeccc7445a982a?source=copy_link#20bb7f13b7a6808c85e4f9d1e755f5bd)에서 확인하실 수 있습니다. -->
