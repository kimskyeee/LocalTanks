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
월드의 목표지점(점령지)가 화면 밖에 위치할 때, 점령지의 위치를 표시하는 인디케이터 UI를 구현했습니다.

https://github.com/user-attachments/assets/8bc39d81-26b6-4028-9a55-93f7b53e554c


- Off-Screen일 때,
  
   1. 시야 중심(ViewPoint) → 대상(Target) 방향 단위 벡터 계산
   2. 방향 벡터를 회전 값 (각도)로 변환해 화면에 표시

<br>

---

## 2. 미니맵 UI
`SceneCaptureComponent2D`를 이용해 플레이어가 현재 위치, 적의 위치, 목적지를 직관적으로 파악할 수 있는 미니맵 UI를 구현했습니다.

https://github.com/user-attachments/assets/0ca26c9d-e48f-4f50-ace4-e04cc719c76d

- 맵은 고정된 형태이므로, 지형은 RenderTarget으로 한번만 찍어 만든 정적인 텍스처 이미지로 사용하였습니다.
- 캐릭터, 적, 목적지 등 **이동 요소만** `GetActorLocation()`을 기준으로 **맵 상 상대 위치 → 미니맵 UI 좌표**로 변환해서 이미지나 아이콘 위치를 이동시켰습니다.

<br>

---

## 3. Beahavior Tree를 활용한 AI 탱크 개발

- Unreal Engine의 Behavior Tree와 Blackboard 시스템, AI Perception 시스템(Sight) 을 활용해 AI의 전투 행동을 상황(체력, 거리, 타겟 여부)에 따라 분기합니다.
- `EEnemyRole` 타입을 통해 Behavior Tree에서 Decorator를 통해 **탱크의 역할별로 분기 실행**되는 구조입니다.

### 행동 분기 다이어그램

![제목 없는 다이어그램](https://github.com/user-attachments/assets/85c49509-cbf5-4614-b789-49f07fe39e33)

- Rusher : 체력이 낮아져도 돌진을 유지하는 공격 우선 탱크
- Hider : 체력이 낮아지면 회피하는 은신 탱크


---

<br>

## 트러블 슈팅
- **점령지 Off-Screen Indicator 화면 해상도 문제들** 을 해결했습니다.
- 프로젝트 관련 트러블 슈팅은 [여기](https://www.notion.so/kimskye/Local-Tanks-1ceb7f13b7a680dcb4eeccc7445a982a?source=copy_link#20bb7f13b7a6808c85e4f9d1e755f5bd)에서 확인하실 수 있습니다.
