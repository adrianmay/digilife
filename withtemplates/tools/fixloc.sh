echo "$1" | sed '/^gen\/[^_/]*\//s/^gen/src\/lit/ ; /^gen\/[^_/.]*\..$/s/^gen/src\/lit/ ; /^gen\/[^./_]*_[^./_]*\//s/^gen\/[^_]*_/src\/tpt\// ; s/^bin\//src\/exe\//'
