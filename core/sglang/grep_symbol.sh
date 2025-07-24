# !/bin/bash

for file in /opt/homebrew/Cellar/llvm/20.1.8/lib/*; do
    if [ -f "$file" ]; then
      if nm -gC "$file" | grep -q "$1"; then
         echo "找到匹配的文件: $file"
         echo "匹配的符号:"
         nm -gC "$file" | grep --color=always "$1"
         echo "--------------------------------------"
      fi
    fi
done