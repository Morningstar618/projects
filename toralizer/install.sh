#!/bin/bash

make

mkdir /usr/local/lib/toralize
mv ./toralize.so /usr/local/lib/toralize

cat > "/usr/local/bin/toralize" << EOF
#!/bin/bash

export LD_PRELOAD=/usr/local/lib/toralize/toralize.so

unset LD_PRELOAD
EOF

sed -i '4i ${@}' /usr/local/bin/toralize

chmod 755 /usr/local/bin/toralize