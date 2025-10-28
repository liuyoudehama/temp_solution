#!/usr/bin/env bash
# initdb using schema.sql (schema file via -f)

set -euo pipefail

DB_NAME="solution_data"
DB_USER="postgres"
HOST="localhost"
PORT="5432"

SCHEMA_FILE=""  # will be set via -f

usage() {
  cat <<'USAGE'
Usage: initdb.sh -f <schema_file> [options]

Required:
  -f <schema_file>   Path to the schema .pgsql file

Optional (env or tweak script if needed):
  --db-name <name>   Database name (default: solution_data)
  --user <user>      DB user (default: postgres)
  --host <host>      DB host (default: localhost)
  --port <port>      DB port (default: 5432)

Examples:
  ./initdb.sh -f ../schema.pgsql
USAGE
}

# Simple args parsing: -f and a few long options if you want them
while [[ $# -gt 0 ]]; do
  case "$1" in
    -f)
      [[ $# -ge 2 ]] || { echo "error: -f requires a file path"; usage; exit 1; }
      SCHEMA_FILE="$2"; shift 2;;
    --db-name)
      DB_NAME="${2:-}"; shift 2;;
    --user)
      DB_USER="${2:-}"; shift 2;;
    --host)
      HOST="${2:-}"; shift 2;;
    --port)
      PORT="${2:-}"; shift 2;;
    -h|--help)
      usage; exit 0;;
    *)
      echo "error: unknown option: $1"; usage; exit 1;;
  esac
done

# Validate schema file
if [[ -z "${SCHEMA_FILE}" ]]; then
  echo "error: schema file not provided. Use -f <schema_file>."
  usage
  exit 1
fi
if [[ ! -f "$SCHEMA_FILE" ]]; then
  echo "error: no schema file: $SCHEMA_FILE"
  exit 1
fi

echo "checking DB $DB_NAME exist..."
# List DBs and check name
if psql -U "$DB_USER" -h "$HOST" -p "$PORT" -lqt | awk '{print $1}' | grep -qx "$DB_NAME"; then
  echo "database named $DB_NAME exist, skip creating..."
else
  echo "database named $DB_NAME doesn't exist, creating..."
  createdb -U "$DB_USER" -h "$HOST" -p "$PORT" "$DB_NAME"
fi

echo "loading schema file..."
if psql -U "$DB_USER" -h "$HOST" -p "$PORT" -d "$DB_NAME" -f "$SCHEMA_FILE"; then
  echo "finish init."
else
  echo "error, please check."
  exit 1
fi
