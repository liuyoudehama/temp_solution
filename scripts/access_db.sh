#!/bin/bash
# access db

DB_NAME="solution_data"
DB_USER="postgres"
HOST="localhost"
PORT="5432"

# create db
echo "checking DB $DB_NAME exist..."
EXISTS=$(psql -U "$DB_USER" -h "$HOST" -p "$PORT" -lqt | awk '{print $1}' | grep -w "$DB_NAME")
if [ -z "$EXISTS" ]; then
    echo -e "database named $DB_NAME doesn't exist..."
    exit 1
fi

# create tables using schema file
echo "loading schema file..."
psql -U "$DB_USER" -h "$HOST" -p "$PORT" -d "$DB_NAME"

# exit
if [ $? -eq 0 ]; then
    echo "finish init."
else
    echo "error, please check."
    exit 1WW
fi
