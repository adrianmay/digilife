echo "$1" | sed '/^gen\/[^_/]*\//s#^gen#lit#; /^gen\/[^_/.]*\..$/s#^gen#lit#; /^gen\/[^./_]*_[^./_]*\//s#^gen/[^_]*_#tpt/#; s#^bin/test/#test/#; s#^bin/hive/#hive/#'
