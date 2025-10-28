#!/bin/bash

DBNAME="solution_data"
PGUSER="postgres"
PGHOST="127.0.0.1"
PGPORT="5432"

echo "Dropping database $DBNAME ..."

psql -U $PGUSER -h $PGHOST -p $PGPORT -c "DROP DATABASE IF EXISTS $DBNAME;" postgres

if [ $? -eq 0 ]; then
    echo "Database $DBNAME dropped successfully."
else
    echo "Failed to drop database $DBNAME."
    exit 1
fi
