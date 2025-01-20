#!/bin/bash

# 추적할 디렉토리 (사용자가 설정)
TARGET_DIR="Content/MWLandscapeAutoMaterial"

# 디렉토리가 존재하는지 확인
if [ ! -d "$TARGET_DIR" ]; then
  echo "Error: Directory '$TARGET_DIR' does not exist."
  exit 1
fi

# 디렉토리와 하위 디렉토리 내 모든 파일 추적
echo "Tracking all files in '$TARGET_DIR' with Git LFS..."
find "$TARGET_DIR" -type f | while read -r FILE; do
  git lfs track "$FILE"
done

# .gitattributes 파일 추가 및 변경 사항 커밋
echo "Adding .gitattributes to Git..."
git add .gitattributes
git commit -m "Track all files in '$TARGET_DIR' and subdirectories with Git LFS"

echo "Git LFS tracking completed successfully."

