#!/bin/bash
# initdb using schema.sql

DB_NAME="solution_data"
DB_USER="postgres"
SCHEMA_FILE="../schema.pgsql"
HOST="localhost"
PORT="5432"

# check schema file
if [ ! -f "$SCHEMA_FILE" ]; then
    echo "error: no schema file: $SCHEMA_FILE"
    exit 1
fi

# create db
echo "checking DB $DB_NAME exist..."
EXISTS=$(psql -U "$DB_USER" -h "$HOST" -p "$PORT" -lqt | awk '{print $1}' | grep -w "$DB_NAME")
if [ -z "$EXISTS" ]; then
    echo -e "database named $DB_NAME doesn't exist, creating..."
    createdb -U "$DB_USER" -h "$HOST" -p "$PORT" "$DB_NAME"
else
    echo "database named $DB_NAME exist, skip creating..."
fi

# create tables using schema file
echo "loading schema file..."
psql -U "$DB_USER" -h "$HOST" -p "$PORT" -d "$DB_NAME" -f "$SCHEMA_FILE"

# exit
if [ $? -eq 0 ]; then
    echo "finish init."
else
    echo "error, please check."
    exit 1WW
fi
