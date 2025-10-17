#!/bin/sh

# NOTE: Make sure all dependencies mentioned in README.md have been installed before proceeding

# ---- RABBITMQ ----
pushd .
cd message_queues/rabbitmq
docker-compose up -d
popd
echo "======= RABBITMQ CREATED ============"
echo

# ---- POSTGRES -----
pushd .
cd database/postgres
docker-compose up -d
echo "[*] Waiting for Postgres to startup..."
sleep 10
echo "[*] Applying migrations..."
migrate -path ./migrations -database "postgres://pulseuser:supersecret@localhost:5432/pulse?sslmode=disable" up
popd
echo "======= POSTGRES CREATED ============"
echo

# ---- SCYLLA-----
pushd .
cd database/scylla
#800000 is the minimum number of IOPS required to start 2 containers
docker run --rm --privileged alpine sysctl -w fs.aio-max-nr=800000 
docker-compose up -d
echo "[*] Waiting for Scylla to startup..."
sleep 60
echo "[*] Applying Migrations..."
migrate -source file://migrations -database "cassandra://localhost:9042/migrate_keyspace" up
docker rm scylla-init
popd 
echo "======= SCYLLA CREATED ============"
echo

# ---- PORTAINER -----
pushd .
cd monitoring/portainer
docker-compose up -d
echo "======= PORTAINER CREATED ============"
popd